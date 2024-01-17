int motorPin = 13;
int in1 = 11;
int in2 = 12;
int speed = 0;  // 0~255

void setup() {
  // put your setup code here, to run once:

  pinMode(motorPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  Serial.begin(115200);
  Serial.println("start");
}

void loop() {
  // put your main code here, to run repeatedly:

  // if(Serial.available()) {
  //   speed = Serial.readString().toInt();
  //   digitalWrite(in1, HIGH);
  //   digitalWrite(in2, LOW);
  //   analogWrite(motorPin, speed);
  //   Serial.println(speed);
  // }
  //Go : 핀 3, 핀 4 에 신호를 다르게 하면 전진
  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);

  analogWrite(motorPin, 10);
  delay(2000);//5sec 

  //Back : 핀 3, 핀 4 에 신호를 다르게 하면 후진
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);  
  delay(2000);//5sec


}
