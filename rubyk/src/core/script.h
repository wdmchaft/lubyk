#ifndef RUBYK_SRC_CORE_SCRIPT_H_
#define RUBYK_SRC_CORE_SCRIPT_H_
#include	<sys/types.h>
#include	<sys/stat.h> // stat for file modification time
#include  <fstream>    // file io

#include "class.h"

/** Abstract class to manage all the script load, reload. */
class Script : public Node {
public:
  Script () : script_mod_time_(0), reload_every_(0), next_reload_(0) {
    set_is_ok(false);
  }
  
  virtual ~Script() {}
  
  // {script}, [script] ?
  const Value script(const Value &val) {
    if (!val.is_nil()) {
      reload_every_ = 0;
      script_       = val.str();
      Value res = save_script();
      if (res.is_error()) return res;
      res = eval_script();
      if (res.is_error()) return res;
      set_is_ok(true);
    }
    return Value(script_);
  }
  
  // {file}, [file] ?
  const Value file(const Value &val) {
    if (!val.is_nil()) {
      script_file_  = val.str();
      reload_every_ = 1.0; // [s]
      return load_script_from_file(true);
    }
    return Value(script_file_);
  }
  
  // {reload}, [reload] ?
  const Value reload(const Value &val) {
    if (!val.is_nil()) {
      reload_every_ = val.r > 0 ? val.r : 0;
    }
    return Value(reload_every_);
  }
  
  void inspect(Value *hash) {
    hash->set("script", script_);
    hash->set("file", script_file_);
    hash->set("reload", reload_every_);
  }
  
 protected:
  
  void reload_script() {
    if ( !reload_every_ || (next_reload_ > worker_->current_time_) ) {
      return;
    }
    
    next_reload_ = worker_->current_time_ + (reload_every_ * ONE_SECOND);
     
    load_script_from_file(false);
  }
  
  const Value load_script_from_file(bool is_new) {
    struct stat info;
    
    if (stat(script_file_.c_str(), &info)) {
      set_is_ok(false);
      return Value(BAD_REQUEST_ERROR, std::string("Could not stat '").append(script_file_).append("'."));
    }
    
    if (!is_new && info.st_mtime == script_mod_time_) {
      // file did not change, skip
      return Value(script_file_);
    }
    
    script_mod_time_ = info.st_mtime;
    
    std::ifstream in(script_file_.c_str() ,std::ios::in);
      // TODO: remove ostringstream and read directly into string ?
      std::ostringstream oss;
      oss << in.rdbuf();
    in.close();
    script_ = oss.str();
    
    Value res = eval_script();
    set_is_ok(!res.is_error());
    if (res.is_error()) return res;
    
    return Value(script_file_);
  }
  
  /** Save script content to file.
   */
  const Value save_script() {
    if (script_file_ == "") return gNilValue;

    // TODO: check file is writeable, limit path, etc
    //   return Value(INTERNAL_SERVER_ERROR, std::string("Could not save to '").append(script_file_).append("' (.....)."));
    
    // try .. ?
    std::ofstream out(script_file_.c_str() ,std::ios::out);
      out << script_;
    out.close();
    
    struct stat info;
    
    if (stat(script_file_.c_str(), &info)) {
      set_is_ok(false);
      return Value(BAD_REQUEST_ERROR, std::string("Could not stat '").append(script_file_).append("'."));
    }
    
    script_mod_time_ = info.st_mtime;
    return gNilValue;
  }
  
  virtual const Value eval_script() = 0;
  
protected:
  /** Script text.
   */
  std::string script_;
  
  /** Path to script file when the text is stored in a file.
   */
  std::string script_file_;
  
  /** Script file's modification time when it was last loaded.
   */
  time_t script_mod_time_;
  
  /** How often we should check for a modification in the source file (in seconds).
   */
  Real reload_every_;
  
  /** Next scheduled reload.
   */
  time_t next_reload_;
};

#endif // RUBYK_SRC_CORE_SCRIPT_H_