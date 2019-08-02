Powering the Flight Computer
============================

During development, the Flight Computer can be powered through a USB cable attached to the mbed LPC1768. This USB connection can also be used for communication with the mbed during program testing even if the battery is being used to power the Flight Computer.

During flight, the Flight Computer is powered through the attached LiPo battery. The mbed LPC1768 has an integrated voltage regulator which will take the LiPo voltage (8.4 V to 6.5 V depending on its charge state) and convert this to 3.3 V. This powers the mbed and is also available to power other components. Each mbed pin can source or sink a maximum of 40 mA and the total that can be drawn from the mbed is 400 mA.
If more current is required, or a different voltage, additional batteries will be needed.

Controlling the Battery
-----------------------

The battery is connected to the switch on the external interface. This switch has three settings.

* Off |--| the battery is disconnected (switch in center position)
* Power |--| the battery is connected to the mbed's VIN (p2), powering the mbed and providing 3.3 V at p40
* Charge |--| the battery is connected to the charging adaptor and can be recharged

Charging the Battery
--------------------

To charge the LiPo battery, connect the charging adaptor to a **7.4 V** LiPo charger (0.5 A capacity). Flip the switch on the external interface to the charging position. Wait for the charger to indicate that it is charging successfully (no error lights).

Always monitor a charging LiPo battery! If the battery looks like it has experienced physical trauma (for example, swelling) or becomes abnormally, immediately stop charging. Do not charge the battery while it is resting on a flammable surface. A LiPo battery fire is hazardous.

Once the battery is finished charing, move the external interface switch to the off position (the middle) and disconnect the charging adaptor.

.. |--| unicode:: U+2013   .. en dash
.. |---| unicode:: U+2014  .. em dash, trimming surrounding whitespace
   :trim:

