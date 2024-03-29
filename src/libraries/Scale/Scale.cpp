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

void Scale::setPortionsPerKG(float portions)
{
  currentPortionsPerKG = portions;
}

float Scale::getPortionsPerKG()
{
  return currentPortionsPerKG;
}

void Scale::setPricePerKG(float price)
{
  currentPricePerKG = price;
}

float Scale::getPricePerKG()
{
  return currentPricePerKG;
}

float Scale::getTotalPrice()
{
  return getTotalWeight() * getPricePerKG() * 0.01; //Should be pprice per 100g #todo 
}

float Scale::getTotalPortions()
{
  return ((getTotalWeight() / getPortionsPerKG())) ;
}

