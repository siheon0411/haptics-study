int pin = 13;
int amplitude = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin, OUTPUT);
  Serial.begin(115200);
  Serial.println("start");

}

void loop() {
  // put your main code here, to run repeatedly:

  if(Serial.available()) {
    amplitude = Serial.readString().toInt();
    analogWrite(pin, amplitude);
    Serial.println(amplitude);
  }

}
