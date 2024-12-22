/*Created by: Ben Makes Everything - youtube.com/@benmakeseverything*/
#include <Adafruit_NeoPixel.h>

#define dataPin 3      //Neopixel control dataPin
#define NUM_PANELS 10  //Number of display panels chained together

int numPixels = NUM_PANELS * 38;  // Total number of NeoPixels - 38 per display

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numPixels, dataPin, NEO_RGB + NEO_KHZ800);

const byte numChars = 11;  //MAX ammount of chars that can be input = #displays + 1
char myData[numChars];     // an array to store the received data
boolean newData = false;   // for checking serial

int digit = 0;

//Byte data array for all characters, follows pixel order from PCB
const uint8_t digitSegments[][5] = {
  { 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 }, /*SPACE*/
  { 0b00000011, 0b11101111, 0b10111101, 0b11101111, 0b10111000 }, /*0*/
  { 0b00000011, 0b11110000, 0b00000000, 0b00000001, 0b11111000 }, /*1*/
  { 0b00001100, 0b01111111, 0b10111000, 0b01101111, 0b10110100 }, /*2*/
  { 0b00001111, 0b11101111, 0b10000000, 0b01101111, 0b10110100 }, /*3*/
  { 0b00001111, 0b11110000, 0b00000011, 0b11110001, 0b11111000 }, /*4*/
  { 0b00001111, 0b11101111, 0b10000011, 0b11111111, 0b11000000 }, /*5*/
  { 0b00001111, 0b11101111, 0b10111101, 0b11101111, 0b10000000 }, /*6*/
  { 0b00001100, 0b00000001, 0b11111000, 0b01111111, 0b11110100 }, /*7*/
  { 0b00001111, 0b11101111, 0b10111011, 0b11101111, 0b10110100 }, /*8*/
  { 0b00001111, 0b11101111, 0b10000011, 0b11101111, 0b10111100 }, /*9*/
  { 0b00001111, 0b11110001, 0b11111101, 0b11101111, 0b10111000 }, /*A*/
  { 0b00001111, 0b11101111, 0b11111101, 0b11111111, 0b10110100 }, /*B*/
  { 0b00000000, 0b01101111, 0b10111101, 0b11101111, 0b10000000 }, /*C*/
  { 0b00000011, 0b11101111, 0b11111101, 0b11111111, 0b10111000 }, /*D*/
  { 0b00001100, 0b01111111, 0b11111101, 0b11111111, 0b11000000 }, /*E*/
  { 0b00001100, 0b00000001, 0b11111101, 0b11111111, 0b11000000 }, /*F*/
  { 0b00000011, 0b11111111, 0b10111101, 0b11101111, 0b10000000 }, /*G*/
  { 0b00001111, 0b11110001, 0b11111101, 0b11110001, 0b11111000 }, /*H*/
  { 0b00000011, 0b11110000, 0b00000000, 0b00000001, 0b11111000 }, /*I*/
  { 0b00000011, 0b11101111, 0b10000000, 0b00000001, 0b11111000 }, /*J*/
  { 0b00001111, 0b11110001, 0b11111101, 0b11110000, 0b00110100 }, /*K*/
  { 0b00000000, 0b01111111, 0b11111101, 0b11110000, 0b00000000 }, /*L*/
  { 0b11111111, 0b11110001, 0b11111111, 0b11110001, 0b11111100 }, /*M*/
  { 0b00000011, 0b11110001, 0b11111101, 0b11111111, 0b10111000 }, /*N*/
  { 0b00000011, 0b11101111, 0b10111101, 0b11101111, 0b10111000 }, /*O*/
  { 0b00001100, 0b00000001, 0b11111101, 0b11111111, 0b10110100 }, /*P*/
  { 0b00000011, 0b11111111, 0b10111101, 0b11101111, 0b10111000 }, /*Q*/
  { 0b00001111, 0b11110001, 0b11111101, 0b11111111, 0b10110100 }, /*R*/
  { 0b00001111, 0b11101111, 0b10000011, 0b11101111, 0b10000000 }, /*S*/
  { 0b00000011, 0b11110000, 0b00000000, 0b01111111, 0b11111000 }, /*T*/
  { 0b00000011, 0b11111111, 0b11111101, 0b11110001, 0b11111000 }, /*U*/
  { 0b00000011, 0b11101111, 0b10111101, 0b11110001, 0b11111000 }, /*V*/
  { 0b11110011, 0b11111111, 0b11111101, 0b11110001, 0b11111000 }, /*W*/
  { 0b00001111, 0b11110001, 0b11111011, 0b11110001, 0b11110100 }, /*X*/
  { 0b00001111, 0b11101111, 0b10000011, 0b11110001, 0b11111100 }, /*Y*/
  { 0b00001100, 0b01111111, 0b11111000, 0b01111111, 0b11110100 }, /*Z*/
  { 0b00000000, 0b00000000, 0b01000000, 0b00000000, 0b00000000 }, /*.*/
  { 0b00001100, 0b00000000, 0b01111000, 0b01101111, 0b10110100 }, /*?*/
  { 0b00000000, 0b00000000, 0b01111101, 0b11110000, 0b00000000 }, /*!*/
  { 0b00001100, 0b00000000, 0b00000000, 0b00000000, 0b00000000 }  /*-*/
};

//This determines the color to be used in each panel
//you can set the all to the same for mono-color text, or define individually
const uint32_t digitColors[10] = {
  pixels.Color(225, 30, 0), //Panel 1
  pixels.Color(225, 30, 0), //Panel 2
  pixels.Color(225, 30, 0), //Panel 3
  pixels.Color(225, 30, 0), //Panel 4
  pixels.Color(225, 30, 0), //Panel 5
  pixels.Color(225, 30, 0), //Panel 6
  pixels.Color(225, 30, 0), //Panel 7
  pixels.Color(225, 30, 0), //Panel 8
  pixels.Color(225, 30, 0), //Panel 9
  pixels.Color(225, 30, 0), //Panel 10
};

/*For quick reference - some colors/values:
//Reds
(255, 0, 0),      //Pure Red
(200, 0, 55),     //Hot Pink
(245, 0, 10),     //Salmon
(225, 30, 0),     //Orange
(155, 100, 0),    //bright yellow
(180, 75, 0),     //Gold
(0, 255, 0),      //Pure Green
(55, 200, 0),     //Lime
(0, 225, 30)      //Teal
(0, 180, 75),     //Cyan
(0, 180, 75),     //Sky Blue
(0, 0, 255),      //Pure Blue
(55,0,200),       //Purple
(85, 85, 85),     //white - brightness balanced w/ other colors
(255, 255, 255),  //white - Full brightness
*/

void setup() {
  pixels.begin();  // Initialize the NeoPixel library
  pixels.show();   // Initialize all pixels to 'off'
  Serial.begin(9600);
}

void loop() {
  receiveData();
  showNewData();
  determineDigit();
  pixels.show();  // Update the NeoPixels to show the new states
  delay(100);     // Delay before showing the next digit
}

//This figures out what pixels to turn on based on the input character
void determineDigit() {
  for (int panNum = 0; panNum < NUM_PANELS; panNum++) {
    if (strlen(myData) > 0) {
      switch (tolower(myData[panNum])) {  // Convert the character to lowercase before checking
        case '0':
          digit = 1;
          break;
        case '1':
          digit = 2;
          break;
        case '2':
          digit = 3;
          break;
        case '3':
          digit = 4;
          break;
        case '4':
          digit = 5;
          break;
        case '5':
          digit = 6;
          break;
        case '6':
          digit = 7;
          break;
        case '7':
          digit = 8;
          break;
        case '8':
          digit = 9;
          break;
        case '9':
          digit = 10;
          break;
        case 'a':
          digit = 11;
          break;
        case 'b':
          digit = 12;
          break;
        case 'c':
          digit = 13;
          break;
        case 'd':
          digit = 14;
          break;
        case 'e':
          digit = 15;
          break;
        case 'f':
          digit = 16;
          break;
        case 'g':
          digit = 17;
          break;
        case 'h':
          digit = 18;
          break;
        case 'i':
          digit = 19;
          break;
        case 'j':
          digit = 20;
          break;
        case 'k':
          digit = 21;
          break;
        case 'l':
          digit = 22;
          break;
        case 'm':
          digit = 23;
          break;
        case 'n':
          digit = 24;
          break;
        case 'o':
          digit = 25;
          break;
        case 'p':
          digit = 26;
          break;
        case 'q':
          digit = 27;
          break;
        case 'r':
          digit = 28;
          break;
        case 's':
          digit = 29;
          break;
        case 't':
          digit = 30;
          break;
        case 'u':
          digit = 31;
          break;
        case 'v':
          digit = 32;
          break;
        case 'w':
          digit = 33;
          break;
        case 'x':
          digit = 34;
          break;
        case 'y':
          digit = 35;
          break;
        case 'z':
          digit = 36;
          break;
        case '.':
          digit = 37;
          break;
        case '?':
          digit = 38;
          break;
        case '!':
          digit = 39;
          break;
        case '-':
          digit = 40;
          break;
        default:
          digit = 0;
          break;
      }
    } else {
      digit = 0;
    }

    // Loop through the 5 bytes of each character (digit or letter)
    for (int byteIndex = 0; byteIndex < 5; byteIndex++) {
      uint8_t byteValue = digitSegments[digit][byteIndex];

      // Loop through each bit of the byte (8 bits per byte)
      for (int bitIndex = 0; bitIndex < 8; bitIndex++) {
        int pixelIndex = byteIndex * 8 + bitIndex;  // Calculate pixel index from byte and bit

        pixelIndex = pixelIndex + (panNum * 38);

        bool bitValue = (byteValue >> (7 - bitIndex)) & 1;  // Extract the bit value (1 or 0)

        // If the bit is 1, turn the pixel on; if it's 0, turn it off
        if (bitValue) {
          pixels.setPixelColor(pixelIndex, digitColors[panNum]);  //Set the LED to the correct color
        } else {
          pixels.setPixelColor(pixelIndex, pixels.Color(0, 0, 0));  // Turn off the LED
        }
      }
    }
  }
}

void receiveData() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      myData[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      myData[ndx] = '\0';  // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
  if (newData == true) {
    newData = false;
  }
}