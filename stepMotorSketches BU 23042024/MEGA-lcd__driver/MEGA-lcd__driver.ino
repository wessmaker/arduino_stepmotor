
#include <Keyboard.h>
#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>


#define ROWS 4 
#define COLS 3

byte byte3 = 3;
byte byte2 = 2;

byte lArrow[8] =
{
0b00000,
0b00000,
0b00100,
0b01100,
0b11111,
0b01100,
0b00100,
0b00000
};

byte rArrow[8] =
{
0b00000,
0b00000,
0b00100,
0b00110,
0b11111,
0b00110,
0b00100,
0b00000
};


char leftArrow = byte(0);
char rightArrow = byte(1);


const char displayButton = '1';
const char timerButton =  '2';
const char pageButton = '0';
const char backButton = '*';
const char forceButton = '1';
const char forceRightButton = '6';
const char forceLeftButton = '4';


const char page1 = '1';
const char page2 = '2';
const char pageT = 'T';
const char pageD = 'D';
const char pageF = 'F';

char page;



byte rowPins[ROWS] = {30, 31, 32, 33}; 
byte colPins[COLS] = {40, 41, 42}; 


int i;
int o;
int a;
int b;
int c;
int currentTime;
int currentStep;


//msg alkuiset muuttujat ovat viestejä joita käytetään arduinojen välisessä viestinnässä ja komentamisessa

int msgForceRight = 2000;
int msgForceLeft = 3000;


int seconds; //TESTAUS


int timeLimit; //max arvo jonka voi syöttää
bool isTyping; //tosi/epätosi, jos ollaan numeron kirjoitustilassa
int newValueCount; //apunumero jolla määritetään mitä numeroa vilkutetaan typingissä


int direction; // suunta pakkokäytössä 1 ja 2 oikea vasen
bool isForce; // tosi/epätosi, onko pakkokäyttö käytössä vai ei 


bool started; // tosi/epätosi onko startti suoritettu oikein
bool changing; // tosi/epätosi pitääkö siirtymien välillä tehdä delay VOI JÄTTÄÄ HUOMIOTTA


char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};


Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
LiquidCrystal_I2C lcd(0x27, 16, 2);


char pageChars[4] = {page1, page2, pageT, pageD};               //Testataan arvot ennen starttia ettei ole päällekkäisyyksiä
char buttonChars[3] = {displayButton, timerButton, pageButton};










void setup() {
  Serial.begin(9600);
  lcd.init();  //tunnistaa näytön
  lcd.backlight(); //näytön takavalo ON
  lcd.createChar(0, lArrow);// luo omat nuolikuviot 
  lcd.createChar(1, rArrow);
}









void loop() { 
  if(!started){ 
    if(isValuesOk() == true){
      start();
    }else if((isValuesOk() == false)){
      lcd.setCursor(0, 0);
      lcd.print("Virhe:");
      lcd.setCursor(0, 1);
      lcd.print("Samat char-arvot!");
      Serial.println("Check values and restart!");
    }
  }

  if(started){
    toInterface(page); //käytännössä pyörittää koodia
    if(changing){
      Serial.println("changing!");
    }
  }
}










void start(){
  lcd.setCursor(0, 0);
  lcd.print("Tervetuloa!");
  delay(1000);
  page = page1;
  started = true;
  changing = true;
}











void toInterface(char page){
  changeDelay();


  currentTime = Serial.read();//TESTAUS


  switch(page){

    case page1:
      toPage1();
      break;

    case page2:
      toPage2();
      break;

    case pageT:
      toTimer();
      break;

    case pageD:
      toDisplay();
      break;

    case pageF:
      toForce();
      break;
  }
}







//Sivu 1 logiikka koodi
void toPage1(){
  changeDelay();
  page = page1;
  char keyPressed = keypad.getKey();
  pageInfo();
  
  //jos painetaan pageButtonia eli 0, niin page 2
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









void toDisplay(){
  changeDelay();
  page = pageD;
  char keyPressed = keypad.getKey();
  pageInfo();
  backInfo();

  if(keyPressed == backButton){
    changing = true;
    toInterface(page1);
  }
}









void toTimer(){
  changeDelay();
  page = pageT;
  char keyPressed = keypad.getKey();
  pageInfo();
  backInfo();  

  if(keyPressed == backButton){
    changing = true;
    toInterface(page1);
  }
  
  if(isTyping){
    toTypeValue();
  }

  if((keyPressed == pageButton && !isTyping)){
    changing = true;
    isTyping = true;
  }
}










void toPage2(){
  changeDelay();
  page = page2;
  char keyPressed = keypad.getKey();
  pageInfo();

  if(keyPressed == pageButton){
    changing = true;
    toInterface(page1);
  }

   if(keyPressed == forceButton){ // forcebuttonilla "1" suoritetaan toInterface(pageF); eli mennään interfacen kautta sivu "Forcelle"
    changing = true;
    toInterface(pageF);
  }
}









void toForce(){

  changeDelay();
  char keyPressed = keypad.getKey();
  page = pageF;
  backInfo();
  pageInfo();

  if(keyPressed == forceRightButton) {
    direction = 2;
    isForce = true;
    
  }

  if(keyPressed == forceLeftButton)  {
    direction = 1;
    isForce = true;

    
    

    
  }

  if(keypad.getState() == 0){
    direction = 0;
    isForce = false;
  }

  if(keyPressed == backButton) {
    direction = 0;
    isForce = false;
    changing = true;
    toInterface(page2);
  }
}











void toTypeValue(){
  char keyPressed = keypad.getKey();

  if(keyPressed == backButton){
    changing = true;
    isTyping = false;
    newValueCount = 0;
    a = 0;
    b = 0;
    c = 0;
  }

  if(isDigit(keyPressed)){
    switch(newValueCount){
      case 0:
        a = keyPressed & 0x0F;
        newValueCount++;
        break;

      case 1:
        b = keyPressed & 0x0F;
        newValueCount++;
        break;

      case 2:
        c = keyPressed & 0x0F;
        newValueCount++;
        break;

      case 3:
        if(keyPressed == pageButton){
          changing = true;
          timeLimit = (a*100)+(b*10)+(c); //tallentaa uuden timelimitin
          delay(200);
          isTyping = false;
          a = 0;
          b = 0;
          c = 0;
          newValueCount = 0; //nollaa tulevaisuutta varten
          break;
        }
    }
  }
}








// Kaikki mitä voidaan printataan näytölle riippuen mikä sivu/tilanne:
void pageInfo(){

  switch(page){
    case page1:
      showPageNumber();
      lcd.setCursor(0, 0);
      lcd.print("DISPL:");
      lcd.setCursor(7, 0);
      lcd.print(displayButton);
      lcd.setCursor(0, 1);
      lcd.print("TIMER:");
      lcd.setCursor(7, 1);
      lcd.print(timerButton);
      break;



    case page2:
      showPageNumber();
      lcd.setCursor(0, 0);
      lcd.print("Force:");
      lcd.setCursor(8, 0);
      lcd.print(forceButton);
      break;



    case pageD:
      lcd.setCursor(0, 0);
      lcd.print("Time");

      if((currentTime >= 0) && (currentTime <= 9)){ //riippuen onko yli 10 tai 100 niin siirtää cursoria
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

      lcd.setCursor(8, 0);//laittaa / -merkin väliin
      lcd.print("/");
      lcd.setCursor(9, 0);
      lcd.print(timeLimit);

      lcd.setCursor(0, 1);
      lcd.print("Step");

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
      break;



    case pageT:
      if(!isTyping){ //EI OLLA KIRJOITUSTILASSA
      changeDelay();
      lcd.setCursor(0, 0);
      lcd.print("Value:");
      lcd.setCursor(8, 0);
      lcd.print(timeLimit);
      lcd.print("s");

      lcd.setCursor(0, 1);
      lcd.print("Change ");
      lcd.write(rightArrow);
      lcd.print(pageButton);
      }

      if(isTyping){ //OLLAAN KIRJOITUS TILASSA UUDELLE ARVOLLE
        changeDelay();

        lcd.setCursor(0, 0);
        lcd.print("Value:");
        
        if(newValueCount <3){
          lcd.setCursor(0, 1);
          lcd.print("Cancel");
          lcd.setCursor(8, 1);
          lcd.print(backButton);
          }
        if(newValueCount >=3){
          lcd.setCursor(0, 1);
          lcd.print("Set   ");
          lcd.setCursor(8, 1);
          lcd.print(pageButton);
          }
        
        lcd.setCursor(7, 1);
        lcd.print(rightArrow);

        lcd.setCursor(8, 0);
        int i = millis()%700;

        if(i > 400){
          switch(newValueCount){
            case 0:
              lcd.print(" ");
              lcd.print(b);
              lcd.print(c);
              break;
              
            case 1:
              lcd.print(a);
              lcd.print(" ");
              lcd.print(c);
              break;

            case 2:
              lcd.print(a);
              lcd.print(b);
              lcd.print(" ");
              break;

            default:
              lcd.print(a);
              lcd.print(b);
              lcd.print(c);
              lcd.print("s");
          }
        }
        if(i < 400){
          lcd.print(a);
          lcd.print(b);
          lcd.print(c);
        }

        lcd.setCursor(11, 0);
        lcd.print("s");
      }
      break;



    case pageF:
      lcd.setCursor(0, 0);
      lcd.print("Force:");

      if(!isForce){
        lcd.setCursor(8, 0);
        lcd.print("OFF");
        lcd.setCursor(7, 1);
        lcd.print(" ");
        lcd.setCursor(9, 1);
        lcd.print(" ");
      }

      if(isForce){
        lcd.setCursor(8, 0);
        lcd.setCursor(8, 0);
        lcd.print("ON");
        lcd.print(" ");
      }


      lcd.setCursor(0, 1);
      lcd.print("Dir:");
      lcd.setCursor(8, 1);
      lcd.print("0");

      if(keypad.getState() >= 1 && keypad.getState() < 3 && direction == 1){ //VAIN JOS PAINETAAN POHJAAN JA KUMPAA PAINETAAN 4/6
        lcd.setCursor(7, 1);
        lcd.write(leftArrow);


        Serial.write(byte3);
        Serial.print("direction: ");
        Serial.print(direction);
        Serial.println();
      }

      if(keypad.getState() >= 1 && keypad.getState() < 3 && direction == 2){
        lcd.setCursor(9, 1);
        lcd.write(rightArrow);
        
        Serial.write(byte2);
        Serial.print("direction: ");
        Serial.print(direction);
        Serial.println();
      }
      break;
  }
}


//backINFO eli kertoo millä napilla päästään takaisin,  (oikealla bk ja ->*)
void backInfo(){
  
  lcd.setCursor(13, 0);
  lcd.print("|");
  lcd.setCursor(14, 0);
  lcd.print("BK");

  lcd.setCursor(13, 1);
  lcd.print("|");
  lcd.setCursor(14, 1);
  lcd.print(rightArrow);
  lcd.print(backButton);
}








//näyttää sivun numeron riippuen P1 vai P2
void showPageNumber(){
  lcd.setCursor(13, 0);
  lcd.print("|");

  lcd.setCursor(14, 0);
  if(page == page1){
    lcd.print("P1");
  }else if(page == page2){
    lcd.print("P2");
  }

  lcd.setCursor(13, 1);
  lcd.print("|");

  lcd.setCursor(14, 1);
  if(page == page1){        //KUMPI SIVU, sen mukaan
    lcd.write(rightArrow);
    lcd.print(pageButton);
  }else if(page == page2){
    lcd.print(rightArrow);
    lcd.print(pageButton);
  }
}








// viive siirtymien välillä riippuen onko changing totta vai ei 
//laittaa changing = false eli epätodeksi. Pyörittää siis vain kerran kun "pyydetään"
void changeDelay(){
  if(changing){
    lcd.clear();
    delay(350);
    changing = false;
  }
}







//chekkaa ennen kuin mitään koko koodissa tapahtuu, onko annetut napit ok vai ei, jos ei ole niin printtaa tiedon
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


//CUSTOM CHARACHTERS: https://lastminuteengineers.com/arduino-1602-character-lcd-tutorial/?utm_content=cmp-true


//OMA NUMERON VILKKUMISKOODIPÄTKÄ:

  int i = millis()%700;
  if(i < 400){
    lcd.setCursor(1, 0);
    lcd.print(" ");
    
    
    
    Serial.println(i);
  }if(i > 400){
    
    lcd.setCursor(0, 0);
    lcd.print("200");
    Serial.println(i);
  }




*/










