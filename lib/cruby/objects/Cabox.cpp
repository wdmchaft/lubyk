#include "Serial.cpp"

class Cabox : public Serial
{
public:
  
  bool init (const Params& p)
  {
    for(int i=0; i<12;i++) {
      mValues[i] = 0.0;
      mOffset[i] = 0.0;
    }
    
    mDebug = false;
    mNewData = false;
    
    mIndex = 0;
    mState = 0; // wait for sync
    mOffsetOnFull = true; // do not offset when 12 values are set
    mSlowPort = false;
    
    if (init_serial(p))
    {
      // enter read data
      return mPort.write_char('b');
    } else
      return false;
  }
  
  // outlet 1
  void stream(Signal& sig)
  {
    if (mNewData) {
      sig.type = DoubleArraySignal;
      sig.doubles.size  = mWindowSize * DEFAULT_WINDOW_COUNT;
      sig.doubles.value = mBuffer + mReadPosition;
      if (mDebug)
        *mOutput << sig << std::endl;
    }
  }
  
  // outlet 2: send highest value
  void high(Signal& sig)
  {
    if (mNewData)
      sig.set(mHighestValue);
  }
  
  // outlet 3: send highest value's direction
  void direction(Signal& sig)
  {
    receive();
    if (mNewData)
      sig.set(mHighestDirection);
  }
  
  void offset()
  {
    mOffsetOnFull = true;
  }
  
  void debug()
  {
    mDebug = !mDebug;
    if (mDebug)
      *mOutput << mName << ": debug on\n";
    else
      *mOutput << mName << ": debug off\n";
  }
  
private:
  
  void receive()
  {
    int c,r;
    double val;
    int i;
    bool read_count = 0;
    mNewData = false;
    while (mPort.read_char(&c)) {
      // empty buffer (readall)
      read_count++;
      
      //printf("%i[%i]\n",mState,c);
      if (mState == 0) {
        if (c == 255) mState = 1;
        continue;
      }
    
      if (mState == 1) { // first sync received
        if (c == 255) { // move into data mode
          mIndex = 0;
          mState = 2;
          continue;
        } else
          mState = 0;
        continue;
      }
    
      if (mState == 2 && ((mIndex % 2) == 0)) {
        mIndex += 1;
        mHigh = c;
        continue;
      }
    
      val  = (mHigh * 256) + c;
      val  = (val/1024) * (3300 - 1650) / 200; // 3300 = fullscale_mv, 1024 = fullscale_bin, 1650 = offset_mv, 200 = sensitivity_mv
      mIndex += 1;
      i = (mIndex/2) - 1;
      mValues[i] = val - mOffset[i];
      
      if (mIndex >= 24) {
        mNewData = true;
        if (mOffsetOnFull) {
          for(int i=0; i<12; i++) {
            mOffset[i] = mValues[i] + mOffset[i];
          }
          mOffsetOnFull = false;
        }
        
        // write to buffer and find highest value/direction
        mHighestValue = 0;
        for(int i=0; i<12; i++) {
          float abs_val;
          val = mValues[i];
          write_buf(val);
          
          abs_val = val < 0 ? -val : val;
          // abs(..) casts to integer ????
          
          if ( abs_val > mHighestValue ) {
            mHighestDirection = i;
            mHighestValue = abs_val;
          }

        }
        
        
        mState = 0;
        mIndex = 0;
      }
    }
  }
  
  
  bool   mDebug;
  bool   mNewData;
  
  double mValues[12];
  double mOffset[12];
  double mHighestValue;
  int mHighestDirection;
  int mIndex;
  int mState;
  int mHigh;
  bool mOffsetOnFull;
  bool mSlowPort;
};

extern "C" void init()
{
  CLASS (Cabox)
  OUTLET(Cabox,stream)
  OUTLET(Cabox,high)
  OUTLET(Cabox,direction)
  METHOD(Cabox,offset)
  METHOD(Cabox,debug)
  SUPER_METHOD(Cabox, Script, set)
  SUPER_METHOD(Cabox, Script, load)
  SUPER_METHOD(Cabox, Script, script)
}