/** 
 * @file    : console-client.cpp
 * @author  : musahl
 * @date    : 15 Jul 2020
 * @version : 0.1
 * @brief   : Short description
 */

#include <SNClient.h>
#include <SNProtocol.h>
#include <SNUtils.h>
#include <asio.hpp>
#include <iostream>
#include <iomanip>
#include <array>
#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // must be included
#include "spdlog/fmt/bin_to_hex.h"


#define UARTBAUDRATE "1000000"
#define SNCLI_VERSION "0.3.0"
#define SNCLI_HASH "# "

bool running = true;

void signalHandler(int signum)
{
  spdlog::critical("Interrupt signal ({0:d}) received",signum);
  // cleanup and close up stuff here
  running = false;
  // terminate program
  exit(signum);
}

void newPrompt(bool newline = true)
{
  if (newline)
    std::cout << std::endl;
  std::cout << SNCLI_HASH;
  std::flush(std::cout);
}

class SNCLIistener : public SNClientListener
{
  void onMessageReceived(const uint8_t *data, const uint_fast16_t length)
  {
    std::array<char, 1024> buf;
    std::memcpy (buf.data(),data,length);

    spdlog::info("received raw bytes ({0:d}):\nt:\n{1}\nb:{2:X}",length,data,spdlog::to_hex(std::begin(buf), std::begin(buf) + length));

    newPrompt();
    eventreceived = true;
  }
  void onSensorData(SNSensorDataSet *sensorData)
  {
    eventreceived = true;
  }
  void onEventReceived(std::shared_ptr<SNProtocolEvent> event)
  {
    //spdlog::info("received {}",*event.get());

    //std::cout << "received event" << std::endl;
    if (event->type == SNProtocolEvent::Type::Detect)
    {
      for (auto const sensorInfo : referenceClient->getSensors())
      {
        std::cout << *sensorInfo << std::endl;
        //spdlog::info("received {}",*sensorInfo);
      }
    }
    if (event->type == SNProtocolEvent::Type::DataFrame)
    {
      SNSensorDataSet *sensors = event->getSensorDataSet();

      struct less_than_key
      {
          inline bool operator() (const std::shared_ptr<SNSensorData> sensor1, const std::shared_ptr<SNSensorData> sensor2)
          {
              return (sensor1->getID() < sensor2->getID());
          }
      };
 
      std::sort(sensors->begin(), sensors->end(), less_than_key());
 
      for (auto sensorData : *sensors)
      {
        switch (sensorData->getType())
        {
        case SNSensorTypes::s_imu:
          std::cout << *((SNSensorData_IMU*)sensorData.get()) << std::endl;
          //spdlog::info("received {}",*((SNSensorData_IMU*)sensorData.get()));

//          spdlog::get("console")->info("user defined type: {}", *((SNSensorData_IMU*)sensorData.get()));
          break;
        default:
          //std::cout << *sensorData << std::endl;
          break;
        }
      }
    }
    eventreceived = true;
  }

public:

  bool eventreceived = false;
  SNSensorDataSet *sensorData;
  std::string *keyThatChanged;
  SNClient *referenceClient;
};

std::string getCommands()
{
  return "commands: \n\r"
         "> {message}     (sends {message} to hub) \n\r"
         "detect          (triggers detection of sensors) \n\r"
         "state           (returns clientState) \n\r"
         "start           (starts measuring) \n\r"
         "stop            (stops measuring) \n\r"
         "eeprom_write    (write eeprom - {sensorID} {datatowrite}\n\r"
         "eeprom_read     (read eeprom - {sensorID}\n\r"
         "test_eeprom_rw  (testing eeprom flash)\n\r"
         "q               (quit) \n\r";
}

int main(int argc, char **argv)
try
{
  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);
//
//  auto console = spdlog::stdout_color_mt("console");
//  auto err_logger = spdlog::stderr_color_mt("stderr");

  std::string device;
  bool rawdata;
  int baudrate;

  cxxopts::Options options("snc-client-cli", "a simple cli implementation that can be used to test the BIONIC CDC-ACM protocol");
  options.add_options()
		  ("help", "Print help")
		  ("d,device", "serial device to use for communicating", cxxopts::value<std::string>()->default_value("/dev/ttyACM0"))
		  ("r,rawdata", "print raw data to console", cxxopts::value<bool>()->default_value("false"))
		  ("b,baudrate", "sets baudrate on the serial", cxxopts::value<int>()->default_value(UARTBAUDRATE));
  auto result = options.parse(argc, argv);
  if (result.count("help"))
  {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  rawdata = result["rawdata"].as<bool>();
  device = result["device"].as<std::string>();
  baudrate = result["baudrate"].as<int>();

  std::cout << "== sn-client-cli - V" << SNCLI_VERSION << " ==" << std::endl;
  std::cout << "build: " << __DATE__ << " - " << __TIME__ << std::endl;
  std::cout << "using: " << device << " (" << baudrate << ")" << std::endl;

  if (rawdata)
    std::cout << "cli will receive raw data" << std::endl;

  SNClient snCLient(device, baudrate);
  SNCLIistener snCLIListener;
  snCLIListener.willReceiveRawData = rawdata;
  snCLIListener.referenceClient = &snCLient;
  snCLient.attachListener(snCLIListener);

  //only for DFKI networks
  snCLient.setDFKIProtocol(false);

  //main loop -> handle messages
  std::string command;
  newPrompt();
  while (std::getline(std::cin, command) && running)
  {

    if (command == "q" || command == "quit" || command == "exit")
      break;

    if (command.rfind("> ", 0) == 0)
      snCLient.sendRawMessage(reinterpret_cast<const uint8_t *>(command.substr(2).c_str()), command.length());
    else if (command == "h" || command == "help")
    {
      std::cout << getCommands() << std::endl;
    }
    else if (command.rfind("detect", 0) == 0)
      snCLient.detectSensors();
    else if (command.rfind("state", 0) == 0)
      snCLient.getstate();
    else if (command.rfind("start", 0) == 0)
      snCLient.startMeasuring();
    else if (command.rfind("eeprom", 0) == 0){
      int pos=0;
      bool should_write = false;;
      std::string address="";
      std::string buffertowrite="";
      for(auto word :SNUtils::wordsFromString(command))
      {
        switch(pos){
          case 0:
            if(word=="eeprom_write")
              should_write=true;
            break;
          case 1:
            if(word.length() > 4)
            {
              std::cout << "wrong ID format, need 4characte wide HEX representation with leading zeros!!! " << word << std::endl;
              break;
            }
            else{
              address = std::string(4 - word.length(), '0') + word;
            }
            if(should_write == false)
              std::cout << "reading eeprom from " << address << std::endl;
            break;
          case 2:
            if(should_write == true)
            {
              int offset= command.find(word);
              std::cout << "offset " << offset << std::endl;
              buffertowrite = command.substr(offset);
              std::cout << "writing " <<  buffertowrite.length() << " bytes to " << address << ":" << std::endl << buffertowrite << std::endl;
            }
            break;
          default:
            //ignore all other words
            continue;
            break;
        }
        pos++;
      }
      if(should_write)
        snCLient.writeEEPROMData(address,reinterpret_cast<const uint8_t *>(buffertowrite.c_str()),buffertowrite.length());
      else {
        bool sensor_found=false;
        for(auto sensor: snCLient.getSensors()) {
          std::string paddedID= std::string(4 - sensor->getID().length(), '0') + sensor->getID();
          if(paddedID == address){
            std::cout << address << "found: " << std::endl << *sensor << std::endl;
            sensor_found=true;
            snCLient.requestEEPROMData(address);
          }
        }
        if(!sensor_found)
          std::cout << "eeprom not found" << std::endl;
      }
    }
    else if (command.rfind("stop", 0) == 0)
      snCLient.stopMeasuring();
    else if (command.rfind("sensors", 0) == 0)
    {
      for (const std::shared_ptr<SNSensorInformation> sensorInfo : snCLient.getSensors())
      {
        std::cout << *sensorInfo.get() << std::endl;
      }
    }
    else if ( command.rfind( "test_eeprom_rw", 0 ) == 0 )
    {

      snCLient.detectSensors();
      sleep(3);

      #define NUMBEROFCHARS 60
      uint8_t test_data[ NUMBEROFCHARS ];

      for ( uint8_t i = 0; i < NUMBEROFCHARS; ++i )
        test_data[i] = 'a';

      snCLient.stopMeasuring();

      std::string sensor_id = "0840";
      bool eeprom_write_status = snCLient.writeEEPROMData( sensor_id, test_data, NUMBEROFCHARS );
      
      if ( eeprom_write_status )
      {
        std::cout << "EEPROM write request sent successfully..." << std::endl;
        sleep(3);
        
        bool eeprom_read_req_status = snCLient.requestEEPROMData( sensor_id );
        sleep(3);
        
        // if( eeprom_read_req_status )
        // {
        //   std::cout << "EEPROM read request sent successfully..." << std::endl;
        //   sleep(3);

          for ( std::shared_ptr<SNSensorInformation> sensor_info :  snCLient.getSensors() ) 
          {
            
            std::cout << " Sensor ID : " << sensor_info -> getID() << " , sensor data : " << sensor_info -> getEEPROMData() << std::endl;
            sleep(3);
            if( sensor_info -> getID().compare( sensor_id ) == 0 ) 
            {

              std::cout << "EEPROM data in sensor " << sensor_id << " is : ";

              std::string fetched_data = sensor_info -> getEEPROMData();

              for ( uint16_t i = 0; i < fetched_data.length(); ++i )
              {
              
                std::cout << fetched_data[i];
                // if( test_data[i] != fetched_data[i] ) 
                // {
                //   std::cout << " Data differ at position " << i << std::endl;
                //   std::cout << " data flashed : " << test_data[i] << '\t' << "data fetched : " << fetched_data[i+5] << std::endl;
                //   break;
                // }    
              }
              break;
            }
          }
        // }
        // else
        // {
        //   std::cout << "Error reading data!!" << std::endl;
        // }
      }
    }

    newPrompt(false);
  }
  //only for DFKI networks
  snCLient.setDFKIProtocol(true);
  std::cout << "goodbye" << std::endl;

  exit(0);
}
catch (const std::exception &ex)
{
  std::cerr << ex.what() << '\n';
}
