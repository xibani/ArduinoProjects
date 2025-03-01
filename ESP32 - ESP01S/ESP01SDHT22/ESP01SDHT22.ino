#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "ADAMO-3210";
const char* password = "UORZ7Y7KKBA6HO";
const char* serverIP = "192.168.1.204";

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado!!!");
}

void loop() {
  // put your main code here, to run repeatedly:
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)){
    StaticJsonDocument<200> doc;
    doc["temperatura"] = temp;
    doc["humedad"] = hum;

    String json;
    serializeJson(doc, json);

    if (client.connect(serverIP, 80)){
      client.println("POST /data HTTP/1.1");
      client.println("Host: " + String(serverIP));
      client.println("Content-Type: application/json");
      client.print("Content-Length: ");
      client.println(json.length());
      client.println();
      client.println(json);
      Serial.println("Enviado: " + json);
    }
    client.stop();
  }
  delay(1000); // Enviar cada 1 segundo
}
