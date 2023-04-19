#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "your-wifi-ssid";
const char* password = "your-wifi-password";
const char* api_key = "your-openai-key";
const char* host = "api.openai.com";
const int port = 443;

BearSSL::WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.println("Connecting...");
  }
  Serial.println("Connected!");
  client.setInsecure();
  if (!client.connect(host, port)) {
    Serial.println("Connect error!");
    return;
  }
  Serial.println("Connected!");
}

void loop() {
  String answer;
  Serial.print("Question: ");
  while (!Serial.available()) {
    delay(100);
  }
  String input = Serial.readStringUntil('\n');
  input.trim();
  Serial.println(input);
  String request = "POST /v1/engines/text-davinci-003/completions HTTP/1.1\r\n";
  request += "Host: api.openai.com\r\n";
  request += "Content-Type: application/json\r\n";
  request += "Authorization: Bearer " + String(api_key) + "\r\n";
  String json_data = "{\"prompt\":\"" + input + "\",\"temperature\":0.5,\"max_tokens\":200,\"top_p\":1,\"frequency_penalty\":0,\"presence_penalty\":0}";
  request += "Content-Length: " + String(json_data.length()) + "\r\n\r\n";
  request += json_data + "\r\n\r\n";
  if (!client.connected()) {
    if (!client.connect(host, port)) {
      Serial.println("Connect error!");
      return;
    }
  }
  
  client.print(request);
  while (!client.available()) {
    delay(100);
  }
  String response = client.readString();
  int start_index = response.indexOf("\"text\":") + 8;
  int end_index = response.indexOf("\",\"index\"");
  answer = response.substring(start_index, end_index);
  answer.replace("\\n", " ");
  Serial.println("Answer: " + answer);
}

