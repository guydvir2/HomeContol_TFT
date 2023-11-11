#include <Arduino.h>
#include <TFT_GUI.h>

/* Initialize TFT */
#define SCREEN_ROT 0
XPT2046_Touchscreen ts(TS_CS);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define numButtons_Main 4
#define numButtons_Alarm 3
#define numButtons_Lights 6
#define numButtons_Lights_Main 3
#define numButtons_Keypad 12
#define numButtons_Windows_Main 4
#define numButtons_Windows_Operate 3

LabelTFT title(tft);
buttonArrayTFT<numButtons_Keypad> genButtonArr(ts, tft);

bool display_clock = true;
char keypad_pressed_chrs[10];
uint8_t activeMenu = 0;
bool light_state_top[numButtons_Lights] = {0, 0, 0, 0, 0, 0};
bool light_state_ext[numButtons_Lights] = {0, 0, 0, 0, 0, 0};
bool light_state_ground[numButtons_Lights] = {0, 0, 0, 0, 0, 0};

unsigned long last_press_millis = 0;

enum menu : const uint8_t
{
  MAIN_MENU,
  ALARM_MAIN,
  ALARM_KEYPAD,
  LIGHTS_MAIN,
  LIGHTS_GROUND,
  LIGHTS_TOP,
  LIGHTS_EXT,
  WINDOWS_MAIN,
  WINDOWS_ALL,
  WINDOWS_GROUND,
  WINDOWS_TOP,
  WINDOWS_PERG,
  WINDOWS_OPER,
  SYSTEM_MAIM
};

void rebuild_screen(uint8_t i);
void external_cb(int i)
{
  Serial.print("CB: #");
  Serial.println(i);
}
void set_buttons_state()
{
  for (uint8_t i = 0; i < numButtons_Lights; i++)
  {
    // genButtonArr.set_pressed()
  }
}
void create_title()
{
  title.tft_entity.w = tft.width();
  title.tft_entity.h = 30;
  title.tft_entity.h_pos = 30 / 2;
  title.tft_entity.w_pos = tft.width() / 2;
  title.tft_entity.txt_size = 2;
  title.tft_entity.border_thickness = 4;
  title.tft_entity.border_color = ILI9341_RED;
  title.tft_entity.face_color = ILI9341_YELLOW;
  title.tft_entity.txt_color = ILI9341_BLACK;
  title.tft_entity.roundRect = false;
  title.tft_entity.useBorder = false;
  title.tft_entity.center_txt = true;

  title.createLabel("12:00");
}
void create_Main()
{
  TFT_entity tft_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  tft_entity.w = (tft.width() - genButtonArr.dw) / 2;                           /* When defined other than 0 , max screen's width's height will be taken */
  tft_entity.h = (tft.height() - title.tft_entity.h - 2 * genButtonArr.dh) / 2; /* When defined other than 0 , max screen's width's height will be taken */
  tft_entity.txt_size = 2;
  tft_entity.border_thickness = 2;
  tft_entity.border_color = ILI9341_RED;
  tft_entity.face_color = ILI9341_WHITE;
  tft_entity.txt_color = ILI9341_BLACK;
  tft_entity.pressface_color = ILI9341_CYAN;
  tft_entity.roundRect = false;
  tft_entity.useBorder = false;
  tft_entity.center_txt = true;
  tft_entity.latchButton = false;
  tft_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"Alarm", "Lights", "Windows", "System"};
  genButtonArr.set_button_properties(tft_entity);
  genButtonArr.create_array(2, 2, a);
}
void create_keypad()
{
  TFT_entity keypad_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  keypad_entity.w = (tft.width() - 2 * genButtonArr.dw) / 3;                       /* When defined other than 0 , max screen's width's height will be taken */
  keypad_entity.h = (tft.height() - 5 * genButtonArr.dh - title.tft_entity.h) / 4; /* When defined other than 0 , max screen's width's height will be taken */
  keypad_entity.txt_size = 2;
  keypad_entity.border_thickness = 2;
  keypad_entity.border_color = ILI9341_RED;
  keypad_entity.face_color = ILI9341_WHITE;
  keypad_entity.txt_color = ILI9341_BLACK;
  keypad_entity.pressface_color = ILI9341_CYAN;
  keypad_entity.roundRect = false;
  keypad_entity.useBorder = true;
  keypad_entity.center_txt = true;
  keypad_entity.latchButton = false;
  keypad_entity.corner_radius = 5;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"};
  genButtonArr.set_button_properties(keypad_entity);
  genButtonArr.create_array(4, 3, a);
}
void create_AlarmMenu()
{
  TFT_entity alarmMenu_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  alarmMenu_entity.w = tft.width();                                                   /* When defined other than 0 , max screen's width's height will be taken */
  alarmMenu_entity.h = (tft.height() - title.tft_entity.h - 3 * genButtonArr.dh) / 3; /* When defined other than 0 , max screen's width's height will be taken */
  alarmMenu_entity.txt_size = 2;
  alarmMenu_entity.border_thickness = 2;
  alarmMenu_entity.border_color = ILI9341_RED;
  alarmMenu_entity.face_color = ILI9341_WHITE;
  alarmMenu_entity.txt_color = ILI9341_BLACK;
  alarmMenu_entity.pressface_color = ILI9341_CYAN;
  alarmMenu_entity.roundRect = false;
  alarmMenu_entity.useBorder = false;
  alarmMenu_entity.center_txt = true;
  alarmMenu_entity.latchButton = false;
  alarmMenu_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"Arm Home", "Arm Away", "Disarm"};
  genButtonArr.set_button_properties(alarmMenu_entity);
  genButtonArr.create_array(3, 1, a);
}
void create_Lights_int_g()
{
  TFT_entity Light_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  Light_entity.w = (tft.width() - genButtonArr.dw) / 2;                           /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.h = (tft.height() - title.tft_entity.h - 3 * genButtonArr.dh) / 3; /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.txt_size = 2;
  Light_entity.border_thickness = 2;
  Light_entity.border_color = ILI9341_RED;
  Light_entity.face_color = ILI9341_WHITE;
  Light_entity.txt_color = ILI9341_BLACK;
  Light_entity.pressface_color = ILI9341_CYAN;
  Light_entity.roundRect = true;
  Light_entity.useBorder = false;
  Light_entity.center_txt = true;
  Light_entity.latchButton = true;
  Light_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"Light_G1", "Light_G2", "Light_G3", "Light_G4", "Light_G5", "Light_G6"};
  genButtonArr.set_button_properties(Light_entity);
  genButtonArr.create_array(3, 2, a);

  // for (uint8_t i = 0; i < numButtons_Lights; i++)
  // {
  //   if (light_state_ground[i])
  //   {
  //     genButtonArr.butarray[i].set_buttonState(light_state_ground[i]);
  //   }
  // }
}
void create_Lights_int_2()
{
  TFT_entity Light_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  Light_entity.w = (tft.width() - genButtonArr.dw) / 2;                           /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.h = (tft.height() - title.tft_entity.h - 3 * genButtonArr.dh) / 3; /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.txt_size = 2;
  Light_entity.border_thickness = 2;
  Light_entity.border_color = ILI9341_RED;
  Light_entity.face_color = ILI9341_WHITE;
  Light_entity.txt_color = ILI9341_BLACK;
  Light_entity.pressface_color = ILI9341_CYAN;
  Light_entity.roundRect = true;
  Light_entity.useBorder = false;
  Light_entity.center_txt = true;
  Light_entity.latchButton = true;
  Light_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"Li21", "Li22", "Lig3", "Li24", "Li25", "Li26"};
  genButtonArr.set_button_properties(Light_entity);

  // for (uint8_t i = 0; i < numButtons_Lights; i++)
  // {
  //   genButtonArr.butarray[i].set_buttonState(light_state_top[i]);
  // }
  genButtonArr.create_array(3, 2, a);
}
void create_Lights_ext()
{
  TFT_entity Light_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  Light_entity.w = (tft.width() - genButtonArr.dw) / 2;                           /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.h = (tft.height() - title.tft_entity.h - 3 * genButtonArr.dh) / 3; /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.txt_size = 2;
  Light_entity.border_thickness = 2;
  Light_entity.border_color = ILI9341_RED;
  Light_entity.face_color = ILI9341_WHITE;
  Light_entity.txt_color = ILI9341_BLACK;
  Light_entity.pressface_color = ILI9341_CYAN;
  Light_entity.roundRect = true;
  Light_entity.useBorder = false;
  Light_entity.center_txt = true;
  Light_entity.latchButton = true;
  Light_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"LX1", "LX2", "LX3", "LX4", "LX5", "LX6"};
  genButtonArr.set_button_properties(Light_entity);

  // for (uint8_t i = 0; i < numButtons_Lights; i++)
  // {
  //   genButtonArr.butarray[i].set_buttonState(light_state_ext[i]);
  // }
  genButtonArr.create_array(3, 2, a);
}
void create_Lights_Main()
{
  TFT_entity Light_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  Light_entity.w = tft.width();                                                                                                   /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.h = (tft.height() - title.tft_entity.h - (numButtons_Lights_Main - 1) * genButtonArr.dh) / numButtons_Lights_Main; /* When defined other than 0 , max screen's width's height will be taken */
  Light_entity.txt_size = 2;
  Light_entity.border_thickness = 2;
  Light_entity.border_color = ILI9341_RED;
  Light_entity.face_color = ILI9341_WHITE;
  Light_entity.txt_color = ILI9341_BLACK;
  Light_entity.pressface_color = ILI9341_CYAN;
  Light_entity.roundRect = true;
  Light_entity.useBorder = false;
  Light_entity.center_txt = true;
  Light_entity.latchButton = false;
  Light_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"Ground Floor", "2nd Floor", "Garden"};
  genButtonArr.set_button_properties(Light_entity);
  genButtonArr.create_array(3, 1, a);
}
void create_Windows_Main()
{
  TFT_entity tft_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  tft_entity.w = tft.width();                                                                                                     /* When defined other than 0 , max screen's width's height will be taken */
  tft_entity.h = (tft.height() - title.tft_entity.h - (numButtons_Windows_Main - 1) * genButtonArr.dh) / numButtons_Windows_Main; /* When defined other than 0 , max screen's width's height will be taken */
  tft_entity.txt_size = 2;
  tft_entity.border_thickness = 2;
  tft_entity.border_color = ILI9341_RED;
  tft_entity.face_color = ILI9341_WHITE;
  tft_entity.txt_color = ILI9341_BLACK;
  tft_entity.pressface_color = ILI9341_CYAN;
  tft_entity.roundRect = true;
  tft_entity.useBorder = false;
  tft_entity.center_txt = true;
  tft_entity.latchButton = false;
  tft_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"All", "Ground Floor", "Top Floor", "Pergs&Kitch"};
  genButtonArr.set_button_properties(tft_entity);
  genButtonArr.create_array(4, 1, a);
}
void create_Windows_Operate()
{
  TFT_entity tft_entity;

  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  tft_entity.w = tft.width();                                                                                                           /* When defined other than 0 , max screen's width's height will be taken */
  tft_entity.h = (tft.height() - title.tft_entity.h - (numButtons_Windows_Operate - 1) * genButtonArr.dh) / numButtons_Windows_Operate; /* When defined other than 0 , max screen's width's height will be taken */
  tft_entity.txt_size = 2;
  tft_entity.border_thickness = 2;
  tft_entity.border_color = ILI9341_RED;
  tft_entity.face_color = ILI9341_WHITE;
  tft_entity.txt_color = ILI9341_BLACK;
  tft_entity.pressface_color = ILI9341_CYAN;
  tft_entity.roundRect = true;
  tft_entity.useBorder = false;
  tft_entity.center_txt = true;
  tft_entity.latchButton = false;
  tft_entity.corner_radius = 2;

  genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh; /* When defined other than 0 - it will not be position at center */
  genButtonArr.shift_pos_w = 0;                                    /* When defined other than 0 - it will not be position at center */
  const char *a[] = {"Up", "Stop", "Down"};
  genButtonArr.set_button_properties(tft_entity);
  genButtonArr.create_array(numButtons_Windows_Operate, 1, a);
}

void update_clk(char *retClk)
{
  const int MILLIS2SEC = 1000;
  const int MILLIS2MIN = 60 * MILLIS2SEC;
  const int MILL2HOUR = 60 * MILLIS2MIN;

  unsigned long mill_counter = millis();
  uint8_t H = mill_counter / MILL2HOUR;
  uint8_t M = (mill_counter - H * MILL2HOUR) / MILLIS2MIN;
  uint8_t S = (mill_counter - H * MILL2HOUR - M * MILLIS2MIN) / MILLIS2SEC;

  sprintf(retClk, "%02d:%02d:%02d", H, M, S);
}
void update_title(const char *ttl = NULL)
{
  if (display_clock)
  {
    static long last_mil = 0;
    if (millis() - last_mil > 1000)
    {
      last_mil = millis();
      char a[20];
      update_clk(a);
      title.createLabel(a);
    }
  }
}

void clear_keypad()
{
  strcpy(keypad_pressed_chrs, "");
}
void add_char_kaypad(const char *a)
{
  strcat(keypad_pressed_chrs, a);
}
void send_keypad_msg_cd()
{
  Serial.println("Code sent");
}

void read_keypad()
{
  static unsigned long last_time_key_pressed = 0;

  uint8_t kpad_dig = genButtonArr.checkPress(numButtons_Keypad);
  if (kpad_dig == 99)
  {
    if (display_clock == false && millis() - last_time_key_pressed > 5000)
    {
      clear_keypad();
      display_clock = true;
    }
  }
  else
  {
    last_press_millis = millis();
    if (kpad_dig == 11)
    {
      send_keypad_msg_cd();
      clear_keypad();
      display_clock = true;
    }
    else if (kpad_dig == 9)
    {
      clear_keypad();
      display_clock = true;
    }
    else
    {
      display_clock = false;
      last_time_key_pressed = millis();
      const char *a[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"};
      add_char_kaypad(a[kpad_dig]);
      title.createLabel(keypad_pressed_chrs);
    }
  }
}
void read_Lights_Main()
{
  uint8_t Light_dig = genButtonArr.checkPress(numButtons_Lights_Main);

  if (Light_dig != 99)
  {
    last_press_millis = millis();

    if (Light_dig == 0)
    {
      rebuild_screen(LIGHTS_GROUND);
      create_Lights_int_g();
    }
    else if (Light_dig == 1)
    {
      rebuild_screen(LIGHTS_TOP);
      create_Lights_int_2();
    }
    else if (Light_dig == 2)
    {
      rebuild_screen(LIGHTS_EXT);
      create_Lights_ext();
    }
  }
}
void read_Lights_int_g()
{
  uint8_t Light_dig = genButtonArr.checkPress(numButtons_Lights);

  if (Light_dig != 99)
  {
    last_press_millis = millis();
    // light_state_ground[Light_dig] = genButtonArr.butarray[Light_dig].get_buttonState();
    external_cb(100 + 10 * (Light_dig + 1) + genButtonArr.butarray[Light_dig].get_buttonState());
  }
}
void read_Lights_int_2()
{
  uint8_t Light_dig = genButtonArr.checkPress(numButtons_Lights);

  if (Light_dig != 99)
  {
    last_press_millis = millis();
    light_state_top[Light_dig] = genButtonArr.butarray[Light_dig].get_buttonState();
    external_cb(200 + 10 * (Light_dig + 1) + genButtonArr.butarray[Light_dig].get_buttonState());
  }
}
void read_Lights_ext()
{
  uint8_t Light_dig = genButtonArr.checkPress(numButtons_Lights);

  if (Light_dig != 99)
  {
    last_press_millis = millis();
    light_state_ext[Light_dig] = genButtonArr.butarray[Light_dig].get_buttonState();
    external_cb(300 + 10 * (Light_dig + 1) + genButtonArr.butarray[Light_dig].get_buttonState());
  }
}
void read_alarmMenu()
{
  uint8_t alarm_dig = genButtonArr.checkPress(numButtons_Alarm);

  if (alarm_dig != 99)
  {
    last_press_millis = millis();
    if (alarm_dig == 2)
    {
      rebuild_screen(ALARM_KEYPAD);
      create_keypad();
    }
    else
    {
      Serial.println("ALARM");
      external_cb(alarm_dig);
      rebuild_screen(MAIN_MENU);
      create_Main();
    }
  }
}
void read_Main()
{
  uint8_t Main_dig = genButtonArr.checkPress(numButtons_Main);

  if (Main_dig != 99)
  {
    last_press_millis = millis();
    if (Main_dig == 0) /* Alarm Menu */
    {
      rebuild_screen(ALARM_MAIN);
      create_AlarmMenu();
    }
    else if (Main_dig == 1) /* Lights Menu */
    {
      rebuild_screen(LIGHTS_MAIN);
      create_Lights_Main();
    }
    else if (Main_dig == 2) /* Windows Menu */
    {
      rebuild_screen(WINDOWS_MAIN);
      create_Windows_Main();
    }
    else if (Main_dig == SYSTEM_MAIM) /* System Menu */
    {
      // rebuild_screen(LIGHTS_MAIN);
      // create_Lights_Main();
    }
  }
}
void read_Windows_Main()
{
  uint8_t Main_dig = genButtonArr.checkPress(numButtons_Windows_Main);

  if (Main_dig != 99)
  {
    last_press_millis = millis();
    if (Main_dig == 0)
    {
      rebuild_screen(WINDOWS_ALL);
    }
    else if (Main_dig == 1)
    {
      rebuild_screen(WINDOWS_GROUND);
    }
    else if (Main_dig == 2)
    {
      rebuild_screen(WINDOWS_TOP);
    }
    else if (Main_dig == 3)
    {
      rebuild_screen(WINDOWS_PERG);
    }
    create_Windows_Operate();
  }
}
void read_Windows_Oper()
{
  uint8_t Main_dig = genButtonArr.checkPress(numButtons_Windows_Operate);

  if (Main_dig != 99)
  {
    last_press_millis = millis();
    external_cb(activeMenu + (Main_dig + 1) * 1000);
  }
}

void rebuild_screen(uint8_t i)
{
  uint16_t background_color = ILI9341_BLUE;
  title.clear_screen();
  tft.fillScreen(background_color);
  create_title();
  activeMenu = i;
}
void timeout_to_mainScreen()
{
  uint8_t TO = 4;
  if (millis() - last_press_millis > TO * 1000 && activeMenu != 0)
  {
    rebuild_screen(0);
    create_Main();
  }
}
void start_GUI()
{
  ts.begin();
  tft.begin();
  tft.setRotation(SCREEN_ROT); /* 0-3 90 deg each */
  rebuild_screen(0);
  create_title();
  create_Main();
}

void setup()
{
  Serial.begin(115200);
  start_GUI();
}
void loop()
{
  switch (activeMenu)
  {
  case MAIN_MENU:
    read_Main();
    break;
  case ALARM_MAIN:
    read_alarmMenu();
    break;
  case ALARM_KEYPAD:
    read_keypad();
    break;
  case LIGHTS_MAIN:
    read_Lights_Main();
    break;
  case LIGHTS_GROUND:
    read_Lights_int_g();
    break;
  case LIGHTS_TOP:
    read_Lights_int_2();
    break;
  case LIGHTS_EXT:
    read_Lights_ext();
    break;
  case WINDOWS_MAIN:
    read_Windows_Main();
    break;
  case WINDOWS_ALL:
    read_Windows_Oper();
    break;
  case WINDOWS_GROUND:
    read_Windows_Oper();
    break;
  case WINDOWS_TOP:
    read_Windows_Oper();
    break;
  case WINDOWS_PERG:
    read_Windows_Oper();
    break;
  }

  update_title();
  timeout_to_mainScreen();
  // for (uint8_t i = 0; i < numButtons_Lights; i++)
  // {
  //   Serial.print(light_state_ground[i]);
  // }
  // Serial.println("");
  // delay(100);
}
