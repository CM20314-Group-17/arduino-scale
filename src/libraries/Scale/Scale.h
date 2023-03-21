#ifndef Scale_h
#define Scale_h
#include "HX711_ADC.h"
#include <Arduino.h>

class Scale
{
  public:
    Scale(uint8_t dout, uint8_t sck); // initialise Scale object with given pins on HX711
    void begin();                     // start HX711 with default settings and tare (blocking)
    void begin(float calFactor);      // start HX711 and tare, specifying calibration factor (blocking)

    float getTotalWeight();           // get detected weight in grams
    float getTotalPrice();            // get price of detected weight
    float getTotalPortions();         // get portions in detected weight

    void setGramsPerPortion(float portions); // specify portion size to use for calculations
    float getGramsPerPortion();           // get portion size currently being used for calculations
    void setPricePer100(float price); // specify price per gram to use for calculations
    float getPricePer100();                  // get price per gram currently being used for calculations

    void tare();                      // blocking tare
    void tareNoDelay();               // nonblocking tare
    bool getTareStatus();             // true if nonblocking tare is finished, false otherwise

  private:
    HX711_ADC* LoadCell;
    const float GRAMS_PER_OZ = 28.3495;
    const int DEFAULT_INIT_TIME = 1000;
    const float DEFAULT_CAL_FACTOR = 0.42;
    float currentGramsPerPortion = 1.0;
    float currentPricePer100 = 1.0;
};

#endif
