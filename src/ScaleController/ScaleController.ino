#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <PN532.h>
#include <PN532_SPI.h>
#include <NfcAdapter.h>
#include <Scale.h>

// LCD SETUP
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display

// SCALE
Scale scale(6, 5);

// CONSTANTS
const int NAMECODE_LENGTH = 13;
const int ZERO_PIN = 2;
const int RIGHT_PIN = 3;
const int DEBOUNCE_TIME = 100;
const int NFC_READ_FREQUENCY = 20;  // do nfc read every this many loops as it's very slow
const int TARE_COOLDOWN = 15; // loops until you can tare again
byte gbpSign[8] = {
	0b00110,
	0b01001,
	0b01000,
	0b01000,
	0b11110,
	0b01000,
	0b01000,
	0b11111
};


//NFC SETUP
PN532_SPI interface(SPI, 10); 
NfcAdapter nfc = NfcAdapter(interface); 


//Environment variables
char current_name_code[NAMECODE_LENGTH];
float current_price; //price per_kg
float current_portions; //portions per_kg
float weight_bias = 0;  //Set when we zero, then take away from all weight measurements
int this_item = 0;
int nfc_read_timer = 0; // timer for when nfc read is called
int tare_timer = 0;
int prices[32];
float total;


unsigned long debounce_time_right = 0; //Long because we need space to store millis
unsigned long debounce_time_zero = 0; // ^^ 


void setup() {
  //LCD STUFF
  lcd.init();
  lcd.clear();         
  lcd.backlight();     
  lcd.createChar(3, gbpSign);
 

  //BUTTONS
  pinMode(ZERO_PIN, INPUT);
  // zeroing the scale during an interrupt does not work - now it just simply checks the button in loop()
  pinMode(RIGHT_PIN,INPUT);
  attachInterrupt(digitalPinToInterrupt(RIGHT_PIN),next_item, RISING);


  //Variable initalisation
  strcpy(current_name_code, "NAME  CODE  ");

  //NFClcd.setCursor(3, 0);
  nfc.begin();

  // START HX711 AND DO TARE
  displayTaringSequence();

  scale.begin();
  
  lcd.clear();
}

void loop() {
  writelcd();
  readZeroButtonLoop();
  readNFCLoop();
  scale.setPortionsPerKG(current_portions);
  scale.setPricePerKG(current_price);
}

void readNFCLoop() {
  if (nfc_read_timer == 0) {  
    readNFC();
    nfc_read_timer = NFC_READ_FREQUENCY;
  }
  nfc_read_timer--;
}

void readZeroButtonLoop() {
  if (tare_timer == 0) {
    if (digitalRead(ZERO_PIN) == HIGH) {
      tare();
      tare_timer = TARE_COOLDOWN;
    }
  } else {
    tare_timer--;
  }
}

void tare() {
  displayTaringSequence();

  scale.tare();

  lcd.clear();
}


void writelcd(){

  //Weight
  lcd.setCursor(0,0); //character x on line y
  lcd.print("Weight: ");
  //FORMAT WEIGHT
  char output_value[5]; 
  dtostrf(current_weight(),5,2,output_value);
  lcd.print(output_value);

  //Portions
  lcd.setCursor(0,1);
  lcd.print("Portion:");
  //FORMAT PORTIONS
  dtostrf(current_portions,5,2,output_value);
  lcd.print(output_value);
  lcd.setCursor(0,2);

  lcd.print("Price:");
  lcd.write(byte(3));
  lcd.print(" ");
  //FORMAT PRICE
  dtostrf(/*current_price*/(int)digitalRead(RIGHT_PIN),5,2,output_value);
  lcd.print(output_value);

  lcd.setCursor(0,3);
  lcd.print("Total:");
  lcd.write(byte(3));
  lcd.print(" ");
  //FORMAT TOTAL
  dtostrf(/*total*/(int)digitalRead(ZERO_PIN),5,2,output_value);
  lcd.print(output_value);

  //Name code
  lcd.setCursor(14,0);
  for (int x  =0; x < NAMECODE_LENGTH-1; x ++){
    if (x < 6){
      lcd.print(current_name_code[x]);
    }
    else if (x ==6){
      lcd.setCursor(14,1);
      lcd.print(current_name_code[x]);
    }
    else{
      lcd.print(current_name_code[x]);
    }
  }
  
  //AMOUNT THROUGH
  lcd.setCursor(14,3);
  if (this_item < 10){
    lcd.print("0");
  }
  lcd.print(this_item);
  lcd.print("/");
  lcd.print("32");
}


void zero_it() {
  //set weight to zero
  if ((millis() - debounce_time_zero) > 1000){ //might need to change this based on physical config
    int weight = current_weight(); 
    //check validity of container weight
    if (weight_bias > 1000){ //todo set value for acceptable container weight
      weight_bias = weight;
    }
    debounce_time_right = millis();
    //tare();
  }
}

int current_weight(){
  return (int)scale.getTotalWeight(); // scale resolution is less than a gram so returns float. should we round to int?
}


void next_item() {
  //move to the right
  if ((millis() - debounce_time_right) > DEBOUNCE_TIME){ //might need to change this based on physical config
    prices[this_item] = current_price;
    //check if we are at last item
    if (this_item < 32){
      this_item++;
      strcpy(current_name_code, "SELECTITEM  ");
      total = total + current_price;
      current_price = 0.0;
      current_portions = 0.0;
    }
    debounce_time_right = millis();
  
  }
}


void item_before(){
  //go back
}

void delete_item(){
  //delete current item
}


void readNFC() {
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    
    if (tag.hasNdefMessage()) {

      //based on https://github.com/don/NDEF/blob/master/examples/ReadTagExtended/ReadTagExtended.ino#L68-L75, Don Coleman - 08/2013 accesed: 26/02/2023
      //get messages
      NdefMessage message = tag.getNdefMessage();
      NdefRecord record = message.getRecord(0);  //todo change this if we have more than one record?
      //get length of message and set variables
      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);
      String current_thing = "";
      char characterbuffer[NAMECODE_LENGTH];
      
      int current = 0;  //current value we are editing
      //This goes through each value, portions, price, name code and sets the values  
      for (int x = 3; x < (payloadLength); x++) {
        if ((char)payload[x] == '_') {
          characterbuffer;
          current_thing.toCharArray(characterbuffer, NAMECODE_LENGTH);
          if (current == 0) {
            
            current_portions = atof(characterbuffer);
          }
          if (current == 1) {
            current_price = atof(characterbuffer);
          }
          if (current == 2) {
            strcpy(current_name_code,characterbuffer);
          }
          current_thing = "";
          current ++;
        }
      else{
          current_thing += (char)payload[x];        
      }
      }


    }
  }
}

void displayTaringSequence() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Zeroing");
  delay(100);
  lcd.setCursor(10, 0);
  lcd.print(".");
  delay(100);
  lcd.setCursor(11, 0);
  lcd.print(".");
  delay(100);
  lcd.setCursor(12, 0);
  lcd.print(".");
  delay(100);
  lcd.setCursor(13, 0);
  lcd.print(".");
  delay(100);
}