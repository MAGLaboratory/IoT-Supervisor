Testing Procedures
=======
This document contains the testing procedures.

Interrupts
==========
1. Verify that the IoT supervisor starts into pin `nRESET` state `PULL-DOWN` to verify that the Vin / Reset state machine runs.

Configuration
=============
This section contains the longest part of the test procedure.

The default modbus communication settings should be something like the
following:

```
import minimalmodbus

instr = minimalmodbus.Instrument("/dev/ttyUSB0", 255)
instr.address = 255
instr.serial.baudrate = 38400
instr.serial.timeout = 0.5
```

Address and CRC Instructions
----------------------------
The instrument stated with address `255`.  This is the default for any modbus
device entering a network in an unconfigured state.

1. Verify that register `0`, `1` read `[194, 0]` in order to verify the 
starting state of the system. 
1. Write `0xDEFA` into register 1 to enter the `cache` state.
1. Verify that register `1` reads `2` to verify that the configuration state machine has entered into the `cache` state.
1. Write SID `1` into register `2`.  There is no need to write a new baud rate
because the register accepts individual bytes and or's them.
1. Write `0x5FAF` into register `1` to enter the `commit` state.
1. Write `0xDEFA` into register `1` to enter the flash `erase` and subsequent
`write` state.  The controller should reboot and recalculate the flash CRC.
1. Verify that register `0`, `1` read `[2, 0]` in order to verify the starting
state of the system.

Generic Baud Instructions
---------
1. Verify that register `1` reads `0` to verify that the configuration state machine is in the `idle` state.
1. Write `0xDEFA` into register 1 to enter the `cache` state.
1. Verify that register `1` reads `2` to verify that the configuration state machine has entered into the `cache` state.
1. Write the desired baud and SID setting into register `2`.
    * Baud rates are always left shifted by eight in register `2`.
Preshifted baud rates with shifts are listed in the table below.
    * Addresses are unshifted.
    * The register setting can be calculated by adding (or or'ing) the shifted
baud rate with the address.
1. Verify that register `2` reads the setting that was entered.
1. Write `0x5FAF` into register `1` to enter the `commit` state.
The baud rate settings at this point should have changed;
the testing software should reflect these changes.
1. Change the testing software to the new baud settings.
The timeout should also change since that also depends on the baud rate.
A good timeout that may work with all baud rates is 500 ms.
    * An example would be as follows:\
`instr.serial.baudrate = 38400`\
`instr.serial.timeout = 0.5`\
`instr.address = 1`
1. Write `0` to register `2` to go back to the configuration SM `idle` state.

| Baud  | Index | Shifted Index |
| ----- | ----- |-------------- |
| 38400 | 1     |  256 |
| 19200 | 2     |  512 |
|  9600 | 3     |  768 |
|  4800 | 4     | 1024 |
|  2400 | 5     | 1280 |
|  1200 | 6     | 1536 |

Specific Baud Instructions
---------
All of the baud rates in the table above should be iterated through.

1. 38400 (default)
2. 19200
3. 9600
4. 4800
5. 2400
6. 1200


Middleware, Main
================
If all baud states were tested in the previous section, the middleware should
be successfully tested.

Modbus Timer
============
The modbus timer can be tested through the modbus interface.
The default timeout is `15` minutes.
This test activates the timer, pets it `7.5` minutes (or halfway into the
cycle) and lets it time out.

1. Write `0x5A` to pet the watchdog and start the watchdog timer.
2. Wait 7.5 minutes.
3. Write `0x5A` to pet the watchdog and reset the watchdog timer.
4. Wait until the watchdog resets in 15 minutes.


Voltage In Comparator
=====================
This test requires a variable voltage power supply and an understanding of the
set point from the resistor divider values.
