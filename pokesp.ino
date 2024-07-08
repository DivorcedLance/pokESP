#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
// Defining the WiFi channel speeds up the connection:
#define WIFI_CHANNEL 6

void setupWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void get(const char* url) {
  Serial.printf("Sending request to %s\n", url);

  HTTPClient http;

  http.begin(url);

  int res = http.GET();

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  // Free resources
  http.end();
}

void post(const char* url, const char* payload) {
  Serial.printf("Sending POST request to %s\n", url);

  HTTPClient http;

  http.begin(url);

  http.addHeader("Content-Type", "application/json");

  int res = http.POST(payload);

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  // Free resources
  http.end();
}

void setup() {
  Serial.begin(115200);
  setupWifi();
}

void loop() {
  const char* url = "https://pokeapi.co/api/v2/pokemon/ditto";
  get(url);
  delay(5000);
}