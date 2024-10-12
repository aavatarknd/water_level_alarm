/*#include <dummy.h>*/

#include <dummy.h>

/*#include <ESP8266.h>*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

/*const char* ssid = "PQTWQ";

const char* password = "konrad123";

const String pushoverUserKey = "utxtwf8ox3tq3oa1bzao4sq8ev594o";

const String pushoverApiToken = "a6kj4ghujhh7w95txqsf8q9n13hw8t";
*/

const char* ssid = "PQTWQ";
const char* password = "konrad123";

const char* apiToken = "a6kj4ghujhh7w95txqsf8q9n13hw8t";
const char* userToken = "utxtwf8ox3tq3oa1bzao4sq8ev594o"; //???

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



const int waterLevelPin = A0;

const int threshold = 380;

/*
void setup() {

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  } 

  Serial.println("Connected to WiFi");

}

*/

void setup() {
  Serial.begin(115200); // 初始化串行通信
  WiFi.begin(ssid, password); // 开始连接到Wi-Fi网络

  // 等待Wi-Fi连接
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("trying to set up the connection"); // 显示点以表示正在尝试连接
  }

  // 检查Wi-Fi连接状态
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected"); // 如果连接成功，打印消息
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // 打印设备的IP地址
  } else {
    Serial.println("\nFailed to connect to WiFi"); // 如果连接失败，打印错误消息
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

// 检查Wi-Fi连接状态
  

  int waterLevel = analogRead(waterLevelPin);

  Serial.println(waterLevel);


  if (waterLevel > threshold) {

     sendPushoverNotification();

      delay(100000);

     return;

  }


  delay(10000);

}

/*

void sendPushoverNotification() {


  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected"); // 如果连接成功，打印消息
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // 打印设备的IP地址
    WiFiClient client; // Create a WiFiClient object

  } 
  else 
  {
    Serial.println("\nFailed to connect to WiFi"); // 如果连接失败，打印错误消息
  }

    HTTPClient http;

    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    

    String payload = "token=" + pushoverApiToken + "&user=" + pushoverUserKey + "&message=The water level has exceeded the threshold!";

    

    int httpCode = http.POST(payload);

    Serial.println(httpCode);  //测试一下httpcode 

    if (httpCode > 0) {

      Serial.println("Pushover notification sent");

    } else {

      Serial.println("Error sending Pushover notification");

    }

    

    http.end();

  }

*/

void sendPushoverNotification() {
  //Make HTTPS POST request to send notification
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected"); // 如果连接成功，打印消息
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // 打印设备的IP地址
    WiFiClient client; // Create a WiFiClient object

  } 
  else 
  {
    Serial.println("\nFailed to connect to WiFi"); // 如果连接失败，打印错误消息
  }

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

