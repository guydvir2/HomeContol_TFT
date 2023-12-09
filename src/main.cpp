#include <Arduino.h>
#include <TFT_GUI.h>
#include "iot.h"

/* Initialize TFT */
#define MAX_TFT_BUTTONS 12
#define SCREEN_ROT 0 // { 0:0, 1:90, 2:180,3:270 }
#define DEFAULT_BACKGROUND_COLOR ILI9341_BLUE
XPT2046_Touchscreen ts(TS_CS);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

LabelTFT title1(tft);
LabelTFT title2(tft);
LabelTFT title3(tft);
LabelTFT title4(tft);
LabelTFT *titleArray[] = {&title1, &title2, &title3, &title4};

buttonArrayTFT<MAX_TFT_BUTTONS> genButtonArr(ts, tft); /* Using one instance for all menus */
#include "alarm.h"
struct menbut_trace
{
  uint8_t m_id = 255;
  uint8_t b_id = 255;
};
struct menu_descriptor
{
  uint8_t r = 0;
  uint8_t c = 0;
  bool latch = false;
  const char **but_txt;
};

uint8_t current_trace = 0;
const uint8_t MAX_TRACES = 5;
menbut_trace MenuButton_Trace[MAX_TRACES];
menu_descriptor Menu_desc;

enum title : const uint8_t
{
  CLOCK_ֹTITLE,
  TEXT_TITLE,
  NO_TITLE
};
enum menu : const uint8_t
{
  MAIN_MENU = 0,

  ALARM_MAIN = 10,
  ALARM_KEYPAD,

  LIGHTS_MAIN = 20,
  LIGHTS_GROUND,
  LIGHTS_TOP,
  LIGHTS_EXT,
  LIGHTS_OPER,

  WINDOWS_MAIN = 30,
  WINDOWS_ALL,
  WINDOWS_GROUND,
  WINDOWS_TOP,
  WINDOWS_PERG,
  WINDOWS_OPER,

  SYSTEM_MAIM = 40,

  EMPT = 99,
};

const menu AlarmMain[] = {ALARM_MAIN, ALARM_KEYPAD};
const menu LightsMain[] = {LIGHTS_MAIN, LIGHTS_GROUND, LIGHTS_TOP, LIGHTS_EXT, LIGHTS_OPER};
const menu WindowsMain[] = {WINDOWS_MAIN, WINDOWS_GROUND, WINDOWS_TOP, WINDOWS_PERG, WINDOWS_OPER};
const menu SystemMain[] = {SYSTEM_MAIM};
const menu MainMenu[] = {AlarmMain[0], LightsMain[0], WindowsMain[0], SystemMain[0]}; // MAIN_MENU, ALARM_MAIN, WINDOWS_MAIN, SYSTEM_MAIM};

uint8_t usedTitles = 0;
uint8_t activeTiltle = CLOCK_ֹTITLE;
unsigned long last_press_millis = 0;

void zero_traces()
{
  for (uint8_t i = 0; i < MAX_TRACES; i++)
  {
    MenuButton_Trace[i].b_id = 255;
    MenuButton_Trace[i].m_id = 255;
  }
  current_trace = 0;
  Serial.println("trace erased");
}
void print_trace(uint8_t i)
{
  if (MenuButton_Trace[i].b_id != 255 || MenuButton_Trace[i].m_id != 255)
  {
    Serial.print("trace: #");
    Serial.print(i);
    Serial.print("; Menu:");
    Serial.print(MenuButton_Trace[i].m_id);

    Serial.print("; Button:");
    Serial.println(MenuButton_Trace[i].b_id);
  }
}
void printAll_trace()
{
  Serial.println("start");
  for (uint8_t i = 0; i < MAX_TRACES; i++)
  {
    print_trace(i);
  }
  Serial.println("End");
}

void build_screen(menu i);
void timeout_to_mainScreen();

void external_cb()
{
  Serial.print("Entity: ");
  print_trace(current_trace - 1);
  Serial.print("Oper: ");
  print_trace(current_trace);
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
  tft_ent.roundRect = false;
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
    tft_entity.h = (tft.height() - titleArray[0]->tft_entity.h - (R + 1) * genButtonArr.dh) / R; /* When defined other than 0 , max screen's width's height will be taken */
    genButtonArr.shift_pos_h = titleArray[0]->tft_entity.h + genButtonArr.dh;                    /* When defined other than 0 - it will not be position at center */
  }
  else
  {
    tft_entity.h = (tft.height() - (R + 1) * genButtonArr.dh) / R; /* When defined other than 0 , max screen's width's height will be taken */
    genButtonArr.shift_pos_h = genButtonArr.dh;                    /* When defined other than 0 - it will not be position at center */
  }

  genButtonArr.shift_pos_w = 0; /* When defined other than 0 - it will not be position at center */
  genButtonArr.set_button_properties(tft_entity);
  for (uint8_t i = 0; i < MAX_TFT_BUTTONS; i++)
  {
    genButtonArr.butarray[i].clear_buttonState();
  }
  genButtonArr.create_array(R, C, a);
}

void update_title(const char *ttl = nullptr)
{
  // if (activeTiltle == CLOCK_ֹTITLE)
  // {
  //   static long last_mil = 0;
  //   if (millis() - last_mil > 1000)
  //   {
  //     last_mil = millis();
  //     char a[20];
  //     strcpy(a, "AAA");
  //     // iot.get_timeStamp(a);
  //     title.createLabel(a);
  //     title2.createLabel(a);
  //   }
  // }
  // else if (activeTiltle == TEXT_TITLE)
  // {
  //   if (ttl != nullptr)
  //   {
  //     title.createLabel(ttl);
  //   }
  //   else
  //   {
  //     //   title.createLabel(" ");
  //     yield();
  //   }
  // }
  // else if (activeTiltle == NO_TITLE)
  // {
  //   yield();
  // }
}
void title_init(uint8_t i, uint8_t w[])
{
  uint8_t title_h = 30;
  uint8_t title_space = 2;

  for (uint8_t n = 0; n < i; n++)
  {
    usedTitles++;
    titleArray[n]->tft_entity.w = w[n];
    titleArray[n]->tft_entity.h = title_h;
    for (uint8_t x = 0; x < n; x++)
    {
      titleArray[n]->tft_entity.w_pos += titleArray[x]->tft_entity.w + title_space;
    }
    titleArray[n]->tft_entity.w_pos += titleArray[n]->tft_entity.w / 2;
    titleArray[n]->tft_entity.h_pos = title_h / 2;
    titleArray[n]->tft_entity.txt_size = 2;
    titleArray[n]->tft_entity.border_thickness = 4;
    titleArray[n]->tft_entity.border_color = ILI9341_RED;
    titleArray[n]->tft_entity.face_color = ILI9341_YELLOW;
    titleArray[n]->tft_entity.txt_color = ILI9341_BLACK;
    titleArray[n]->tft_entity.roundRect = false;
    titleArray[n]->tft_entity.useBorder = false;
    titleArray[n]->tft_entity.center_txt = true;
    titleArray[n]->createLabel("*");
  }
}
void subtitle_update(uint8_t i, const char *txt, uint16 color = ILI9341_YELLOW)
{
  titleArray[i]->tft_entity.face_color = color;
  titleArray[i]->createLabel(txt);
}

void read_keypad(uint8_t pressed_digit, bool use_asterik = true, bool use_hash = true, uint8_t defined_code_len = 255)
{
  if (use_hash == true && pressed_digit == 11)
  {
    update_title("SENT");
    delay(500);
    if (send_keypad_Code(AlarmCodes, 3))
    {
      update_title(("** CODE OK **"));
      delay(1000);
      build_screen(MAIN_MENU);
    }
    else
    {
      update_title(("** CODE FAIL **"));
      delay(1000);
      clear_keypad();
    }
  }
  else if (use_asterik == true && pressed_digit == 9)
  {
    update_title(("CLEAR"));
    delay(1000);
    clear_keypad();
  }
  else
  {
    const char *a[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"};
    add_char_kaypad(a[pressed_digit]);
    update_title(keypad_pressed_chrs);
  }
}
void read_activeMenu()
{
  timeout_to_mainScreen();
  uint8_t activeMenu = MenuButton_Trace[current_trace].m_id;
  uint8_t pressed_digit = genButtonArr.checkPress(Menu_desc.r * Menu_desc.c);

  if (pressed_digit != 99)
  {
    last_press_millis = millis();
    MenuButton_Trace[current_trace].b_id = pressed_digit;

    if (activeMenu == MAIN_MENU)
    {
      current_trace++;
      build_screen(MainMenu[pressed_digit]);
    }
    else if (activeMenu == ALARM_MAIN)
    {
      current_trace++;
      if (pressed_digit == 2)
      {
        build_screen(ALARM_KEYPAD);
      }
      else
      {
        external_cb();
        build_screen(MAIN_MENU);
      }
    }
    else if (activeMenu == LIGHTS_MAIN)
    {
      current_trace++;
      build_screen(LightsMain[pressed_digit]);
    }
    else if (activeMenu == WINDOWS_MAIN)
    {
      current_trace++;
      build_screen(WindowsMain[pressed_digit]);
    }
    else if (activeMenu == ALARM_KEYPAD)
    {
      current_trace++;
      read_keypad(pressed_digit);
    }
    else if (activeMenu == LIGHTS_GROUND || activeMenu == LIGHTS_TOP || activeMenu == LIGHTS_EXT)
    {
      current_trace++;
      build_screen(LIGHTS_OPER);
    }
    else if (activeMenu == LIGHTS_OPER)
    {
      // current_trace++;
      external_cb();
    }
    else if (activeMenu == WINDOWS_ALL || activeMenu == WINDOWS_GROUND || activeMenu == WINDOWS_TOP || activeMenu == WINDOWS_PERG)
    {
      current_trace++;
      external_cb();
    }
    else if (activeMenu == WINDOWS_OPER)
    {
      // current_trace++;
      external_cb();
    }
  }
}

void build_screen(menu i)
{
  tft.fillScreen(DEFAULT_BACKGROUND_COLOR);
  activeTiltle = NO_TITLE;
  const char *a[] = {"Alarm", "Lights", "Windows", "System"};
  const char *b[] = {"Arm Home", "Arm Away", "Disarm KeyPad"};
  const char *c[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#"};
  const char *d[] = {"Ground Floor", "2nd Floor", "Garden"};
  const char *e[] = {"Light_G1", "Light_G2", "Light_G3", "Light_G4", "Light_G5", "Light_G6"};
  const char *f[] = {"TopLight1", "TopLight2", "TopLight3", "TopLight4", "TopLight5", "TopLight6"};
  const char *g[] = {"extLight1", "extLight2", "extLight3", "extLight4", "extLight5", "extLight6"};
  const char *h[] = {"On", "Off"};
  const char *j[] = {"All", "Ground Floor", "Top Floor", "Pergs&Kitch"};
  const char *k[] = {"Up", "Stop", "Down"};
  uint8_t z[3] = {50, 50, 150};
  switch (i)
  {
  case MAIN_MENU:
    activeTiltle = CLOCK_ֹTITLE;
    title_init(3, z);
    Menu_desc.r = 2;
    Menu_desc.c = 2;
    Menu_desc.latch = false;
    Menu_desc.but_txt = a;
    zero_traces();
    break;
  case ALARM_MAIN:
    Menu_desc.r = 3;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = b;
    break;
  case ALARM_KEYPAD:
    activeTiltle = TEXT_TITLE;
    update_title("\"*\" Send; \"#\"Clear");
    Menu_desc.r = 4;
    Menu_desc.c = 3;
    Menu_desc.latch = false;
    Menu_desc.but_txt = c;
    break;
  case LIGHTS_MAIN:
    Menu_desc.r = 3;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = d;
    break;
  case LIGHTS_GROUND:
    Menu_desc.r = 3;
    Menu_desc.c = 2;
    Menu_desc.latch = false;
    Menu_desc.but_txt = e;
    break;
  case LIGHTS_TOP:
    Menu_desc.r = 3;
    Menu_desc.c = 2;
    Menu_desc.latch = false;
    Menu_desc.but_txt = f;
    break;
  case LIGHTS_EXT:
    Menu_desc.r = 3;
    Menu_desc.c = 2;
    Menu_desc.latch = false;
    Menu_desc.but_txt = g;
    break;
  case LIGHTS_OPER:
    Menu_desc.r = 2;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = h;
    break;
  case WINDOWS_MAIN:
    Menu_desc.r = 4;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = j;
    break;
  case WINDOWS_ALL:
    Menu_desc.r = 3;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = k;
    break;
  case WINDOWS_GROUND:
    Menu_desc.r = 3;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = k;
    break;
  case WINDOWS_TOP:
    Menu_desc.r = 3;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = k;
    break;
  case WINDOWS_PERG:
    Menu_desc.r = 3;
    Menu_desc.c = 1;
    Menu_desc.latch = false;
    Menu_desc.but_txt = k;
    break;

  default:
    break;
  }

  create_gen_menu(Menu_desc.r, Menu_desc.c, Menu_desc.latch, Menu_desc.but_txt);
  MenuButton_Trace[current_trace].m_id = i; /* Update menu in current trace */
  printAll_trace();
}
void timeout_to_mainScreen()
{
  uint8_t TO = 8;
  if (millis() - last_press_millis > TO * 1000 && MenuButton_Trace[current_trace].m_id != MAIN_MENU)
  {
    build_screen(MAIN_MENU);
  }
}
void start_GUI()
{
  ts.begin();
  tft.begin();
  tft.setRotation(SCREEN_ROT);
  build_screen(MAIN_MENU);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nStart");
  start_GUI();
  start_iot2();
}
void loop()
{
  read_activeMenu();
  // update_title();
  // send_msgsInQue();
  iot.looper();
  checkWifi_connectivity(1);
}
