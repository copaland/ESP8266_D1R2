/*************************************************************
  Blynk is a platform with iOS and Android apps to control
  ESP32, Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build mobile and web interfaces for any
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: https://www.blynk.io
    Sketch generator:           https://examples.blynk.cc
    Blynk community:            https://community.blynk.cc
    Follow us:                  https://www.fb.com/blynkapp
                                https://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************
  This example runs directly on ESP8266 chip.

  NOTE: This requires ESP8266 support package:
    https://github.com/esp8266/Arduino

  Please be sure to select the right ESP8266 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/
//ESP8266 Standalone
/*
uno d1 remarks
A0 A0 POT
xA1 x CDS
xA2 x LM35
D0 3 RXD
D1 1 TXD 
D2 16 S2
D3 5  S3
D4 4 SCL DHT11
D5 0 SDA BUZZER sink
D6 2 IR with LED sink
D7 14
D8 12
D9 13 rRGB
D10 15 bRGB SS
D11 13 gRGB MOSI
D12 12 rLED MISO 
D13 14 bLED SCK
*/
/* Comment this out to disable prints and save space */
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6z9rg2rgr"
#define BLYNK_TEMPLATE_NAME "iotsw"
#define BLYNK_AUTH_TOKEN "B-Ka5YNaBFzKSjixrsBrq7A3JMj06VLm"//

char auth[] = BLYNK_AUTH_TOKEN;  // Blynk 앱에서 생성된 인증 토큰을 입력하세요

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "Wokwi-GUEST"; //"YourNetworkName";
//char pass[] = ""; //"YourPassword";

// WiFi 정보를 입력하세요
char ssid[] = "copaland";
char pass[] = "24868080";

#define SWITCH1_PIN 16  // 스위치 버튼 1을 연결한 핀 번호
#define SWITCH2_PIN 5   // 스위치 버튼 2를 연결한 핀 번호
#define LED1_PIN 14     // LED1을 연결한 핀 번호
#define LED2_PIN 12     // LED2를 연결한 핀 번호

BlynkTimer timer;
bool led1state = LOW;
bool led2state = LOW;

int switch1State = HIGH;  // 스위치 버튼 1의 상태 변수
int switch2State = HIGH;  // 스위치 버튼 2의 상태 변수
int lastSwitch1State = HIGH;  // 이전 스위치 버튼 1의 상태 변수
int lastSwitch2State = HIGH;  // 이전 스위치 버튼 2의 상태 변수
unsigned long lastSwitch1Time = 0;  // 이전 스위치 버튼 1의 시간
unsigned long lastSwitch2Time = 0;  // 이전 스위치 버튼 2의 시간
unsigned long debounceDelay = 50;  // 디바운스 딜레이 설정

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

//--------------------------------------------------------------------------
// This function is called every time the Virtual Pin state change
//i.e when web push switch from Blynk App or Web Dashboard
BLYNK_WRITE(V0) {
  switch1State = param.asInt();
  digitalWrite(LED1_PIN, switch1State);
  Blynk.virtualWrite(V2, switch1State); 
}
BLYNK_WRITE(V1) {
  switch2State = param.asInt();
  digitalWrite(LED2_PIN, switch2State);
  Blynk.virtualWrite(V3, switch2State); 
}

void setup() {
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  
  digitalWrite(LED1_PIN, LOW);  // 초기에 LED1은 꺼짐 상태로 설정
  digitalWrite(LED2_PIN, LOW);  // 초기에 LED2는 꺼짐 상태로 설정
  
  Blynk.begin(auth, ssid, pass);

}

void loop() {
  Blynk.run();
  timer.run();
  checkSwitchStates();
}

void checkSwitchStates() {
  // 스위치 버튼 1 상태 확인
  int reading1 = digitalRead(SWITCH1_PIN);
  if (reading1 != lastSwitch1State) {
    lastSwitch1Time = millis();  // 버튼 상태 변경 시간 저장
  }
  if ((millis() - lastSwitch1Time) > debounceDelay) {
    if (reading1 != switch1State) {
      switch1State = reading1;
      led1state = digitalRead(LED1_PIN);
      if (switch1State == LOW) {
        led1state = !led1state;
        digitalWrite(LED1_PIN, led1state);   // 스위치 버튼 1이 눌렸을 때 LED1 켜기
        Blynk.virtualWrite(V0, led1state);       // 가상 핀 V0에 1(켜짐)을 보냄
        Blynk.virtualWrite(V2, led1state); 
      }
    }
  }
  lastSwitch1State = reading1;  // 현재 버튼 상태를 이전 버튼 상태로 업데이트
  
  // 스위치 버튼 2 상태 확인
  int reading2 = digitalRead(SWITCH2_PIN);
  if (reading2 != lastSwitch2State) {
    lastSwitch2Time = millis();  // 버튼 상태 변경 시간 저장
  }
  if ((millis() - lastSwitch2Time) > debounceDelay) {
    if (reading2 != switch2State) {
      switch2State = reading2;
      led2state = digitalRead(LED2_PIN);
      if (switch2State == LOW) {
        led2state = !led2state;
        digitalWrite(LED2_PIN, led2state);   // 스위치 버튼 2가 눌렸을 때 LED2 토글
        Blynk.virtualWrite(V1, led2state);       // 가상 핀 V1에 토글 보냄
        Blynk.virtualWrite(V3, led2state);
      }
    }
  }
  lastSwitch2State = reading2;  // 현재 버튼 상태를 이전 버튼 상태로 업데이트
}
