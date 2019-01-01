#include "DTH_I2CKeypad.h"

#define KEYPAD_ADDRESS    0x20
#define ROWS 4
#define COLS 4

const char keyMap_4_3[4][3] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
const char keyMaps_4_4[4][4] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
DTH_I2CKeypad keys(&keyMaps_4_4[0][0], ROWS, COLS);

void setup() {
  // put your setup code here, to run once:
  int error;
  Serial.begin(115200);
  Serial.println("Test I2C Keypad by Duy Huynh");  
  Wire.begin();
  Wire.beginTransmission(KEYPAD_ADDRESS);
  error = Wire.endTransmission(); 
  if (error == 0) 
  {
    Serial.println("I2C keypad found.");
    keys.begin(KEYPAD_ADDRESS);   
  } 
  else 
  {
    Serial.println("I2C keypad not found.");
  }   
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keys.readKeyPressed();
  if(key != '\0')
  {
    Serial.println(key);
  }
}
