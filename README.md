This is a simple arduino based "smart" charger.
It will drive and report statut of 3x TP4056 boards, with built-in protection if you can.

Additionnaly it will manage a current budget, i.e. make sure the total current consumption does not exceed
the maximum (2A by default). Charging will be delayed until the requested current is available.

Components short list :
* Nx TP 4056  with protection circuit (o TP4056 + separate battery protection circuit)
* N* P Mosfet
* INA3221 ( or N* INA219)
* 1 arduino
* 1 ST7735 color screen
* 1x level shifter for the screen (5 entries needed)

+ resistors etc...

