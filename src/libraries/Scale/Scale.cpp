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

void Scale::setPortionSize(float grams)
{
  currentPortionSize = grams;
}

float Scale::getPortionSize()
{
  return currentPortionSize;
}

void Scale::setPricePerGram(float pricePerGram)
{
  currentPricePerGram = pricePerGram;
}

float Scale::getPricePerGram()
{
  return currentPricePerGram;
}

float Scale::getTotalPrice()
{
  return getTotalWeight() * getPricePerGram();
}

float Scale::getTotalPortions()
{
  return getTotalWeight() * getPortionSize();
}