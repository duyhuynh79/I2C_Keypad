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

#include "DTH_I2CKeypad.h"


DTH_I2CKeypad::DTH_I2CKeypad(const char *keyMaps, int rows, int cols)
{	 
	int i, z, idx = 0;	

	_keyMaps = keyMaps;	
	_max_rows = rows;
	_max_cols = cols;	
	_hold_time = KEYPAD_HOLD_TIME;
	/* Clear all data */
	for(i = 0; i < MAX_LIST; i++)
	{
		keyList[i].keyChar = '\0';
		keyList[i].state = IDLE;
		keyList[i].keyChanged = 0;
	}
	/* Initialize key list */
	for(i = 0; i < _max_rows; i++)
	{
		for(z = 0; z < _max_cols; z++)
		{      
			idx = (i*_max_cols)+ z; 
			keyList[idx].keyChar = _keyMaps[idx];         
		}
	}
}

DTH_I2CKeypad::DTH_I2CKeypad(const char *keyMaps, int rows, int cols, uint8_t addr)
{  
	int i, z, idx = 0;  

	_keyMaps = keyMaps; 
	_max_rows = rows;
	_max_cols = cols;	
	_hold_time = KEYPAD_HOLD_TIME;
	/* Clear all data */
	for(i = 0; i < MAX_LIST; i++)
	{
		keyList[i].keyChar = '\0';
		keyList[i].state = IDLE;
		keyList[i].keyChanged = 0;
	}
	/* Initialize key map */
	for(i = 0; i < _max_rows; i++)
	{
		for(z = 0; z < _max_cols; z++)
		{      
			idx = (i*_max_cols)+z; 
			keyList[idx].keyChar = _keyMaps[idx];       
		}
	}
	begin(addr);
}
DTH_I2CKeypad::~DTH_I2CKeypad()
{
  
}

void DTH_I2CKeypad::begin(uint8_t address)
{
	_i2c_addr = address;  
	/* All PCF8574 ports high */
	writeDataToPCF8574(_i2c_addr, 0xff);	
}

uint8_t DTH_I2CKeypad::scanKey()
{
	int tmp_data;
	int row = 0, col = 0;
	uint8_t row_mask_bit, col_mask_bit;
	uint8_t flag = 0;


	row_mask_bit = ROW_MASK_BIT;
	for( row = 0; row < _max_rows; row++)
	{
		/* Send data with low level for row select to PCF5748 */		
		writeDataToPCF8574(_i2c_addr, ~row_mask_bit | COL_MASK);
		delayMicroseconds(10);
		tmp_data = readDataFromPCF8574(_i2c_addr);    
		col_mask_bit = COL_MASK_BIT;
		for(col = 0; col < _max_cols; col++)
		{			
			if((tmp_data & col_mask_bit) == 0)
			{
				/* Press buttom */				
				if(keyList[(row * _max_cols) + col].state == IDLE)
				{
					keyList[(row * _max_cols) + col].count++;
					if(keyList[(row * _max_cols) + col].count >= KEYPAD_COUNT_CHANGED)
					{
						keyList[(row * _max_cols) + col].count = 0;
						keyList[(row * _max_cols) + col].keyChanged = 1;
						keyList[(row * _max_cols) + col].state = PRESSED;
						keyList[(row * _max_cols) + col].updated_time = millis();
						if(keyEventListener != NULL)
							keyEventListener(keyList[(row * _max_cols) + col].keyChar, PRESSED);
					}
				}
				else if(keyList[(row * _max_cols) + col].state == PRESSED)
				{				 	
					if((millis() - _hold_time) >= keyList[(row * _max_cols) + col].updated_time )
					{
						keyList[(row * _max_cols) + col].keyChanged = 1;
						keyList[(row * _max_cols) + col].state = HOLD;   
						if(keyEventListener != NULL)
							keyEventListener(keyList[(row * _max_cols) + col].keyChar, HOLD);
						keyList[(row * _max_cols) + col].updated_time = millis();
					}         
				}	
				else if(keyList[(row * _max_cols) + col].state == HOLD)
				{				 	
					if((millis() - _hold_time) >= keyList[(row * _max_cols) + col].updated_time )
					{
						keyList[(row * _max_cols) + col].keyChanged = 1;						   						
						keyList[(row * _max_cols) + col].updated_time = millis();
					}         
				}	
				flag = 1;
			}
			else
			{
				/* Release button */
				if((keyList[(row * _max_cols) + col].state == PRESSED) || (keyList[(row * _max_cols) + col].state == HOLD))
				{
					keyList[(row * _max_cols) + col].count++;
					if(keyList[(row * _max_cols) + col].count >= KEYPAD_COUNT_CHANGED)
					{
						keyList[(row * _max_cols) + col].count = 0;
						keyList[(row * _max_cols) + col].keyChanged = 1;
						keyList[(row * _max_cols) + col].state = RELEASED; 
						if(keyEventListener != NULL)
							keyEventListener(keyList[(row * _max_cols) + col].keyChar, RELEASED);
					}
				}
				else if ( keyList[(row * _max_cols) + col].state == RELEASED)
				{					
					keyList[(row * _max_cols) + col].keyChanged = 0;
					keyList[(row * _max_cols) + col].state = IDLE;  
					if(keyEventListener != NULL)
						keyEventListener(keyList[(row * _max_cols) + col].keyChar, IDLE);
				} 				
			} 
			col_mask_bit = (col_mask_bit << 1);  
		}
		row_mask_bit = (row_mask_bit << 1);
	}
done:  
	writeDataToPCF8574(_i2c_addr, 0xff);
	return flag;
}

char DTH_I2CKeypad::readKey()
{
	char key = '\0';
	int i;
	
	if(scanKey() != 0)
	{
		for(i = 0; i < MAX_LIST; i++)
		{
			if(((keyList[i].state == PRESSED) || (keyList[i].state == HOLD)) && (keyList[i].keyChanged != 0))
			{
				key = keyList[i].keyChar;	
				keyList[i].keyChanged = 0;
				break;
			}
		} 
	}		
	return  key;
}

char DTH_I2CKeypad::readKeyPressed()
{
	char key = '\0';
	int i;
	
	if(scanKey() != 0)
	{
		for(i = 0; i < MAX_LIST; i++)
		{
			if((keyList[i].state == PRESSED) && (keyList[i].keyChanged != 0))
			{
				key = keyList[i].keyChar;	
				keyList[i].keyChanged = 0;
				break;
			}
		} 
	}		
	return  key;
}

char DTH_I2CKeypad::readKeyBlocking()
{
	char key = '\0';

	do {
		key = readKeyPressed();
	}while(key == '\0');
	return key;
}

void DTH_I2CKeypad::setHoldTime(uint32_t time)
{
	_hold_time = time;
}

uint8_t DTH_I2CKeypad::keyIsPressing(char keyChar)
{
	int i;
	for(i = 0; i < MAX_LIST; i++)
	{
		if(((keyList[i].state == PRESSED) || (keyList[i].state == HOLD)) && (keyList[i].keyChar == keyChar))
		{
			return 1;
		}
	}
	return 0;
}

void DTH_I2CKeypad::installEventListener(void (*listener)(char key, KeyState state)){
	keyEventListener = listener;
}

void DTH_I2CKeypad::writeDataToPCF8574(uint8_t address, uint8_t data)
{
	Wire.beginTransmission(address);
	Wire.write(data);
	Wire.endTransmission();
}

int DTH_I2CKeypad::readDataFromPCF8574(uint8_t address)
{
	Wire.requestFrom(address, (uint8_t)1);
	return Wire.read();
}

