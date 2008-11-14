#include "class.h"
#include	<sys/types.h>
#include	<sys/stat.h> // stat for file modification time
#include  <fstream>    // file io

/** Abstract class to manage all the script load, reload. */
class Script : public Node
{
public:
  Script () : mScriptModTime(0), mReloadEvery(0), mNextReload(0), mScriptOK(false) {}
  
  bool set_script(const Params& p)
  {
    std::string str;
    
    if (p.get(&str, "load")) {
      mScriptFile  = str;
      mScriptOK    = load_script_from_file(true);
      mReloadEvery = 1;
    } else if (p.get(&str, "script", true)) {
      mScriptFile  = "";
      mReloadEvery = 0;
      mScriptOK = eval_script(str);
    }
    if (mScriptFile != "")
      mReloadEvery = p.val("reload", mReloadEvery);
    
    return mScriptOK;
  }
  
  // load script from file.
  void load(const Params& p)
  {
    
    if (!p.get(&mScriptFile) && mScriptFile == "") {
      *mOutput << "Please give a filename.\n" << std::endl;
      return;
    }
    
    mReloadEvery = p.val("reload", mReloadEvery);
    if (!p.get(&mReloadEvery, "reload") && !mReloadEvery)
      mReloadEvery = 1;
    
    mScriptOK = load_script_from_file(true);
  }
  
  void reload_script()
  {
    if ( !mReloadEvery || (mNextReload > mServer->mCurrentTime) ) {
      return;
    }
    
    mNextReload = mServer->mCurrentTime + (mReloadEvery * ONE_SECOND);
     
    mScriptOK = load_script_from_file(false);
  }
  
  bool load_script_from_file(bool isNewFile)
  {
    struct stat info;
    
    if (stat(mScriptFile.c_str(), &info)) {
      *mOutput << "Could not stat '" << mScriptFile << "'." << std::endl;
      return false;
    }
    
    if (!isNewFile && info.st_mtime == mScriptModTime) {
      // file did not change, skip
      return true;
    }
    
    mScriptModTime = info.st_mtime;
    
    std::ifstream in(mScriptFile.c_str() ,std::ios::in);
      std::ostringstream oss;
      oss << in.rdbuf();
    in.close();
    oss << "\n";
    if (!eval_script(oss.str())) return false;
    *mOutput << mName << ": script loaded.\n";
    return true;
  }
  
  virtual bool eval_script(const std::string& pScript) = 0;
  
  void script(const Params& p)
  {  
    if (mScript == "" && mReloadEvery) reload_script();
    *mOutput << mScript << std::endl;
  }
  
  virtual void spy() 
  { 
    bprint(mSpy, mSpySize,"%s", mScriptFile.c_str() );
  }
  
protected:
  std::string mScript;         /**< Script text. */
  std::string mScriptFile;     /**< Script file path. */
  time_t      mScriptModTime;  /**< Script file's modification time on last load. */
  double      mReloadEvery;    /**< How often should we check for a modification in the source file (in seconds). */
  time_t      mNextReload;     /**< Compute time for the next check to reload the script. */
  bool        mScriptOK;       /**< Script compilation status. Might change on next reload. */
  
};