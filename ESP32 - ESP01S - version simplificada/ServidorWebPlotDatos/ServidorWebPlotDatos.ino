#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

#define DHTPIN 4  // GPIO4 del ESP32
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "ADAMO-3210";
const char* password = "UORZ7Y7KKBA6HO";

// Crear el servidor web
WebServer server(80);

// Definir las variable de temperaturas y humedad
float temperatura_esp01s = 0;
float humedad_esp01s = 0;
float temperatura_dht11 = 0;
float humedad_dht11 = 0;

// Maneja los datos enviados por el ESP-01S
void handleData() {
    if (server.hasArg("temp") && server.hasArg("hum")) {
        temperatura_esp01s = server.arg("temp").toFloat();
        humedad_esp01s = server.arg("hum").toFloat();

        Serial.printf("ESP-01S -> Temp: %.2f°C, Hum: %.2f%%\n", temperatura_esp01s, humedad_esp01s);
        server.send(200, "text/plain", "Datos recibidos");
    } else {
        server.send(400, "text/plain", "Faltan parámetros");
    }
}

void setup() {
    Serial.begin(115200);
    
    WiFi.begin(ssid, password);
    Serial.print("Conectando a WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" ¡Conectado!");

    dht.begin();

    server.on("/data", handleData);
    server.begin();
}

void loop() {
    server.handleClient();

    // Leer temperatura y humedad desde el DHT11 en el ESP32
    temperatura_dht11 = dht.readTemperature();
    humedad_dht11 = dht.readHumidity();

    // Si las lecturas son válidas, imprimir y comparar
    if (!isnan(temperatura_dht11) && !isnan(humedad_dht11)) {
        Serial.printf("ESP32 (DHT11) -> Temp: %.2f°C, Hum: %.2f%%\n", temperatura_dht11, humedad_dht11);

        // Formato para Serial Plotter
        Serial.print("ESP01S_Temp: ");
        Serial.print(temperatura_esp01s);
        Serial.print("\tESP01S_Hum: ");
        Serial.print(humedad_esp01s);
        Serial.print("\tESP32_Temp: ");
        Serial.print(temperatura_dht11);
        Serial.print("\tESP32_Hum: ");
        Serial.println(humedad_dht11);
    }

    delay(2000); // Leer cada 2 segundos
}
