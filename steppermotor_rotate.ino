

#include <Stepper.h>

const int stepsPerRevolution = 300;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, D0, D1,D2, D3);

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(50);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {
  // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  //delay(500);

  
}
