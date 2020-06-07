# TTGO-T-Beam-GPS
TTGO T-Beam GPS example

## Howto
Flash it to your board using Arduino IDE (see my [TTGO-T-Beam-Blinking example](https://github.com/luckynrslevin/TTGO-T-Beam-Blinking) for details).

Connect to via serial console to your board, e.g. on mac you can use screen:
```
TERM=xterm screen -L -h 5000 /dev/tty.SLAB_USBtoUART  115200
```
Maybe you have to replace the tty device name in case your's is different.

## Output
Until a proper GPS signal is found you will see "Positioning" message with an increasing counter:
```
Positioning(1)
Positioning(2)
Positioning(3)
Positioning(4)
...
```
as soon as a valid GPS signal is available it will change to display the coordinates:

```
UTC:17:1:55
LNG:-73.9837
LAT:40.7405
satellites:4
...
UTC:17:2:20
LNG:-73.9893
LAT:40.7419
satellites:6
...
```
