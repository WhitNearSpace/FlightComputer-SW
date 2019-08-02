Transmitting data to the Command Module
=======================================

In this example we will some data from an analog input to the Command Module. The Command Module is in charge of deciding when it wants the data, so we will keep updating and the Flight Computer will just transmit the most recent value when the request is received.

First, edit the :file:`platformio.ini` configuration file so the real-time operating system (RTOS) is enabled and it knows to import libraries from GitHub that we created to support the Flight Computer to Command Module communication. This was described in the :doc:`transmitData_example` example.

No :file:`mbed_app.json` file is required in this example because the SD card will not be used.

Before you can successfully transmit data to the Command Module, you will need to configure it so it knows to expect transmissions from this Flight Computer and what data will be coming. This configuration is done using the Launch Control app that is part of the High Altitude Balloon Open Source Collection.

.. DANGER:: A link to the Launch Control documentation is needed.

In this example we will be sending a single ``float`` value (a non-integer number).  In C++, a float is stored in 4 bytes. Look for that number in the example code.

.. code-block:: c++
  :caption: main.cpp

  #include <mbed.h>
  #include <FC_to_CM.h>

  DigitalOut clockLight(p23);   // use one of the external interface LEDs to signal waiting for clock
  DigitalOut runLight(p22);  // use another LED to signal collecting data

  FC_to_CM radio(p28, p27); // flight computer to command module link

  AnalogIn my_sensor(p20);

  int main() {
    Timer data_timer; // Is it time to queue data for transmission?

    // Start with both LEDs off
    clockLight = 0;
    runLight = 0;

    // Tell the Command Module we will send data (and request clock)
    radio.setResponseState(RESPONSE_DATA);
    radio.setDataTransmitSize(4); // float = 4 bytes
    while (!radio.checkClock()) { // wait for the clock to be set by the Command Module
      clockLight = !clockLight; // toggle the clock LED while waiting
      wait(0.5);
    }
    runLight = 0;

    data_timer.start();
    expt_timer.start();

    runLight = 1;
    while (expt_timer < 300) { // transmit data for 300 seconds
      if (data_timer > 5) { // write data to the queue every 5 seconds
        data_timer.reset(); // set back to 0
        radio.saveFloat(3.3*my_sensor);
      }
    }
    runLight = 0; // all done
    return 0;
  }

