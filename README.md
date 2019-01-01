# Arduino DTH_I2CKeypad library
I2C_Keypad is a library that provides keypads functionality for Arduino.
This library uses 3x4 or 4x4 matrix keypad and PCF8574 module.
## Functionality
The functions available in the library include
```c
begin(address)      // start I2C_Keypad with an address.
readKey()	          // read a key press. If you hold the key, it will return a key every 600 milliseconds
readKeyPressed()    // read a key press. It only returns a key in the pressing state. It does not return every 600 milliseconds if you hold the key.
readKeyBlocking()   // Repeat and wait for a key to be pressed.
keyIsPressing(key)  // Check if a key is pressed or not  
setHoldTime(time)   // set a hold time. The default value of hold time is 600 milliseconds.
installEventListener(keypadEventHandler)  // set a keypad event callback function.
scanKey()           // scan keypad
```
## Examples
The DTH_I2CKeypad directory contains the I2C-Keypad library and some example sketches
illustrating how the library can be used with various keypad sources:
- `keypad_event.ino`- example of using event handler.  
- `read_key.ino` - example of using read a key every 600 milliseconds.
- `read_key_pressed.ino`- example of using a read-only key press once.
