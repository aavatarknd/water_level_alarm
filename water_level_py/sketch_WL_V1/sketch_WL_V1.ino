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

  if (temp >= 250)
  {
    digitalWrite (LED, HIGH);
    digitalWrite (BUZZER, HIGH);
  }
  else 
  {
    digitalWrite (LED, LOW);
    digitalWrite (BUZZER, LOW);
  }
  delay(500);

}
