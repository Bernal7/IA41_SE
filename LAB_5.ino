#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 2          // Pin donde conectaste el DHT
#define DHTTYPE DHT11     // Cambia a DHT22 si usas ese sensor

DHT dht(DHTPIN, DHTTYPE);

// Dirección típica del LCD: 0x27 o 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
