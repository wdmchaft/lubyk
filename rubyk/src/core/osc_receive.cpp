#include "osc_receive.h"
#include "osc_send.h"
#include "planet.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscPacketListener.h"
#include "ip/UdpSocket.h"

OscReceive::OscReceive(Planet * pPlanet, uint pPort) : mPlanet(pPlanet)
{ 
  mSocket = new UdpListeningReceiveSocket( IpEndpointName( IpEndpointName::ANY_ADDRESS, pPort ), this );
  
  std::cout << "Listening for OSC messages on port " << pPort << ".\n";
  mListenThreadId = mPlanet->root()->new_thread<OscReceive, &OscReceive::start_thread>(this);
}
  
OscReceive::~OscReceive()
{
  mSocket->AsynchronousBreak();
  pthread_join(mListenThreadId, NULL);  // wait
  delete mSocket;
}
  
void OscReceive::start_thread()
{
  // Run until break.
  mSocket->Run();
}
  
void OscReceive::ProcessMessage( const osc::ReceivedMessage& pMsg, const IpEndpointName& remoteEndpoint )
{
  std::string url(pMsg.AddressPattern());
  Value res, params = value_from_osc(pMsg);
  
  std::cout << "\n>> " << url << " " << params << "\n> ";
  
  if (url == "/reply_to") {
    std::cout << "registering satellite\n";
    mPlanet->register_satellite(new OscSend(remoteEndpoint, params));
  } else
    mPlanet->receive(url, params);
  
  // TODO send return value
}

Value OscReceive::value_from_osc(const osc::ReceivedMessage& pMsg)
{
  const char * type = pMsg.TypeTags();
  
  osc::ReceivedMessage::const_iterator arg = pMsg.ArgumentsBegin();
  osc::ReceivedMessage::const_iterator end = pMsg.ArgumentsEnd();
  
  // TODO: receive multiple values and put them in Array.
  if (arg == end) return gNilValue;
  
  switch (*type) {
  case osc::TRUE_TYPE_TAG:
    return Number(1.0);
  case osc::FALSE_TYPE_TAG:
    return Number(0.0);
  case osc::NIL_TYPE_TAG:
    return gNilValue;
  case osc::INFINITUM_TYPE_TAG:
    return gNilValue; // not implemented...
  // zero length
  break;

  case osc::INT32_TYPE_TAG:
    return Number((arg++)->AsInt32Unchecked());
  case osc::FLOAT_TYPE_TAG:
    return Number((arg++)->AsFloatUnchecked());
  case osc::CHAR_TYPE_TAG:
    return Number((real_t)(arg++)->AsCharUnchecked());
  case osc::RGBA_COLOR_TYPE_TAG:
    return Error("Unsupported format.");
  case osc::MIDI_MESSAGE_TYPE_TAG:
    return Error("Unsupported format.");
  case osc::INT64_TYPE_TAG:
    return Error("Unsupported format.");
  case osc::TIME_TAG_TYPE_TAG:
    return Error("Unsupported format.");
  case osc::DOUBLE_TYPE_TAG:
    return Number((arg++)->AsDoubleUnchecked());
  case osc::STRING_TYPE_TAG:
    return String((arg++)->AsStringUnchecked());
  case osc::SYMBOL_TYPE_TAG:
    return Error("Symbol OSC type not supported.");
  case osc::BLOB_TYPE_TAG:
    return Error("BLOB OSC type not supported.");
  default:
    return Error("Unknown OSC type tag.");
  }
}
