Logging messages to an SD card
==============================

In this example we will write a log-type message to an SD card. Saving status or error messages to a file can be a useful tool for testing your program or understanding what might have taken place during a flight.

The Flight Computer contains an SD card interface. The SD card interface communicates with the mbed through a protocol called SPI.  The main part of the SPI communication uses pins 5, 6, and 7. The SPI protocol allows these pins to be connected to multiple devices. The particular device that should listen for the message from the mbed is indicated by a fourth connection, called chip or slave select. The Flight Computer is configured to use pin 8 for the chip select for the SD card interface.

In order to access the needed functions of the mbed we first need to tell the PlatformIO system a few things. First, we will instruct PlatformIO to include the real-time operating system (RTOS) portion of Mbed OS 5 (this step is not necessary if you are using the online compiler). To do this, open the :file:`platformio.ini`. This file is a configuration file that tells PlatformIO about the particular microcontroller that is being used as well as special features to include. You need to add the ``build_flags`` line highlighted below.

.. code-block:: ini
  :caption: platform.ini
  :emphasize-lines: 6

  [env:lpc1768]
  platform = nxplpc
  board = lpc1768
  framework = mbed

  build_flags = -D PIO_FRAMEWORK_MBED_RTOS_PRESENT

.. warning:: The default firmware on an mbed LPC1768 when it is received from the manufacturer may be out of date. We have found that this older firmware runs into trouble when you switch to using the RTOS. There is an error in the clock (it is off by a factor of 8) and this causes timing-sensitive features to fail.

  Updating the firmware is quick and easy.  Instructions can be found in the `Mbed Handbook <https://os.mbed.com/handbook/Firmware-LPC1768-LPC11U24>`_.

Next we need to create an :file:`mbed_app.json` file in the same directory as :file:`platformio.ini`. To do this, right click in the project folder below :file:`platformio.ini`, select :guilabel:`New file`, and then name it :file:`mbed_app.json`. This file is where configuration of specific hardware features can be set. To activate parts of the Mbed OS relevant to SD card storage type the following:

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

With those features activated, you can now use them in your code (in :file:`main.cpp` as usual).

.. code-block:: c++
  :caption: main.cpp

  #include <mbed.h>
  #include <FATFileSystem.h>
  #include <SDBlockDevice.h>

  DigitalOut errorLight(p23);   // use one of the external interface LEDs to signal errors

  SDBlockDevice sd(p5, p6, p7, p8);
  FATFileSystem fs("fs");
  Serial pc(USBTX,USBRX);

  int main() {
    errorLight = 0;  // no errors yet so turn off

    // try to mount the SD card
    int errors;  // holds error codes (if any)
    errors = fs.mount(&sd);  // connect fs to sd
    if (errors) {  // true (not zero) if errors occurred
      errorLight = 1;  // turn on error indicator LED
      pc.printf("The SD card failed to mount. Error code: %i", errors);
      return -1;   // exit the program indicating an error
    }

    // Open the file on the SD and get its file descriptor (link)
    FILE* fd = fopen("/fs/log.txt", "a");  // a = append (add to file if exists, create if not)
    if (fd == NULL) {  // file pointer will be NULL if not opened successfully
      errorLight = 1;                             
      return -1;
    }

    // write a message to the log.txt file
    // fprintf has the same format string options as printf
    fprintf(fd, "Success in writing to a file\r\n");   

    // close the file when done
    fclose(fd);
    return 0; // exit indicating success
  }

