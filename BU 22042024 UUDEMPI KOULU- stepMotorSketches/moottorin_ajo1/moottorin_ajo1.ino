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


 String suunta;

bool k = true;
bool isTimer;
bool s;
bool p;

int n;
int timeWent;
int sarja;

Stepper stepMotor(STEPPER_STEPSIZE, DRIVER_IN1, DRIVER_IN2, DRIVER_IN3, DRIVER_IN4);










void setup() {
  Serial.begin(9600);
  stepMotor.setSpeed(50);
}










void loop() {

 if (Serial.available() >0){

   sarja = Serial.read();
  Serial.println(sarja);
 }
 
  if(sarja == 208)
  {
    suunta = "oikea";
    Fmotor();
 }

    
  if(sarja == 184)
  {
    suunta = "vasen";
    Fmotor();
}
    if (!(sarja == 2 || sarja == 3)){
      suunta = " ";
    }

}



void Fmotor() {

  if (suunta == "oikea"){

    stepMotor.step(1);
    Serial.println("OK OIK");
    delay(4);

  }
  if (suunta == "vasen"){
  
   stepMotor.step(-1);
   Serial.println("OK VAS");
   delay(4);
  }
   suunta == "oikea";
   

}





void timerRESET(){
timerValue = 0;
n = 0;
isTimer = false;
}









void timerON(){

  if(!isTimer){
    zeroTime = millis();     
    isTimer = true;         
  }

  timerValue = millis()-zeroTime;
  if(timerValue%1000 == 1){
    timeWent++;
  }
  
  Serial.write(timeWent);

  n = timerValue%2000;
  int p = timerValue / 1000;
  Serial.print(p);
  if (n >1000){
    takeSteps();
  }
}






void takeSteps(){
  stepMotor.step(1);
  delay(4);
}









/*



"int timerValue" on se aika jota käytetään ajastimissa, se siis voidaan nollata ja laittaa pyörimään.
Se toimii millisekntien tarkkuudella


Lähteet koodeille: 

Funktiot: https://www.arduino.cc/reference/en/

*/

