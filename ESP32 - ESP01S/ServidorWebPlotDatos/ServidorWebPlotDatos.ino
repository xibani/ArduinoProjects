#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "ADAMO-3210";
const char* password = "UORZ7Y7KKBA6HO";

// Crear el servidor web
WebServer server(80);

float temperatura = 0;
float humedad = 0;

void handleRoot(){
  String html = "<html><head>";
  html += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  html += "</head><body><h1>Temperatura y Humedad</h1>";
  html += "<canvas id='chart' width='400' height='200'></canvas>";
  html += "<script>";
  html += "var ctx = document.getElementById('chart').getContext('2d');";
  html += "var chart = new Chart(ctx, {type: 'line', data: { labels: ['Último dato'], datasets: [";
  html += "{ label: 'Temperatura (°C)', data: [" + String(temperatura) + "], borderColor: 'red', fill: false },";
  html += "{ label: 'Humedad (%)', data: [" + String(humedad) + "], borderColor: 'blue', fill: false }]}, options: {responsive: true}});";
  html += "setTimeout(() => {location.reload();}, 5000);";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Recibe los datos del ESP-01S en JSON
void handleData(){
  if (server.hasArg("plain")){
    StaticJsonDocument<200> doc;
    deserializeJson(doc, server.arg("plain"));

    temperatura = doc["temperatura"];
    humedad = doc["humedad"];

    Serial.printf("Datos recibidos: Temp: %.2f°C, Hum: %.2f%%\n", temperatura, humedad);
    server.send(200, "text/plain", "Datos recibidos");
  } else {
    server.send(400, "text/plain", "Formato incorrecto");
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("Conectado al WiFi!!!");

  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", HTTP_POST, handleData);
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();
}
