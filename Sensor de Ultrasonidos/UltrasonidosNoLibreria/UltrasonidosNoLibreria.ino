const int EchoPin = 5;
const int TriggerPin = 6;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  Serial.println("Programa iniciado correctamente.");
}

void loop() {
  // put your main code here, to run repeatedly:
  int cm = ping(TriggerPin, EchoPin);
  Serial.print("Distancia: ");
  Serial.println(cm);
  delay(100);
}

// Definir la funcion de medida
int ping(int TriggerPin, int EchoPin){
  long distanceCm, duration;

  // Para asegurar un disparo limpio, se va a poner el trigger a LOW durante 4 us
  digitalWrite(TriggerPin, LOW);
  delayMicroseconds(4);

  // Disparamos el trigger (HIGH) durante al menos 10us para asegurar el disparo y lo volvemos a poner en LOW.
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  // Determinamos el tiempo que tarda en cambiar de estado con la funcion pulseIn, que hace eso.
  duration = pulseIn(EchoPin, HIGH);

  // Determinar la distancia con la formula.
  distanceCm = duration * 10 / 292 / 2;

  return distanceCm;
}
