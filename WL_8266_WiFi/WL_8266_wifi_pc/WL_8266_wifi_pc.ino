#include <ESP8266WiFi.h>

const char* ssid = "PQTWQ";
const char* password = "konrad123";
const char* host = "YOUR_PC_IP_ADDRESS"; // This is the IP address of your PC running a server

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Read the water level sensor value
  int sensorValue = analogRead(A0);

  // Print the sensor value to the serial monitor
  Serial.print("Water Level Sensor Value: ");
  Serial.println(sensorValue);

  // Send data to PC via HTTP GET request
  WiFiClient client;
  if (client.connect(host, 80)) {
    String url = String("/update?value=") + String(sensorValue);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(500);
  }

  delay(10000); // Delay before sending the next value (10 seconds)
}