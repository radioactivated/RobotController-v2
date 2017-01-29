const byte basePotPin = A0;
const byte elbowPotPin = A1;
const byte effectorPotPin = A2; // manually adjust how "open" the end effector is

void setup() {
  Serial.begin(9600);
  //Serial.println("Ready");
  pinMode(7,INPUT);
  pinMode(8,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int basePos = analogRead(basePotPin);
  basePos = map(basePos,0,1023,1023,0);
  int elbowPos = analogRead(elbowPotPin);
  elbowPos = map(elbowPos, 69, 1013, 0, 1023);
  int effectorPos = analogRead(effectorPotPin);

  Serial.print(digitalRead(7) * 1023);
  Serial.print(" ");
  Serial.print(digitalRead(8) * 1023);
  Serial.print(" ");
  Serial.print(basePos);
  Serial.print(" ");
  Serial.print(elbowPos);
  Serial.print(" ");
  Serial.println(effectorPos);

  delay(20);
}
