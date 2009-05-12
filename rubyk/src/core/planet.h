#ifndef _PLANET_H_
#define _PLANET_H_
#include "oscit.h"

#include "group.h"
#include "text_command.h"
#include "worker.h"

#include <queue>

class ClassFinder;

#define DEFAULT_OBJECTS_LIB_PATH "/usr/local/lib/rubyk"
#define RUBYK_DEFAULT_NAME "rubyk"

/** A planet is just a root with a worker. */
class Planet : public Root
{
 public:
  TYPED("Object.Root.Planet")
  
  Planet() : Root(RUBYK_DEFAULT_NAME), worker_(this), classes_(NULL) {
    init();
  }
  
  Planet(uint port) : Root(RUBYK_DEFAULT_NAME), worker_(this), classes_(NULL) {
    init();
    open_port(port);
  }
  
  Planet(int argc, char * argv[]) : Root(RUBYK_DEFAULT_NAME), worker_(this), classes_(NULL) {
    // TODO: get port from command line
    init();
    
    if (argc > 1) {
      std::string file_name(argv[1]);
      set_name(file_name.substr(0, file_name.rfind(".")));
      std::ifstream in(argv[1], std::ios::in);
      std::ostringstream oss;
      oss << in.rdbuf();
      in.close();

      TextCommand * command = adopt_command(new TextCommand(std::cin, std::cout), false);
      command->set_silent();
      oss << "\n";
      command->parse(oss.str());
      delete command;
    }
  }
  
  void open_port(uint port) {
    adopt_command(new OscCommand(port));
  }
  
  void start() {
    worker_.start();
  }
  
  /** Only used with direct loop control. */
  void should_run(bool should_run) {
    worker_.should_run(should_run);
  }
  
  void join() {
    worker_.join();
  }
  
  void quit() {
    worker_.kill();
    clear(); // kill commands and destroy objects
  }
  
  bool loop() {
    return worker_.loop();
  }
  
  time_t current_time() {
    return worker_.current_time_;
  }
  
  inline Worker *worker() { return &worker_; }
  
  /** Used to access '/class' when rko objects are loaded. */  
  ClassFinder *classes() { return classes_; }
  
  /** Create/remove a link between two slots. */
  const Value link(const Value &val);
  
  /** Tell worker to stop.
   */
  const Value quit(const Value &val) {
    worker_.quit();
    return gNilValue;
  }
  
  /** Calls 'inspect' on a node. */
  const Value inspect(const Value &val);
  
 private:
  /** Add a pending link. */
  const Value add_pending_link(const Value &val) {
    Value params;
    params.copy(val);
    params[1].set("?");
    pending_links_.push_back(Call(LINK_URL, params));
    return params;
  }
  
  /** Remove a link from the pending link list. */
  const Value remove_pending_link(const Value &val);
  
  /** Create pending links. Return a list of created links [[sffs][sffs]...]. */
  const Value create_pending_links();
  
  
  /** Create base objects (public for testing, should not be used). */
  void init();
  
  std::list<Call>  pending_links_;        /**< List of pending connections waiting for variable assignements. */
  Worker worker_;
  ClassFinder *classes_;
};

#endif // _PLANET_H_