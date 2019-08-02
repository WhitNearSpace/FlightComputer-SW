Controlling external interface LED
==================================

The three LEDs on the external interface are connected to pins 21-23. To use them, simply declare a ``DigitalOut`` object assigned to one of these pins, then change it to the desired state.

.. note:: If a ``DigitalOut`` for a particular LED is not declared and then assigned a value, the voltage on this pin will be unspecified (not zero as most beginners assume). This means uncontrolled LEDs on the external interface will, in general, not be off.  Instead, they will probably be dimly lit (somewhere between off and the brightness of fully on).

The following example will make LED 1 blink forever. This is easiest way to do it, but not necessarily the best! The Mbed OS provides timing objects (``Ticker`` and ``Timer``) that might be more appropriate in a larger program. Alternatively, the real-time operating system (RTOS) could be used to put the blinking in a separate thread so the main process can focus on more important tasks.

.. code-block:: c++
  :caption: main.cpp

  #include <mbed.h>

  DigitalOut ledA(p21);  // external interface LED pin

  int main() {
    while (true) {    // loops forever          
      ledA = 1;       // turns on the LED
      wait(0.5);      // waits half a second
      ledA = 0;       // turns off the LED
      wait(0.5);
    }
  }


