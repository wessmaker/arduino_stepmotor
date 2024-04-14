#include <Stepper.h>

#define MOTOR_STEPS 64
#define MOTOR_GEAR 64
#define STEPPER_STEPSIZE MOTOR_STEPS*MOTOR_GEAR
#define DRIVER_IN1 8
#define DRIVER_IN2 9
#define DRIVER_IN3 10
#define DRIVER_IN4 11

#define second 1000 //Voidaan muuttaa ja toimii silti
#define fullCircle 2048


unsigned long timerValue = 0;
unsigned long zeroTime;
bool timerMode = false;          //timerMode (false - true), false = älä ota askelia vaan odota 1s, true = ota askelia sekunnin ajan
unsigned long timePassed = 0;


bool isTimer;
bool switched = false;

bool k = true;
bool s;

int sTime;
int n;


/*
ALIMMAISENA ON TIETOA KOODISTA KOMMENTOITUNA!
*/




Stepper stepMotor(STEPPER_STEPSIZE, DRIVER_IN1, DRIVER_IN2, DRIVER_IN3, DRIVER_IN4);



void setup() {
  Serial.begin(38400);
  stepMotor.setSpeed(100);

  
  

  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);


}




void loop() {
if(isTimer){
  
  Serial.print(", ");
  Serial.print("TimerValue: ");
  Serial.print(timerValue);
  Serial.println();
  timerON(); //timerON pitää pyöriä koko ajan jotta se toimii, ehkä tekisi oman funktion jolla pyörittäisi nollattua timeriä
}
  Serial.println();
  Serial.print("TIMERMODE: ");
  Serial.print(timerMode);
  Serial.print("  ");


  testButtons();
  debug();
  

  
  
  
  
  

  if(timerValue >=0 && isTimer && timerMode){
    stepMotor.step(1);
    Serial.print("STEPPED");
  
  }
    

}




/*
int calcTimePassed(unsigned long nt){
return timePassed;
}
*/

void timerRESET(){
timerValue = 0;
isTimer = false;
timerMode = false;
n = 0;
k = true;
sTime = 0;

Serial.print("Timer reset!");
}




void timerON(){
  
  if(!isTimer){
    zeroTime = millis();     // ottaa aika-arvon sisäiseltä ajastimella jolla nollataan timerValue
    timerMode = false;
    isTimer = true;         //estää tämän if komennon pyörimisen uudestaan ennen resettiä
  }
  timerValue = millis()-zeroTime;
  
  



  if(k){
    n = timerValue/second; // sekunnit mennyt
    k = false;
  }
  

  sTime = timerValue/second - n;


  Serial.println();
  Serial.print(sTime);
  Serial.print(",   ");
  Serial.print(n);
  Serial.print(",   ");
  Serial.print(timerValue/second);
  Serial.print(",   T: ");
  Serial.print(timerMode);


  
 
  if (sTime == 1){
        timerMode = true;
    }
  else if (sTime == 0){
    timerMode = false;
  }
  if (sTime == 2){
    k = true;
  }




}
void debug(){
  if (timerValue > 999000){
    Serial.println();
    Serial.print("DEBUG: timerValue on yli 999s, timerValue: ");
    Serial.print(timerValue);
  }
}

void testButtons(){
    if(digitalRead(6) == HIGH){
    digitalWrite(2, LOW);
  }
  else{
    digitalWrite(2, HIGH);
    timerON();

  }

  if(digitalRead(7) == HIGH){
    digitalWrite(4, LOW);
  }
  else{
    digitalWrite(4, HIGH);
    timerRESET();
  }
}








/*



"int timerValue" on se aika jota käytetään ajastimissa, se siis voidaan nollata ja laittaa pyörimään.
Se toimii millisekntien tarkkuudella


Lähteet koodeille: 

Funktiot: https://www.arduino.cc/reference/en/

*/

