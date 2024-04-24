#include <Stepper.h>

#define MOTOR_STEPS 64
#define MOTOR_GEAR 64
#define STEPPER_STEPSIZE MOTOR_STEPS*MOTOR_GEAR
#define DRIVER_IN1 8
#define DRIVER_IN2 9
#define DRIVER_IN3 10
#define DRIVER_IN4 11


//unsigned long on suurempi long arvo, negatiivisetkin arvot ovat käännetty positiiviseksi
unsigned long timerValue = 0;   //käytetään millis() -funktion arvon talentamiseen
unsigned long zeroTime;         //käytetään nollaamaan timervalue sen hetkisellä millis() -funktion arvolla


bool isTimer;   //onko timer päällä vai ei true/false
bool timerSetOn;  //käytännössä apu-boolean, jolla erotellaan logiikkaa toisistaan 
bool isReset;


int sarja;
int stepSuunta;
int vastaanotto;
int newValueCount;
int timeLimit;
int suunta;
int stepsTaken;


//apu-muuttujat
int a;
int b;
int c;


int n; //käytetään tallentamaan 2000 millisekunnin jaksoja timerissa


int ib;   // "incoming Byte" käytetään tallentamaan kaikki saapuvat bitit Serialista


//ajettaessa moottoria käytetään "stepMotor" objektia
Stepper stepMotor(STEPPER_STEPSIZE, DRIVER_IN1, DRIVER_IN3, DRIVER_IN2, DRIVER_IN4); //kertoo millä inputeilla ajetaan moottoria ja askeleiden suuruus












//metodi jolla asetetaan timerin max aika-arvo sen mukaan mitä serialiin tulee HUOM! käytetään int sarjaa, ei suoraan Serial.read()
void setNewvalue(){ 

if(sarja <=9 && sarja >= 0 || sarja == 160){    //JOS sarja on 0-9 tai 160, niin switch/case voidaan suorittaa, muuten ei
  switch(newValueCount){  //newValueCount -arvoon lisätään aina 1 kun a, b, tai c on tallennettu. 
    case 0:
        a = sarja;                  // newValueCount = 0  ->  tallenetaan a:n aroksi sarja (0-9)
        newValueCount++;                  
        sarja = -1;
        break;
      
    case 1:
        b = sarja;                  // newValueCount = 1  ->  tallenetaan b:n aroksi sarja (0-9)
        newValueCount++;
        sarja = -1;
        break;

    case 2:
        c = sarja;                  // newValueCount = 2  ->  tallenetaan c:n aroksi sarja (0-9)
        newValueCount++;
        sarja = -1;
        break;
      
    case 3:                               //kun newValueCount = 3, niin odotetaan komentoa "160". Se tulee kun hyväksytään MEGAsta asetettu arvo
    if(sarja == 160){                     //sen jälkeen timeLimitiksi tallennetaan: a*100 + b*10 + c ja sitten logiikan apuarvot nollataan. Ja vastaanotto = 0
      timeLimit = (a*100)+(b*10)+(c); 
      a = 0;
      b = 0;
      c = 0;
      newValueCount = 0; 
      vastaanotto = 0;
      break;
    }   
   }   
 }
}
  










//Aloittaa timerin, ainoastaan siis UNOssa
//Aloitus tapahtuu, kun MEGAssa display sivulla painetaan 5
void timerON(){
  isReset = false;    //antaa mahdollisuuden resetoida timer
  if(!isTimer){       //aina kun timer käsketään päälle niin tämä pyörähtää vain kerran, jotta saadaan sen hetkinen aika-arvo, joka miinustetaan alempana
    zeroTime = millis();     // "miinusaika"
    isTimer = true;           //estää timerin laittamisen päälle uudestaan, ennen resettiä
  }

  timerValue = millis()-zeroTime;   //timervalue:  tämän hetkinen arduinon päälläolo aika  -  aloitusaika  =  aika mennyt  

  if(timerValue / 1000 <= timeLimit){   //JOS timerin aika (timerValue) jaettuna 1000 on pienempi kuin MEGAssa asetettu maximi aika  
    n = timerValue % 2000;          //n on timerin ajan jakojäännös tuhannesta. eli n kasvaa nollasta kahteen tuhanteen ja sitten aloittaa alusta
    if (n >= 1000){
      stepSuunta = 1;  // (suunta oikealle)
                                 //jos alle 1000ms (1s) on mennyt niin ei tapahdu mitään
      takeSteps(stepSuunta);      //jos 1000ms (1s) tai yli on mennyt niin ottaa askelia niin monta kuin kerkiää (yhden sekunnin aikana)
      stepsTaken++;               //laskee otetut askeleet mutta niitä ei dokumentoida mihinkään
    }
  }
}







//nollaa timerin, ainoastaan siis UNOssa
//nollaus tapahtuu, kun MEGAssa display sivulla painetaan 5
void timerRESET(){
  if(!isReset){       //IF !isReset eli isReset = false ---> timer voidaan nollata vain jos timer on jo päällä, koska timerON():ssa laitetaan isReset = true
    timerValue = 0;
    n = 0;
    isTimer = false;  //laittaa isTimer = false, koska timerin päällelaitossa tarvitaan zeroTime ja isTimerin pitää olla epätosi jotta se voidaan ottaa 

    stepsTaken = 0;   //olisi askeleiden määrän nollaus, mutta askeleiden määrä ei dokumentoida mihinkään

    isReset = true;   // estää uuden resetoimisen, ennen kuin timer on taas laitettu päälle uudestaan
  }
}










void takeSteps(int step){   //ottaa askelia sen mukaan mikä suunta step on -> step = 1 (yksi askel oikealle) tai step = -1 (yksi askel vasemmalle)
  stepMotor.step(step);
  delay(4);                 //tässä delay 4ms, jotta steppimoottori pysyy perässä (pienemmällä ei toiminut)
}









void setup() {
  Serial.begin(9600);         //Serial nopeuteen 9600 (sama kuin MEGAssa)
  stepMotor.setSpeed(100);    //stepMotorin nopeus maximiin
}








//KOKO logiikka jolla ohjataan metodeja (toimintoja) esim. pakko-ohjaus tai timer
void loop() {

  if(timerSetOn){
    timerON();

  }else if(!timerSetOn){
    timerRESET();
  }

  if(Serial.available() > 0 ){
    ib = Serial.read();
    if((ib <= 9 && ib >=0) || ib == 70 || ib == 120 || ib == 130 || ib == 140 || ib == 150 || ib == 160 || ib == 170 || ib == 180 || ib == 200 ){     //"suodattaa" Serial.read() -arvoja
      //^^^^ JOS ib eli serial.read() on joko 0-9 tai joku ylempänä olevista arvoista^^^^

      sarja = ib;     //tallentaa suodattimen läpi pääseen arvon sarja:an eikä siis seuraavaa tulevaa Serial.read() -arvoa
      Serial.print("SARJA: ");
      Serial.print(sarja);
      Serial.print(",    ");
      Serial.print("suunta: ");
      Serial.print(suunta);
      Serial.println();
    }
  }


  //sarjan JOS-lauseet
  if (sarja == 120) {   //laittaa ja pitää timerin päällä
    timerSetOn = true;
  }

  if (sarja == 130) {   //laittaa timerin pois päältä ja resetoi sen
    timerSetOn = false;
  }

  if (sarja == 200){
    vastaanotto = 2; //pakkoajon vastaanottamisen salliminen
  }

  if (sarja == 150) {
    vastaanotto = 1;  //uuden ajan asettamisen salliminen
  }

  if(sarja == 70){    //nollaa aika-arvojen asettamisen
    vastaanotto = 0;
    newValueCount = 0;
    a = 0;
    b = 0;
    c = 0;
  }



  //vastaanoton JOS-lauseet
  //(vastaanotto voi olla myös 0, jolloin ei tapahdu mitään)

  //JOS vastaanotto == 1 eli uuden aika-arvon asettamisen ja timerin käytön salliminen
  if (vastaanotto == 1){
    setNewvalue();
  }

  //JOS vastaanotto == 2 eli pakko-ohjauksen logiikan salliminen
  if(vastaanotto == 2){   
    if(sarja == 170){   //Pakkoajaa vasemmalle
      suunta = -1;
      stepMotor.step(suunta);
      delay(4);
    }
    if(sarja == 180){   //Pakkoajaa oikealle
      suunta = 1;
      stepMotor.step(suunta);
      delay(4);
    }
    if(sarja == 140){ //pakkoajo sallittu, mutta ei suuntaa eikä käsketty suorittamaan  
      suunta = 0;
    }
  }
}


 

 





/*
Lähteet koodeille: 

Funktiot: https://www.arduino.cc/reference/en/

*/

