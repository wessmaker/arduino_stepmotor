#include <LiquidCrystal_I2C.h>



#include <Key.h>
#include <Keypad.h>

#define ROWS 4 
#define COLS 3

byte rowPins[ROWS] = {30, 31, 32, 33}; 
byte colPins[COLS] = {40, 41, 42}; 

int a;
int b;
int i = 0;
int numberCount = 0;
int lastNumberCount = 0;
int numberPosition;

char value[3];
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


LiquidCrystal_I2C lcd(0x27, 16, 2);






void setup(){
  Serial.begin(9600);
  lcd.init();  
  lcd.backlight();
}
  
void loop(){
  

  a = 0;
  b = 0;

  lcd.setCursor(a, b);
  
  


  char keyPressed = keypad.getKey();
  
  if(isDigit(keyPressed) && !(keyPressed == '#') && numberCount <= 2){
    value[numberCount] = keyPressed;
    Serial.println(keyPressed);
    numberCount++;
  }



  if(keyPressed == '#'){
    updateLCD(value, numberCount);
    numberCount = 0;
    Serial.println();
    Serial.print(keyPressed);
  }



}




void updateLCD(char numbers[], int numberCount){

//Ensiksi poistaa vanhat ja sitten tallentaa nykyisen määrän numeroita lastNumberCounttiin
  while(i <= lastNumberCount){
    lcd.setCursor(i, 0);
    lcd.print(" ");
    i++;
  }
  i = 0;
  lastNumberCount = numberCount; 
  

  
  while(i < numberCount){     
    lcd.setCursor(i, 0);
    lcd.print(numbers[i]);
    i++;
  }
  i = 0;
}










/*
Lähteet koodeille:

LCD NÄYTÖSTÄ TIETOA: https://www.make-it.ca/i2c-lcd-display-on-arduino/

FUNKTIOT: https://www.arduino.cc/reference/en/

ARRAYS: https://www.arduino.cc/reference/en/language/variables/data-types/array/

LiquidCrystal_I2C lib: https://github.com/johnrickman/LiquidCrystal_I2C

Keypad:

#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey){
    Serial.println(customKey);
  }
}





*/