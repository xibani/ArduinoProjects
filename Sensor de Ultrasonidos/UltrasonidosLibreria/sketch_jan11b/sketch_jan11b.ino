#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>

#define TRIGGER_PIN 6
#define ECHO_PIN 5
#define MAX_DISTANCE 200

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  lcd.init(); // Inicializar la pantalla LCD
  lcd.backlight(); //Encender la retroiluminacion
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(30);
  float echoTime = sonar.ping_median(5);
  float distanceCm = sonar.convert_cm(echoTime);
  Serial.print(distanceCm); // obtener el valor en cm (0 = fuera de rango)
  Serial.println("cm");

  // Plotear en la pantalla el tiempo
  lcd.setCursor(6, 0);
  lcd.print(echoTime);
  lcd.setCursor(13, 0);
  lcd.print(" us");
  // lcd distancia
  lcd.setCursor(6, 1);
  lcd.print(distanceCm);
  lcd.setCursor(11, 1);
  lcd.print(" cm");
}
