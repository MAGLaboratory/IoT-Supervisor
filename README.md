# MAG Laboratory IoT Supervisor
An IoT supervisor board

## Project Description
This is an IoT SBC supervisor for the MAG Laboratory IoT devices.

Included are a voltage comparator which automatically resets devices if the
input voltage drops below an acceptable voltage.  (Manually configured through
a resistor divider)

## Branch Description
Somehow, this is a branch for separating out the modbus library... and then doing other work
on the library.

## TODO
* Separate out the modbus library usage and make it portable again
* Use a single buffer for RX and TX 

## Deployment
TODO

## Help
TODO

## Version History
 * Initial Commit
   * Low Voltage Comparator
   * Startup delay
   * Modbus, I guess

## License
TODO

## Authors
Brandon Kirisaki
