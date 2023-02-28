#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String tagId = "None";
byte nuidPICC[4];

String current_price; //price per_kg
String current_portions; //portions per_kg
String current_name_code;

void setup(void) {
  Serial.begin(9600);
  Serial.println("System initialized");
  nfc.begin();
}

void loop() {
  readNFC();
}

void readNFC() {
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    if (tag.hasNdefMessage()) {

      //based on https://github.com/don/NDEF/blob/master/examples/ReadTagExtended/ReadTagExtended.ino#L68-L75, Don Coleman - 08/2013 accesed: 26/02/2023
      NdefMessage message = tag.getNdefMessage();
      //message.print();
      NdefRecord record = message.getRecord(0);  //todo change this if we have more than one record?
      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);
      //String payloadAsString = "";

      //for (int c = 3; c < payloadLength; c++) {
      //payloadAsString += (char)payload[c];
      // }
      //Serial.println(payloadAsString);
      String current_thing = "";
      int current = 0;  //current value we are editing
      for (int x = 3; x < payloadLength; x++) {
        if ((char)payload[x] == '_') {
          if (current == 0) {
            current_portions = current_thing;
          }
          if (current == 1) {
            current_price = current_thing;
          }
          if (current == 2) {
            current_name_code = current_thing;
          }
          current_thing = "";
          current ++;
        }
      else{
          current_thing += (char)payload[x];        
      }
      }

      //atoi!!!
    Serial.println(current_price);
    Serial.println(current_portions);
    Serial.println(current_name_code);
    Serial.println(current_thing);
    delay(5000);

    }
  }
}
