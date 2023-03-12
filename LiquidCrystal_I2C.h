//YWROBOT
#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

#include "i2c_user.h"
#include <unistd.h>
#include <inttypes.h>

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

// тактируемый бит (1 - запись полубайта, 0 - повтор полубайта. Т.е. полубайт посылается дважды, а экрон принимает полубайт при смене E-бита с 1 на 0)
#define En 0B00000100  // Enable bit
#define Rw 0B00000010  // Read/Write bit (1 - пишем в экран, 0 - читаем из экрана)
#define Rs 0B00000001  // Register select bit (1 - данные, 0 - команда управления экраном)

typedef struct 
{
  uint8_t _Addr;
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numlines;
  uint8_t _fontsize;
  bool _oled;
  uint8_t _cols;
  uint8_t _rows;
  uint8_t _backlightval;
} LiquidCrystal_I2C_Data;

void lcd_init(LiquidCrystal_I2C_Data *data, uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows, uint8_t fontsize);
void lcd_begin(LiquidCrystal_I2C_Data *data, uint8_t cols, uint8_t lines, uint8_t dotsize);
void lcd_clear(LiquidCrystal_I2C_Data *data);
void lcd_home(LiquidCrystal_I2C_Data *data);
void lcd_noDisplay(LiquidCrystal_I2C_Data *data);
void lcd_display(LiquidCrystal_I2C_Data *data);
void lcd_noBlink(LiquidCrystal_I2C_Data *data);
void lcd_blink(LiquidCrystal_I2C_Data *data);
void lcd_noCursor(LiquidCrystal_I2C_Data *data);
void lcd_cursor(LiquidCrystal_I2C_Data *data);
void lcd_scrollDisplayLeft(LiquidCrystal_I2C_Data *data);
void lcd_scrollDisplayRight(LiquidCrystal_I2C_Data *data);
void lcd_printLeft(LiquidCrystal_I2C_Data *data);
void lcd_printRight(LiquidCrystal_I2C_Data *data);
void lcd_leftToRight(LiquidCrystal_I2C_Data *data);
void lcd_rightToLeft(LiquidCrystal_I2C_Data *data);
void lcd_shiftIncrement(LiquidCrystal_I2C_Data *data);
void lcd_shiftDecrement(LiquidCrystal_I2C_Data *data);
void lcd_noBacklight(LiquidCrystal_I2C_Data *data);
void lcd_backlight(LiquidCrystal_I2C_Data *data);
void lcd_autoscroll(LiquidCrystal_I2C_Data *data);
void lcd_noAutoscroll(LiquidCrystal_I2C_Data *data); 
void lcd_createChar_from_uint8(LiquidCrystal_I2C_Data *data,uint8_t, uint8_t[]);
void lcd_createChar_from_charmap(LiquidCrystal_I2C_Data *data,uint8_t location, const char *charmap);
// Example: 	const char bell[8] PROGMEM = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};

void lcd_setCursor(LiquidCrystal_I2C_Data *data,uint8_t, uint8_t); 
void lcd_write(LiquidCrystal_I2C_Data *data,uint8_t);
void lcd_command(LiquidCrystal_I2C_Data *data,uint8_t);
void lcd_oled_init(LiquidCrystal_I2C_Data *data);

////compatibility API function aliases
void lcd_blink_on(LiquidCrystal_I2C_Data *data);						// alias for blink(LiquidCrystal_I2C_Data *data)
void lcd_blink_off(LiquidCrystal_I2C_Data *data);       					// alias for noBlink(LiquidCrystal_I2C_Data *data)
void lcd_cursor_on(LiquidCrystal_I2C_Data *data);      	 					// alias for cursor(LiquidCrystal_I2C_Data *data)
void lcd_cursor_off(LiquidCrystal_I2C_Data *data);      					// alias for noCursor(LiquidCrystal_I2C_Data *data)
void lcd_setBacklight(LiquidCrystal_I2C_Data *data,uint8_t new_val);				// alias for backlight(LiquidCrystal_I2C_Data *data) and nobacklight(LiquidCrystal_I2C_Data *data)
void lcd_load_custom_character(LiquidCrystal_I2C_Data *data,uint8_t char_num, uint8_t *rows);	// alias for createChar(LiquidCrystal_I2C_Data *data)
void lcd_print(LiquidCrystal_I2C_Data *data,const char[]);

void _lcd_init_priv(LiquidCrystal_I2C_Data *data);
void _lcd_send(LiquidCrystal_I2C_Data *data,uint8_t, uint8_t);
void _lcd_write4bits(LiquidCrystal_I2C_Data *data,uint8_t);
void _lcd_expanderWrite(LiquidCrystal_I2C_Data *data,uint8_t);
void _lcd_pulseEnable(LiquidCrystal_I2C_Data *data,uint8_t);


#endif
