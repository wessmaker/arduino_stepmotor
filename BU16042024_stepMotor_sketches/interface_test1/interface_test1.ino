#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>

#define ROWS 4 
#define COLS 3



int currentTime = 99;
int currentStep = 50;

const char displayButton = '1';
const char timerButton =  '2';
const char pageButton = '0';
const char backButton = '*';

const char page1 = '1';
const char page2 = '2';
const char pageT = 'T';
const char pageD = 'D';




byte rowPins[ROWS] = {30, 31, 32, 33}; 
byte colPins[COLS] = {40, 41, 42}; 


int i;
int o;
char page;

int a;
int b;
int c;


int currentMaxTime;
bool typing;
int newValue;
int newValueCount;



bool started;
bool changing;


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27, 16, 2);

char pageChars[4] = {page1, page2, pageT, pageD};
char buttonChars[3] = {displayButton, timerButton, pageButton};


void setup() {
  Serial.begin(9600);
  lcd.init();  
  lcd.backlight();
  

}



void loop() {
  

  if(!started){ //aloitus
    if(isValuesOk() == true){
      start();
    }else if((isValuesOk() == false)){
      lcd.setCursor(0, 0);
      lcd.print("Virhe:");
      lcd.setCursor(0, 1);
      lcd.print("Sama char-arvo!");
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
      toTimer();
      break;
    case pageD:
      Serial.println("PAGE D!");
      Serial.println();
      toDisplay();
      break;
  }
  
  

}



void toPage1(){
  changeDelay();
  char keyPressed = keypad.getKey();
  
  page = page1;
  pageInfo();
  
  if(keyPressed == pageButton){
    changing = true;
    toInterface(page2);
  }
  if(keyPressed == timerButton){
    changing = true;
    toInterface(pageT);
  }
  if(keyPressed == displayButton){
    changing = true;
    toInterface(pageD);
  }
}



void toPage2(){
  changeDelay();
  char keyPressed = keypad.getKey();
  page = page2;
  pageInfo();

  if(keyPressed == pageButton){
    changing = true;
    toInterface(page1);
  }
}



void toTimer(){
  changeDelay();
  
  char keyPressed = keypad.getKey();
  page = pageT;
  backInfo();
  pageInfo();
  

  if(keyPressed == backButton){
    changing = true;
    toInterface(page1);
  }
  if((keyPressed == pageButton && !typing)){
    changing = true;
    typing = true;
    
  }
  if(typing){
    typeValue();
  }


}

void typeValue(){
  char keyPressed = keypad.getKey();
  if(keyPressed == backButton){
    typing = false;
    newValueCount = 0;
    a = 0;
    b = 0;
    c = 0;
    changing = true;
  }

  if(isDigit(keyPressed)){
    switch(newValueCount){
      case 0:
        a = keyPressed & 0x0F;
        Serial.println("A");
        newValueCount++;
        break;
      case 1:
        b = keyPressed & 0x0F;
        Serial.println("B");
        newValueCount++;
        break;
      case 2:
        c = keyPressed & 0x0F;
        Serial.println("C");
        newValueCount++;
        break;
      case 3:   //Tämä sen takia että saadaan viimeinen numero näkyviin näyttöön ennen palaamista timersivulle
        newValueCount++;
    }
  }
  currentMaxTime = (a*100)+(b*10)+(c);

  if(newValueCount >= 4){
      delay(200);
      typing = false;
      a = 0;
      b = 0;
      c = 0;
      newValueCount = 0;
      changing = true;
  }

  
  
  


}




void toDisplay(){
  changeDelay();
  page = pageD;
  char keyPressed = keypad.getKey();
  backInfo();
  pageInfo();

  if(keyPressed == backButton){
    changing = true;
    toInterface(page1);
  }
}


void pageInfo(){

  // 1 PAGE
  if(page == page1){

    
    currentTime = 0;
    currentStep = 0;


    lcd.setCursor(0, 0);
    lcd.print("DISPL:");
    lcd.setCursor(0, 1);
    lcd.print("TIMER:");

    lcd.setCursor(7, 0);
    lcd.print(displayButton);
    lcd.setCursor(7, 1);
    lcd.print(timerButton);

    showPageNumber();
  }
  

  // 2 PAGE
  if(page == page2){
    showPageNumber();
  }




  // DISPLAY PAGE
  if(page == pageD){

    if(currentTime < currentMaxTime){
      currentTime++;
      currentStep++;
    }
    


    lcd.setCursor(0, 0);
    lcd.print("Time");

    lcd.setCursor(0, 1);
    lcd.print("Step");
    

    if((currentTime >= 0) && (currentTime <= 9)){
      lcd.setCursor(7, 0);
      lcd.print(currentTime);
    }
    else if((currentTime >= 10) && (currentTime <= 99)){
      lcd.setCursor(6, 0);
      lcd.print(currentTime);
    }
    else if((currentTime >= 100) && (currentTime <= 999)){
      lcd.setCursor(5, 0);
      lcd.print(currentTime); 
    }

    lcd.setCursor(8, 0);
    lcd.print("/");
    lcd.setCursor(9, 0);
    lcd.print(currentMaxTime);


    if((currentStep >= 0) && (currentStep <= 9)){
      lcd.setCursor(7, 1);
      lcd.print(currentStep);      
    }
    else if((currentStep >= 10) && (currentStep <= 99)){
      lcd.setCursor(6, 1);
      lcd.print(currentStep);
    }
    else if((currentStep >= 100) && (currentStep <= 999)){
      lcd.setCursor(5, 1);
      lcd.print(currentStep); 
    }


  } 




  // TIMER PAGE
  if(page == pageT){
    
    if(typing){
      changeDelay();
      lcd.setCursor(0, 0);
      lcd.print("Limit:");
      lcd.setCursor(7, 0);
      lcd.print(a);
      lcd.print(b);
      lcd.print(c);

      lcd.setCursor(0, 1);
      lcd.print("Cancel >");
      lcd.print(backButton);
      
    }
    if(!typing){
      lcd.setCursor(0, 0);
      lcd.print("Limit:");
      lcd.setCursor(7, 0);
      lcd.print(currentMaxTime);

      lcd.setCursor(0, 1);
      lcd.print("Change >");
      lcd.print(pageButton);
    }

    
  }

}





void backInfo(){
  
  lcd.setCursor(12, 0);
  lcd.print("|");
  lcd.setCursor(12, 1);
  lcd.print("|");
  lcd.setCursor(13, 0);
  lcd.print("BCK");
  lcd.setCursor(13, 1);
  lcd.print("<");
  lcd.print(backButton);




}





void showPageNumber(){
  lcd.setCursor(12, 0);
  lcd.print("|");
  lcd.setCursor(12, 1);
  lcd.print("|");
  lcd.setCursor(13, 0);

  if(page == page1){
    lcd.print("P1");
  }else if(page == page2){
    lcd.print("P2");
  }

  lcd.setCursor(13, 1);

  if(page == page1){
    lcd.print(" ");
    lcd.print(pageButton);
    lcd.print(">");
    
    
  }else if(page == page2){
    lcd.print("<");
    lcd.print(pageButton);
  }
}










void changeDelay(){
  if(changing){
    lcd.clear();
    delay(350);
    changing = false;
  }
}
bool isValuesOk(){
  bool k;
 
  for(i = 0; i <=1; i++){
    for(o = i+1; o <= 2; o++){
      if((buttonChars[i] == buttonChars[o])){
        k = false;
        return false;
      }
      else{
        k = true;
      }
    }
  }


  for(i = 0; i <=2; i++){
    for(o = i+1; o <= 3; o++){
      if((pageChars[i] == pageChars[o])){
        k = false;
        return false;
      }
      else{
        k = true;
      }
    }
  }
  if(k){
    return true;
  }else{
    return false;
  }
}












/*--------------------------------------------------
Koodia käytetty numeroiden checkaamiselle startissa:

toInt: https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/toint/

LINK: https://forum.arduino.cc/t/testing-to-see-if-some-value-is-an-element-of-an-array/272284/2


primes [] = {2,3,5,7};

if (n == primes[n])

This only looks at the n'th element in the array. Push the button 3 times then look at the 3rd element.

You need to cycle through the array to check all elements.

For count = 0; count =< number of elements; increment count
If n = primes[count]
Found = true
Exit for loop




CHAR TO INT:

Hi @richo777
try this way;

byte x; // 
int abc[10];
char str;

void setup () {
  Serial.begin(115200);
}

void loop () {

  str = '7'; 

  x = str & 0x0F;
  abc[x] = 45;
  
  Serial.println(abc[x]);
  delay(1000);
}


*/










