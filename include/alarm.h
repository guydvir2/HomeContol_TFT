#include <Arduino.h>

char keypad_pressed_chrs[10];
const char *AlarmCodes[] = {"131075", "180582", "030107"};

void clear_keypad()
{
  strcpy(keypad_pressed_chrs, "");
}
void add_char_kaypad(const char *a)
{
  strcat(keypad_pressed_chrs, a);
}
bool send_keypad_Code(const char *codes[], uint8_t n)
{
  for (uint8_t i = 0; i < n; i++)
  {
    if (strcmp(keypad_pressed_chrs, codes[i]) == 0)
    {
      // title.tft_entity.face_color = ILI9341_GREEN;
      return 1;
    }
  }
  return 0;
}
