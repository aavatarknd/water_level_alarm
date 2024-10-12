#include <dummy.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#define rangPin 14  // GPIO5 Trig pin connected to GPIO5 (14)
#define outputPin A0  // Output pin for analog reading
#define MAX_DISTANCE 200  // Maximum distance to measure (in cm)
#define TANK_HEIGHT  40  // Tank height in cm

const int threshold = 15; //need to check and calibraton the number 

const int numReadings = 10;
int readings[numReadings];      
int readIndex = 0;              
int total = 0;                  
int average = 0;  


//const char* ssid = "PQTWQ";
//const char* password = "konrad123";

const char* ssid = "ASUS_TX";
const char* password = "Konrad123";

const char* apiToken = "a6kj4ghujhh7w95txqsf8q9n13hw8t";
const char* userToken = "utxtwf8ox3tq3oa1bzao4sq8ev594o"; 

const char* pushoverApiEndpoint = "https://api.pushover.net/1/messages.json";

//Pushover root certificate (valid from 11/10/2006 to 15/01/2038)
const char *PUSHOVER_ROOT_CA = "-----BEGIN CERTIFICATE-----\n"
                  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
                  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
                  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
                  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
                  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
                  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
                  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
                  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
                  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
                  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
                  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
                  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
                  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
                  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
                  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
                  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
                  "MrY=\n"
                  "-----END CERTIFICATE-----\n";

// Create a list of certificates with the server certificate
X509List cert(PUSHOVER_ROOT_CA);


void setup() {

  Serial.begin(115200); // Initialize the serial port communicattion 

  pinMode(rangPin, OUTPUT);  // Set the Rang pin as output

  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0; // Initialize all readings to 0
  }

  WiFi.begin(ssid, password); // start to connect the Wi-Fi network

  // waiting for Wi-Fi connection 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("trying to set up the connection"); // display the point to show the try to build the connection 
  }
  
  // check the Wi-Fi connecton status 
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected"); // if Wi-Fi connection success, print the message 
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // print the local device IP address 
  } 
  else 
  {
    Serial.println("\nFailed to connect to WiFi"); // if Wi-Fi connection failed, print the error message 
  }
  
  // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

}


void loop() {

// caculate the water level 

  digitalWrite(rangPin, HIGH);  // Send a pulse to start ranging
  delayMicroseconds(10);
  digitalWrite(rangPin, LOW);

  

  //int analogValue = analogRead(outputPin);  // Read the analog value from the Output pin

  total = total - readings[readIndex];
  readings[readIndex] = analogRead(outputPin); // Read from the sensor
  total = total + readings[readIndex];
  readIndex = readIndex + 1;

  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  average = total / numReadings; // Calculate the average

  int analogValue=average;
  
  Serial.print("Average: ");
  Serial.print(average);


  float voltage = analogValue * (3.3 / 1024.0);  // Convert the analog value to a voltage (assuming a 3.3V reference)
  float distanceToWater = voltage * 100/3;  // Convert the voltage to distance in centimeters

// Protection: Cap the measured distance to MAX_DISTANCE
  if (distanceToWater > MAX_DISTANCE) {
    distanceToWater = MAX_DISTANCE;
  }

  // Calculate the water level
  float waterLevel = TANK_HEIGHT - distanceToWater;


  // Protection: Ensure the water level is non-negative
  if (waterLevel < 0) {
    waterLevel = 0;
  }


  Serial.print("DistanceToWater: ");
 Serial.print(distanceToWater);
 Serial.println(" cm");

  Serial.print("Water Level: ");
  Serial.print(waterLevel);
  Serial.println(" cm");


  if (waterLevel > threshold) {

     sendPushoverNotification();

      delay(10000);

     return;

  }

  delay(1000);

}


void sendPushoverNotification() {
  
  //Make HTTPS POST request to send notification
/*
  // check the Wi-Fi connecton status 
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected"); // if Wi-Fi connection success, print the message 
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // print the local device IP address 
  } else {
    Serial.println("\nFailed to connect to WiFi"); // if Wi-Fi connection failed, print the error message 
  }
*/
    // Create a JSON object with notification details
    // Check the API parameters: https://pushover.net/api
    StaticJsonDocument<512> notification;
    notification["token"] = apiToken;
    notification["user"] = userToken;
    notification["message"] = "Flood alert! Evacuate the area immediately!";
    notification["title"] = "Water_Level_JingMu_Station Notification";
    notification["url"] = "";
    notification["url_title"] = "";
    notification["html"] = "";
    notification["priority"] = "";
    notification["sound"] = "cosmic";
    notification["timestamp"] = "";

    // Serialize the JSON object to a string
    String jsonStringNotification;
    serializeJson(notification, jsonStringNotification);

    // Create a WiFiClientSecure object
    WiFiClientSecure client;
    // Set the certificate
    client.setTrustAnchors(&cert);

    // Create an HTTPClient object
    HTTPClient http;

    // Specify the target URL
    http.begin(client, pushoverApiEndpoint);

    // Add headers
    http.addHeader("Content-Type", "application/json");

    // Send the POST request with the JSON data
    int httpResponseCode = http.POST(jsonStringNotification);

    // Check the response
    if (httpResponseCode > 0) {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
      String response = http.getString();
      Serial.println("Response:");
      Serial.println(response);
    } else {
      Serial.printf("HTTP response code: %d\n", httpResponseCode);
    }

    // Close the connection
    http.end();

}

