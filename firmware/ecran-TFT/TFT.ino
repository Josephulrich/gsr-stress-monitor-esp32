#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Ecran SSD1306 I2C ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
#define OLED_ADDR    0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Capteur MLX90614 ---
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// --- Broches I2C ESP8266 ---
#define I2C_SDA 4  // D2
#define I2C_SCL 5  // D1

// --- Filtrage EMA pour stabiliser l'affichage ---
float emaObj = NAN, emaAmb = NAN;
const float ALPHA = 0.25f;  // 0<alpha<=1 (plus grand = plus réactif)

void drawCenteredBig(const String &txt, int y, int textSize) {
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(textSize);
  display.getTextBounds(txt, 0, y, &x1, &y1, &w, &h);
  int16_t x = (SCREEN_WIDTH - w) / 2;
  display.setCursor(x, y);
  display.print(txt);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Ecran
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("Erreur: SSD1306 non detecte (addr 0x3C?)."));
    while (1) delay(10);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Thermometre IR"));
  display.display();

  // Capteur MLX90614
  if (!mlx.begin()) {
    display.println(F("MLX90614 introuvable (0x5A)"));
    display.display();
    while (1) delay(10);
  }

  delay(300);
}

void loop() {
  // Lectures en degres Celsius
  float amb = mlx.readAmbientTempC();
  float obj = mlx.readObjectTempC();

  // EMA
  if (isnan(emaObj)) { emaObj = obj; }
  else { emaObj = ALPHA * obj + (1 - ALPHA) * emaObj; }

  if (isnan(emaAmb)) { emaAmb = amb; }
  else { emaAmb = ALPHA * amb + (1 - ALPHA) * emaAmb; }

  // Affichage
  display.clearDisplay();

  // Titre
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Thermometre IR (MLX90614)"));

  // Temp objet en grand, centree
  char buf[16];
  snprintf(buf, sizeof(buf), "%.1f C", emaObj);
  drawCenteredBig(String(buf), 20, 3);

  // Temp ambiante en bas
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.print(F("Tamb: "));
  display.print(emaAmb, 1);
  display.println(F(" C"));

  display.display();

  // Debug serie
  Serial.print(F("Obj=")); Serial.print(emaObj, 2);
  Serial.print(F("C  Amb=")); Serial.print(emaAmb, 2);
  Serial.println(F("C"));

  delay(150);
}
