#include <ResponsiveAnalogRead.h>

// #include <ResponsiveAnalogRead.h>

#include <Servo.h>

#include <AccelStepper.h>
#include <MultiStepper.h>


AccelStepper hinge(AccelStepper :: FULL4WIRE,39,41,43,45);

Servo seg3;
Servo claw;

AccelStepper base(AccelStepper :: FULL2WIRE,12,13);
AccelStepper elbow(AccelStepper :: FULL4WIRE,47,49,51,53);; // enable on 50

const byte elbow_en = 50;
const byte basePotPin = A7;
const byte elbowPotPin = A8;
const byte effectorPotPin = A9; // manually adjust how "open" the end effector is

//smoothed analog inputs
ResponsiveAnalogRead baseReading(basePotPin,true);
ResponsiveAnalogRead elbowReading(elbowPotPin,true);
ResponsiveAnalogRead effReading(effectorPotPin,true);

// add some buttons or whatever
const byte rightbutton = 22; // 
const byte leftbutton = 24; //
// misc consts
// flip consts
const byte normal = 0; // not verified
const byte invert = 180; // not verified
const byte change = invert - normal;
// analog angle consts
const int baseMin = 0; // verified by startup calibration
const int baseMax = 1000; // need to check this

// misc vars
int pos = 1000;
int elbpos = 150;

// interrupt vars
volatile char claw_moving_dir = 0;

void unfold() {
  // put your setup code here, to run once:
  elbow.setMaxSpeed(100);
  elbow.setAcceleration(30);
  elbow.setSpeed(25);
  hinge.setMaxSpeed(1000);
  hinge.setAcceleration(50);
  hinge.setSpeed(20);

  elbow.moveTo(20);
  while(elbow.distanceToGo() != 0) elbow.run();
  //delay(100);

  hinge.moveTo(75);
  while(hinge.distanceToGo() != 0) hinge.run();
  //delay(100);

  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
  pinMode(11,OUTPUT);
  digitalWrite(11,HIGH);
  pinMode(8,OUTPUT);
  digitalWrite(8,LOW);
  pinMode(9,OUTPUT);
  digitalWrite(9,LOW);
  base.setMaxSpeed(500);
  base.setAcceleration(200);
  elbow.setMaxSpeed(500);
  elbow.setAcceleration(150);
  //elbow.setEnablePin(50);
  //elbow.setMaxSpeed(3000);
  //base.setAcceleration(1000);
  base.setSpeed(-200);
  elbow.setSpeed(-20);
  pinMode(10,INPUT);
  while(!digitalRead(10)) {
    base.runSpeed();
  }
  pinMode(7,INPUT);
  while(!digitalRead(7)) {
    elbow.runSpeed(); 
  }
  base.stop();
  elbow.stop();
  base.setCurrentPosition(0);
  elbow.setCurrentPosition(0);
  delay(100);
  base.moveTo(pos);
  elbow.moveTo(elbpos);
  
  delay(100);
  seg3.attach(6);
  seg3.write(45);
  delay(100);
  elbow.moveTo(40);
  while(elbow.distanceToGo() != 0) elbow.run();
  delay(100);
  elbow.moveTo(elbpos);
  seg3.write(90);
  delay(50);
}

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  unfold();
  pinMode(rightbutton,INPUT);
  pinMode(leftbutton,INPUT);
  // set user inputs, digital
  // TODO: actually map ISRs to interrupt pins
  // both buttons currently used have pulldown resistors on the pin's node, so rising = pressed
  claw.attach(5); // pin number TBD
  attachInterrupt(digitalPinToInterrupt(24),isr_left_press,RISING); // check parameters against hardware config
  attachInterrupt(digitalPinToInterrupt(24),isr_release,FALLING);
  attachInterrupt(digitalPinToInterrupt(22),isr_release,FALLING);
  attachInterrupt(digitalPinToInterrupt(22),isr_right_press,RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  // update analogValues
  baseReading.update();
  elbowReading.update();
  effReading.update();

  int basePos = baseReading.getValue();
  basePos = map(basePos,0,1023,0,980);
  basePos = constrain(basePos,0,980);
  int elbowPos = elbowReading.getValue();
  elbowPos = map(elbowPos, 69, 1013, 20, 170);
  elbowPos = constrain(elbowPos,20,170);
  // stepper moveTo and runs
  //if(abs(basePos - basePrevious) > 3)
  base.moveTo(basePos);
  //if(abs(elbowPos-elbowPrevious) > 3) 
  elbow.moveTo(elbowPos);

  base.run();
  elbow.run();
  seg3.write(map(effReading.getValue(),0,1023,0,180));
  
  // write servo values (they will change when interrupts are  t r i g g e r e d)
  int clawpos = claw.read();
  if(digitalRead(rightbutton)) clawpos++;
  if(digitalRead(leftbutton)) clawpos--;
  clawpos = constrain(clawpos,0,180); // WARNING: these are probably horrifyingly wrong stop values
  
  claw.write(clawpos);

}


  

