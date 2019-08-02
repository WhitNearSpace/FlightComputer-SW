Connections to the mbed LPC1768
===============================

The mbed LPC1768 has 40 user-accessible pins, but not all of those are available when it is integrated into a Flight Computer. Some pins are connected via the PCB to parts of the Flight Computer (SD card interface, XBee radio, and LEDs on the external interface), a few pins are left without connections, and the majority of the pins are accessible to the user.

On the **standard** flight computer, user-accessible mbed pins are routed to sets of header sockets immediately adjacent to the breadboard.  Jumper wires can connect these to the breadboard or to an exterior set of header sockets. The exterior header sockets are linked to corresponding screw terminals on the edge of the PCB.  These screw terminals provide a convenient and secure way to make connections to external components.

On the **mini** Flight Computer, user-accessible mbed pins are routed directly to screw terminals.

..  list-table:: Pins routed to Flight Computer components
    :header-rows: 1
    :widths: auto

    * - Pin
      - Function
      - Connection
      - Shared with user?
    * - p1
      - ground (0 V)
      - GND
      - Y
    * - p2
      - VIN
      - LiPo battery
      - N
    * - p5
      - SPI MOSI
      - SD card interface
      - Y
    * - p6
      - SPI MISO
      - SD card interface
      - Y
    * - p7
      - SPI SCK
      - SD card interface
      - Y
    * - p8
      - GPIO
      - SD card (chip select)
      - N
    * - p21
      - DigitalOut
      - external interface (LED A)
      - N
    * - p22
      - DigitalOut
      - external interface (LED B)
      - N
    * - p23
      - DigitalOut
      - external interface (LED C)
      - N
    * - p26
      - DigitalOut
      - XBee radio reset
      - N
    * - p27
      - Serial Rx
      - XBee radio Tx
      - N
    * - p28
      - Serial Tx
      - XBee radio Rx
      - N
    * - p40
      - power (3.3 V)
      - VOUT
      - Y

..  list-table:: Pins reserved for user. All can be GPIO or the additional function(s) shown.
    :header-rows: 1
    :widths: auto

    * - Pin
      - Function
      - Block Label on Standard
    * - p9
      - Serial Tx or I2C SDA
      - I2C
    * - p10
      - Serial Rx or I2C SCL
      - I2C
    * - p11
      - SPI MOSI
      - no label
    * - p12
      - SPI MISO
      - no label
    * - p13
      - Serial Tx or SPI SCK
      - Serial
    * - p14
      - Serial Rx
      - Serial
    * - p15
      - AnalogIn
      - Analog
    * - p16
      - AnalogIn
      - Analog
    * - p17
      - AnalogIn
      - Analog
    * - p18
      - AnalogOut or AnalogIn
      - Analog
    * - p19
      - AnalogIn (no interrupts)
      - Analog
    * - p20
      - AnalogIn (no interrupts)
      - Analog
    * - p24
      - PwmOut
      - PWM
    * - p25
      - PwmOut
      - PWM
    * - p29
      - CAN
      - CAN
    * - p30
      - CAN
      - CAN




