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
int FREQ = 100; // 목표진동수 100Hz
int UPDATE_RATE = 1000; // 업데이트 속도 (Hz)
int rate = UPDATE_RATE / FREQ;
const int NUM = 128; // 룩업 테이블의 데이터 수

int idx = 0;

void setup() {
  // analogWriteResolution(8); // 아날로그 출력 해상도 설정 (8비트: 0~255)
  // Serial.begin(115200);
  // Serial.println("start");
}

void loop() {
  // for (int i = 0; i < NUM; i++) {
  //   int outputValue = sinewave[i];
  //   analogWrite(LRA_PIN, outputValue); // 사인파 출력 값을 DAC에 출력
  //   // Serial.println(outputValue);  // 출력 값 그리기용 프린트
  //   delay(10000 / NUM / FREQ);  // Delay 시간 계산
  // }

  int outputValue = sinewave[idx];
  analogWrite(LRA_PIN, outputValue);
  delay(1000 / FREQ);
  idx++;

  if (idx > 127) {
    idx = 0;
  }

  // 필요한 경우 시간을 조정하여 정확한 주파수를 맞출 수 있음
  // 오실로스코프로 생성된 신호를 확인
}