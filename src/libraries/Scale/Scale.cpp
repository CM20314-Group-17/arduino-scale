#include "Scale.h"
#include "HX711_ADC.h"
#include <Arduino.h>

Scale::Scale(uint8_t dout, uint8_t sck)
{
  LoadCell = new HX711_ADC(dout, sck);
}

void Scale::begin(float calFactor)
{
  LoadCell->begin();
  LoadCell->start(DEFAULT_INIT_TIME);
  LoadCell->setCalFactor(calFactor);
}

void Scale::begin()
{
  begin(DEFAULT_CAL_FACTOR);
}

void Scale::tare() {
  LoadCell->tare();
}

void Scale::tareNoDelay() {
  LoadCell->tareNoDelay();
}

bool Scale::getTareStatus() {
  return LoadCell->getTareStatus();
}

float Scale::getTotalWeight()
{
  LoadCell->update();
  return LoadCell->getData();
}

void Scale::setGramsPerPortion(float portions)
{
  currentGramsPerPortion = portions;
}

float Scale::getGramsPerPortion()
{
  return currentGramsPerPortion;
}

void Scale::setPricePer100(float price)
{
  currentPricePer100 = price;
}

float Scale::getPricePer100()
{
  return currentPricePer100;
}

float Scale::getTotalPrice()
{
<<<<<<< Updated upstream
  return (getTotalWeight() / 100) * getPricePer100();
=======
  return getTotalWeight() * getPricePerKG() * 0.1; //Should be pprice per 100g #todo 
>>>>>>> Stashed changes
}

float Scale::getTotalPortions()
{
<<<<<<< Updated upstream
  return getTotalWeight() / getGramsPerPortion();
}
=======
  return ((getTotalWeight() / getPortionsPerKG)) ;
}

>>>>>>> Stashed changes
