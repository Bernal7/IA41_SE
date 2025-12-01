#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#define DHTPIN 2          // Pin donde conectaste el DHT
#define DHTTYPE DHT11     // Cambia a DHT22 si usas ese sensor

DHT dht(DHTPIN, DHTTYPE);
