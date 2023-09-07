# MAG Laboratory IoT Supervisor
An IoT supervisor board\
https://maglaboratory.github.io/IoT-Supervisor/

## Project Description
This is an IoT SBC supervisor for the MAG Laboratory IoT devices.

Included are a voltage comparator which automatically resets devices if the
input voltage drops below an acceptable voltage.  (Manually configured through
a resistor divider)

## Branch Description
This branch is for initial development.  The style of calling something "zero"
came from a style of anime calling their prequel series "zero."

## TODO
* Documentation
  * Voltage Divider Set-Point Documentation
  * Modbus registers
  * How-to (quickstart) guide
  * State machines

## Deployment
Is this section supposed to be about how this project was developed or how to
start developing for it?

The project started in 2019 after a lot of SBC's were found hung and needing
manual intervention.  Hopefully, after two years of development, this project
can automatically reset boards.

Development and builds can be done with the Keil C51 toolchain and Silicon Labs
Simplicity Studio v5.
Unfortunately, SDCC support and other (let me know which, pls) compilers that
may support the 8051 instruction set are not supported at the moment.

### Building
Building the project while utilizing the built-in program memory CRC requires
some manual intervention and two cycles of clicking the "build" button.
It is possible to build and use the supervisor without using the program memory
CRC.
The only caveat to this is that the supervisor will always report a program CRC
error.

When creating a build, please look in the `.m51` file for information regarding
the end of code memory.
The constant `C_FOUND_PROG_END` in the file `IoT Supervisor.c`
should be manually changed based on the information in the `.m51` file:
* Look for a section called `C O D E   M E M O R Y`.
* Under that section, look for a `**GAP**` before `?CO?KIRISAKI_CRC`.
* Copy the `START` address of the `**GAP**` to be the constant called
`C_FOUND_PROG_END` in `IoT Supervisor.c`.

![image](pics/found_prog_end.png)

## Help
Please feel free to leave a bug report or start development on your own branch.

Other forms of communication outside of github are acceptable.

## Version History
 * Initial Commit
   * Low Voltage Comparator
   * Startup delay
   * Modbus, I guess
 * v0.0
   * It now supports configuration memory in FLASH.

## License
GNU GPL v3

## Authors
Brandon Kirisaki
