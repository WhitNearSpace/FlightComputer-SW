#include "FC_to_CM.h"

FC_to_CM::FC_to_CM(PinName tx, PinName rx) : _xbee(tx, rx), _rx_thread(osPriorityAboveNormal, 1000,NULL,"our_rx_thread"), _check_for_invitation(osPriorityBelowNormal, 500,NULL,"invitation_thread") {
  _timeout = 100;
  _invitation_timeout = 180;                                      //3 minutes
  _rx_thread.start(callback(this, &FC_to_CM::_listen_for_rx));    //start rx thread to read in serial communication from xbee modem
  _check_for_invitation.start(callback(this, &FC_to_CM::_wait_for_invitation));
  _flightState = FLIGHT_STATE_LABMODE;
  _rsvpState = RESPONSE_DECLINE;
  _dataTransmitSize = 0;
  _partialDataIndex = 0;
  _readyToSendData = false;
  _goodClock = false;
  _dataInterval = 0;

  _timeOfLaunch = 0;

  set_time(0);

  printf("> rx thread used space: %d\r\n", int(_rx_thread.used_stack()));
  printf("> invitation used space: %d\r\n", int(_check_for_invitation.used_stack()));
}

void FC_to_CM::_wait_for_invitation() {
  _invitation_timer.start();
  while(true) {
    if (_xbee.associated()) {
      _invitation_timer.stop();
      //stop thread
    }
    if (_invitation_timer.read() > _invitation_timeout) {
      //dissociate
      _invitation_timer.reset();  
    }
  }
}

void FC_to_CM::_listen_for_rx() {
  int len;
  char msg[MAX_MSG_LENGTH];
  uint64_t sender = 0;
  _xbee.set_frame_alert_thread_id(osThreadGetId());
  while (true) {
    osSignalWait(0x01,osWaitForever);
    if (_xbee.readable()) {
      len = _xbee.rxPacket(msg, &sender);
      if (len > 0) {
        switch (msg[0]) {
          case 0x00:  // Invitation to connect
            _process_invitation(sender);    //send 0x10 responce
            break;
          case 0x01:  // primed for launch
            _flightState = FLIGHT_STATE_PRELAUNCH;
            _dataInterval = msg[1];
            break;
          case 0x02:  // launch detected          //flight state 0x03-descent not yet implemented
            _timeOfLaunch = time(NULL);           //set the time of launch
            _flightState = FLIGHT_STATE_FLIGHT;
            break;
          case 0x03:  // landing confirmed
            _flightState = FLIGHT_STATE_LANDED;
            break;
          case 0x20: // set clock
            _process_clock_set(msg);
            break;
          case 0x21: // test clock
            _process_clock_test(msg, sender);
            break;
          case 0x40: // request data
            _process_request_data(sender);
            break;
          default:
            // Nothing should fall into this category
            printf("Error! Unexpected rx msg code %0X\r\n", msg[0]);
        }
      }
    }
  }
}
 
void FC_to_CM::_process_invitation(uint64_t address) {    //sends 0x10 trasmission to confirm invitation, and responce (decline, clock only, clock and data)
  char msg[2];
  msg[0] = 0x10;
  msg[1] = _rsvpState;
  _xbee.txAddressed(address, msg, 2);
  printf("I've got an invitation!\r\n");
}

void FC_to_CM::_process_clock_set(char* timeBytes) {
  time_t t = 0;
  for (int i = 0; i < 4; i++){            //convert 4 bytes into time_t type
    t = (t<<8) | timeBytes[i+1];
  }
  set_time(t);                    //set mbed clock to UNIX time
  printf("my clock has been set!\r\n");
}

void FC_to_CM::_process_clock_test(char* timeBytes, uint64_t address) {
  char msg[2];
  msg[0] = 0x31;
  time_t t = 0;
  for (int i = 0; i < 4; i++){            //convert 4 bytes into time_t type
    t = (t<<8) | timeBytes[i+1];
  }
  time_t now;
  now = time(NULL);
  int deltaT = now - t;
  if ((deltaT >= 1) || (deltaT <= -1)) {                  //compare local time to CM time
    _goodClock = 0;
    msg[1] = 0x01;  //fail
  } else {
    _goodClock = 1;
    msg[1] = 0x00;  //pass
  }
  _xbee.txAddressed(address, msg, 2);
  printf("my clock has been tested! any errors?: %d\r\n", msg[1]);
}


void FC_to_CM::_process_request_data( uint64_t address){
  if (_readyToSendData) {                                     //check to see if data set is ready to be transmitted
    if(_data_mutex.trylock_for(_timeout)){                    //lockout, so transfer data cannot be called and change data durring transmission
      //_readyToSendData = false;                               //clear flag -- this causes the FC to only send unique data, and never send the same data twice
      char msg[MAX_MSG_LENGTH];                               //create message
      msg[0] = 0x50;                                          //sending data is code 0x50
      for (int i = 0; i < _dataTransmitSize; i++) {           //load complete data set into message
        msg[i + 1] = _fullDataSet[i];
      }
    _xbee.txAddressed(address, msg, _dataTransmitSize + 1);   //send msg
      _data_mutex.unlock();                                   //unlock mutex
    } 
  } else {                                                    //if data is not ready
    char msg[1] = {0x50};
    _xbee.txAddressed(address, msg, 1);                       //send empty data packet
  }
  printf("data has been requested of me!\r\n");
}

void FC_to_CM::_transferPartialData() {                       //moves data in partial data to full data when ready to transmit
  if (_data_mutex.trylock_for(_timeout)) {                    //lockout
    for (int i = 0; i < _dataTransmitSize; i++) {
      _fullDataSet[i] = _partialDataSet[i];
    }
    _readyToSendData = true;
    _partialDataIndex = 0;
    _data_mutex.unlock();
  }
}
/*
void FC_to_CM::saveInt(int val) {
  _addBytesToData(val);
}
*/
void FC_to_CM::saveUInt8(uint8_t val) { _addBytesToData(val); }
void FC_to_CM::saveInt8(int8_t val) { _addBytesToData(val); }
void FC_to_CM::saveUInt16(uint16_t val) { _addBytesToData(val); }
void FC_to_CM::saveInt16(int16_t val) { _addBytesToData(val); }
void FC_to_CM::saveUInt32(uint32_t val) { _addBytesToData(val); }
void FC_to_CM::saveInt32(int32_t val) { _addBytesToData(val); }
void FC_to_CM::saveFloat(float val) { _addBytesToData(val); }
void FC_to_CM::saveFloatAsInt16(float val, int precision) {
  float newVal = val;
  for (int i = 0; i < precision; i++){                      //multiply val by 10^(precision)
    newVal = newVal * 10;
  }
  int16_t shortVal = int16_t(newVal);
  _addBytesToData(shortVal);
}
void FC_to_CM::saveDouble(double val) { _addBytesToData(val); }

template<typename T>
void FC_to_CM::_addBytesToData(T value) {
  int size = sizeof(T);
  printf("Size: %d Stored: %d\r\n", size, _partialDataIndex);
  if ((size + _partialDataIndex) <= _dataTransmitSize) {       //check to see if room is availible in data set
    unsigned char bytes[16];                       //create array of bytes (max size is 16 bytes)

    memcpy(bytes, &value, size);                     //copy value's bytes into array

    for (int i = 0; i < size; i++) {
      _partialDataSet[_partialDataIndex++] = bytes[i];    //add bytes to partial data
      printf("0x%X ", bytes[i]);
    } 
    printf("\r\n");
  }
  else {
    printf("data could not fit in storage\r\n");              //failed, not enough room
    
  }
  if (_partialDataIndex == _dataTransmitSize) {               //once enough data has been added, transfer partial data to full data
    _transferPartialData();
    printf("Data ready for transmission.\r\n");
  }
}


time_t FC_to_CM::getTime() {
  time_t currentTime = time(NULL);
  return currentTime;
}

time_t FC_to_CM::getTimeSinceLaunch() {
  if (_timeOfLaunch == 0) {
    return 0;
  } else { 
    time_t currentTime = time(NULL);
    return currentTime - _timeOfLaunch;
  }
}

void FC_to_CM::getTimeFormatted(char* t_string, int len) {
  time_t rawTime;
  struct tm* timeinfo;

  time(&rawTime);
  timeinfo = gmtime(&rawTime);
  
  strftime(t_string,len,"%D %R",timeinfo);
}
