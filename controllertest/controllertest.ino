#include <ResponsiveAnalogRead.h>

#include <Servo.h>

#include <AccelStepper.h>
#include <MultiStepper.h>


AccelStepper hinge(AccelStepper :: FULL4WIRE,39,41,43,45);

Servo seg3;

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
const byte flip = 22; // not actually used yet
const byte refoldButton = 24; // to be implemented for that bonus
// misc consts
// flip consts
const byte normal = 0; // not verified
const byte invert = 180; // not verified
const byte change = invert - normal;
// analog angle consts
const int baseMin = 0; // verified by startup calibration
const int baseMax = 1000; // need to check this



// misc vars
volatile byte flipPos = normal; // volatile because it's ref'd inside an ISR
volatile byte coinsLeft = 0; // keeps track of coins left. assumes 10 coins when flipped
volatile byte isFlipping = 0;
int pos = 1000;
int elbpos = 150;


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
  //delay(100);
  base.moveTo(pos);
  elbow.moveTo(elbpos);
  
  //delay(100);
  seg3.attach(200);
  seg3.write(45);
  //delay(100);
  elbow.moveTo(40);
  while(elbow.distanceToGo() != 0) elbow.run();
  //delay(100);
  elbow.moveTo(elbpos);
  seg3.write(90);
  delay(50);
}

void setup() {
  // put your setup code here, to run once:
  unfold();
  // set user inputs, digital
  // TODO: actually map ISRs to interrupt pins
}

void loop() {
  // put your main code here, to run repeatedly:
  // stepper moveTo and runs
  //if(abs(basePos - basePrevious) > 3)
  base.moveTo(baseReading.getValue());
  //if(abs(elbowPos-elbowPrevious) > 3) 
  elbow.moveTo(elbowReading.getValue());

  base.run();
  elbow.run();

  seg3.write(map(effReading.getValue(),0,1023,0,180));
  //basePrevious = basePos;
  //elbowPrevious = elbowPos;
  
  // write servo values (they will change when interrupts are  t r i g g e r e d)
}

void isr_flip() { // currently unused until effector is sorted out
  flipPos = flipPos - change;
  if (flipPos == invert && coinsLeft == 0) {
    coinsLeft = 10;
  }
}

void isr_eject() {
  // set a global flag variable to request an eject
  if (isFlipping == 0) {
    isFlipping = 1;
  }
}

