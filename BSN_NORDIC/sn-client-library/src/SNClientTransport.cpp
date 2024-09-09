/** 
 * @file    : SNClientTransport.cpp
 * @author  : musahl
 * @date    : 5 Jul 2020
 * @version : 0.0
 * @brief   : Short description
 */

#include "SNClientTransport.h"
#include <algorithm>    // std::find

#include "Debug.h"

void SNClientTransport::attachListener(SNClientTransportListener &listener) {
  listeners.push_back(&listener);
  while(pendingMessages.size())
  {
    std::string message = pendingMessages.front();
    pendingMessages.pop();
    notifyListeners(reinterpret_cast<const uint8_t*>(message.c_str()), message.length());
  }
}


void SNClientTransport::detachListener(SNClientTransportListener &listener) {

  std::vector<SNClientTransportListener*>::iterator it;

  it = std::find(listeners.begin(), listeners.end(), &listener);

  if(it != listeners.end())
    listeners.erase(it);

}

bool SNClientTransport::notifyListeners(const uint8_t *data, const uint_fast32_t length) {
  //DEBUG() << " message << " << std::endl << data << std::endl;
  if (listeners.size() == 0) {
    DEBUG() << " no listener yet! saving message" << std::endl;
    std::string messageToSave(reinterpret_cast<char const*>(data), length);
    pendingMessages.push(messageToSave);
    return false;
  }
  for (SNClientTransportListener *const listener : listeners) {
    listener->onMessageReceived(data, length);
  }
  return true;
}
