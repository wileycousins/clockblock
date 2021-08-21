# firmware

## setup

### Arduino (one time setup)
Make sure you have `Arduino.app` installed in the usual place.

You'll need a special Board for Arduino. Add `http://drazzy.com/package_drazzy.com_index.json` to the `Additional Boards Manager URLs: ` in the Preferences in Arduino.

You'll need to setup your Arduino Uno for `ArduinoISP`. Select `ArduinoISP` from the `File > Examples > ArduinoISP`. Upload this sketch to the Uno.

Now wire the AVR ISP board into the appropriate pins on the Uno. Also, very important! Put a 10µf capacitor between ground and reset. Make sure to put the ground of the capacitor in the ground (the short leg).

### .env

This is being tracked in git which is debatable, but we should have the same env at this point and nothing sensitive is in here. But this is how the .env was made just for reference:

```bash
# Add the Arduino avr stuff to your path
echo "export PATH=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin:\$PATH" >> .env

# make sure Arduino Uno is plugged in and setup for ArduinoISP
echo "export SERIAL_PORT=\$(ls /dev/cu.usb*)" >> .env
```



Follow dmg package install steps

## build

```bash
source .env
cd src
make
```

## flash

```bash
# make sure you env is all good to go
source ../.env
make test
# assuming the test comes back all good.
make flash
```