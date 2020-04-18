void setup(){
  pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  digitalWrite(31, HIGH);
  digitalWrite(30, HIGH);
  digitalWrite(29, HIGH);
  digitalWrite(28, HIGH);
}

void loop(){
  digitalWrite(1,digitalRead(31));
  digitalWrite(2,digitalRead(30));
  digitalWrite(3,digitalRead(29));
  digitalWrite(4,digitalRead(28));
}
