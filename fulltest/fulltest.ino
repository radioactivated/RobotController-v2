#include <AccelStepper.h>
#include <MultiStepper.h>
#include <Servo.h>

AccelStepper hinge(AccelStepper :: FULL4WIRE,39,41,43,45);

Servo seg3;

AccelStepper base(AccelStepper :: FULL2WIRE,12,13);
AccelStepper elbow(AccelStepper :: FULL4WIRE,47,49,51,53);; // enable on 50
const byte elbow_en = 50;
int pos = 1000;
int elbpos = 150;

void setup() {
  // put your setup code here, to run once:
  elbow.setMaxSpeed(100);
  elbow.setAcceleration(30);
  elbow.setSpeed(25);
  hinge.setMaxSpeed(100);
  hinge.setAcceleration(50);
  hinge.setSpeed(20);

  elbow.moveTo(20);
  while(elbow.distanceToGo() != 0) elbow.run();
  delay(1000);

  hinge.moveTo(75);
  while(hinge.distanceToGo() != 0) hinge.run();
  delay(1000);

  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
  pinMode(11,OUTPUT);
  digitalWrite(11,HIGH);
  pinMode(8,OUTPUT);
  digitalWrite(8,LOW);
  pinMode(9,OUTPUT);
  digitalWrite(9,LOW);
  base.setMaxSpeed(1000);
  base.setAcceleration(500);
  elbow.setMaxSpeed(1000);
  elbow.setAcceleration(150);
  //elbow.setEnablePin(50);
  //elbow.setMaxSpeed(3000);
  //base.setAcceleration(1000);
  base.setSpeed(-200);
  pinMode(10,INPUT);
  while(!digitalRead(10) || !digitalRead(7)) {
    if(!digitalRead(10)){
      base.runSpeed();
    }else{
      base.stop();
    }
    if(!digitalRead(7)){
      elbow.runSpeed();
    }else
      elbow.stop();
    } 
  }
  base.stop();
  elbow.stop();
  base.setCurrentPosition(0);
  elbow.setCurrentPosition(0);
  delay(200);
  base.moveTo(pos);
  elbow.moveTo(elbpos);
  
  delay(500);
  seg3.attach(6);
  seg3.write(45);
  delay(100);
  elbow.moveTo(40);
  while(elbow.distanceToGo() != 0) elbow.run();
  delay(100);
  elbow.moveTo(elbpos);
  seg3.write(90);
  delay(200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (base.distanceToGo() == 0) {
    //delay(100);
    pos = 1000 - pos;
    base.moveTo(pos);
  }
  
 // elbow.runSpeed();

  if (elbow.distanceToGo() == 0) {
    delay(100);
    elbpos = 175-elbpos;
    elbow.moveTo(elbpos);
}
  elbow.run();
  base.run();
}
