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
#define DEBUG 1

// Rubyk wii::Remote
#include "wii/Remote.h"

#include "rubyk/cocoa.h"

// WiiRemoteFramework WiiRemote
#import "WiiRemote.h"

/* ======================== LWiiRemoteDelegate ===================================== */

/*

@interface NSObject( WiiRemoteDelegate )

- (void) irPointMovedX:(float)px Y:(float)py wiiRemote:(WiiRemote*)wiiRemote;
- (void) rawIRData: (IRData[4])irData wiiRemote:(WiiRemote*)wiiRemote;
- (void) buttonChanged:(WiiButtonType)type isPressed:(BOOL)isPressed wiiRemote:(WiiRemote*)wiiRemote;
- (void) accelerationChanged:(WiiAccelerationSensorType)type accX:(unsigned char)accX accY:(unsigned char)accY accZ:(unsigned char)accZ wiiRemote:(WiiRemote*)wiiRemote;
- (void) joyStickChanged:(WiiJoyStickType)type tiltX:(unsigned char)tiltX tiltY:(unsigned char)tiltY wiiRemote:(WiiRemote*)wiiRemote;
- (void) analogButtonChanged:(WiiButtonType)type amount:(unsigned)press wiiRemote:(WiiRemote*)wiiRemote;
- (void) wiiRemoteDisconnected:(IOBluetoothDevice*)device;


//- (void) dataChanged:(unsigned short)buttonData accX:(unsigned char)accX accY:(unsigned char)accY accZ:(unsigned char)accZ mouseX:(float)mx mouseY:(float)my;


@end

*/
@interface LWiiRemoteDelegate : NSObject {
  wii::Remote *master_;
}

- (id) initWithRemote:(wii::Remote*)master;

//////// WiiRemoteDelegate ///////////
- (void) accelerationChanged:(WiiAccelerationSensorType)type accX:(unsigned char)accX accY:(unsigned char)accY accZ:(unsigned char)accZ wiiRemote:(WiiRemote*)wiiRemote;
- (void) buttonChanged:(WiiButtonType)type isPressed:(BOOL)isPressed wiiRemote:(WiiRemote*)wiiRemote;
@end

/* ======================== LWiiRemoteDelegate @implementation ===================== */

static const char *button_name_from_type(WiiButtonType type) {
  switch (type) {
  case WiiRemoteAButton:
    return "Remote.A";
  case WiiRemoteBButton:
    return "Remote.B";
  case WiiRemoteOneButton:
    return "Remote.1";
  case WiiRemoteTwoButton:
    return "Remote.2";
  case WiiRemoteMinusButton:
    return "Remote.-";
  case WiiRemoteHomeButton:
    return "Remote.H";
  case WiiRemotePlusButton:
    return "Remote.+";
  case WiiRemoteUpButton:
    return "Remote.U";
  case WiiRemoteDownButton:
    return "Remote.D";
  case WiiRemoteLeftButton:
    return "Remote.L";
  case WiiRemoteRightButton:
    return "Remote.R";

  case WiiNunchukZButton:
    return "Nunchuk.Z";
  case WiiNunchukCButton:
    return "Nunchuk.C";

  case WiiClassicControllerXButton:
    return "Classic.X";
  case WiiClassicControllerYButton:
    return "Classic.Y";
  case WiiClassicControllerAButton:
    return "Classic.A";
  case WiiClassicControllerBButton:
    return "Classic.B";
  case WiiClassicControllerLButton:
    return "Classic.L";
  case WiiClassicControllerRButton:
    return "Classic.R";
  case WiiClassicControllerZLButton:
    return "Classic.ZL";
  case WiiClassicControllerZRButton:
    return "Classic.ZR";
  case WiiClassicControllerUpButton:
    return "Classic.U";
  case WiiClassicControllerDownButton:
    return "Classic.D";
  case WiiClassicControllerLeftButton:
    return "Classic.L";
  case WiiClassicControllerRightButton:
    return "Classic.R";
  case WiiClassicControllerMinusButton:
    return "Classic.-";
  case WiiClassicControllerHomeButton:
    return "Classic.H";
  case WiiClassicControllerPlusButton:
    return "Classic.+";
  default:
    return "???";
  }
}

@implementation LWiiRemoteDelegate
- (id) initWithRemote:(wii::Remote*)master {
  self = [super init];
  if (self) {
    master_ = master;
  }
  return self;
}

- (void) accelerationChanged:(WiiAccelerationSensorType)type accX:(unsigned char)x accY:(unsigned char)y accZ:(unsigned char)z wiiRemote:(WiiRemote*)wiiRemote {
  master_->acceleration(
    type == WiiRemoteAccelerationSensor ? "Remote" : "Nunchuk",
    (float)x / 0xff,
    (float)y / 0xff,
    (float)z / 0xff
  );
}

- (void) buttonChanged:(WiiButtonType)type isPressed:(BOOL)isPressed wiiRemote:(WiiRemote*)wiiRemote {
  master_->button(
    button_name_from_type(type),
    isPressed
  );
}

@end // LWiiRemoteDelegate


/* ======================== wii::Remote ======================== */

namespace wii {

class Remote::Implementation
{
  Remote *master_;
  WiiRemote *wii_remote_;
  LWiiRemoteDelegate *wii_remote_delegate_;
public:
  Implementation(Remote *master)
   : master_(master) {
    wii_remote_delegate_ = [[LWiiRemoteDelegate alloc] initWithRemote:master_];
    wii_remote_ = nil;
  }

  void set_remote(WiiRemote *remote) {
    if (wii_remote_ == remote) return;

  	if (wii_remote_) {
  		[wii_remote_ release];
      wii_remote_ = nil;
  	}

  	wii_remote_ = remote;
  	[wii_remote_ setDelegate:wii_remote_delegate_];
  }

  ~Implementation() {
    if (wii_remote_) {
      [wii_remote_ release];
      wii_remote_ = nil;
    }

    [wii_remote_delegate_ release];
  }
};

Remote::Remote(rubyk::Worker *worker, const char *remote_name)
 : acceleration_(worker),
   button_(worker) {
  ScopedPool pool;
  if (remote_name) name_ = remote_name;
  impl_ = new Implementation(this);
}

Remote::~Remote() {
  delete impl_;
}

void Remote::set_remote(void *remote) {
  impl_->set_remote((WiiRemote *)remote);
}

} // wii


// One compilation unit to cope with static _mii_data_buf.

/* ================================================================================================== BROWSER */

// Rubyk wii::Browser
#include "wii/Browser.h"
#include "wii/Remote.h"

#include "rubyk/cocoa.h"

// WiiRemoteFramework WiiRemoteDiscovery
#import "WiiRemoteDiscovery.h"

/* ======================== LWiiDiscoveryDelegate ===================================== */
@interface LWiiDiscoveryDelegate : NSObject {
  wii::Browser *master_;
  rubyk::Worker *worker_;
}

- (id) initWithBrowser:(wii::Browser*)master worker:(rubyk::Worker*) worker;

//////// WiiDiscoveryDelegate ///////////

- (void) WiiRemoteDiscovered:(WiiRemote*)wiimote;
- (void) WiiRemoteDiscoveryError:(int)code;
@end

/* ======================== LWiiDiscoveryDelegate @implementation ===================== */

@implementation LWiiDiscoveryDelegate

- (id) initWithBrowser:(wii::Browser*)master worker:(rubyk::Worker*) worker {
  self = [super init];

  if (self) {
    printf("Start discovery delegate !\n");
    master_ = master;
    worker_ = worker;
  }
  return self;
}

- (void) willStartWiimoteConnections {
  // do nothing
}

- (void) WiiRemoteDiscovered:(WiiRemote*)wiimote {
  printf("Found !\n");
  wii::Remote *remote = new wii::Remote(worker_, [[wiimote address] UTF8String]);
  remote->set_remote(remote);
  master_->found(remote);
}

- (void) WiiRemoteDiscoveryError:(int)code {
  printf("WiiRemoteDiscoveryError %i\n", code);
}

@end // LWiiDiscoveryDelegate

/* ======================== wii::Browser::Implementation ======================= */
namespace wii {

class Browser::Implementation
{
  Browser *master_;
  LWiiDiscoveryDelegate *wii_discovery_delegate_;
	WiiRemoteDiscovery *discovery_;
public:
  Implementation(Browser *master)
   : master_(master) {
    wii_discovery_delegate_ = [[LWiiDiscoveryDelegate alloc] initWithBrowser:master_ worker:master_->worker_];
    discovery_ =  [WiiRemoteDiscovery discoveryWithDelegate:wii_discovery_delegate_];
    [discovery_ retain];
    IOReturn ret = [discovery_ start];
    if (ret != kIOReturnSuccess) {
      printf("Error starting wii Browser.\n");
    }
  }

  ~Implementation() {
    [discovery_ stop];
  	[discovery_ release];
    [wii_discovery_delegate_ release];
  }
};

Browser::Browser(rubyk::Worker *worker)
 : LuaCallback(worker) {
  ScopedPool pool;
  // we need to run the browser in an NSThread
  // or something else that has an event loop so that we
  // get IO...
  // My idea here would be to create a thread with [NSApp run] in the worker
  // if we are on OS X...
  // http://lists.apple.com/archives/Bluetooth-dev/2006/Mar/msg00006.html
  impl_ = new Implementation(this);
}

Browser::~Browser() {
  delete impl_;
}

} // wii