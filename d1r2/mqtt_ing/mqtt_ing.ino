#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTStable.h>

// WiFi 정보를 입력하세요
const char* ssid = "YourWiFiSSID";
const char* password = "YourWiFiPassword";

const char* mqttServer = "YourMQTTBrokerIP";
const int mqttPort = 1883;
const char* mqttUser = "YourMQTTUser";
const char* mqttPassword = "YourMQTTPassword";

#define DHT_PIN 4    // DHT11 센서를 연결한 핀 번호
#define SWITCH1_PIN 16  // 스위치 버튼 1을 연결한 핀 번호
#define SWITCH2_PIN 5   // 스위치 버튼 2를 연결한 핀 번호
#define LED1_PIN 14     // LED1을 연결한 핀 번호
#define LED2_PIN 12     // LED2를 연결한 핀 번호

DHTStable DHT;
WiFiClient espClient;
PubSubClient client(espClient);
BlynkTimer timer;

int switch1State = HIGH;  // 스위치 버튼 1의 상태 변수
int switch2State = HIGH;  // 스위치 버튼 2의 상태 변수
int lastSwitch1State = HIGH;  // 이전 스위치 버튼 1의 상태 변수
int lastSwitch2State = HIGH;  // 이전 스위치 버튼 2의 상태 변수
unsigned long lastSwitch1Time = 0;  // 이전 스위치 버튼 1의 시간
unsigned long lastSwitch2Time = 0;  // 이전 스위치 버튼 2의 시간
unsigned long debounceDelay = 50;  // 디바운스 딜레이 설정

void setup() {
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  digitalWrite(LED1_PIN, LOW);  // 초기에 LED1은 꺼짐 상태로 설정
  digitalWrite(LED2_PIN, LOW);  // 초기에 LED2는 꺼짐 상태로 설정

  // WiFi 연결
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // MQTT 연결
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" Retry in 5 seconds...");
      delay(5000);
    }
  }

  timer.setInterval(1000L, readDHTSensor);  // 1초마다 DHT 센서 값을 읽기 위한 타이머 설정
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timer.run();
  checkSwitchStates();
}

void callback(char* topic, byte* payload, unsigned int length) {
  // MQTT 메시지를 수신할 때 호출되는 콜백 함수
  // 여기에 메시지 처리 로직을 추가하세요.
  // 예를 들어, 특정 토픽에 대한 동작을 수행하거나 상태를 변경할 수 있습니다.
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(client.state());
      Serial.println(" Retry in 5 seconds...");
      delay(5000);
    }
  }
}

void readDHTSensor() {
  float humidity = DHT.readHumidity(DHT_PIN);
  float temperature = DHT.readTemperature(DHT_PIN);

  if (!isnan(humidity) && !isnan(temperature)) {
    Blynk.virtualWrite(V4, temperature);   // 가상 핀 V4에 온도 값을 보냄
    Blynk.virtualWrite(V5, humidity);      // 가상 핀 V5에 습도 값을 보냄

    // MQTT 메시지 게시
    char temperatureStr[6];
    dtostrf(temperature, 5, 2, temperatureStr);
    client.publish("temperature", temperatureStr);
    
    char humidityStr[6];
    dtostrf(humidity, 5, 2, humidityStr);
    client.publish("humidity", humidityStr);
  }
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
      if (switch1State == LOW) {
        digitalWrite(LED1_PIN, HIGH);   // 스위치 버튼 1이 눌렸을 때 LED1 켜기
        Blynk.virtualWrite(V0, 1);       // 가상 핀 V0에 1(켜짐)을 보냄
        
        // MQTT 메시지 게시
        client.publish("switch1", "1");
      } else {
        digitalWrite(LED1_PIN, LOW);    // 스위치 버튼 1이 해제되었을 때 LED1 끄기
        Blynk.virtualWrite(V0, 0);       // 가상 핀 V0에 0(꺼짐)을 보냄
        
        // MQTT 메시지 게시
        client.publish("switch1", "0");
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
      if (switch2State == LOW) {
        digitalWrite(LED2_PIN, HIGH);   // 스위치 버튼 2가 눌렸을 때 LED2 켜기
        Blynk.virtualWrite(V1, 1);       // 가상 핀 V1에 1(켜짐)을 보냄
        
        // MQTT 메시지 게시
        client.publish("switch2", "1");
      } else {
        digitalWrite(LED2_PIN, LOW);    // 스위치 버튼 2가 해제되었을 때 LED2 끄기
        Blynk.virtualWrite(V1, 0);       // 가상 핀 V1에 0(꺼짐)을 보냄
        
        // MQTT 메시지 게시
        client.publish("switch2", "0");
      }
    }
  }
  lastSwitch2State = reading2;  // 현재 버튼 상태를 이전 버튼 상태로 업데이트
}

