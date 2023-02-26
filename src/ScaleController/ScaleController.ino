#include <LiquidCrystal_I2C.h>
#include <string.h>
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
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display
// button setup
int zero_pin = 2;
int right_pin = 4;

//Environment variables
char current_name_code[12];
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
  lcd.createChar(0, gbpSign);
 

  //BUTTONS
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), zero_it, RISING);
  pinMode(4,INPUT);
  attachInterrupt(digitalPinToInterrupt(3),next_item, RISING);
  //NFC STUFF


  //Variable initalisation
  strcpy(current_name_code, "NAME  CODE  ");


}

void loop() {
  writelcd();
}

void writelcd(){
  lcd.setCursor(0,0); //character x on line y
  lcd.print("Weight: ");
  //FORMAT WEIGHT
  char output_value[5]; 
  dtostrf(current_weight(),5,2,output_value);
  lcd.print(output_value);

  lcd.setCursor(0,1);
  lcd.print("Portion:");
  //FORMAT PORTIONS
  dtostrf(current_portions,5,2,output_value);
  lcd.print(output_value);
  lcd.setCursor(0,2);

  //pound sign dosen't working using placehodler, implement custom pound sign here??
  lcd.print("Price:");
  lcd.write(byte(0));
  lcd.print(" ");
  //FORMAT PRICE
  dtostrf(current_price,5,2,output_value);
  lcd.print(output_value);

  lcd.setCursor(0,3);
  lcd.print("Total:");
  lcd.write(byte(0));
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

void read_nfc(){
  //GET VALUE FROM NFC
  char input[] = "NAME CODE"; // TODO GET VALUE FROM NFC
  strcpy(current_name_code, input);
}

void next_item() {
  //move to the right
  if ((millis() - debounce_time_right) > 1000){ //might need to change this based on physical config
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
