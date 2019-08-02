Communicating with attached computer
====================================

When you are developing code for the Flight Computer it is often useful to send diagnostic messages to a computer attached to the Flight Computer via a USB cable. In this short example, a ``Serial`` object called ``pc`` is created that is attached to the built-in USB transmit and receive lines. By default this will communicate at 9600 baud (as expected by PlatformIO's monitor).

The standard C function `printf` is used to send information to the computer. More details about the options for the format string can be found at `C++ references <http://www.cplusplus.com/reference/cstdio/printf/>`__.

.. code-block:: c++
  :caption: main.cpp

  #include <mbed.h>

  Serial pc(USBTX, USBRX);

  int main() {
    pc.printf("Hello, world!\r\n");  // the special code \r\n moves to the next line

    int value = 10;
    pc.printf("The value is %i\r\n", value);  // % codes indicate where values should be inserted

    float var1 = 22.2;
    int var2 = 0xFF;

    pc.printf("Multiple values can be in the same statement: %f, %X\r\n", var1, var2);
  }

After compiling (|check icon|) and downloading (|-> icon|) the program to the mbed, you can observe the output on the computer using PlatformIO's monitor (launched by clicking on |plug icon|).

.. |check icon| image:: https://api.iconify.design/octicon:check.svg?color=blue&height=16
.. |-> icon| image:: https://api.iconify.design/octicon:arrow-right.svg?color=blue&height=16
.. |plug icon| image:: https://api.iconify.design/octicon:plug.svg?color=blue&height=16

