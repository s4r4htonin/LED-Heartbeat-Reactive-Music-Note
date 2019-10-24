#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(62, 6, NEO_GRB + NEO_KHZ800);

struct Color { //John is the best for writing this struct
  byte r;
  byte g;
  byte b;
  Color (byte R, byte G, byte B) {
    r = R;
    g = G;
    b = B;
  }
  Color () {
    r = 255;
    g = 255;
    b = 255;
  }
  Color(byte hue) {
    hue = 255 - hue;
    if(hue < 85) {
      r = 255 - hue * 3;
      g = 0;
      b = hue * 3;
    } else if(hue < 170) {
      hue -= 85;
      r = 0;
      g = hue * 3;
      b = 255 - hue * 3;
    } else {
      hue -= 170;
      r = hue * 3;
      g = 255 - hue * 3;
      b = 0;
    }
  }
  void add (Color c2) {
    int r1 = r, g1 = g, b1 = b;
    r1 += c2.r;
    g1 += c2.g;
    b1 += c2.b;

    if (r1 > 255) r1 = 255;
    if (g1 > 255) g1 = 255;
    if (b1 > 255) b1 = 255;

    r = r1;
    g = g1;
    b = b1;
  }
};

//Definitions  
const int HR_RX = 2;
byte oldSample, sample;
Color currentColor;
byte position = 0;
float dimAmount = .04;

void setup () {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // colorWipe(strip.Color(0, 0, 10), 50); 
  
  Serial.begin(9600);
  pinMode (HR_RX, INPUT);  //Signal pin to input  
  
  Serial.println("Waiting for heart beat...");

  // Wait until a heart beat is detected  
  while (!digitalRead(HR_RX)) {};
  Serial.println ("Heart beat detected!");
}

void loop () {
  currentColor = Color(position);
  
  // change position over time
  position += 1; // change position every call to loop()
  if (position > 255) { // if past max
    position = 0; // reset (color wheel has made a full "revolution")
  }

  // dim over time
  dimAmount -= 0.1;
  if (dimAmount < .04) { // below minimum?
    dimAmount = .04; // force to be at minimum
  }

  // check for heartbeat
  sample = digitalRead(HR_RX);  //Store signal output 
  if (sample && (oldSample != sample)) {
    dimAmount = .8; // reset dimmer
  }
  oldSample = sample;           //Store last signal received 

  Color dimColor = currentColor;
  float r = dimColor.r;
  float g = dimColor.g;
  float b = dimColor.b;

  r *= dimAmount;
  g *= dimAmount;
  b *= dimAmount;

  dimColor.r = r;
  dimColor.g = g;
  dimColor.b = b;

  for (volatile int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(dimColor.r, dimColor.g, dimColor.b));
  }
  strip.show();
  delay(5);
}


// Fill the dots one after the other with a color
void colorWipe (uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void heartBeat () {
  Serial.println("heartbeat");
  for (volatile int i=0; i<strip.numPixels(); i++){
    //Serial.println(strip.getPixelColor(i));
    strip.setPixelColor(i, strip.Color(currentColor.r, currentColor.g, currentColor.b)); // switch to bright mode
    //Serial.println(strip.getPixelColor(i));
  }
  strip.show();
  delay(150);
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}