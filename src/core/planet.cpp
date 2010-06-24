/*
  ==============================================================================

   This file is part of the RUBYK project (http://rubyk.org)
   Copyright (c) 2007-2010 by Gaspard Bucher - Buma (http://teti.ch).

  ------------------------------------------------------------------------------

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.

  ==============================================================================
*/

#include "rubyk/node.h"
#include "rubyk/class_finder.h"
#include "rubyk/text_command.h"
#include "rubyk/planet.h"
#include "rubyk/patch_method.h"

namespace rk {

void Planet::init() {
  set_context(&worker_);

  // build application methods

  //           /.inspect
  // FIXME: remove /.inspect...
  adopt(new TMethod<Planet, &Planet::inspect>(this, Url(INSPECT_URL).name(), StringIO("Returns inspect information on a given url.")));

  //          /class
  classes_ = adopt(new ClassFinder(Url(CLASS_URL).name(), DEFAULT_OBJECTS_LIB_PATH));

  //          /rubyk
  Object *rubyk = adopt(new Object(Url(RUBYK_URL).name()));

  //          /rubyk/link [[["","source url"],["", "target url"]], "Create a link between two urls."]
  rubyk->adopt(new TMethod<Planet, &Planet::link>(this, Url(LINK_URL).name(), JsonValue("[['','', ''],'url','op','url','Update a link between the two provided urls. Operations are '=>' (link) '||' (unlink) or '?' (pending).']")));

  //          /rubyk/quit
  rubyk->adopt(new TMethod<Planet, &Planet::quit>(this, Url(QUIT_URL).name(), BangIO("Stop all operations and quit.")));
}

bool Planet::expose_views(const std::string &path, Value *error) {
  if (this->Root::expose_views(path, error)) {
    // /views/patch
    ObjectHandle views;
    if (get_object_at(VIEWS_PATH, &views)) {
      views->adopt(new PatchMethod(Url(PATCH_VIEW_URL).name(), "patch.rks"));
    }
    return true;
  }

  return false;
}

// typetag: "ss" (inlet, outlet)
const Value Planet::link(const Value &val) {
  // std::cout << "link: " << val << std::endl;
  if (val.is_nil()) {
    return create_pending_links();
  }

  Value error;
  ObjectHandle source, target;
  if (!get_object_at(Url(val[0].str()), &error, &source) || !get_object_at(Url(val[2].str()), &error, &target)) {
    // not found
    if (val[1].str() == "=>") {
      return add_pending_link(val);
    } else if (val[1].str() == "||") {
      // remove from pending links
      return remove_pending_link(val);
    }
    return val;
  }

  Outlet *source_outlet = source.type_cast<Outlet>();

  ObjectHandle out, outlet;

  if (source_outlet != NULL) {
    return val[1].str() == "||" ? source_outlet->unlink(val[2]) : source_outlet->link(val[2]);
  } else if (source->get_child(NODE_OUT_KEY, &out) && out->first_child(&outlet)) {
    // was a link to/from default slots: /met --> /counter
    if ( (source_outlet = outlet.type_cast<Outlet>()) ) {
      return val[1].str() == "||" ? source_outlet->unlink(val[2]) : source_outlet->link(val[2]);
    } else {
      return Value(BAD_REQUEST_ERROR, std::string("Object at '").append(source->url()).append("' does not support links (using first child of '").append(source->url()).append("')."));
    }
  } else {
    return Value(BAD_REQUEST_ERROR, std::string("Object at '").append(source->url()).append("' does not support links (not an Outlet)."));
  }
}

// FIXME: on node deletion/replacement, remove/move all pending links related to this node ?.
// FIXME: thread safety !
const Value Planet::create_pending_links() {
  std::list<Call>::iterator it  = pending_links_.begin();
  std::list<Call>::iterator end = pending_links_.end();

  Value res;
  Value list;

  while (it != end) {
    res = it->trigger_call(this);
    if ((res.type_id() == SelectIOTypeId && res[1].str() == "=>") || res.is_error()) {
      list.push_back(res);
      it = pending_links_.erase(it);  // call succeeded or definitely failed
    } else {
      ++it;
    }
  }
  // return list of created links
  return list;
}


// FIXME: on node deletion/replacement, remove/move all pending links related to this node ?.
const Value Planet::remove_pending_link(const Value &val) {
  std::list<Call>::iterator it  = pending_links_.begin();
  std::list<Call>::iterator end = pending_links_.end();

  Value res;
  res.set_nil();

  while (it != end) {

    if (it->param_[0].str() == val[0].str() && it->param_[2].str() == val[2].str()) {
      res = it->param_;
      res[1].set("||");
      it = pending_links_.erase(it);  // removed
      break;
    } else {
      ++it;
    }
  }
  return res;
}

const Value Planet::inspect(const Value &val) {
  if (!val.is_string()) return Value(BAD_REQUEST_ERROR, "Bad arguments:'inspect' should be called with an url.");
  Value res;
  ObjectHandle object;
  if (!find_or_build_object_at(val.str(), &res, &object)) return res;
  Node *node = object.type_cast<Node>();
  if (!node) return Value(BAD_REQUEST_ERROR, std::string("Bad target '").append(object->url()).append("':inspect only works on Nodes (class is '").append(object->class_path()).append("')."));
  return node->do_inspect();
}

/** Return a hash representation of the whole system.
 */
void Planet::insert_in_hash(Value *hash) {
  HashValue patch;

  // FIXME: remove insert_in_hash method.
  hash->set(VIEW_KEY, pos_.to_hash());

  { ScopedRead lock(children_vector_);
    if (children_vector_.size() > 0) {
      std::vector<Object*>::iterator it, end = children_vector_.end();

      for(it = children_vector_.begin(); it != end; ++it) {
        Node *node = TYPE_CAST(Node, *it);
        if (node) {
          Value node_hash;
          node->insert_in_hash(&node_hash);
          patch.set(node->name(), node_hash);
        }
      }
    }
  }

  hash->set(NODES_KEY, patch);
}

/** Set/get the patch from a hash representation. If the value is nil,
 * return the full view.
 */
const Value Planet::patch(const Value &hash) {
  if (!hash.is_hash()) {
    return to_hash();
  } else {
    // FIXME: set nodes from content...
    // clear and update

    clear_on_register_callbacks();

    this->Object::clear();

    return update(hash);
  }
}

/** Update the content of the nodes by doing a deep merge.
 */
void Planet::from_hash(const Value &hash, Value *result) {
  if (!hash.is_hash()) return;

  Value nodes;
  Value view_result, view = hash[VIEW_KEY];

  if (view.is_hash()) {
    Value view_result;
    pos_.from_hash(view, &view_result);
    // FIXME: replace void from_hash(const Value &hash, Value *result)
    //        by      const Value from_hash(const Value &hash)
    // ==> result.set(VIEW_KEY, pos_.from_hash(hash[VIEW_KEY]));
    result->set(VIEW_KEY, view_result);
  }

  if (hash.get(NODES_KEY, &nodes) && nodes.is_hash()) {
    HashValue nodes_result;

    HashIterator it, end = nodes.end();
    ObjectHandle obj;
    Node *node;
    Value node_value;
    std::string key;

    for (it = nodes.begin(); it != end; ++it) {
      key = *it;
      if (!nodes.get(key, &node_value)) continue; // this should never happen, just in case ;-)

      if (get_child(key, &obj)) {
        // existing Object
        if (! (node = TYPE_CAST(Node, obj.ptr()))) continue;

        // It's a Node

        if (node_value.is_hash()) {
          // update Node
          Value node_result;
          node->from_hash(node_value, &node_result);
          nodes_result.set(key, node_result);
        } else if (node_value.is_nil()) {
          // delete Node
          node->release();
          nodes_result.set(key, gNilValue);
        } else {
          // invalid argument (can only create/update Node from Hash).
          // Ignore. Insert error ?
        }
      } else {
        // create Node
        if (!node_value.has_key(CLASS_KEY)) {
          nodes_result.set(key, ErrorValue(BAD_REQUEST_ERROR, "Missing @class to create new Node."));
          continue;
        }

        create_node(key, node_value[CLASS_KEY], node_value, &nodes_result);
      }
    }
    result->set(NODES_KEY, nodes_result);
  }
}

void Planet::create_node(const std::string &name, const Value &class_url, const Value &params, Value *result) {
  if (!class_url.is_string()) return; // ignore

  Value view_params = params[VIEW_KEY];

  if (!view_params.has_key(POS_X)) {
    // get next location for new Node

    { ScopedRead lock(children_vector_);
      size_t size = children_vector_.size();
      Node *node = NULL;

      // get last child that is a Node
      for(size_t i = size - 1; i >= 0; --i) {
        if ( (node = TYPE_CAST(Node, children_vector_[i])) ) break;
      }

      if (node) {
        view_params.set(POS_X, node->pos_x() + 15);
        view_params.set(POS_Y, node->pos_y() + 15);
      }
    }
  }

  ObjectHandle obj;
  Node *node;

  // /class/Metro/new 'x' params
  Value args;
  args.push_back(name);
  args.push_back(params);

  Value name_or_error = call(std::string(class_url.str()).append("/new"), args);

  if (name_or_error.is_string() && get_object_at(name_or_error.str(), &obj)) {

    node = TYPE_CAST(Node, obj.ptr());

    if (!node) {
      // should never happen
      assert(false);
      return;
    }

  } else {
    result->set(name, name_or_error);
    return;
  }

  Value links = create_pending_links(); // create pending links

  result->set(Url(name_or_error.str()).name(), node->to_hash());
}

} // rk
