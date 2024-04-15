#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>

#define ROWS 4 
#define COLS 3





const char timerButton =  '1';
const char controlButton = '2';
const char pageButton = '0';
const char page1 = '1';
const char page2 = '2';
const char pageT = 'T';
const char pageC = 'C';


byte rowPins[ROWS] = {30, 31, 32, 33}; 
byte colPins[COLS] = {40, 41, 42}; 

int a;
int b;
int i;
int o;
int numberCount;
int lastNumberCount;
char page;

byte lastPage;



bool started;
bool changing;


bool isValuesOk;


char value[3];
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27, 16, 2);

char pageChars[4] = {page1, page2, pageT, pageC};
char buttonChars[3] = {timerButton, controlButton, pageButton};


void setup() {
  Serial.begin(9600);
  lcd.init();  
  lcd.backlight();
  

}



void loop() {
  

  if(!started){ //aloitus
    checkValues();
    if(isValuesOk){
      start();
    }else if(!isValuesOk){
      Serial.println("Check values and restart!");
    }
    
  }

  if(started){  // tämä heti startin jälkeen
    toInterface(page);
  }
  


  

}




void start(){
  lcd.setCursor(0, 0);
  lcd.print("Tervetuloa!");
  delay(1000);
  page = '1';
  started = true;
  changing = true;
}



void toInterface(byte page){
  changeDelay();
  switch(page){
    case '1':
      Serial.println("PAGE 1!");
      Serial.println();
      toPage1();
      break;
    case '2':
      Serial.println("PAGE 2!");
      Serial.println();
      toPage2();
      break;
    case pageT:
      Serial.println("PAGE T!");
      Serial.println();
      toTime();
      break;
    case pageC:
      Serial.println("PAGE C!");
      Serial.println();
      toControl();
      break;
  }
  
  

}



void toPage1(){
  changeDelay();
  char keyPressed = keypad.getKey();
  
  page = '1';
  pageInfo();
  
  if(keyPressed == pageButton){
    changing = true;
    toPage2();
  }
  if(keyPressed == timerButton){
    changing = true;
    toTime();
  }
  if(keyPressed == controlButton){
    changing = true;
    toControl();
  }
}



void toPage2(){
  changeDelay();
  char keyPressed = keypad.getKey();
  page = '2';
  pageInfo();

  
  

  if(keyPressed == pageButton){
    changing = true;
    toPage1();
  }
}



void toTime(){
  changeDelay();
  page = 'pageT';
  char keyPressed = keypad.getKey();
  backInfo();
  lcd.setCursor(0,0);
  lcd.print("TIME: ");
  if(keyPressed == '*'){
    changing = true;
    toInterface('1');
  }
}



void toControl(){
  changeDelay();
  page = 'C';
  char keyPressed = keypad.getKey();
  backInfo();
  lcd.setCursor(0,0);
  lcd.print("CONTROL: ");
  if(keyPressed == '*'){
    changing = true;
    toInterface('1');
  }
}



void backInfo(){
  lcd.setCursor(11, 0);
  lcd.print("|");
  lcd.setCursor(11, 1);
  lcd.print("|");
  lcd.setCursor(12, 0);
  lcd.print("BACK");
  lcd.setCursor(12, 1);
  lcd.print(" <*");
}



void pageInfo(){
  if(page == '1'){
    lcd.setCursor(0, 0);
    lcd.print("TIMER:");
    lcd.setCursor(0, 1);
    lcd.print("CTRL:");

    lcd.setCursor(7, 0);
    lcd.print(timerButton);
    lcd.setCursor(7, 1);
    lcd.print(controlButton);

    lcd.setCursor(12, 0);
    lcd.print("|");
    lcd.setCursor(12, 1);
    lcd.print("|");
    lcd.setCursor(13, 0);
    lcd.print("P1");
    lcd.setCursor(13, 1);
    lcd.print(" 0>");
  }
  if(page == '2'){
    lcd.setCursor(12, 0);
    lcd.print("|");
    lcd.setCursor(12, 1);
    lcd.print("|");
    lcd.setCursor(13, 0);
    lcd.print("P2");
    lcd.setCursor(13, 1);
    lcd.print("<0 ");
  }
}
  

void changeDelay(){
  if(changing){
    lcd.clear();
    delay(350);
    changing = false;
  }
}



void checkValues(){
  /*
  if(isValuesOk){
    for(i = 0; i < 3; i++){
        if(!(pageChars[i] == pageChars[i+1])){
          isValuesOk = true;
        }else{
          isValuesOk = false;
        }
      
    }
  }
}
*/
if(!isValuesOk){
  for(i = 0; i <= 2; i++){
    for(o = 1; o <= 3; o++){
      if(pageChars[i] != pageChars[o]){
        isValuesOk = true;
        }
        else{
          isValuesOk = false;
        }
      }
    }

  for(i = 0; i <= 1; i++){
    for(o = 1; o <= 1; o++){
      if(buttonChars[i] != buttonChars[o]){
        isValuesOk = true;
        }
        else{
          isValuesOk = false;
        }
      }
    }
  }
  else{
  isValuesOk = true;
  return isValuesOk;
}return isValuesOk;
}






/*
Koodia käytetty numeroiden checkaamiselle startissa:



LINK: https://forum.arduino.cc/t/testing-to-see-if-some-value-is-an-element-of-an-array/272284/2


primes [] = {2,3,5,7};

if (n == primes[n])

This only looks at the n'th element in the array. Push the button 3 times then look at the 3rd element.

You need to cycle through the array to check all elements.

For count = 0; count =< number of elements; increment count
If n = primes[count]
Found = true
Exit for loop





*/










