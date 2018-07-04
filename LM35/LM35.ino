const int LM35 = A0;
float temperatura;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  temperatura = (float(analogRead(LM35)) * 5 / (1023)) / 0.01;
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  delay(500);
}
