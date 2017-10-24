//flower.ino
#include <FastLED.h>
#define CENTER_LED_PIN     6
#define NUM_LEDS    608

// wind sensor pins 
#define analogPinForRV    1   
#define analogPinForTMP   0


#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define BRIGHTNESS  255

#define UPDATES_PER_SECOND 100

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


CRGB circle_leds[NUM_LEDS];




CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
	delay( 3000 ); // power-up safety delay
	
	  Serial.begin(57600);   // faster printing to get a bit better throughput on extended info
	  // remember to change your serial monitor

    FastLED.addLeds<LED_TYPE, CENTER_LED_PIN, COLOR_ORDER>(circle_leds, NUM_LEDS).setCorrection( TypicalLEDStrip );    
    FastLED.setBrightness(  BRIGHTNESS );
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

void loop() {  
  ReadWind();
  uint8_t brightness =  map(WindSpeed_MPH, 0, 20, 100, 255);
  CRGB color = ColorFromPalette( currentPalette,  map(WindSpeed_MPH, 0, 20, 0, 255), brightness, currentBlending);
  for (int i = 0; i <= NUM_LEDS; ++i ) {
        circle_leds[i] = color;
  }  	
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}


void ReadWind () {
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
   
}
