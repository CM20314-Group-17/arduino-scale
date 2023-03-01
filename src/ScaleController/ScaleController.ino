  #include <LiquidCrystal_I2C.h>
#include <string.h>
#include <PN532.h>
#include <PN532_SPI.h>
#include <NfcAdapter.h>


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
// LCD SETUP
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display
// button setup
int zero_pin = 2;
int right_pin = 4;

//NFC SETUP
PN532_SPI interface(SPI, 10); 
NfcAdapter nfc = NfcAdapter(interface); 


//Environment variables
char current_name_code[13];
float current_price; //price per_kg
float current_portions; //portions per_kg
float weight_bias = 0;  //Set when we zero, then take away from all weight measurements
int this_item = 0;
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
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), zero_it, FALLING);
  pinMode(4,INPUT);
  attachInterrupt(digitalPinToInterrupt(3),next_item, FALLING);
  

  //Variable initalisation
  strcpy(current_name_code, "NAME  CODE  ");

  //NFC
  nfc.begin();


}

void loop() {
  writelcd();
  readNFC();
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
  dtostrf(current_price,5,2,output_value);
  lcd.print(output_value);

  lcd.setCursor(0,3);
  lcd.print("Total:");
  lcd.write(byte(3));
  lcd.print(" ");
  //FORMAT TOTAL
  dtostrf(total,5,2,output_value);
  lcd.print(output_value);

  //Name code
  lcd.setCursor(14,0);
  for (int x  =0; x < 12; x ++){
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
  }
}

int current_weight(){
  //
}


void next_item() {
  //move to the right
  if ((millis() - debounce_time_right) > 20){ //might need to change this based on physical config
    prices[this_item] = current_price;
    //check if we are at last item
    if (this_item < 32){
      this_item++;
      strcpy(current_name_code, "SELECTITEM  ");
      total = total + current_price;
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
      char characterbuffer[13];
      
      int current = 0;  //current value we are editing
      //This goes through each value, portions, price, name code and sets the values  
      for (int x = 3; x < (payloadLength); x++) {
        if ((char)payload[x] == '_') {
          characterbuffer;
          current_thing.toCharArray(characterbuffer, 13);
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
