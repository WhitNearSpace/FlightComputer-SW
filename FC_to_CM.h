#ifndef FC_TO_CM_H
#define FC_TO_CM_H

#include "XBeeAPIParser.h"
#include "mbed.h"
#include "rtos.h"
#include <string> 

#define MAX_FC 6
#define MAX_MSG_LENGTH 70
#define MAX_POD_DATA_BYTES 50

class FC_to_CM {
private:
  XBeeAPIParser _xbee;
  uint32_t _timeout;
  uint32_t _invitation_timeout;

  //RTOS management
  Mutex _data_mutex;
  Thread _rx_thread;
  Thread _check_for_invitation;                         //dissociates radio every _invitation_timeout seconds, terminates once connection to CM is made

  void _listen_for_rx();                                //listens for rx, then runs one of the appropriate responces below, may handle smaller tasks like switching flags 
  void _process_invitation(uint64_t address);           //if recieved 0x00, sends 0x10
  void _process_clock_set(char*);
  void _process_clock_test(char*, uint64_t address);    //if recieved 0x21, send 0x31 
  void _process_request_data(uint64_t address);         //if recieved 0x40, send 0x50

  void _wait_for_invitation();
  Timer _invitation_timer;

  char _flightState;                                    //stores the state of the flight from CM broadcasts--enum: preLaunch(0x00),ascent(0x01),descent(0x02),landed(0x03),labTest(0x10)
  char _dataInterval;                                   //in seconds
  bool _goodClock;
  char _rsvpState;

  uint8_t _dataTransmitSize;                            //size of data to be transmitted, set by user, deafults to 0
  char _partialDataSet[MAX_POD_DATA_BYTES];             //where data is stored while being collected
  uint8_t _partialDataIndex;                            //tracks index of partial data
  
  char _fullDataSet[MAX_POD_DATA_BYTES];                //completed data set, will be held until a more recent partial set is completes
  bool _readyToSendData;                                //tracks if there is data to send
  void _transferPartialData();                          //moves partail data into full data once the cartial set is completed

  template<typename T>
  void _addBytesToData(T value);                        //converts any data type to bytes, adds them to partial data

public:
  FC_to_CM(PinName tx, PinName rx);

  //set functions to change FC responce state, defaults to 0x00 (decline) in constructor
  void setResponseDeclineResponce() { _rsvpState = 0x00;}      
  void setResponseClockOnly()       { _rsvpState = 0x01;}
  void setResponseClockAndData()    { _rsvpState = 0x02;}
  
  //functions to set data transmit size, and save data by type. This must match with the order and quantity that is on LC
  void setDataTransmitSize(int size) { _dataTransmitSize = size; }
  void saveUInt8(uint8_t val);
  void saveInt8(int8_t val);
  void saveUInt16(uint16_t val);
  void saveInt16(int16_t val);
  void saveUInt32(uint32_t val);
  void saveInt32(int32_t val);
  //void saveInt(int val);
  void saveFloat(float val);
  void saveFloatAsInt(float val, int precision);
  void saveDouble(double val);

  bool checkClock() { return _goodClock; }
  char getFlightState() { return _flightState; }        
  int getDataInterval() { return _dataInterval; }

  time_t getTime();
  std::string getTimeFormatted();
  time_t getTimeSinceStatup();
};

#endif