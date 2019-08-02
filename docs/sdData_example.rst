Logging data to an SD card
==========================

In this example we will write some data from an analog input to the SD card. Knowing data but not when it was recorded is often meaningless so we will also include that. The microcontroller has a real-time clock (RTC) but it is reset when it loses power. This means we need to get the time into the mbed LPC1768. We do this be using an XBee radio module to communicate with the Command Module. The Command Module has a GPS receiver that allows it to set its time and then share that with Flight Computers that request clock data.

First, edit the :file:`platformio.ini` configuration file so the real-time operating system (RTOS) is enabled (as was done in the :doc:`sdLog_example` example). You also need to tell it to import libraries we created to support the Flight Computer to Command Module communication (both located on GitHub). That is done with the ``lib_deps`` flag as shown in the :file:`platformio.ini` file below. This is the standard configuration for programs using the Flight Computer.

.. code-block:: ini
  :caption: platform.ini
  :emphasize-lines: 8-9

  [env:lpc1768]
  platform = nxplpc
  board = lpc1768
  framework = mbed

  build_flags = -D PIO_FRAMEWORK_MBED_RTOS_PRESENT

  lib_deps = FC_to_CM= https://github.com/JohnMLarkin/FlightComputer-SW
             XBeeAPIParser= https://github.com/JohnMLarkin/XBeeAPIParser

The :file:`mbed_app.json` file is identical to that in the :doc:`sdLog_example` example:

.. code-block:: json
  :caption: mbed_app.json

  {
    "target_overrides": {
      "*": {
        "target.features_add": ["STORAGE"],
        "target.components_add": ["SD"]
      }
    }
  }

In this example we will be logging the voltage read on pin 20. The Mbed OS ``AnalogIn`` object reports that voltage as a fraction of the mbed's reference voltage (approximately 3.3 V), with 1.0 meaning it is equal to the reference voltage, 0.5 meaning it is half the reference voltage, and 0.0 meaning it is 0 V. We will assume in this example that the reference voltage is precisely 3.3 V, but that is actually just a typical value so you should measure your mbed's p40 output with a multimeter to determine what the reference voltage really is.

Another thing to note is the use of Mbed's ``Timer`` objects to keep track of time related to three tasks: the total time to writing data and ensuring that the data has actually been written to the SD card. The ``fprintf`` function does not force anything to be written to the SD card at that moment.  Instead, it is placed in a buffer that is then written to the card when one of several conditions is encountered: the buffer contains *enough* data, the ``fclose`` function is called, or the ``fflush`` function is called.  If power should fail for some reason before one of these conditions is reached, the data would never be written to the SD card.  Therefore, it is wise to force this to occur using the ``fflush`` function (periodically, at intermediate times) and then ``fclose`` when all writing is done.

.. code-block:: c++
  :caption: main.cpp

  #include <mbed.h>
  #include <FATFileSystem.h>
  #include <SDBlockDevice.h>
  #include <FC_to_CM.h>

  DigitalOut errorLight(p23);   // use one of the external interface LEDs to signal errors
  DigitalOut runLight(p22);  // use another LED to signal collecting data

  SDBlockDevice sd(p5, p6, p7, p8);
  FATFileSystem fs("fs");
  FC_to_CM radio(p28, p27); // flight computer to command module link

  AnalogIn my_sensor(p20);

  int main() {
    // Create three timers
    Timer data_timer; // Is it time to write data?
    Timer flush_timer; // Is it time to flush data to SD?
    Timer expt_timer;  // Is it time to stop?

    // Start with both LEDs off
    errorLight = 0;
    runLight = 0;

    // Tell the Command Module we want to set our clock
    radio.setResponseState(RESPONSE_CLOCK);
    while (!radio.checkClock()) { // wait for the clock to be set by the Command Module
      runLight = !runLight; // toggle the run LED while waiting
      wait(0.5);
    }
    runLight = 0;

    // Mount the SD card
    int errors;
    errors = fs.mount(&sd);
    if (errors) {
      errorLight = 1;
      return -1;
    }

    // Open the file on the SD
    FILE* fd = fopen("/fs/data.txt", "a");  // txt usually indicates tab-separated values
    if (fd == NULL) {
      errorLight = 1;                             
      return -1;
    }
    runLight = 1; // solid run LED = don't remove SD card!
    fprintf("Time\tVoltage\r\n"); // write header row (\t = tab)

    data_timer.start();
    flush_timer.start();
    expt_timer.start();

    while (expt_timer < 180) { // take data for 180 seconds
      if (data_timer > 10) { // write data every 10 seconds
        data_timer.reset(); // set back to 0
        fprintf("%s\t%0.2f\r\n", radio.getTimeFormattted(), 3.3*my_sensor);
      }
      if (flush_timer > 60) { // force SD update every 60 seconds
        flush_timer.reset();
        fflush(fd);
      }
    }
    fclose(fd);
    runLight = 0; // ok to remove SD card when LED off
    return 0;
  }


