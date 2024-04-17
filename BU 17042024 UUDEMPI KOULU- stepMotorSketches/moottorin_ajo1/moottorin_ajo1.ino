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




bool k = true;
bool isTimer;
bool s;
bool p;

int n;
int timeWent;


Stepper stepMotor(STEPPER_STEPSIZE, DRIVER_IN1, DRIVER_IN2, DRIVER_IN3, DRIVER_IN4);










void setup() {
  Serial.begin(9600);
  stepMotor.setSpeed(50);
}










void loop() {
  int l = Serial.read();
  if(l == 100){
    p = true;
  }if(l == 200){
    p = false;
  }

  if(p){
    timerON();
  }if(!p){
    timerRESET();
  }
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

