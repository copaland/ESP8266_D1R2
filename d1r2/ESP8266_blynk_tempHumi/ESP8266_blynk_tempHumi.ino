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
#include <DHTStable.h>

/* Fill in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL6qhqeAA9P"
#define BLYNK_TEMPLATE_NAME "TempMon"
#define BLYNK_AUTH_TOKEN "24_4s3J9Yjrjwg5QnjLTQpylNlePso1A"

char auth[] = BLYNK_AUTH_TOKEN;  // Blynk 앱에서 생성된 인증 토큰을 입력하세요

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "Wokwi-GUEST"; //"YourNetworkName";
//char pass[] = ""; //"YourPassword";

// WiFi 정보를 입력하세요
char ssid[] = "copaland";
char pass[] = "24868080";

#define DHT_PIN 4    // DHT11 센서를 연결한 핀 번호
#define SWITCH1_PIN 16  // 스위치 버튼 1을 연결한 핀 번호
#define SWITCH2_PIN 5   // 스위치 버튼 2를 연결한 핀 번호
#define LED1_PIN 14     // LED1을 연결한 핀 번호
#define LED2_PIN 12     // LED2를 연결한 핀 번호

DHTStable DHT;
BlynkTimer timer;
bool led1state = LOW;
bool led2state = LOW;
float t;
float h;

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
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
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
  
  timer.setInterval(1000L, readDHT);  // 1초마다 DHT 센서 값을 읽기 위한 타이머 설정
}

void loop() {
  Blynk.run();
  timer.run();
  checkSwitchStates();
}

void readDHT() {
  int chk = DHT.read11(DHT_PIN);
  if(chk == DHTLIB_OK){
    float humi = DHT.getHumidity();
    h = Truncate(humi);
    float temp = DHT.getTemperature();
    t = Truncate(temp);

    Blynk.virtualWrite(V4, t);   // 가상 핀 V4에 온도 값을 보냄
    Blynk.virtualWrite(V5, h);   // 가상 핀 V5에 습도 값을 보냄
  }
}
/*
void readDHT()
{
int chk;
float hf,tf;

  chk = DHT.read11(DHT_PIN); //chk = DHT.read(DHT11_PIN); // //READ DATA// 
  switch (chk)
  {
    case DHTLIB_OK:  
        hf = DHT.getHumidity();
        h = Truncate(hf);

        tf = DHT.getTemperature(); // or dht.readTemperature(true) for Fahrenheit
        t = Truncate(tf);

        Serial.print("OK        "); 
        Serial.print(DHT.getHumidity(), 1); //Serial.print(DHT.humidity,1);//
        Serial.print("       ");
        Serial.println(DHT.getTemperature(), 1); //Serial.print(DHT.temperature,1);//
     
        Blynk.virtualWrite(V4, t);
        Blynk.virtualWrite(V5, h);
        
        break;
    case DHTLIB_ERROR_CHECKSUM: 
        Serial.print("Checksum error        "); 
        break;
    case DHTLIB_ERROR_TIMEOUT: 
        Serial.print("Time out error        "); 
        break;
    default: 
        Serial.print("Unknown error         "); 
        break;
  }
}
*/
float Truncate(float input)
{
  int i = (int)(input * 10.0);
  return i/10.0;
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

/*
#define BLYNK_PRINT Serial
#include <DHTStable.h>//
DHTStable DHT;
#define DHT11_PIN 4

//#define KEY1_vpin    V0
//#define KEY2_vpin    V1

int POT = A0;
int KEY1 = 16; //pullup
int KEY2 = 5; //pullup
int LED1 = 14; //source
int LED2 = 12; //source
//int LED2 = 2; //SINK ir Sink


#define BLYNK_TEMPLATE_ID "TMPL6qhqeAA9P"
#define BLYNK_TEMPLATE_NAME "TempMon"
#define BLYNK_AUTH_TOKEN "24_4s3J9Yjrjwg5QnjLTQpylNlePso1A"//"YourAuthToken"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "Wokwi-GUEST"; //"YourNetworkName";
//char pass[] = ""; //"YourPassword";

char ssid[] = "copaland"; //"YourNetworkName";
char pass[] = "24868080"; //"YourPassword";

BlynkTimer timer;
//WidgetLED led1(V2);
//WidgetLED led2(V3);

float t;
float h;
bool LED1_state = LOW;
bool LED2_state = LOW;
bool SW1_state = HIGH;
bool SW2_state = HIGH;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
}

//--------------------------------------------------------------------------
// This function is called every time the Virtual Pin state change
//i.e when web push switch from Blynk App or Web Dashboard
BLYNK_WRITE(V0) {
  SW1_state = param.asInt();
  digitalWrite(LED1, SW1_state);
  Blynk.virtualWrite(V2, SW1_state); 
}
BLYNK_WRITE(V1) {
  SW2_state = param.asInt();
  digitalWrite(LED2, SW2_state);
  Blynk.virtualWrite(V3, SW2_state); 
}

void setup()
{
  // Debug console
  Serial.begin(9600);

  pinMode(KEY1, INPUT_PULLUP);
  pinMode(KEY2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW); //OFF
  digitalWrite(LED2, LOW); //Off

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, DHT11); //READ SENSOR
}

void loop()
{
  Blynk.run();
  timer.run();
  listen_push_buttons();
}

void DHT11()
{
int chk;
float hf,tf;

  chk = DHT.read11(DHT11_PIN); //chk = DHT.read(DHT11_PIN); // //READ DATA// 
  switch (chk)
  {
    case DHTLIB_OK:  
        hf = DHT.getHumidity();
        h = Truncate(hf);

        tf = DHT.getTemperature(); // or dht.readTemperature(true) for Fahrenheit
        t = Truncate(tf);

        Serial.print("OK        "); 
        Serial.print(DHT.getHumidity(), 1); //Serial.print(DHT.humidity,1);//
        Serial.print("       ");
        Serial.println(DHT.getTemperature(), 1); //Serial.print(DHT.temperature,1);//
      
        Blynk.virtualWrite(V4, t);
        Blynk.virtualWrite(V5, h);
        
        break;
    case DHTLIB_ERROR_CHECKSUM: 
        Serial.print("Checksum error        "); 
        break;
    case DHTLIB_ERROR_TIMEOUT: 
        Serial.print("Time out error        "); 
        break;
    default: 
        Serial.print("Unknown error         "); 
        break;
  }
}

float Truncate(float input)
{
  int i = (int)(input * 10.0);
  return i/10.0;
}

void listen_push_buttons(){
  //--------------------------------------------------------------------------
  if(digitalRead(KEY1) == LOW){ //High LOW
    delay(100);
    //control_relay(1);
    LED1_state = SW1_state = !SW1_state;
    digitalWrite(LED1, LED1_state);
    Blynk.virtualWrite(V0, SW1_state); //update button state
    Blynk.virtualWrite(V2, LED1_state); 
  }
  //--------------------------------------------------------------------------
  else if (digitalRead(KEY2) == LOW){
    delay(100);
    //control_relay(2);
    LED2_state = SW2_state = !SW2_state;
    digitalWrite(LED2, LED2_state);
    Blynk.virtualWrite(V1, SW2_state); //update button state
    Blynk.virtualWrite(V3, LED2_state);
  }
}
*/