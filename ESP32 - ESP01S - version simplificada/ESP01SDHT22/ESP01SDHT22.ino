#include <ESP8266WiFi.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "ADAMO-3210";
const char* password = "UORZ7Y7KKBA6HO";
const char* serverIP = "192.168.1.204";

WiFiClient client;

void setup() {
    Serial.begin(115200);
    dht.begin();

    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" Conectado!");
}

void loop() {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
        // Construimos la URL con los datos
        String url = "/data?temp=" + String(temp) + "&hum=" + String(hum);

        if (client.connect(serverIP, 80)) {
            client.print("GET " + url + " HTTP/1.1\r\n");
            client.print("Host: " + String(serverIP) + "\r\n");
            client.print("Connection: close\r\n\r\n");
            Serial.println("Enviado: " + url);
        }
        client.stop();
    }
    delay(2000);  // Envía cada 2 segundos
}