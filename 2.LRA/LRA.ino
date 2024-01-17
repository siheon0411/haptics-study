short sinewave[128] = { 245, 248, 250, 251, 253, 254, 254, 255,
                        255, 255, 254, 254, 253, 251, 250, 248,
                        245, 243, 240, 237, 234, 230, 226, 222,
                        218, 213, 208, 203, 198, 193, 188, 182,
                        176, 170, 165, 158, 152, 146, 140, 134,
                        128, 121, 115, 109, 103, 97, 90, 85,
                        79, 73, 67, 62, 57, 52, 47, 42,
                        37, 33, 29, 25, 21, 18, 15, 12,
                        10, 7, 5, 4, 2, 1, 1, 0,
                        0, 0, 1, 1, 2, 4, 5, 7,
                        10, 12, 15, 18, 21, 25, 29, 33,
                        37, 42, 47, 52, 57, 62, 67, 73,
                        79, 85, 90, 97, 103, 109, 115, 121,
                        128, 134, 140, 146, 152, 158, 165, 170,
                        176, 182, 188, 193, 198, 203, 208, 213,
                        218, 222, 226, 230, 234, 237, 240, 243
                      };

int LRA_PIN = DAC1; // DAC1 핀을 사용하여 LRA 연결
int UPDATE_RATE = 1000; // 업데이트 속도 (Hz)
const int NUM_SAMPLES = 128; // 룩업 테이블의 데이터 수

void setup() {
// DAC 설정
  analogWriteResolution(8); // 아날로그 출력 해상도 설정 (8비트)
  analogWrite(LRA_PIN, 128); // 초기 출력 값 설정 (0에서 255 사이의 중간값)  
  Serial.begin(115200);
  Serial.println("start");

}

void loop() {
  //unsigned long startTime = micros(); // 시작 시간 기록

  for (int i = 0; i < NUM_SAMPLES; i++) {
    int outputValue = map(sinewave[i], -128, 127, 0, 255);
    analogWrite(LRA_PIN, outputValue); // 변환된 값을 DAC에 출력
    Serial.println(outputValue);
    // delayMicroseconds(1000000 / UPDATE_RATE); // Delay 시간 계산
    delay(1000 / UPDATE_RATE);
  }

  //unsigned long elapsedTime = micros() - startTime; // 경과 시간 계산
  // 필요한 경우 시간을 조정하여 정확한 주파수를 맞출 수 있음
  // 오실로스코프로 생성된 신호를 확인할 수 있음
}