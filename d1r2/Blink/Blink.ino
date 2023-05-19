/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/
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
D5 0 SDA BUZZER
D6 2 IR with LED
D7 14
D8 12
D9 13 rRGB
D10 15 bRGB SS
D11 13 gRGB MOSI
D12 12 rLED MISO 
D13 14 bLED SCK
*/
// the setup function runs once when you press reset or power the board
#define LED 13//2
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(200);                      // wait for a second
  digitalWrite(LED, LOW);   // turn the LED off by making the voltage LOW
  delay(300);                      // wait for a second
}
