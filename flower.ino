//flower.ino
#include <FastLED.h>
#define CENTER_LED_PIN     6
#define CENTER_NUM_LEDS    608
#define LEAF_LED_PIN     5
#define LEAF_NUM_LEDS    379 // ?

#define LED_TYPE    WS2811
#define COLOR_ORDER GRB

#define BRIGHTNESS  255

#define UPDATES_PER_SECOND 100
// the last pixel of each circle
int circle[] = {-1,17,45,82,129,185,251,326,410,504,607}; // circle 0 is -1 so always the start pixel is the previus + 1

// the last pixel of each leaf
int leaf[] = {-1,36,74,112,150,188,226,264,302,340,378}; //leaf 0 is -1 so always the start pixel is the previus + 1
 

CRGB circle_leds[CENTER_NUM_LEDS];
CRGB leaf_leds[LEAF_NUM_LEDS];



CRGBPalette16 currentPalette;
TBlendType    currentBlending;

extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void setup() {
	delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, CENTER_LED_PIN, COLOR_ORDER>(circle_leds, CENTER_NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.addLeds<LED_TYPE, LEAF_LED_PIN, COLOR_ORDER>(leaf_leds, LEAF_NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

void loop() {
	ChangePalettePeriodically();
	static uint8_t startIndex = 0;
    startIndex = startIndex + 1; /* motion speed */
    
    FillLEDsFromPaletteColors( startIndex);
    FastLED.show();
    FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 1; i <= 10; i++) {
    	CRGB color = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    	lightCircle(i,color);    	
    	lightLeaf(i,color);
    	colorIndex += 3;    
    }
}

void lightLeaf(int leaf_num,CRGB color) {
	for (int i = leaf[leaf_num-1] + 1; i <= leaf[leaf_num]; ++i ) {
		leaf_leds[i] = color;
	}
}
void lightCircle(int circle_num,CRGB color) {
	for (int i = circle[circle_num-1] + 1; i <= circle[circle_num]; ++i ) {
		circle_leds[i] = color;
	}
}
// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};


