#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_MPU6050.h>
#include <ESP32Servo.h>
#include <Adafruit_BMP280.h>

// Definición de pines
#define DHTPIN1 4
#define DHTPIN2 25
#define DHTTYPE DHT22
#define SERVO_PIN 32

// Definir pines I2C separados
#define SDA_HW611 21
#define SCL_HW611 22
#define SDA_MPU6050 18
#define SCL_MPU6050 19

// Direcciones I2C de los sensores
#define HW611_ADDR 0x76    // Dirección del HW-611 (presión barométrica)
#define MPU6050_ADDR 0x68  // Dirección del MPU6050

// Instancias de sensores
DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
Adafruit_MPU6050 mpu;
Adafruit_BMP280 bmp;
Servo servo;

void setup() {
    Serial.begin(115200);

    // Inicializar sensores DHT22
    dht1.begin();
    dht2.begin();

    // Configurar líneas I2C independientes
    Wire.begin(SDA_HW611, SCL_HW611);  // HW-611 en Wire (I2C principal)
    Wire1.begin(SDA_MPU6050, SCL_MPU6050);  // MPU6050 en Wire1 (I2C secundario)

    // Inicializar BMP280 en el bus corespondiente
    if (!bmp.begin(0x76)) {
      Serial.println("No se encontró el sensor BMP280 en 0x76.");
      while (1);
    }
    Serial.println("Sensor BMP280 detectado.");

    // Inicializar MPU6050 en el bus Wire1
    if (!mpu.begin(MPU6050_ADDR, &Wire1)) {
        Serial.println("Error al encontrar el MPU6050");
        while (1) delay(10);
    }
    Serial.println("MPU6050 inicializado correctamente");

    // Inicializar el servo con ESP32Servo
    servo.setPeriodHertz(50);  // Servos SG90 funcionan a 50Hz
    servo.attach(SERVO_PIN, 500, 2500);  // Min 500us, Max 2500us
    servo.write(0);  // Mantener en posición inicial
}

// Función para leer presión barométrica desde HW-611
float leerPresionHW611() {
    return bmp.readPressure() / 100.0F; // Valor inválido
}

void loop() {
    // Leer temperatura y humedad de los DHT22
    float humedad1 = dht1.readHumidity();
    float temperatura1 = dht1.readTemperature();
    float humedad2 = dht2.readHumidity();
    float temperatura2 = dht2.readTemperature();

    if (isnan(humedad1) || isnan(temperatura1) || isnan(humedad2) || isnan(temperatura2)) {
        Serial.println("Error al leer los sensores DHT22");
        return;
    }

    // Leer presión barométrica del HW-611 en Wire (bus principal)
    float presion = leerPresionHW611();
    if (presion == -1) {
        Serial.println("Error al leer el sensor HW-611");
    }

    // Leer datos del MPU6050 en Wire1 (bus secundario)
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Imprimir valores en el monitor serie
    Serial.print("DHT22 (1) - Temp: ");
    Serial.print(temperatura1);
    Serial.print("°C, Hum: ");
    Serial.print(humedad1);
    Serial.print("% | DHT22 (2) - Temp: ");
    Serial.print(temperatura2);
    Serial.print("°C, Hum: ");
    Serial.print(humedad2);
    Serial.println("%");

    Serial.print("HW-611 - Presión Atmosférica: ");
    Serial.print(presion);
    Serial.println(" hPa");

    Serial.print("MPU6050 - Aceleración (X,Y,Z): ");
    Serial.print(a.acceleration.x);
    Serial.print(", ");
    Serial.print(a.acceleration.y);
    Serial.print(", ");
    Serial.println(a.acceleration.z);

    Serial.print("MPU6050 - Giroscopio (X,Y,Z): ");
    Serial.print(g.gyro.x);
    Serial.print(", ");
    Serial.print(g.gyro.y);
    Serial.print(", ");
    Serial.println(g.gyro.z);

    delay(200);
}
