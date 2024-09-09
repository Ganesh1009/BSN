
#ifndef SRC_SNClientTransportMockup_H_
#define SRC_SNClientTransportMockup_H_

#include <asio.hpp>


class SNClientTransportMockup : public SNClientTransport {

  public:

  SNClientTransportMockup(std::string welcomemessage = "bionic V0.0.6\nhardware haptic_sensor_node\ntag upper\nserialnr 0\n\n"){
    setNextResponse(reinterpret_cast<const uint8_t*>(welcomemessage.c_str()), welcomemessage.length(),true);
  }

  bool sendMessage(const uint8_t *data, const uint_fast32_t length) {
    //i'm just sending what was previously set as fake data
    notifyListeners(asio::buffer_cast<const uint8_t *>(faked_response.data()), static_cast<const uint32_t>(faked_response.size()));
    faked_response.consume(length);
    return true;
  }
  bool setNextResponse(const uint8_t *data, const uint_fast32_t length, bool directLoopBack=false){
    std::ostream output(&faked_response);
    output.write(reinterpret_cast<const char*>(data), static_cast<const uint32_t>(length));
    faked_response.commit(length);
    if(directLoopBack)
      sendMessage(data,length);
    return true;
  }
 private:
  asio::streambuf faked_response;

};

#endif