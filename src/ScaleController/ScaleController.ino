#include <LiquidCrystal_I2C.h>
#include <string.h>

// LCD SETUP
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F for a 16 chars and 2 line display
// button setup
int zero_pin = 2;
int right_pin = 4;

//Environment variables
char current_name_code[12];
int current_price; //price per_kg
int current_portions; //portions per_kg
int weight_bias = 0;  //Set when we zero, then take away from all weight measurements
int this_item = 0;

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

void setup() {
  //LCD STUFF
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on

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
  lcd.print("Weight:");
  lcd.setCursor(0,1);
  lcd.print("Portions:");
  lcd.setCursor(0,2);
  //pound sign dosen't working using placehodler, implement custom pound sign here??
  lcd.print("Price:");
  lcd.write(byte(0));
  lcd.setCursor(0,3);
  lcd.print("Total:");
  lcd.write(byte(0));

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
  weight_bias = current_weight(); 
}

int current_weight(){
  //
}

void read_nfc(){
  //GET VALUE FROM NFC
  char input[] = "NAME CODE"; // GET VALUE FROM NFC
  strcpy(current_name_code, input);
}

void next_item() {
  //move to the right 
  this_item ++;

}


void item_before(){

}