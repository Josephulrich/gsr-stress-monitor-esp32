/*
  GSR – ESP32 (lecture + lissage + normalisation + état)
  - Entrée analogique: GPIO 34 (ADC1)
  - Affiche : brut | moyenne | normalisé [0..1] | état
*/

const int GSR_PIN = 34;

// --- Réglages ---
const int WINDOW_SIZE = 20;        // taille de la fenêtre pour la moyenne glissante
const float ALPHA = 0.1;           // filtrage IIR complémentaire (faible passe-bas)
const int RAW_MIN = 1500;          // bornes pour la normalisation (à ajuster à ton montage)
const int RAW_MAX = 3000;

// Seuils d'interprétation (sur la valeur normalisée 0..1)
const float TH_REPOS   = 0.30;
const float TH_EVEIL   = 0.65;

// --- Variables internes ---
int   buf[WINDOW_SIZE];
int   idx = 0;
bool  filled = false;
long  sum = 0;

float filt = 0.0;

void setup() {
  Serial.begin(115200);
  delay(800);
  Serial.println("GSR ESP32 - Lissage + Normalisation + Etat");

  // Préremplir le buffer avec la 1re mesure pour démarrer proprement
  int first = analogRead(GSR_PIN);
  for (int i = 0; i < WINDOW_SIZE; i++) {
    buf[i] = first;
    sum += first;
  }
  filled = true;
  filt = first;
}

void loop() {
  // 1) Lecture brute
  int raw = analogRead(GSR_PIN);

  // 2) Mise à jour de la moyenne glissante
  if (filled) {
    sum -= buf[idx];
  }
  buf[idx] = raw;
  sum += raw;
  idx = (idx + 1) % WINDOW_SIZE;
  if (idx == 0) filled = true;

  float mean = filled ? (float)sum / WINDOW_SIZE : (float)raw;

  // 3) Petit filtre IIR pour stabiliser
  filt = ALPHA * mean + (1.0 - ALPHA) * filt;

  // 4) Normalisation (borne à [0..1])
  //    map() renvoie un long; on fait l'équivalent en float pour plus de finesse
  float normalized = (float)(filt - RAW_MIN) / (float)(RAW_MAX - RAW_MIN);
  if (normalized < 0.0) normalized = 0.0;
  if (normalized > 1.0) normalized = 1.0;

  // 5) Interprétation
  String state;
  if (normalized < TH_REPOS) {
    state = "Repos";
  } else if (normalized < TH_EVEIL) {
    state = "Alerte légère / éveil";
  } else {
    state = "Stress / activation";
  }

  // 6) Affichage (format proche de ta capture)
  Serial.print("gsr brut : ");
  Serial.print(raw);
  Serial.print(" | Moyenne : ");
  Serial.print(mean, 1);
  Serial.print(" | Normalisé : ");
  Serial.print(normalized, 2);
  Serial.print(" | Etat : ");
  Serial.println(state);

  delay(100);
}
