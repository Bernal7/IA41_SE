/*
  * Using ESP32
  * LCD Module
  * OLED Module
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Ajusta direcciones y tamaño según tus módulos
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 16x2 en 0x27
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9); // SDA=8, SCL=9 (cambia si usas otros pines)
  delay(50);

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("LCD OK");

  // Inicializar OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // dirección 0x3C o 0x3D
    Serial.println("SSD1306 allocation failed");
    // En caso de fallo, muestra en LCD
    lcd.setCursor(0,1);
    lcd.print("OLED NO DETECT");
  } else {
    Serial.println("OLED detectado");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("OLED OK");
    display.display();
  }
}

void loop() {
  // Ejemplo: actualizar LCD y OLED
  static unsigned long t0 = 0;
  if (millis() - t0 > 1000) {
    t0 = millis();

    // Actualizar LCD
    lcd.setCursor(0,1);
    lcd.print("Millis:");
    lcd.print(millis()/1000);
    lcd.print("s   "); // borrar restos

    // Actualizar OLED
    display.clearDisplay();

    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.print("OLED OK");

    display.setCursor(0,20);
    display.setTextSize(2);
    display.print("t:");
    display.print(millis()/1000);
    display.print("s");
    display.display();
  }
}
