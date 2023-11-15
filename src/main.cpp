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
buttonArrayTFT<numButtons_Keypad> genButtonArr(ts, tft); /* Using one instance for all menus */

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
  SYSTEM_MAIM
};
enum title : const uint8_t
{
  CLOCK_ֹTITLE,
  TEXT_TITLE,
  NO_TITLE
};

uint8_t activeMenu = MAIN_MENU;
uint8_t activeTiltle = CLOCK_ֹTITLE;

char keypad_pressed_chrs[10];
unsigned long last_press_millis = 0;
bool light_state_top[numButtons_Lights] = {0, 0, 0, 0, 0, 0};
bool light_state_ext[numButtons_Lights] = {0, 0, 0, 0, 0, 0};
bool light_state_ground[numButtons_Lights] = {0, 0, 0, 0, 0, 0};

void build_screen(uint8_t i, const char *ttl = nullptr);
void external_cb(int i)
{
  Serial.print("CB: #");
  Serial.println(i);
}
void set_buttons_state(uint8_t n, bool array[]) /* on re-creatoin of a menu - when using is latch button */
{
  for (uint8_t i = 0; i < n; i++)
  {
    if (genButtonArr.butarray[i].tft_entity.latchButton && genButtonArr.butarray[i].get_buttonState() != array[i])
    {
      genButtonArr.butarray[i].set_buttonState(array[i]); /* rebuild button in correct state */
    }
  }
}
void set_def_TFT(TFT_entity &tft_ent, bool latch = false)
{
  genButtonArr.dw = 2;
  genButtonArr.dh = 2;
  tft_ent.txt_size = 2;
  tft_ent.border_thickness = 2;
  tft_ent.border_color = ILI9341_RED;
  tft_ent.face_color = ILI9341_WHITE;
  tft_ent.txt_color = ILI9341_BLACK;
  tft_ent.pressface_color = ILI9341_CYAN;
  tft_ent.roundRect = true;
  tft_ent.useBorder = false;
  tft_ent.center_txt = true;
  tft_ent.latchButton = latch;
  tft_ent.corner_radius = 4;
}
void create_gen_menu(uint8_t R, uint8_t C, bool latch, const char *a[])
{
  TFT_entity tft_entity;
  set_def_TFT(tft_entity, latch);

  tft_entity.w = (tft.width() - genButtonArr.dw) / C; /* When defined other than 0 , max screen's width's height will be taken */
  if (activeTiltle != NO_TITLE)
  {
    tft_entity.h = (tft.height() - title.tft_entity.h - (R + 1) * genButtonArr.dh) / R; /* When defined other than 0 , max screen's width's height will be taken */
    genButtonArr.shift_pos_h = title.tft_entity.h + genButtonArr.dh;                    /* When defined other than 0 - it will not be position at center */
  }
  else
  {
    tft_entity.h = (tft.height() - (R + 1) * genButtonArr.dh) / R; /* When defined other than 0 , max screen's width's height will be taken */
    genButtonArr.shift_pos_h = genButtonArr.dh;                    /* When defined other than 0 - it will not be position at center */
  }

  genButtonArr.shift_pos_w = 0; /* When defined other than 0 - it will not be position at center */
  genButtonArr.set_button_properties(tft_entity);
  for (uint8_t i = 0; i < numButtons_Keypad; i++)
  {
    genButtonArr.butarray[i].clear_buttonState();
  }
  genButtonArr.create_array(R, C, a);
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
}
void create_Main()
{
  const char *a[] = {"Alarm", "Lights", "Windows", "System"};
  create_gen_menu(2, 2, false, a);
}
void create_keypad()
{
  const char *a[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"};
  create_gen_menu(4, 3, false, a);
}
void create_AlarmMenu()
{
  const char *a[] = {"Arm Home", "Arm Away", "Disarm KeyPad"};
  create_gen_menu(3, 1, false, a);
}
void create_Lights_int_g()
{
  const char *a[] = {"Light_G1", "Light_G2", "Light_G3", "Light_G4", "Light_G5", "Light_G6"};
  create_gen_menu(3, 2, true, a);
  set_buttons_state(numButtons_Lights, light_state_ground);
}
void create_Lights_int_2()
{
  const char *a[] = {"TopLight1", "TopLight2", "TopLight3", "TopLight4", "TopLight5", "TopLight6"};
  create_gen_menu(3, 2, true, a);
  set_buttons_state(numButtons_Lights, light_state_top);
}
void create_Lights_ext()
{
  const char *a[] = {"extLight1", "extLight2", "extLight3", "extLight4", "extLight5", "extLight6"};
  create_gen_menu(3, 2, true, a);
  set_buttons_state(numButtons_Lights, light_state_ext);
}
void create_Lights_Main()
{
  const char *a[] = {"Ground Floor", "2nd Floor", "Garden"};
  create_gen_menu(3, 1, false, a);
}
void create_Windows_Main()
{
  const char *a[] = {"All", "Ground Floor", "Top Floor", "Pergs&Kitch"};
  create_gen_menu(4, 1, false, a);
}
void create_Windows_Operate()
{
  const char *a[] = {"Up", "Stop", "Down"};
  create_gen_menu(3, 1, false, a);
}

void calc_clk(char *retClk)
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
void update_title(const char *ttl = nullptr)
{
  if (activeTiltle == CLOCK_ֹTITLE)
  {
    static long last_mil = 0;
    if (millis() - last_mil > 1000)
    {
      last_mil = millis();
      char a[20];
      calc_clk(a);
      title.createLabel(a);
    }
  }
  else if (activeTiltle == TEXT_TITLE)
  {
    if (ttl != nullptr)
    {
      title.createLabel(ttl);
    }
    // else
    // {
    //   title.createLabel(" ");
    // }
  }
  else if (activeTiltle == NO_TITLE)
  {
    // Serial.println("NO_TTL");
    yield();
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
  uint8_t kpad_dig = genButtonArr.checkPress(numButtons_Keypad);
  if (kpad_dig == 99)
  {
    if (activeTiltle != CLOCK_ֹTITLE && millis() - last_press_millis > 5000)
    {
      clear_keypad();
    }
  }
  else
  {
    last_press_millis = millis();
    if (kpad_dig == 11)
    {
      send_keypad_msg_cd();
      clear_keypad();
    }
    else if (kpad_dig == 9)
    {
      clear_keypad();
    }
    else
    {
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
      build_screen(LIGHTS_GROUND);
    }
    else if (Light_dig == 1)
    {
      build_screen(LIGHTS_TOP);
    }
    else if (Light_dig == 2)
    {
      build_screen(LIGHTS_EXT);
    }
  }
}
void read_Lights_int_g()
{
  uint8_t Light_dig = genButtonArr.checkPress(numButtons_Lights);

  if (Light_dig != 99)
  {
    last_press_millis = millis();
    light_state_ground[Light_dig] = genButtonArr.butarray[Light_dig].get_buttonState();
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
      build_screen(ALARM_KEYPAD);
    }
    else
    {
      Serial.println("ALARM");
      external_cb(alarm_dig);
      build_screen(MAIN_MENU);
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
      build_screen(ALARM_MAIN);
    }
    else if (Main_dig == 1) /* Lights Menu */
    {
      build_screen(LIGHTS_MAIN);
    }
    else if (Main_dig == 2) /* Windows Menu */
    {
      build_screen(WINDOWS_MAIN);
    }
    else if (Main_dig == SYSTEM_MAIM) /* System Menu */
    {
      // build_screen(LIGHTS_MAIN);
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
      build_screen(WINDOWS_ALL);
    }
    else if (Main_dig == 1)
    {
      build_screen(WINDOWS_GROUND);
    }
    else if (Main_dig == 2)
    {
      build_screen(WINDOWS_TOP);
    }
    else if (Main_dig == 3)
    {
      build_screen(WINDOWS_PERG);
    }
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
void read_activeMenu()
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
}

void build_screen(uint8_t i, const char *ttl)
{
  uint16_t background_color = ILI9341_BLUE;
  title.clear_screen();
  tft.fillScreen(background_color);

  activeMenu = i;
  activeTiltle = NO_TITLE;

  switch (activeMenu)
  {
  case MAIN_MENU:
    activeTiltle = CLOCK_ֹTITLE;
    update_title(ttl);
    create_Main();
    break;
  case ALARM_MAIN:
    create_AlarmMenu();
    break;
  case ALARM_KEYPAD:
    activeTiltle = TEXT_TITLE;
    update_title("\"*\" Send; \"#\"Clear");
    create_keypad();
    break;
  case LIGHTS_MAIN:
    create_Lights_Main();
    break;
  case LIGHTS_GROUND:
    create_Lights_int_g();
    break;
  case LIGHTS_TOP:
    create_Lights_int_2();
    break;
  case LIGHTS_EXT:
    create_Lights_ext();
    break;
  case WINDOWS_MAIN:
    create_Windows_Main();
    break;
  case WINDOWS_ALL:
    create_Windows_Operate();
    break;
  case WINDOWS_GROUND:
    create_Windows_Operate();
    break;
  case WINDOWS_TOP:
    create_Windows_Operate();
    break;
  case WINDOWS_PERG:
    create_Windows_Operate();
    break;
  default:
    break;
  }
}
void timeout_to_mainScreen()
{
  uint8_t TO = 8;
  if (millis() - last_press_millis > TO * 1000 && activeMenu != MAIN_MENU)
  {
    build_screen(MAIN_MENU);
  }
}
void start_GUI()
{
  ts.begin();
  tft.begin();
  tft.setRotation(SCREEN_ROT); /* 0-3 90 deg each */
  create_title();
  build_screen(MAIN_MENU, "Hello !");
}

void setup()
{
  Serial.begin(115200);
  start_GUI();
}
void loop()
{
  read_activeMenu();
  update_title();
  timeout_to_mainScreen();
}
