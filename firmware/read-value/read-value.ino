/*
 * Lecture du capteur GSR (Galvanic Skin Response)
 * Microcontrôleur : ESP32 / Arduino
 * Capteur connecté sur une entrée analogique (ex: GPIO 34)
 */

const int GSR_PIN = 34;   // broche analogique
int gsrValue = 0;
float voltage = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Initialisation du capteur GSR...");
}

void loop() {
  // Lecture analogique brute
  gsrValue = analogRead(GSR_PIN);

  // Conversion en tension (0 - 3.3V sur ESP32)
  voltage = (gsrValue / 4095.0) * 3.3;

  // Affichage des valeurs
  Serial.print("Valeur brute GSR : ");
  Serial.print(gsrValue);
  Serial.print("\tTension : ");
  Serial.print(voltage, 3);
  Serial.println(" V");

  delay(500);
}
