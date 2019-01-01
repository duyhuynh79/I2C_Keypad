/*
 *  I2C keypad 4x4 library.
 *
 *  Copyright (C) 2018 Duy Huynh
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Version 0.1
 *  Author: Duy Huynh
 *  Email: duytel@gmail.com
 */
#ifndef DTH_I2C_KEYPAD_H_
#define DTH_I2C_KEYPAD_H_

/*------------------------------------------------------------------*
 *                           Includes                               *
 *------------------------------------------------------------------*/ 
#include "Arduino.h"
#include <Wire.h>

/*------------------------------------------------------------------*
 *                          Define macro                            *
 *------------------------------------------------------------------*/
/* define pin mapping */
#define ROWS  4
#define COLS  4
#define MAX_LIST (ROWS * COLS)
#define KEYPAD_HOLD_TIME 600 /* milisecond */
#define KEYPAD_COUNT_CHANGED 5 

#define ROW0  0  
#define ROW1  1  
#define ROW2  2  
#define ROW3  3 

#define COL0  4  
#define COL1  5  
#define COL2  6  
#define COL3  7 

#define COL_MASK 0xF0
#define ROW_MASK 0x0F
#define COL_MASK_BIT 0x10
#define ROW_MASK_BIT 0x01

typedef enum _dth_keyState {
	IDLE = 0,
	PRESSED,
	HOLD,
	RELEASED
}KeyState;
 
/*------------------------------------------------------------------*
 *                        Class declaration                         *
 *------------------------------------------------------------------*/
class DTH_I2CKeypad {
	
	public:
		DTH_I2CKeypad(const char *keyMaps, int rows, int cols);		
		DTH_I2CKeypad(const char *keyMaps, int rows, int cols, uint8_t address);
		~DTH_I2CKeypad();		
		char readKey();
		char readKeyPressed();
		char readKeyBlocking();
		uint8_t keyIsPressing(char keyChar);
		void begin(uint8_t address);		
		void setHoldTime(uint32_t time);		
		void installEventListener(void (*listener)(char key, KeyState state));
		uint8_t scanKey();

	private:
		void writeDataToPCF8574(uint8_t address, uint8_t data);
		int readDataFromPCF8574(uint8_t address);		
		void (*keyEventListener)(char key, KeyState state);
		
	protected:
		/*
		*  KEYPAD MAPPING   
		*  Pin 0 - 3 is row. Pin 4 -7 is column
		*        |P4|P5|P6|P7|   
		*        |--|--|--|--|
		*  col   |1 |2 |3 |4 | 
		*  ------|--|--|--|--|----
		*  row 1 |1 |2 |3 |A |P0 | 
		*  row 2 |4 |5 |6 |B |P1 |
		*  row 3 |7 |8 |9 |C |P2 |
		*  row 4 |* |0 |# |D |P3 |
		*  -----------------------
		*/
		char *_keyMaps;
		struct Key
		{    
			char keyChar;
			KeyState state;
			uint8_t keyChanged;	
			uint8_t count;
			uint32_t updated_time;			
		};
		Key keyList[MAX_LIST];		
		/* I2C address of PCF5784 */
		uint8_t _i2c_addr;		
		uint32_t _hold_time;		
		int _max_rows;
		int _max_cols;
};

#endif   //DTH_I2C_KEYPAD_H_
