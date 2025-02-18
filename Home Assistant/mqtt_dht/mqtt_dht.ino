#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Configuramos el Wifi
const char* ssid = "sagemcomC950";
const char* password = "JWMKN2MHDJZYMZ";

// Configurar mosquitto
const char* mqtt_server = "192.168.0.21"; // IP ddel home assistant
const int mqtt_port = 1883;
const char* mqtt_user = "mqtt-user"; //Si no usas autenticacion, dejar vacio
const char* mqtt_password = "mqtt-user";

WiFiClient espClient;
PubSubClient client(espClient);

// Configuración de los sensores
// Definir el DHT1
#define DHTPIN_1 4
#define DHTTYPE DHT22
DHT dht_1(DHTPIN_1, DHTTYPE);

// Definir el DHT2
#define DHTPIN_2 25
DHT dht_2(DHTPIN_2, DHTTYPE);



void setup(){
  Serial.begin(115200);

  // Conectarse a la red WiFi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado a la red WiFi!");

  // Conectarse al servidor mqtt
  client.setServer(mqtt_server, mqtt_port);
  reconnectMQTT();

  // Iniciar los sensores DHT
  dht_1.begin();
  dht_2.begin();
}



void loop(){
  if (!client.connected()){
    reconnectMQTT();
  }
  client.loop();

  // Medir las cosas que se quiere
  float temperatura_1 = dht_1.readTemperature();
  float humedad_1 = dht_1.readHumidity();

  float temperatura_2 = dht_2.readTemperature();
  float humedad_2 = dht_2.readHumidity();

  if (!isnan(temperatura_1) && !isnan(humedad_1)){
    String temp1Str = String(temperatura_1);
    String hum1Str = String(humedad_1);

    client.publish("home/esp32_v1/dht22_v1/temperature_1", temp1Str.c_str());
    client.publish("home/esp32_v1/dht22_v1/humidity_1", hum1Str.c_str());

    Serial.print("Temperatura_1: "); Serial.print(temperatura_1);
    Serial.print(" ºC, Humedad_1: "); Serial.print(humedad_1);
    Serial.println(" %");
  } else {
    Serial.println("Error al leer el sensor DHT22_v1");
  }

  // Mandar la info del segundo sensor DHT22_v2
  if (!isnan(temperatura_2) && !isnan(humedad_2)){
    String temp2Str = String(temperatura_2);
    String hum2Str = String(humedad_2);

    client.publish("home/esp32_v1/dht22_v2/humidity_2", hum2Str.c_str());
    client.publish("home/esp32_v1/dht22_v2/temperatura_2", temp2Str.c_str());

    Serial.print("Temperatura_2: "); Serial.print(temperatura_2);
    Serial.print(" ºC, Humedad_2: "); Serial.print(humedad_2);
    Serial.println(" %");
  } else {
    Serial.println("Error al leer el sensor DHT22_v2");
  }



  delay(1000);
}


void reconnectMQTT(){
  while(!client.connected()){
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)){
      Serial.println("Conectado!!!");
    } else{
      Serial.print("Error, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");
      delay(5000);
    }
  }
}

