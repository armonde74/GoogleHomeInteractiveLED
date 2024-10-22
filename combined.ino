#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <FastLED.h>
#include <EEPROM.h>
#define LED_PIN     2
#define NUM_LEDS    60                                      // Number of lights
#define BRIGHTNESS  120                                       // Brightness
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define sensitivity 5                                     // Can set 0-10, recommended at 5
CRGB leds[NUM_LEDS];
uint8_t color;
uint8_t colour;                                               // Ripple colour is randomized.
int center = 0;                                               // Center of the current ripple.
int step = -1;                                                // -1 is the initializing step.
uint8_t myfade = 255;                                         // Starting brightness.
#define maxsteps 16
int num,nums = 1;
int w = 0;
int forth = 0;
int b=50;
int Red   = 50;
int Green = 50;
int Blue  = 50;
/****************************** Speed ******************************************/

int UPDATES_PER_SECOND = 250;// High the number the faster the patterns will move

/********************************************************************************/
#define CHANCE_OF_TWINKLE 2
#define DELTA_COLOR_DOWN CRGB(1,0,1)
#define DELTA_COLOR_UP   CRGB(4,0,4)
#define PEAK_COLOR       CRGB(0,128,128)
#define BASE_COLOR       CRGB(0,32,32)
#define MASTER_BRIGHTNESS   255

#include <ESP8266WiFi.h>

String next;
#define led               D7

#define WLAN_SSID       "Your SSID"             // Your SSID
#define WLAN_PASS       "Your PW"        // Your password

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Your Username"            // Replace it with your username
#define AIO_KEY         "Your Key"   // Replace with your Project Auth Key

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/


// Setup a feed called 'onoff' for subscribing to changes.

/****************************** FeedName ************************************/
Adafruit_MQTT_Subscribe Mode = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/mode");



CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


void MQTT_connect();


void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  pinMode(5, OUTPUT);
  pinMode(0, OUTPUT);

  pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);

    delay( 100 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    color = random16(0,255);
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;


  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  digitalWrite(led,LOW);

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&Mode);

}

void loop() {

  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;

  while ((subscription = mqtt.readSubscription(2))) {

    if (subscription == &Mode) {
      Serial.print("Mode > ");
      Serial.print(F("Got: "));
      Serial.println((char *)Mode.lastread);
      byte Mode1_state = atoi((char *)Mode.lastread);
      //analogWrite(0, Light2_State);
      //EEPROM.write(1, Mode_state);
      EEPROM.commit();
      next = ((char *)Mode.lastread);
      ModeChange();
      //Serial.println(EEPROM.read(1));
      //Serial.println(Light2_State);
    }
  }

if(w==1){
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */

    FillLEDsFromPaletteColors( startIndex);
    }
    else if(w==3){
      ripple();
    }
    else if(w==2){
       Red = 0;
      Green = 0;
      Blue = 0;
      FillSolid();
      w=0;
    }
    else if(w==4){
     TwinkleMapPixels();
    }
    else if(w==5){
      back();
    }
    else if(w==6){
      rave();
      //FillSolid();
    }
    else if(w==7){
     Musicpixels();
    }
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    //Serial.println("Connected");
    return;
  }

  digitalWrite(led,HIGH);
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      //while (1);
      Serial.println("Wait 10 min to reconnect");
      delay(600000);
    }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(led,LOW);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;

    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}

void ModeChange(){
  Serial.println(next);
  if (next == "Off" or next == "off"or next == "one" or next == "1"){
    Serial.println("Item 1 selected");
      w=2;
  }
  else if (next == "Rainbow" or next == "rainbow"or next == "two"or next == "2"){
   currentPalette = RainbowColors_p;
       w=1;
  }

   else if (next == "rainbow colors" or next == "three"or next == "3"){
    currentPalette = RainbowStripeColors_p;
      w=1;
  }
   else if (next == "Heat" or next == "heat"or next == "four"or next == "4"){
    currentPalette = HeatColors_p;
      w=1;
  }
   else if (next == "Lava" or next == "Lava" or next == "five"or next == "5"){
     currentPalette = LavaColors_p;
      w=1;
  }
  else if (next == "Forest" or next == "six"or next == "6"){
    currentPalette =ForestColors_p;
      w=1;
  }
   else if (next == "Clouds" or next == "seven"or next == "7"){
   currentPalette = CloudColors_p;
      w=1;
  }
   else if (next == "Ocean" or next == "eight"or next == "8"){
    currentPalette = OceanColors_p;
      w=1;
  }
   else if (next == "party colors" or next == "nine"or next == "9"){
      currentPalette = PartyColors_p;
      w=1;
  }
   else if (next == "changing colors" or next == "ten"or next == "10"){
    SetupPurpleAndGreenPalette();
      w=1;
  }
   else if (next == "random colors" or next == "11"){
    SetupTotallyRandomPalette();
       w=1;
  }
  else if (next == "one color" or next == "12"){
    OneColour();
      w=1;
  }
  else if (next == "two colors" or next == "13"){
    OneColourlots();
      w=1;
  }
  else if (next == "four colors" or next == "14"){
    OneColourlotss();
      w=1;
  }
  else if (next == "Ripple" or next == "15"){
      w=3;
  }
  else if (next == "twinkle" or next == "16"){
      w=4;
      InitPixelStates();
  }
  else if (next == "back and forth" or next == "17"){
      w=5;
      InitPixelStates();
  }
  else if (next == "Christmas" or next == "18"){
      w=1;
      red();
  }
  else if (next == "Rave" or next == "19"){
    w=6;
  }
  else if (next == "Music" or next == "music" or next == "20"){
    w=7;
  }
}

void back(){
  delay(2000 /UPDATES_PER_SECOND);
for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0,0,0);

  if (forth ==0){
    colour = random16(0,255);
  }
  if (forth == 0){
    num = 1;
  }
  else if(forth == (NUM_LEDS-1)){
    num = -1;
  }
  if (b ==0){
    color = random16(0,255);
  }
  if (b == 0){
    nums = 1;
  }
  else if(b == (NUM_LEDS-1)){
    nums = -1;
  }
   leds[forth] = CHSV(colour, 255, 255);
   leds[b] = CHSV(color, 255, 255);
  forth = forth + num;
  b=b+nums;
  //Serial.println(forth);
}

void FillSolid(){
CRGB colour = CRGB( Red, Green, Blue);
fill_solid(leds,NUM_LEDS ,CRGB( Red, Green, Blue));

}

void party(){
  colour = random16(0,255);
  fill_solid(leds,NUM_LEDS ,CHSV(colour, 255, 255));
  delay(100);
}

void rave(){
   uint8_t Hand = (millis() /100) % 2;
    static uint8_t y = 99;
    if( y != Hand) {
        y = Hand;
    if(Hand == 1){colour = random16(0,255); w=6;}
}  fill_solid(leds,NUM_LEDS ,CHSV(colour, 255, 255));}

void red()
{
    CRGB purple = CHSV( HUE_RED, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;

    currentPalette = CRGBPalette16(
                                  green,  purple, green,  purple,
                                   green,  purple, green,  purple,
                                   green,  purple, green,  purple,
                                   green,  purple, green,  purple );
}

void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PINK, 255, 255);
    CRGB green  = CRGB::White;
    CRGB black  = CRGB::Black;
    CRGB blue   = CHSV( HUE_AQUA,255,255);
    CRGB yellow   = CHSV( HUE_YELLOW,255,255);
    CRGB orange   = CHSV( HUE_ORANGE,255,255);
    currentPalette = CRGBPalette16(
                                   green,  yellow,  black,  black,
                                   purple, purple, black,  black,
                                   blue,  blue,  orange,  black,
                                   purple, orange, yellow,  black );
}

void ripple() {
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB(0,0,0);  // Rotate background colour.
  delay(3000 /UPDATES_PER_SECOND);
  switch (step) {

    case -1:                                                          // Initialize ripple variables.
      center = random(NUM_LEDS);
      colour = random16(0,255);
      step = 0;
      break;

    case 0:
      leds[center] = CHSV(colour, 255, 255);
      // Display the first pixel of the ripple.
      step ++;
      break;

    case maxsteps:                                                    // At the end of the ripples.
      step = -1;
      break;

    default:                                                          // Middle of the ripples.
        leds[wrap(center + step)] += CHSV(colour, 255, myfade/step*2);   // Display the next pixels in the range for one side.
        leds[wrap(center - step)] += CHSV(colour, 255, myfade/step*2);
        leds[wrap(center + (step+1))] += CHSV(colour, 255, myfade/step*2);   // Display the next pixels in the range for one side.
        leds[wrap(center - (step-1))] += CHSV(colour, 255, myfade/step*2);// Display the next pixels in the range for the other side.
        leds[wrap(center + (step+2))] += CHSV(colour, 255, myfade/step*2);   // Display the next pixels in the range for one side.
        leds[wrap(center - (step-2))] += CHSV(colour, 255, myfade/step*2);
        step ++;                                                      // Next step.
        break;
  } // switch step
} // ripple()

int wrap(int step) {
  if(step < 0) return NUM_LEDS + step;
  if(step > NUM_LEDS - 1) return step - NUM_LEDS;
  return step;
} // wrap()


enum { SteadyDim, GettingBrighter, GettingDimmerAgain };
uint8_t PixelState[NUM_LEDS];

void InitPixelStates()
{
  memset( PixelState, sizeof(PixelState), SteadyDim); // initialize all the pixels to SteadyDim.
  fill_solid( leds, NUM_LEDS, BASE_COLOR);
}

void OneColour()
{
    CRGB black  = CRGB::Black;
    CRGB color   = CRGB(Red,Green,Blue);
    currentPalette = CRGBPalette16(
                                   color,  black,  black,  black,
                                   black, black, black,  black,
                                   color,  black,  black,  black,
                                   black, black, black,  black );
}


void OneColourlots()
{
    CRGB black  = CRGB::Black;
    CRGB color   = CRGB(Red,Green,Blue);
    currentPalette = CRGBPalette16(
                                   color,  black,  black,  black,
                                   color, black, black,  black,
                                   color,  black,  black,  black,
                                   color, black, black,  black );
}

void OneColourlotss()
{
    CRGB black  = CRGB::Black;
    CRGB color   = CRGB(Red,Green,Blue);
    currentPalette = CRGBPalette16(
                                   color,  black,  color,  black,
                                   color, black, color,  black,
                                   color,  black,  color,  black,
                                   color, black, color,  black );
}

void TwinkleMapPixels()
{
  for( uint16_t i = 0; i < NUM_LEDS; i++) {
    if( PixelState[i] == SteadyDim) {
      // this pixels is currently: SteadyDim
      // so we randomly consider making it start getting brighter
      if( random8() < CHANCE_OF_TWINKLE) {
        PixelState[i] = GettingBrighter;
      }

    } else if( PixelState[i] == GettingBrighter ) {
      // this pixels is currently: GettingBrighter
      // so if it's at peak color, switch it to getting dimmer again
      if( leds[i] >= PEAK_COLOR ) {
        PixelState[i] = GettingDimmerAgain;
      } else {
        // otherwise, just keep brightening it:
        leds[i] += DELTA_COLOR_UP;
      }

    } else { // getting dimmer again
      // this pixels is currently: GettingDimmerAgain
      // so if it's back to base color, switch it to steady dim
      if( leds[i] <= BASE_COLOR ) {
        leds[i] = BASE_COLOR; // reset to exact base color, in case we overshot
        PixelState[i] = SteadyDim;
      } else {
        // otherwise, just keep dimming it down:
        leds[i] -= DELTA_COLOR_DOWN;
      }
    }
  }
}
void Musicpixels() {
  int x = analogRead(0);
  x = x * sensitivity;
    if (x < 71) {
      leds[(NUM_LEDS/2)] = CRGB(255, 0, 0);
    }
    else if (x > 71 && x <= 142) {
      leds[(NUM_LEDS/2)] = CRGB(255, 154, 0);
    }
    else if (x > 142 && x <= 213) {
      leds[(NUM_LEDS/2)] = CRGB(255, 255, 0);
    }
    else if (x > 213 && x <= 284) {
      leds[(NUM_LEDS/2)] = CRGB(0, 255, 0);
    }
    else if (x > 284 && x <= 355) {
      leds[(NUM_LEDS/2)] = CRGB(0, 0, 255);
    }
    else if (x > 355 && x <= 426) {
      leds[(NUM_LEDS/2)] = CRGB(150, 102, 255);
    }
    else {
      leds[(NUM_LEDS/2)] = CRGB(255, 0, 255);
    }

  FastLED.show();
  delay(10);
  for (int z = NUM_LEDS; z > (NUM_LEDS/2); z--) {
    leds[z] = leds[z - 1];
  }
  for (int z = 0; z < (NUM_LEDS/2); z++) {
    leds[z] = leds[z + 1];
  }
}
