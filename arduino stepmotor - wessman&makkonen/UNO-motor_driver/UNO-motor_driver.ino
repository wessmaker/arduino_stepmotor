#include <Stepper.h>

#define MOTOR_STEPS 64
#define MOTOR_GEAR 64
#define STEPPER_STEPSIZE MOTOR_STEPS*MOTOR_GEAR
#define DRIVER_IN1 8
#define DRIVER_IN2 9
#define DRIVER_IN3 10
#define DRIVER_IN4 11

#define second 1000
#define fullCircle 2048


unsigned long timerValue = 0;
unsigned long zeroTime;




bool isTimer;
bool s;
bool p;

bool timerSetOn;
bool isReset;

int n;
int timeWent;
int sarja;
int stepSuunta;
int vastaanotto;
int newValueCount;
int timeLimit;
int suunta;
int stepsTaken;

int a;
int b;
int c;

int ib;

Stepper stepMotor(STEPPER_STEPSIZE, DRIVER_IN1, DRIVER_IN3, DRIVER_IN2, DRIVER_IN4);










void setNewvalue(){
if(sarja <=9 && sarja >= 0 || sarja == 160){
  switch(newValueCount){
    case 0:
        a = sarja;
        newValueCount++;
        sarja = -1;
        break;
      
    case 1:
        b = sarja;
        newValueCount++;
        sarja = -1;
        break;

    case 2:
        c = sarja;
        newValueCount++;
        sarja = -1;
        break;
      
    case 3:
    if(sarja == 160){
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
  









void timerRESET(){
  if(!isReset){
    timerValue = 0;
    n = 0;
    isTimer = false;
    stepsTaken = 0;
    isReset = true;
  }
}









void timerON(){
  isReset = false;
  if(!isTimer){
    zeroTime = millis();     
    isTimer = true;         
  }
  timerValue = millis()-zeroTime;
 
  

  if(timerValue / 1000 <= timeLimit){
    n = timerValue % 2000;
    if (n >= 1000){
      stepSuunta = 1;
      takeSteps(stepSuunta);
      stepsTaken++;
    }
   
  }
}






void takeSteps(int step){
  stepMotor.step(step);
  delay(4);
}













void setup() {
  Serial.begin(9600);
  stepMotor.setSpeed(100);
}








//Ongelma "int sarja" käyttämisessä oli, että kun Serial.available() > 0 lakkaa olemasta,
//niin sarja jää sihen arvoon mihinkä se viimeksi asetettiin, eli se jättää ohjelman päälle 

void loop() {
  

if(timerSetOn){
  timerON();

}else if(!timerSetOn){
  timerRESET();

}


  if(Serial.available() > 0 ){
    ib = Serial.read();
    if((ib <= 9 && ib >=0) || ib == 70 || ib == 130 || ib == 120 || ib == 140 || ib == 150 || ib == 160 || ib == 170 || ib == 180 || ib == 200 ){
      sarja = ib;

      Serial.print("SARJA: ");
      Serial.print(sarja);
      Serial.print(",    ");
      Serial.print("suunta: ");
      Serial.print(suunta);
      Serial.println();

    }

    
   
    }




  if (sarja == 120) {
    timerSetOn = true;
  }

  if (sarja == 130) {
    timerSetOn = false;
  }

  if (sarja == 200){
    vastaanotto = 2; //PAKKOAJON VASTAANOTTO
  }

  if (sarja == 150) {
    vastaanotto = 1;  //UUDEN AJAN VASTAANOTTO
  }


  
  if(sarja == 70){
    vastaanotto = 0;
    newValueCount = 0;
    a = 0;
    b = 0;
    c = 0;
  }



  if(vastaanotto == 2){
    if(sarja == 170){
      suunta = -1;
      stepMotor.step(suunta);
      delay(4);
    }
    if(sarja == 180){
      suunta = 1;
      stepMotor.step(suunta);
      delay(4);
    }
    if(sarja == 140){
      suunta = 0;
    }
  }


  if (vastaanotto == 1){
    setNewvalue();
  }

  
    
   
    
}


 

 









/*



"int timerValue" on se aika jota käytetään ajastimissa, se siis voidaan nollata ja laittaa pyörimään.
Se toimii millisekntien tarkkuudella


Lähteet koodeille: 

Funktiot: https://www.arduino.cc/reference/en/

*/

