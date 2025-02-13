#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>

// Sensor DHT
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensor HW-611
#define I2C_SDA 21
#define I2C_SCL 22
Adafruit_BMP280 bmp;

// Sensor MPU6050
#define I2C_SDA_2 25
#define I2C_SCL_2 26
TwoWire I2C_MPU = TwoWire(1); // Crear la nueva linea I2C
Adafruit_MPU6050 mpu;

const char* ssid = "ADAMO-3210";
const char* password = "UORZ7Y7KKBA6HO";

// Dirección de InfluxDB en la Raspberry Pi
const char* influxDB_url = "http://192.168.1.105:8086/api/v2/write?org=Casimiro&bucket=sensores&precision=s";

// API para InfluxDB
String influxDB_token = "iMAtY4WqYqfs268ho3MUce-bNn153-bgy7IW7vGarPOvkcg9R6Q7Oo2wx7ZgOE42U3AgsU6ZeIEaWCwzuf5h5g==";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status()!=WL_CONNECTED){
    delay(1000);
    Serial.println("Conectando...");
  }
  Serial.println("Conectado a WiFI");
  dht.begin();

  // Iniciar las las comunicaciones I2C
  // Para el WH-61
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!bmp.begin(0x76)){
    Serial.println("No se encontró el BMP280");
    while(1);
  }

  // Para el MPU6050
  I2C_MPU.begin(I2C_SDA_2, I2C_SCL_2, 100000); 
  if (!mpu.begin(0x68, &I2C_MPU)){
    Serial.println("No se encontró el MPU6050");
    while(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED){
    // Leer el DHT11
    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    // Leer el WH-611
    float sensor_hw611 = bmp.readPressure() / 100.0F;

    // Lectura del MPU6050
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    if (!isnan(temperatura) && !isnan(humedad)) {
      // Iniciar el clitente HTTP
      HTTPClient http;
      http.begin(influxDB_url);
      http.addHeader("Authorization", "Token " + influxDB_token);
      http.addHeader("Content-Type", "text/plain");

      // Formato Line Protocol de InfluxDB
      String data = "dht11,location=livingroom temperatura=" + String(temperatura) + 
                    ",humedad=" + String(humedad) +
                    "\nhw611,location=room valor=" + String(sensor_hw611) +
                    "\nmpu6050,location=outside acc_x=" + String(a.acceleration.x) +
                    ",acc_y=" + String(a.acceleration.y) + 
                    ",acc_z=" + String(a.acceleration.z) +
                    ",gyro_x=" + String(g.gyro.x) + 
                    ",gyro_y=" + String(g.gyro.y) +
                    ",gyro_z=" + String(g.gyro.z);

      int httpResponseCode = http.POST(data);

      Serial.println("Enviando datos: " + data);
      Serial.println("Codigo de respuesta: " + String(httpResponseCode));

      http.end();
    }
  }

  delay(3000);
}
