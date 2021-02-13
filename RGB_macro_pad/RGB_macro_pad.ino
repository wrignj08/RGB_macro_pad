#include <FastLED.h>
#define NUM_LEDS_PER_STRIP 20
#define DATA_PIN 7
#define NUM_STRIPS 1
#define BRIGHTNESS 255
#define SATURATION 255
#define MAX_BRIGHTNESS 255

#include <RotaryEncoder.h>
#include <Bounce.h>
#include <Keypad.h>
#include <Keyboard.h>

Bounce button0 = Bounce(8, 10);
Bounce button1 = Bounce(9, 10);
Bounce button2 = Bounce(10, 10);

unsigned long LED_speed = 1;
unsigned long previousMillis = 0;
struct CRGB leds[NUM_LEDS_PER_STRIP];

// iterator for UV
int uv_int = 0;

// used for timing pulse_LED
int cust_millis = 0;

// used as brighness for click light up effect
int click_light = 255;

// to change lighting effect
int lighting_effect = 0;

// four rows
const byte ROWS = 4;
// four columns
const byte COLS = 4;

// the library will return the character inside this array
// when the appropriate button is pressed.
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};

// Looking at the keypad from the front, the row pins are on the left.
byte rowPins[ROWS] = {20, 21, 22, 23}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {16, 17, 18, 19}; //connect to the column pinouts of the keypad

// setting up each RE
RotaryEncoder encoder1(1, 2);
RotaryEncoder encoder2(3, 4);
RotaryEncoder encoder3(5, 6);

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  // setup fastleds
  FastLED.addLeds<NUM_STRIPS, WS2812, DATA_PIN, GRB>(leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.clear();
  //  fill_solid(leds, NUM_LEDS_PER_STRIP, CRGB::White);
  Serial.begin(9600); // USB is always 12 Mbit/sec
  Keyboard.begin();
  // setup each RE button
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
}


void loop() {

  // grab time since boot
  unsigned long currentMillis = millis();

  // if enught time has elapsed update LEDS
  if (currentMillis - previousMillis >= LED_speed) {
    previousMillis = currentMillis;
    update_LED();
  }
  // check if RE has moved
  rotary();

  // check if RE buttons pressed
  rotary_press();

  // check if key pressed
  keypad_press();
}

// runs if enught time has time elapsed
void update_LED() {

  // run one of the following depending on 'rotary_press' button 2 state

  if (lighting_effect == 0) {
    // the lighting brightness is set by this function,
    pulse_LED();
  }
  if (lighting_effect == 1) {
    unicorn_vomit();
  }
  if (lighting_effect == 2) {
    freeze_unicorn_vomit();
  }
  if (lighting_effect == 3) {
    click_light_up();
  }
  if (lighting_effect == 4) {
    LED_off();
  }
  // reset to 0 when past 4
  if (lighting_effect > 4) {
    lighting_effect = 0;
  }
}

// white puse effect
void pulse_LED() {
  // sets all leds to white
  fill_solid(leds, NUM_LEDS_PER_STRIP, CRGB::White);
  // iterate custom timer
  cust_millis += 10;
  // nice stead breath effect
  float breath = (exp(sin(cust_millis / 2000.0 * PI)) - 0.36787944) * 108.0;
  FastLED.setBrightness(breath);
  FastLED.show();
}

// RGB sweep effect
void unicorn_vomit() {
  // reset colour if about to overflow
  if (uv_int == 255) {
    uv_int = 0;
  }

  // iterate colour
  uv_int = uv_int + 1;

  // update colour to all leds
  fill_solid(leds, NUM_LEDS_PER_STRIP, CHSV(uv_int, BRIGHTNESS, SATURATION));

  //for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
  //leds[i] = CHSV(i - (uv_int * 1), BRIGHTNESS, SATURATION);
  //}
  FastLED.show();
}

// apply the last unicorn vomit effect then stop
void freeze_unicorn_vomit() {
  fill_solid(leds, NUM_LEDS_PER_STRIP, CHSV(uv_int, BRIGHTNESS, SATURATION));
  FastLED.show();
}

// pulse light after each button click
void click_light_up() {
  // click_light brightness is set after each button press, this just decreases it over time
  if (click_light > 0) {
    click_light = click_light - 1;
    update_all_LED(click_light);
  }
}

// set all leds to off
void LED_off() {
  update_all_LED(0);
}

// will set all leds to one value
void update_all_LED(int inc) {
  fill_solid(leds, NUM_LEDS_PER_STRIP, CRGB( inc, inc, inc));
  FastLED.show();
}

// checks state of RE
void rotary() {

  static int pos1 = 0;
  static int pos2 = 0;
  static int pos3 = 0;

  encoder1.tick();
  encoder2.tick();
  encoder3.tick();

  // time warp undo redo with dial 1
  int newPos1 = encoder1.getPosition();
  // if moved do stuff
  if (pos1 != newPos1) {
    if (pos1 < newPos1) {
      Keyboard.press(MODIFIERKEY_CTRL);
      Keyboard.press('z');
      delay(10);
      Keyboard.releaseAll();
    }
    else {
      Keyboard.press(MODIFIERKEY_CTRL);
      Keyboard.press(MODIFIERKEY_SHIFT);
      Keyboard.press('z');
      delay(10);
      Keyboard.releaseAll();
    }
    pos1 = newPos1;
  }
  // changes lighting speed
  int newPos2 = encoder2.getPosition();
  // if moved do stuff
  if (pos2 != newPos2) {
    // check if getting small, if so make slower
    if (pos2 > newPos2) {
      if (LED_speed > 4) { //if above minimum reduce by 3
        LED_speed = LED_speed - 3;
      }
    }
    // else must be bigger so make faster
    else {
      if (LED_speed < 150) {
        LED_speed = LED_speed + 3;
      }
    }
    pos2 = newPos2;
  }
  // change volume
  int newPos3 = encoder3.getPosition();
  // if moved do stuff
  // same a above but tith volume
  if (pos3 != newPos3) {
    if (pos3 > newPos3) {
      Keyboard.press(KEY_MEDIA_VOLUME_INC);
      delay(5);
      Keyboard.releaseAll();
    }
    else {
      Keyboard.press(KEY_MEDIA_VOLUME_DEC);
      delay(5);
      Keyboard.releaseAll();
    }
    pos3 = newPos3;
  }
}

// check for RE buttons press
void rotary_press() {

  button0.update();
  button1.update();
  button2.update();
  // play pause media should find something better to do....
  if (button0.fallingEdge()) {
    Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
    Keyboard.release(KEY_MEDIA_PLAY_PAUSE);
  }
  // switch lighting effect
  if (button1.fallingEdge()) {
    //    LED_off();/
    lighting_effect = lighting_effect + 1;
  }
  // play pause media should find something better to do....
  if (button2.fallingEdge()) {
    Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
    Keyboard.release(KEY_MEDIA_PLAY_PAUSE);
  }
}

// checks for key presses
void keypad_press() {
  char key = keypad.getKey();
  Serial.println(key);
  switch (key) {
    
    case '1':
      Keyboard.press(KEY_MEDIA_PREV_TRACK);
      Keyboard.release(KEY_MEDIA_PREV_TRACK);
      //      Keyboard.write('x');
      click_light = 255;
      break;

    case '2':
      Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
      Keyboard.release(KEY_MEDIA_PLAY_PAUSE);
      //      Keyboard.write('2');
      click_light = 255;
      break;

    case '3':
      Keyboard.press(KEY_MEDIA_NEXT_TRACK);
      Keyboard.release(KEY_MEDIA_NEXT_TRACK);
      click_light = 255;
      //      Keyboard.write('3');
      break;

    case 'A':
      //      read selected text in chrome using 'Read Aloud" extention
      Keyboard.press(KEY_LEFT_ALT);;
      //      stop reading if already started
      Keyboard.press('o');
      Keyboard.release('o');
      Keyboard.press('p');
      delay(10);
      Keyboard.releaseAll();
      click_light = 255;
      //      Keyboard.write('A');
      break;

    case '4':
      Keyboard.press(KEY_MEDIA_MUTE);
      Keyboard.release(KEY_MEDIA_MUTE);
      //      Keyboard.write('4');
      click_light = 255;
      break;

    case '5':
      Keyboard.press(KEY_MEDIA_VOLUME_DEC );
      Keyboard.release(KEY_MEDIA_VOLUME_DEC );
      //      Keyboard.write('5');
      click_light = 255;
      break;

    case '6':
      Keyboard.press(KEY_MEDIA_VOLUME_INC);
      Keyboard.release(KEY_MEDIA_VOLUME_INC);
      //      Keyboard.write('6');
      click_light = 255;
      break;

    case 'B':
      //     open terminal
      terminal("");
      click_light = 255;
      break;

    case '7':
      //      Keyboard.write('7');
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('g');
      delay(50);
      Keyboard.releaseAll();
      click_light = 255;
      break;

    case '8':
      //      bring outlook PWA to front
      //      run_console("wmctrl -a Outlook");
      //      Keyboard.write('8');

      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('o');
      delay(50);
      Keyboard.releaseAll();
      click_light = 255;
      break;

    case '9':
      //      bring TY music to front
      //      run_console("wmctrl -a YouTube Music");
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('m');
      delay(50);
      Keyboard.releaseAll();
      click_light = 255;
      //      Keyboard.write('9');
      break;

    case 'C':
      terminal("conda activate fastai && jupyter notebook");
      //      Keyboard.write('c');
      click_light = 255;
      break;

    case '*':
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('c');
      delay(50);
      Keyboard.releaseAll();
      //      Keyboard.write('*');
      click_light = 255;
      break;

    case '0':
      //      bring up app switcher
      Keyboard.press(MODIFIERKEY_GUI);
      Keyboard.press('s');
      delay(10);
      Keyboard.releaseAll();
      //      Keyboard.write('0');
      click_light = 255;
      break;

    case '#':
      //open home folder
      Keyboard.press(MODIFIERKEY_CTRL);
      Keyboard.press(MODIFIERKEY_ALT);
      Keyboard.press('h');
      delay(10);
      Keyboard.releaseAll();
      //      Keyboard.write('#');
      click_light = 255;
      break;

    case 'D':
      //      bring up app switcher
      Keyboard.press(KEY_LEFT_ALT);;
      Keyboard.press(KEY_TAB);;
      delay(100);
      Keyboard.release(KEY_TAB);
      //      hold it open for 3 decs
      delay(3000);
      Keyboard.release(KEY_LEFT_ALT);
      //      Keyboard.write('D');
      click_light = 255;
      break;
  }
}

// opens run console and runs command
void run_console(String cmd) {
  Keyboard.press(KEY_LEFT_ALT);;
  Keyboard.press(KEY_F2);;
  delay(10);
  Keyboard.releaseAll();
  delay(500);
  Keyboard.println(cmd);
}

//opens terminal and runs a command
void terminal(String cmd) {
  Keyboard.press(KEY_LEFT_ALT);;
  Keyboard.press(KEY_LEFT_CTRL);;
  Keyboard.press('t');
  delay(10);
  Keyboard.releaseAll();
  //  only run if command is set
  if (cmd.length() > 0) {
    delay(500);
    Keyboard.println(cmd);
  }
}
