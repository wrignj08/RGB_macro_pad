#include <FastLED.h>
#define NUM_LEDS_PER_STRIP 20
#define DATA_PIN 7
#define NUM_STRIPS 1
CRGB leds[NUM_LEDS_PER_STRIP * NUM_STRIPS];

#include <RotaryEncoder.h>
#include <Bounce.h>

#include <Keypad.h>
#include <Keyboard.h>


Bounce button0 = Bounce(8, 10);
Bounce button1 = Bounce(9, 10);
Bounce button2 = Bounce(10, 10);


//CRGB leds[NUM_LEDS];/

unsigned long LED_speed = 10;
unsigned long previousMillis = 0;

//the starting brightness of the white pulse
int pulse_val = 254;//

//direction of the white pulse
int LED_dir = 1;//

// iterator for// UV
int uv_int = 0;

//used as brighness for click light up effect
int click_light = 0;

//lighting long press timer
//unsigned long lighting_long_press_timer = millis();
//unsigned long lighting_last_press = 0;

#define BRIGHTNESS 255   /* Control the brightness of your leds */
#define SATURATION 255   /* Control the saturation of your leds */

int lighting_effect = 0;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

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



RotaryEncoder encoder1(1, 2);
RotaryEncoder encoder2(3, 4);
RotaryEncoder encoder3(5, 6);

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


void setup() {
  FastLED.addLeds<NUM_STRIPS, WS2812, DATA_PIN, GRB>(leds, NUM_LEDS_PER_STRIP);
  Serial.begin(9600); // USB is always 12 Mbit/sec
  Keyboard.begin();
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
}




void loop() {

  // put your main code here, to run repeatedly:

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= LED_speed) {
    previousMillis = currentMillis;
    update_LED();
  }

  rotary();
  rotary_press();
  keypad_press();

}

void update_LED() {

  if (lighting_effect == 0) {
    pulse_LED();
  }
  if (lighting_effect == 1) {
    unicorn_vomit();
  }
  if (lighting_effect == 2) {
    freez_unicorn_vomit();
  }

  if (lighting_effect == 3) {
    click_light_up();
  }
  if (lighting_effect == 4) {
    LED_off();
  }

  if (lighting_effect > 4) {
    lighting_effect = 0;
  }

}


void pulse_LED() {
  pulse_val = pulse_val + (LED_dir * 1);

  if (pulse_val > 255) {
    LED_dir = -1;
    pulse_val = 255;
  }

  if (pulse_val < 0 ) {
    LED_dir = 1;
    pulse_val = 0;
  }
  update_all_LED(pulse_val);
}


void unicorn_vomit() {
  if (uv_int == 255) {
    uv_int = 0;
  }

  uv_int = uv_int + 1;

  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[i] = CHSV(i - (uv_int * 1), BRIGHTNESS, SATURATION); /* The higher the value 4 the less fade there is and vice versa */
  }
  FastLED.show();

}

void freez_unicorn_vomit() {
///

//  uv_int = uv/_int + 1;

  for (int i = 0; i < NUM_LEDS_PER_STRIP; i++) {
    leds[i] = CHSV(i - (uv_int * 1), BRIGHTNESS, SATURATION); /* The higher the value 4 the less fade there is and vice versa */
  }
  FastLED.show();

}


void LED_off() {
  update_all_LED(0);
}

///pulse light after each button click
void click_light_up() {

  if (click_light > 0) {
    click_light = click_light - 1;
    update_all_LED(click_light);
  }

}

void update_all_LED(int inc) {

  for (int LED = 0; LED < NUM_LEDS_PER_STRIP; LED++) {
    leds[LED] = CRGB( inc, inc, inc);

  }
  FastLED.show();
}


void rotary() {

  static int pos1 = 0;
  static int pos2 = 0;
  static int pos3 = 0;

  encoder1.tick();
  encoder2.tick();
  encoder3.tick();

  int newPos1 = encoder1.getPosition();
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
  int newPos2 = encoder2.getPosition();
  if (pos2 != newPos2) {
    if (pos2 > newPos2) {
      if (LED_speed > 5) { //if above minimum reduce by 3
        LED_speed = LED_speed - 3;
      }
    }
    else {
      if (LED_speed < 150) {
        LED_speed = LED_speed + 3;
      }
    }

    pos2 = newPos2;
  }
  int newPos3 = encoder3.getPosition();
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


void rotary_press() {

  button0.update();
  button1.update();
  button2.update();

  if (button0.fallingEdge()) {
    Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
    Keyboard.release(KEY_MEDIA_PLAY_PAUSE);
  }
  if (button1.fallingEdge()) {
    LED_off();

    lighting_effect = lighting_effect + 1;

  }
  if (button2.fallingEdge()) {
    Keyboard.press(KEY_MEDIA_PLAY_PAUSE);
    Keyboard.release(KEY_MEDIA_PLAY_PAUSE);
  }

}



void keypad_press() {
  char key = keypad.getKey();
  Serial.println(key);
  switch (key) {


    case '1':
      Keyboard.press(KEY_MEDIA_PREV_TRACK);
      Keyboard.release(KEY_MEDIA_PREV_TRACK);
      //      Keyboard.write('x');

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
      //      Keyboard.write('A');
      break;

    case '4':
      Keyboard.press(KEY_MEDIA_MUTE);
      Keyboard.release(KEY_MEDIA_MUTE);
      //      Keyboard.write('4');
      break;

    case '5':
      Keyboard.press(KEY_MEDIA_VOLUME_DEC ); //KEY_MEDIA_VOLUME_DOWN
      Keyboard.release(KEY_MEDIA_VOLUME_DEC );
      //      Keyboard.write('5');
      break;

    case '6':
      Keyboard.press(KEY_MEDIA_VOLUME_INC); //KEY_MEDIA_VOLUME_UP
      Keyboard.release(KEY_MEDIA_VOLUME_INC);
      //      Keyboard.write('6');
      break;

    case 'B':
      //     open terminal
      terminal("");
      //        Serial.print("test");
      break;

    case '7':
      //      Keyboard.write('7');
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('g');
      delay(50);
      Keyboard.releaseAll();

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

      //      Keyboard.write('9');
      break;

    case 'C':
      terminal("conda activate fastai && jupyter notebook");
      //      Keyboard.write('c');
      break;

    case '*':
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_LEFT_SHIFT);
      Keyboard.press('c');
      delay(50);
      Keyboard.releaseAll();
      //      Keyboard.write('*');
      break;

    case '0':
      //      bring up app switcher
      Keyboard.press(MODIFIERKEY_GUI);
      Keyboard.press('s');
      delay(10);
      Keyboard.releaseAll();
      //      Keyboard.write('0');
      break;

    case '#':
      //open home folder
      Keyboard.press(MODIFIERKEY_CTRL);
      Keyboard.press(MODIFIERKEY_ALT);
      Keyboard.press('h');
      delay(10);
      Keyboard.releaseAll();
      //      Keyboard.write('#');
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
