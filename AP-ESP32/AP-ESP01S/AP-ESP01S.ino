#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///  Cambiar la direccion IP por la correspondiente a cada uno de los ESP-01S dependiendo de que ponga en el
//    IPAddress(192,168,4,101)
//    IPAddress(192,168,4,102)
//    IPAddress(192,168,4,103)
//    IPAddress(192,168,4,104)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const char* ssid = "RaspberryAP";
const char* password = "12345678";

ESP8266WebServer server(80);

// Manejar la solicitud de datos
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
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n🚀 Iniciando ESP32...");

  dht.begin();
  Serial.println("✅ Sensor DHT inicializado.");

  WiFi.config(IPAddress(192,168,4,102), IPAddress(192,168,4,1), IPAddress(255,255,255,0));

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

  WiFi.setSleep(false); // Desactiva modo ahorro de energía
  server.on("/data", handleData);
  server.begin();
  Serial.println("✅ Servidor HTTP iniciado.");
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
