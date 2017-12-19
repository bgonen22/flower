#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

#define NumOfPixels 618
#define analogPinForRV    A1   // change to pins you the analog pins are using
#define analogPinForTMP   A0

// to calibrate your sensor, put a glass over it, but the sensor should not be
// touching the desktop surface however.
// adjust the zeroWindAdjustment until your sensor reads about zero with the glass over it. 

const float zeroWindAdjustment =  .2; // negative numbers yield smaller wind speeds and vice versa.

int TMP_Therm_ADunits;  //temp termistor value from wind sensor
float RV_Wind_ADunits;    //RV output from wind sensor 
float RV_Wind_Volts;
unsigned long lastMillis;
int TempCtimes100;
float zeroWind_ADunits;
float zeroWind_volts;
float WindSpeed_MPH;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NumOfPixels, PIN, NEO_GRB + NEO_KHZ800);
float DC = 0;
void setup() {

  Serial.begin(57600);   // faster printing to get a bit better throughput on extended info
  // remember to change your serial monitor

  Serial.println("start");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {
 // if (DC == 0) {   
 //  DC = GetDC();
 // }   
   float WindSpeed_MPH = getWindSpeed();
   
   WindSpeed_MPH -= DC;
   while (WindSpeed_MPH < 0) {
      WindSpeed_MPH = getWindSpeed();
      DC = GetDC();
      WindSpeed_MPH -= DC;   
      Serial.print(" DC ");
      Serial.println((float)DC);
   }
   Serial.print("   WindSpeed MPH ");
   Serial.println((float)WindSpeed_MPH);
   Serial.print(" DC ");
   Serial.println((float)DC);
   
  //  Serial.print("   WindSpeed MPH - DC ");
  //  Serial.println((float)WindSpeed_MPH);
   
   /*
    Serial.print("  TMP volts ");
    Serial.print(TMP_Therm_ADunits * 0.0048828125);
    
    Serial.print(" RV volts ");
    Serial.print((float)RV_Wind_Volts);

    Serial.print("\t  TempC*100 ");
    Serial.print(TempCtimes100 );

    Serial.print("   ZeroWind volts ");
    Serial.print(zeroWind_volts);
*/
  //if (WindSpeed_MPH < 4) {
//    WindSpeed_MPH = 0;
//  }
    Serial.print("   WindSpeed MPH ");
    Serial.println((float)WindSpeed_MPH);
    
    int power = map(WindSpeed_MPH, 4, 10, 0, 255);
  //  Serial.print("power ");
  //  Serial.println(power);

    for(int i=0; i< NumOfPixels; i++) {
      uint32_t color = Wheel(i,power);
     // Serial.print("color ");
   // Serial.println(color);
      strip.setPixelColor(i, Wheel(i,power));
    }
    strip.show();
    delay(150);

}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos, int power) {

  return strip.Color(power, 0, 0);
/*  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  */
}
float GetDC() {
    float sum;
    for (int i =0 ; i<10 ; i++) {
        float WindSpeed_MPH = getWindSpeed();
    //    Serial.print("   WindSpeed MPH ");
        //Serial.println((float)WindSpeed_MPH);
        if (WindSpeed_MPH == WindSpeed_MPH) {
          sum += WindSpeed_MPH;
        }
        //Serial.print("sum ");
        //Serial.println(sum);
      }
  
  float DC = sum/10;
  Serial.print("sum ");
  Serial.println(sum);

  return DC;

}


float getWindSpeed() {
  TMP_Therm_ADunits = analogRead(analogPinForTMP);
    RV_Wind_ADunits = analogRead(analogPinForRV);
    RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);

    // these are all derived from regressions from raw data as such they depend on a lot of experimental factors
    // such as accuracy of temp sensors, and voltage at the actual wind sensor, (wire losses) which were unaccouted for.
    TempCtimes100 = (0.005 *((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;  

    zeroWind_ADunits = -0.0006*((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172;  //  13.0C  553  482.39

    zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;  

    // This from a regression from data in the form of 
    // Vraw = V0 + b * WindSpeed ^ c
    // V0 is zero wind at a particular temperature
    // The constants b and c were determined by some Excel wrangling with the solver.
    
   WindSpeed_MPH =  pow(((RV_Wind_Volts - zeroWind_volts) /.2300) , 2.7265);   
   return WindSpeed_MPH;
}
