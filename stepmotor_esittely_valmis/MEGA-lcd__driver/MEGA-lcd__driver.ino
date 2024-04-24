

// Ottaa tarvittavat kirjastot käyttöön
#include <Keyboard.h> 
#include <LiquidCrystal_I2C.h>
#include <Key.h>
#include <Keypad.h>


// Määritää mitkä arvot ROWS ja COLS ovat 
#define ROWS 4 
#define COLS 3


//Oikea nuoli -merkin määrittely
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


//Oikea nuoli -merkin määrittely
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



//Laittaa leftArrow ja rightArrow nimien taakse nuolet, jotta voidaan käytää koodissa vain arvojen nimillä, kun halutaan piirtää nuoli
char leftArrow = byte(0);
char rightArrow = byte(1);




//nappien kirjaimet / numerot, esim pageButton on oikeasti '0'
//käytetään yhtenäistämään nappeja ja näitä voidaan muokata jos haluttaisiin ohjata eri napeilla
const char displayButton = '1';
const char timerButton =  '2';
const char pageButton = '0';
const char backButton = '*';
const char forceButton = '1';
const char forceRightButton = '6';
const char forceLeftButton = '4';
const char timerONOFFButton = '5';



//näillä ohjataan ohjelmaa toInterfacen switch/case logiikan kautta, niitä asetetaan char pageen
const char page1 = '1';
const char page2 = '2';
const char pageT = 'T';
const char pageD = 'D';
const char pageF = 'F';


char page;    //tämän arvoa verrataan toInterfacessa, sen arvot vaihtelevat 



byte rowPins[ROWS] = {30, 31, 32, 33}; 
byte colPins[COLS] = {40, 41, 42}; 


//kaikki Integer arvot 
int i;
int o;
int a;    //a, b ja c:tä käytetään apuarvoina uuden ajan asettamiseen näytölle
int b;
int c;
int currentTime;    //Nykyinen aika joka olisi mennyt UNOssa, ei kuitenkaan ole laitettu toimimaan
int currentStep;    //Sen hetkinen askelten määrä UNOssa, ei kuitenkaan ole laitettu toimimaan



//byte byteX = X, näillä lähetetään komentoja serialiin, jotak tunnistetaan UNOssa ja niillä ohjataan moottoria UNOssa
byte byte70 = 70;     //
byte byte100 = 100;
byte byte200 = 200;
byte byte120 = 120;
byte byte130 = 130;
byte byte140 = 140;
byte byte150 = 150;
byte byte160 = 160;
byte byte170 = 170;
byte byte180 = 180;


byte byteA;   //byteA, byteB ja byteC:tä käytetään apuarvoina uuden ajan lähettämiseen UNOlle samaan aikaan kun se asetetaan MEGAssa
byte byteB;   
byte byteC; 







bool timerOn; //onko timer päällä vai ei (NÄYTÖN MUKAAN)



int direction; // suunta pakkokäytössä 1 ja 2 oikea vasen
bool isForce; // tosi/epätosi, onko pakkokäyttö käytössä vai ei 


bool started; // tosi/epätosi onko startti suoritettu oikein
bool changing; // tosi/epätosi pitääkö siirtymien välillä tehdä delay VOI JÄTTÄÄ HUOMIOTTA


bool isTyping; //ollaanko kirjoitustilassa vai ei: true/false
int timeLimit; // maximi aika-arvo, kuinka pitkään moottori pyörittää timer ohjauksessa
int newValueCount; //apunumero jolla määritetään mitä numeroa vilkutetaan kun näppäillään uusia aika-arvoja



char hexaKeys[ROWS][COLS] = {     //määrittää minkälainen näppäimistö on käytössä 
  {'1', '2', '3'},                //ja mitä printtaa kun jotain näppäintä painetaan
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};



Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);  //asettaa keypadin pinnit kohilleen
LiquidCrystal_I2C lcd(0x27, 16, 2);













void setup() {
  Serial.begin(9600); //laittaa serialin nopeuden 9600
  lcd.init();         //tunnistaa näytön
  lcd.backlight();    //näytön taustavalistus päälle
  lcd.createChar(0, lArrow);// luo oikean nuolen merkin 
  lcd.createChar(1, rArrow);// luo vasemman nuolen merkin
}








//startin komennot (koodi suoritetaan vain kerran aloituksessa)
void start(){
  lcd.setCursor(0, 0);
  lcd.print("Tervetuloa!");
  delay(1000);
  page = page1;
  started = true;
  changing = true; 
}









//Koko ohjelman perusta
//toInterfacen kautta ohjataan koko ohjelmaa, riippuen mikä on "page":n kirjain
//page asetetaan aina kyseisen sivun page-arvoon

void toInterface(char page){
  changeDelay();            //changeDelay aiheuttaa aina 350ms delayn kun "changing" asetetaan trueksi

  switch(page){
    case page1:   //jos page == '1'
      toPage1();
      break;

    case page2:   //jos page == '2'
      toPage2();
      break;

    case pageT:   //jos page == 'T'
      toTimer();
      break;

    case pageD:   //jos page == 'D'
      toDisplay();
      break;

    case pageF:   //jos page == 'F'
      toForce();
      break;
  }
}












//Kun ollaan sivulla 1, määritetään mitä näppäimet tekee
void toPage1(){
  changeDelay();
  page = page1;
  char keyPressed = keypad.getKey();
  pageInfo();
  
  
  if(keyPressed == pageButton){   //nappi 0 (pageButton) vaihtaa sivuksi: sivu2
    changing = true;
    toInterface(page2);
  }

  if(keyPressed == timerButton){  //nappi 2 (timerButton) vaihtaa sivuksi: timer
    changing = true;
    toInterface(pageT);
  }

  if(keyPressed == displayButton){  //nappi 1 (displayButton) vaihtaa sivuksi: display
    changing = true;
    toInterface(pageD);
  }
}












//Kun ollaan sivulla display, määritetään mitä näppäimet tekee
void toDisplay(){
  changeDelay();
  page = pageD;
  char keyPressed = keypad.getKey();
  pageInfo();
  backInfo();   //paluunäppäimen näyttö

  if(keyPressed == backButton){   //nappi * (backButton) vaihtaa sivuksi: page1
    changing = true;
    toInterface(page1);
  }
  
  if(keyPressed == timerONOFFButton){   //nappi 5 (timerONOFFButton) laittaa timerin päälle tai pois päältä
    if (!timerOn){
      Serial.write(byte120);    //käsketään Serialiin:  120 eli timerSetON = true (laittaa timerin päälle UNOssa)
      timerOn = true;
    }else if(timerOn){
      Serial.write(byte130);    //käsketään Serialiin: 130 eli timerSetON = false (resettaa timerin UNOssa)
      timerOn = false;
    }
  }
}










//Kun ollaan sivulla timer, määritetään mitä näppäimet tekee
void toTimer(){
  changeDelay();
  page = pageT;
  char keyPressed = keypad.getKey();
  pageInfo();

  backInfo();  //paluunäppäimen näyttö

  if(keyPressed == backButton){   //nappi * (backButton) vaihtaa sivuksi: page1
    changing = true;
    toInterface(page1);
  }
  
  if(isTyping){       //jos isTyping on true, niin muuttaa hieman ja antaa mahdollisuuden asettaa uuden ajan
    toTypeValue();
  }

  if((keyPressed == pageButton && !isTyping)){    //nappi 0 (pageButton) asettaa isTyping trueksi, jos kirjoitus ei vielä ole aktivoitu, jotta voidaan kirjoittaa uusi aika

    Serial.write(byte150);     //käsketään Serialiin: 150 eli vastaanotto = 1 (uuden aika arvon asettamisen salliminen)
    changing = true;
    isTyping = true;
    
  }
}











//Kun ollaan sivulla 2, määritetään mitä näppäimet tekee
void toPage2(){
  changeDelay();
  page = page2;
  char keyPressed = keypad.getKey();
  pageInfo();

  if(keyPressed == pageButton){     //nappi 0 (pageButton) vaihtaa sivuksi: sivu1
    changing = true;
    toInterface(page1);
  }

   if(keyPressed == forceButton){ // forcebuttonilla "1" suoritetaan toInterface(pageF) eli mennään interfacen kautta sivu "Forcelle"
    changing = true;
    Serial.write(byte200);    //käsketään Serialiin: 200 eli vastaanotto = 2 (pakko-ohjauksen salliminen)
    toInterface(pageF);       
    
  }
}











//Kun ollaan sivulla force, määritetään mitä näppäimet tekee
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












//Kun ollaan toTypeValue -moodissa, määritetään mitä näppäimet tekee
void toTypeValue(){
  char keyPressed = keypad.getKey();

  if(keyPressed == backButton){ // jos painetaan * (backButton), niin nollaa arvot ja lopettaa kirjoitusmoodin
    
    isTyping = false;
    newValueCount = 0;
    a = 0;
    b = 0;
    c = 0;
    Serial.write(byte70);   //käsketään Serialiin: 70 x3 eli vastaanotto = 0, newValueCount = 0, a,b,c = 0 (nollaa aika-arvojen asettamisen)
    Serial.write(byte70);
    Serial.write(byte70);
    changing = true;
  }

  if(isDigit(keyPressed)){ //jos painetaan jotain näppäintä, kirjoitusmoodissa niin tallentaa sen, riippuu moneski numero menossa, että onko a, b vai c
    switch(newValueCount){
      case 0:
        a = keyPressed & 0x0F;
        byteA = a;
        Serial.write(byteA);    //käsketään Serialiin: byteA:n arvo (asettaa UNOssa a:n arvoksi MEGAn a:n arvon)
        newValueCount++;
        break;

      case 1:
        b = keyPressed & 0x0F;
        byteB = b;
        Serial.write(byteB);    //käsketään Serialiin: byteB:n arvo (asettaa UNOssa a:n arvoksi MEGAn b:n arvon)
        newValueCount++;
        break;

      case 2:
        c = keyPressed & 0x0F;
        byteC = c;
        Serial.write(byteC);   //käsketään Serialiin: byteC:n arvo (asettaa UNOssa a:n arvoksi MEGAn c:n arvon)
        newValueCount++;
        break;

      case 3:
        if(keyPressed == pageButton){     // jos newValueCount on 3, niin silloin abc -arvot ovat asetettu ja jos silloin painettaessa 0 (pageButton)

          Serial.write(byte160);          //käsketään Serialiin: 160 (uusien aika-arvojen tallentamisen ja niiden nollauksen)
          changing = true;
          timeLimit = (a*100)+(b*10)+(c); //tallentaa uuden timelimitin
          isTyping = false;
          a = 0;
          b = 0;
          c = 0;
          byteA = 0;    //nollaa myös bittien apuarvot joilla lähetetiin abc arvot Serialiin
          byteB = 0;
          byteC = 0;
          newValueCount = 0; //nollaa tulevaisuutta varten
          break;
        }
    }
  }
}











// käytännössä asettaa cursorin LCD näytölle ja kirjoittaa siihen halutut tekstit
// RIIPPUEN SIVUSTA (page:n arvosta)
void pageInfo(){

  //JOS sivu = sivu 1
  if(page == page1){

    showPageNumber();
    lcd.setCursor(0, 0);
    lcd.print("DISPL:");
    lcd.setCursor(7, 0);
    lcd.print(displayButton);
    lcd.setCursor(0, 1);
    lcd.print("TIMER:");
    lcd.setCursor(7, 1);
    lcd.print(timerButton);
  }




  //JOS sivu = sivu 2
  if(page == page2){
    
    showPageNumber();
    lcd.setCursor(0, 0);
    lcd.print("Force:");
    lcd.setCursor(8, 0);
    lcd.print(forceButton);
  }




  //JOS sivu = sivu "display"
  if(page == pageD){
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

    i = millis()%700;       //i on millis jakojäännös 700
    if(i > 400 && timerOn){ //JOS millis() jakojäännös 700 on suurempi kuin 400 JA timer on päällä niin tyhjää timernapin paikan
      lcd.setCursor(13,0);
      lcd.print("   ");
    }
    if(i < 400 || !timerOn){ //JOS millis() jakojäännös 700 on pienempi kuin 400 tai timer on pois päältä niin kirjoittaa timernapin 
    lcd.setCursor(13,0);
    lcd.print("T");         //Käytännössä vilkuttaa sillon JOS timer on päällä ja ei vilkuta JOS timer ei ole päällä
    lcd.print(rightArrow);
    lcd.print(timerONOFFButton);
    }
    
  }




  //JOS sivu = sivu "timer"
  if(page == pageT){
    
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
      int i = millis()%700;   //ottaa 700 jakojäännöksen sen hetkisestä ajasta (ms)

      if(i > 400){                  //jos jakojäännös on yli 400, niin printtaa asetettavan arvon tilalle tyhjän   ---> aiheuttaa tarkoituksellisen vilkkumisen
        switch(newValueCount){
          case 0:
            lcd.print(" ");     //kirjoitetaan a:ta -> tyhjä kohta siihen
            lcd.print(b);
            lcd.print(c);
            break;
            
          case 1:
            lcd.print(a);   
            lcd.print(" ");     //kirjoitetaan b:ta -> tyhjä kohta siihen
            lcd.print(c);
            break;

          case 2:
            lcd.print(a);
            lcd.print(b);
            lcd.print(" ");    //kirjoitetaan c:ta -> tyhjä kohta siihen
            break;

          default:
            lcd.print(a);      // jos minkään (a, b tai c) kohdalla ei olla, niin printtaa ne normaalisti
            lcd.print(b);      // siis silloin kun odotetaan arvojen hyväksymistä
            lcd.print(c);
            lcd.print("s");
        }
      }

      if(i < 400){      //aina kun jakojäännös on alle 400, niin kirjoittaa kaikki arvot normaalisti
        lcd.print(a);
        lcd.print(b);
        lcd.print(c);
      }

      lcd.setCursor(11, 0);
      lcd.print("s");
    }
  }



  //JOS sivu = sivu "force"
  if(page == pageF){
    lcd.setCursor(0, 0);
    lcd.print("Force:");

    if(!isForce){
      lcd.setCursor(8, 0);
      lcd.print("OFF");
      lcd.setCursor(7, 1);
      lcd.print(" ");
      lcd.setCursor(9, 1);
      lcd.print(" ");
      Serial.write(byte140);    //käsketään Serialiin: 140 eli suunta = 0 (ei pakkoajoa)
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


    if(keypad.getState() >= 1 && keypad.getState() < 3 && direction == 1){ //vain jos painetaan pohjaan ja suunta on 1 eli vasemmalle
      lcd.setCursor(7, 1);
      lcd.write(leftArrow);
      Serial.write(byte170);    //käsketään Serialiin: 170 eli suunta = -1 ja step.motor(suunta) (pakkoajaa vasemmalle)

      Serial.print("direction: ");
      Serial.print(direction);
      Serial.println();
    }


    if(keypad.getState() >= 1 && keypad.getState() < 3 && direction == 2){      //vain jos painetaan pohjaan ja suunta on 2 eli oikealle
      lcd.setCursor(9, 1);
      lcd.write(rightArrow);
      Serial.write(byte180);  //käsketään Serialiin: 180 eli suunta = 1 ja step.motor(suunta) (pakkoajaa oikealle)
      
      
      Serial.print("direction: ");
      Serial.print(direction);
      Serial.println();
    }
  }
}















//backINFO eli kertoo millä napilla päästään takaisin,  (oikealla bk ja ->*)
void backInfo(){
  lcd.setCursor(14, 1);
  lcd.print(rightArrow);
  lcd.print(backButton);
}














//näyttää sivun numeron riippuen P1 vai P2
void showPageNumber(){
    lcd.setCursor(14,1);
    lcd.write(rightArrow);
    lcd.print(pageButton);
}















// viive siirtymien välillä riippuen onko changing totta vai ei 
//laittaa changing = false eli epätodeksi. Pyörittää siis vain kerran kun pyydetään
void changeDelay(){
  if(changing){
    lcd.clear();
    delay(350);
    changing = false;
  }
}












//LOOPin aloituksessa suorittaa start()
void loop() { 
  if(!started){ 
    start();
  }
  if(started){
    toInterface(page); //startin jälkeen alkaa pyöritäämään toInterfacea riippuen mikä sivu sillä hetkellä on valittuna
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









