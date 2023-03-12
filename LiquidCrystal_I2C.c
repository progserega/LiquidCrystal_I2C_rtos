// Based on the work by DFRobot

#include "LiquidCrystal_I2C.h"

inline void lcd_write(LiquidCrystal_I2C_Data *data,uint8_t value) {
	_lcd_send(data,value, Rs);
}

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

void lcd_init(LiquidCrystal_I2C_Data *data, uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows, uint8_t fontsize)
{
  data->_Addr = lcd_Addr;
  data->_cols = lcd_cols;
  data->_rows = lcd_rows;
  data->_fontsize = fontsize;
  data->_oled = false;
  data->_backlightval = LCD_NOBACKLIGHT;
	data->_displayfunction = LCD_4BITMODE | LCD_1LINE;
  if (fontsize>8)
    data->_displayfunction |= LCD_5x10DOTS;
  else
    data->_displayfunction |= LCD_5x8DOTS;
	lcd_begin(data, lcd_cols, lcd_rows, fontsize);
}

void lcd_oled_on(LiquidCrystal_I2C_Data *data){
  data->_oled = true;
  lcd_begin(data,data->_cols, data->_rows, data->_fontsize);
}

void lcd_begin(LiquidCrystal_I2C_Data *data, uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		data->_displayfunction |= LCD_2LINE;
	}
	data->_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize > 8) && (lines == 1)) {
		data->_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending lcd_commands. Arduino can turn on way befer 4.5V so we'll wait 50
	usleep(50); 
  
	// Now we pull both RS and R/W low to begin lcd_commands
	_lcd_expanderWrite(data,data->_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	usleep(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	  // we start in 8bit mode, try to set 4 bit mode
   _lcd_write4bits(data,0x03 << 4);
   usleep(4500); // wait min 4.1ms
   
   // second try
   _lcd_write4bits(data,0x03 << 4);
   usleep(4500); // wait min 4.1ms
   
   // third go!
   _lcd_write4bits(data,0x03 << 4); 
   usleep(150);
   
   // finally, set to 4-bit interface
   _lcd_write4bits(data,0x02 << 4); 


	// set # lines, font size, etc.
	lcd_command(data,LCD_FUNCTIONSET | data->_displayfunction);  
	
	// turn the display on with no cursor or blinking default
	data->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcd_display(data);
	
	// clear it off
	lcd_clear(data);
	
	// Initialize to default text direction (for roman languages)
	data->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	lcd_command(data,LCD_ENTRYMODESET | data->_displaymode);
	
	lcd_home(data);
}

/********** high level lcd_commands, for the user! */
void lcd_clear(LiquidCrystal_I2C_Data *data){
	lcd_command(data,LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	usleep(2000);  // this lcd_command takes a long time!
  if (data->_oled) lcd_setCursor(data,0,0);
}

void lcd_home(LiquidCrystal_I2C_Data *data){
	lcd_command(data,LCD_RETURNHOME);  // set cursor position to zero
	usleep(2000);  // this lcd_command takes a long time!
}

void lcd_setCursor(LiquidCrystal_I2C_Data *data,uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > data->_numlines ) {
		row = data->_numlines-1;    // we count rows starting w/0
	}
	lcd_command(data,LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcd_noDisplay(LiquidCrystal_I2C_Data *data) {
	data->_displaycontrol &= ~LCD_DISPLAYON;
	lcd_command(data,LCD_DISPLAYCONTROL | data->_displaycontrol);
}
void lcd_display(LiquidCrystal_I2C_Data *data) {
	data->_displaycontrol |= LCD_DISPLAYON;
	lcd_command(data,LCD_DISPLAYCONTROL | data->_displaycontrol);
}

// Turns the underline cursor on/off
void lcd_noCursor(LiquidCrystal_I2C_Data *data) {
	data->_displaycontrol &= ~LCD_CURSORON;
	lcd_command(data,LCD_DISPLAYCONTROL | data->_displaycontrol);
}
void lcd_cursor(LiquidCrystal_I2C_Data *data) {
	data->_displaycontrol |= LCD_CURSORON;
	lcd_command(data,LCD_DISPLAYCONTROL | data->_displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_noBlink(LiquidCrystal_I2C_Data *data) {
	data->_displaycontrol &= ~LCD_BLINKON;
	lcd_command(data,LCD_DISPLAYCONTROL | data->_displaycontrol);
}
void lcd_blink(LiquidCrystal_I2C_Data *data) {
	data->_displaycontrol |= LCD_BLINKON;
	lcd_command(data,LCD_DISPLAYCONTROL | data->_displaycontrol);
}

// These lcd_commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(LiquidCrystal_I2C_Data *data) {
	lcd_command(data,LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void lcd_scrollDisplayRight(LiquidCrystal_I2C_Data *data) {
	lcd_command(data,LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(LiquidCrystal_I2C_Data *data) {
	data->_displaymode |= LCD_ENTRYLEFT;
	lcd_command(data,LCD_ENTRYMODESET | data->_displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(LiquidCrystal_I2C_Data *data) {
	data->_displaymode &= ~LCD_ENTRYLEFT;
	lcd_command(data,LCD_ENTRYMODESET | data->_displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(LiquidCrystal_I2C_Data *data) {
	data->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	lcd_command(data,LCD_ENTRYMODESET | data->_displaymode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(LiquidCrystal_I2C_Data *data) {
	data->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcd_command(data,LCD_ENTRYMODESET | data->_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar_from_uint8(LiquidCrystal_I2C_Data *data,uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	lcd_command(data,LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		lcd_write(data,charmap[i]);
	}
}

//createChar with PROGMEM input
void lcd_createChar_from_charmap(LiquidCrystal_I2C_Data *data,uint8_t location, const char *charmap) {
	location &= 0x7; // we only have 8 locations 0-7
	lcd_command(data,LCD_SETCGRAMADDR | (location << 3));
  /* FIXME
	for (int i=0; i<8; i++) {
	    	lcd_write(data,pgm_read_byte_near(charmap++));
	}*/
}

// Turn the (optional) backlight off/on
void lcd_noBacklight(LiquidCrystal_I2C_Data *data) {
	data->_backlightval=LCD_NOBACKLIGHT;
	_lcd_expanderWrite(data,0);
}

void lcd_backlight(LiquidCrystal_I2C_Data *data) {
	data->_backlightval=LCD_BACKLIGHT;
	_lcd_expanderWrite(data,0);
}



/*********** mid level lcd_commands, for sending data/cmds */
//mode: 1 - data, 0 - command
inline void lcd_command(LiquidCrystal_I2C_Data *data,uint8_t value) {
	_lcd_send(data, value, 0);
}


/************ low level data pushing lcd_commands **********/

// write either lcd_command or data
// 0
void _lcd_send(LiquidCrystal_I2C_Data *data,uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
  _lcd_write4bits(data,(highnib)|mode);
	_lcd_write4bits(data,(lownib)|mode); 
}

void _lcd_write4bits(LiquidCrystal_I2C_Data *data,uint8_t value) {
	_lcd_expanderWrite(data,value);
	_lcd_pulseEnable(data,value);
}

/*void lcd__lcd_expanderWrite(uint8_t _data){                                        
	Wire.beginTransmission(_Addr);
	printIIC((int)(_data) | _backlightval);
	Wire.endTransmission();   
}
*/
void _lcd_expanderWrite(LiquidCrystal_I2C_Data *data,uint8_t _data){
  I2C_SendByteByADDR(_data|data->_backlightval,data->_Addr);
}

void _lcd_pulseEnable(LiquidCrystal_I2C_Data *data,uint8_t _data){
	_lcd_expanderWrite(data,_data | En);	// En high
	usleep(1);		// enable pulse must be >450ns
	
	_lcd_expanderWrite(data,_data & ~En);	// En low
	usleep(50);		// lcd_commands need > 37us to settle
} 


// Alias functions
void lcd_cursor_on(LiquidCrystal_I2C_Data *data){
	lcd_cursor(data);
}

void lcd_cursor_off(LiquidCrystal_I2C_Data *data){
	lcd_noCursor(data);
}

void lcd_blink_on(LiquidCrystal_I2C_Data *data){
	lcd_blink(data);
}

void lcd_blink_off(LiquidCrystal_I2C_Data *data){
	lcd_noBlink(data);
}

void lcd_load_custom_character(LiquidCrystal_I2C_Data *data,uint8_t char_num, uint8_t *rows){
		lcd_createChar_from_uint8(data,char_num, rows);
}

void lcd_setBacklight(LiquidCrystal_I2C_Data *data,uint8_t new_val){
	if(new_val){
		lcd_backlight(data);		// turn backlight on
	}else{
		lcd_noBacklight(data);		// turn backlight off
	}
}

void lcd_print(LiquidCrystal_I2C_Data *data,const char *st){
  uint8_t i=0;
  while(st[i]!=0)
  {
    lcd_write(data,st[i]);
    i++;
  }
}


// unsupported API functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void lcd_off(LiquidCrystal_I2C_Data *data){}
void lcd_on(LiquidCrystal_I2C_Data *data){}
void lcd_setDelay (LiquidCrystal_I2C_Data *data,int cmdDelay,int charDelay) {}
uint8_t lcd_status(LiquidCrystal_I2C_Data *data){return 0;}
uint8_t lcd_keypad (LiquidCrystal_I2C_Data *data){return 0;}
uint8_t lcd_init_bargraph(LiquidCrystal_I2C_Data *data,uint8_t graphtype){return 0;}
void lcd_draw_horizontal_graph(LiquidCrystal_I2C_Data *data,uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void lcd_draw_vertical_graph(LiquidCrystal_I2C_Data *data,uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void lcd_setContrast(LiquidCrystal_I2C_Data *data,uint8_t new_val){}
#pragma GCC diagnostic pop
	
