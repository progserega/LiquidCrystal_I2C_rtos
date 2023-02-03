#include "main.h"
//------------------------------------------------
static const char *TAG = "main";
//------------------------------------------------
typedef struct
{
  unsigned char y_pos;
  unsigned char x_pos;
  char *str;
} qLCDData;
//------------------------------------------------
xQueueHandle lcd_string_queue = NULL;
struct LiquidCrystal_I2C_Data lcd;
//------------------------------------------------
void vLCDTask(void* arg)
{
  BaseType_t xStatus;
  qLCDData xReceivedData;
  for(;;) {
    xStatus = xQueueReceive(lcd_string_queue, &xReceivedData, 10000 /portTICK_RATE_MS);
    if (xStatus == pdPASS)
    {
      //LCD_SetPos(xReceivedData.x_pos,xReceivedData.y_pos);
      //LCD_String(xReceivedData.str);
      lcd_setCursor(&lcd,xReceivedData.x_pos,xReceivedData.y_pos);
      lcd_print(&lcd,xReceivedData.str);

      ESP_LOGI("vLCDTask", "set to position %d,%d string: %s", xReceivedData.x_pos, xReceivedData.y_pos, xReceivedData.str);
    }
  }
}
//------------------------------------------------
void app_main(void)
{
  uint16_t i=0;
  char str01[10];
  esp_err_t ret;
  qLCDData xLCDData;
  lcd_string_queue = xQueueCreate(10, sizeof(qLCDData));
  xTaskCreate(vLCDTask, "vLCDTask", 2048, NULL, 2, NULL);
  ret = i2c_ini();
  ESP_LOGI(TAG, "i2c_ini: %d", ret);
  lcd_init(&lcd,126,16,2,8);  // set the LCD address to 0x27 for a 16 chars and 2 line display, 8 - small font, 10 - big font
  ESP_LOGI(TAG, "set backlight = on");
  lcd_setBacklight(&lcd,1);

  //LCD_ini(126); // 0x4E - для 4-х строчного дисплея LCD2004A, 126 - для LCD1602A (младшие три бита адреса можно задать перемычками A0,A1,A2 на i2c плате - по умолчанию они подтянуты к 1б но можно замкнуть на землю)
  vTaskDelay(100 / portTICK_PERIOD_MS);
  xLCDData.str = str01;
  for(i=0;i<2;i++){
    xLCDData.x_pos = i*3;
    xLCDData.y_pos = i;
    sprintf(str01,"String %d",i+1);
    xQueueSendToBack(lcd_string_queue, &xLCDData, 0);
  }
  vTaskDelay(2000 / portTICK_PERIOD_MS);
  //lcd_clear(&lcd);
  xLCDData.x_pos = 9;
  xLCDData.y_pos = 1;
  xLCDData.str = "        ";
  xQueueSendToBack(lcd_string_queue, &xLCDData, 0);
  i = 0;
  xLCDData.str = str01;
  while (1) {
    i++;
    if(i>65534) i=0;
    // мигаем подсветкой:
    if(i%2)
      lcd_setBacklight(&lcd,0);
    else
      lcd_setBacklight(&lcd,1);
    sprintf(str01,"%5d",i);
    xLCDData.x_pos = 11;
    xLCDData.y_pos = 1;
    xQueueSendToBack(lcd_string_queue, &xLCDData, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
//------------------------------------------------
