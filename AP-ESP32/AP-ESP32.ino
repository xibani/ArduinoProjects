#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "RaspberryAP";
const char* password = "12345678";

WebServer server(80);

void handleData() {
    Serial.println("📡 Solicitud recibida en /data");

    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    if (!isnan(temperatura) && !isnan(humedad)) {
        String response = "{\"temperatura\":" + String(temperatura) + ",\"humedad\":" + String(humedad) + "}";
        server.send(200, "application/json", response);
        Serial.println("✅ Datos enviados: " + response);
    } else {
        server.send(500, "text/plain", "Error leyendo DHT");
        Serial.println("❌ Error leyendo DHT");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n🚀 Iniciando ESP32...");

    dht.begin();
    Serial.println("✅ Sensor DHT inicializado.");

    WiFi.config(IPAddress(192,168,4,101), IPAddress(192,168,4,1), IPAddress(255,255,255,0));

    Serial.print("🌐 Conectando al AP de la Raspberry...");
    WiFi.begin(ssid, password);
    int intentos = 0;

    while (WiFi.status() != WL_CONNECTED && intentos < 20) {
        delay(500);
        Serial.print(".");
        intentos++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ ¡Conectado!");
        Serial.print("📡 Dirección IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n❌ ERROR: No se pudo conectar a la WiFi. Reiniciando...");
        ESP.restart();
    }

    WiFi.setSleep(false);  // Desactiva modo ahorro de energía
    server.on("/data", handleData);
    server.begin();
    Serial.println("✅ Servidor HTTP iniciado.");
}

void loop() {
    server.handleClient();
    //Serial.println("⌛ Esperando solicitudes...");
    //delay(100);
}

