#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Escaneando dispositivos I2C ....");
}

void loop() {
  // put your main code here, to run repeatedly:
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0){
      Serial.print("Dispositivo I2C encontrado en la direccion 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
  }
}
