#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

void setupWifi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Connecting to WiFi ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void get(const char* url) {
  Serial.printf("Sending GET request to %s\n", url);

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
  http.end();
}

void put(const char* url, const char* payload) {
  Serial.printf("Sending PUT request to %s\n", url);

  HTTPClient http;

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  int res = http.PUT(payload);

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  http.end();
}

void del(const char* url) {
  Serial.printf("Sending DELETE request to %s\n", url);

  HTTPClient http;

  http.begin(url);

  int res = http.sendRequest("DELETE");

  if (res > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(res);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("Error code: ");
    Serial.println(res);
  }
  http.end();
}

void validateUser(const char* url, const char* userid, const char* password) {
  String payload = "{\"userid\":\"";
  payload += userid;
  payload += "\",\"password\":\"";
  payload += password;
  payload += "\"}";

  post(url, payload.c_str());
}

void getProduct(const char* url, const char* ean13) {
  String endpoint = String(url) + "/" + ean13;
  get(endpoint.c_str());
}

void updatePrice(const char* url, const char* ean13, float price) {
  String endpoint = String(url) + "/price/" + ean13;
  String payload = "{\"price\":";
  payload += String(price);
  payload += "}";
  put(endpoint.c_str(), payload.c_str());
}

void updateAmount(const char* url, const char* ean13, int amount) {
  String endpoint = String(url) + "/amount/" + ean13;
  String payload = "{\"amount\":";
  payload += String(amount);
  payload += "}";
  put(endpoint.c_str(), payload.c_str());
}

void deleteProduct(const char* url, const char* ean13) {
  String endpoint = String(url) + "/" + ean13;
  del(endpoint.c_str());
}

void setup() {
  Serial.begin(115200);
  setupWifi();

  // Test functions
  const char* apiBaseUrl = "http://your-api-base-url"; // Replace with your API base URL

  // Validate user
  validateUser((String(apiBaseUrl) + "/users/validate").c_str(), "testuser", "testpassword");

  // Get product
  getProduct((String(apiBaseUrl) + "/products").c_str(), "1234567890123");

  // Update price
  updatePrice((String(apiBaseUrl) + "/products").c_str(), "1234567890123", 19.99);

  // Update amount
  updateAmount((String(apiBaseUrl) + "/products").c_str(), "1234567890123", 50);

  // Delete product
  deleteProduct((String(apiBaseUrl) + "/products").c_str(), "1234567890123");
}

void loop() {
  // Add your repeated code here
}
