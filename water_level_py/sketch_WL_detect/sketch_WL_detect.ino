#include <ESP8266.h>

#define LED 9 
#define BUZZER 10 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode (BUZZER, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
   int temp=analogRead(A0);
  Serial.println(temp);

  if (temp >= 200)
  {
    digitalWrite (LED, HIGH);
    digitalWrite (BUZZER, HIGH);
    Serial.println("BEEP");
  }
  else 
  {
    digitalWrite (LED, LOW);
    digitalWrite (BUZZER, LOW);
  }
  delay(1000);
}
