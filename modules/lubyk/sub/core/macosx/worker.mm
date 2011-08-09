/*
  ==============================================================================

   This file is part of the LUBYK project (http://lubyk.org)
   Copyright (c) 2007-2011 by Gaspard Bucher (http://teti.ch).

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
#include "lubyk/worker.h"
#include "lubyk/cocoa.h"
#include "lubyk/lua.h"

using namespace lubyk;

#include <mach-o/dyld.h> // _NSGetExecutablePath


class Worker::Implementation : public lubyk::Thread {
public:
  Implementation() {
    // create a thread that will run NSApp
    startThread<Implementation, &Implementation::run>(this, NULL);
  }

  ~Implementation() {
    CFRunLoopStop(CFRunLoopGetCurrent());
    //[[NSRunLoop currentRunLoop] halt];
  }

  void run(Thread *thread) {
    //  release calling thread semaphore
    thread_ready();
    ScopedPool pool;
    printf("Run Loop\n");
    [NSApplication sharedApplication];
    [NSApp run];
    //[[NSRunLoop currentRunLoop] configureAsServer];
    //[[NSRunLoop currentRunLoop] run];
  }
};

Worker::Worker(lua_State *L)
 : lua_(L),
  zmq_context_(NULL),
  zmq_context_refcount_(0) {
  //impl_ = new Worker::Implementation;
  lock();
}

Worker::~Worker() {
  //delete impl_;
  unlock();
}

void Worker::run() {
  printf("Run App\n");
  ScopedUnlock unlock(this);
  [NSApplication sharedApplication];
  [NSApp run];
}

static char *getExecPath() {

  // http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
  uint32_t bufsize = 200;
  char *buf = (char*)malloc(bufsize * sizeof(char));
  if (_NSGetExecutablePath(buf, &bufsize)) {
    // buffer too small
    char * nbuf = (char*)realloc(buf, bufsize * sizeof(char));
    if (nbuf) {
      if (_NSGetExecutablePath(nbuf, &bufsize)) {
        // error again .... ?
        free(nbuf);
        return NULL;
      } else {
        return nbuf;
      }
    } else {
      // could not allocate memory
      free(buf);
      return NULL;
    }
  } else {
    // ok
    return buf;
  }
}
/** Get the current executable's path.
 */
LuaStackSize Worker::execPath(lua_State *L)
{
  char *path = getExecPath();
  if (path) {
    lua_pushstring(L, path);
    free(path);
    return 1;
  } else {
    return 0;
  }
}

/** Start a new process with the given Lua script.
 */
LuaStackSize Worker::spawn(const char *script, lua_State *L)
{
  char *path = getExecPath();
  if (path) {
    char arg1[] = "-e";
    char *argv[] = {path, arg1, const_cast<char*>(script), NULL};

    int pid = fork();
    if (pid == 0) {
      // child process
      execv(argv[0], argv);
      // unreachable
      exit(0);
      return 0;
    } else {
      // parent process
      free(path);
      lua_pushnumber(L, pid);
      return 1;
    }
  } else {
    // could not get executable path
    return 0;
  }
}

void Worker::exit(int status) {
  unlock();
  ::exit(status);
}

int Worker::waitpid(int pid) {
  int child_status;
  ScopedUnlock unlock(this);
  ::waitpid(pid, &child_status, 0);
  if (WIFEXITED(child_status)) {
    return WEXITSTATUS(child_status);
  } else {
    return -1;
  }
}
