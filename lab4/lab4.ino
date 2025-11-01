#include "DHT.h"

#define DHTPIN 2

// Pin donde está conectado el DHT11

#define DHTTYPE DHT11

//Especificamos el tipo de sensor

int btnPin = 3;

int ledRojoPin = 12;
int ledAmarilloPin = 11;
int ledVerdePin = 10;

int pushed = 0;

DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);

  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledRojoPin, OUTPUT);
  pinMode(ledAmarilloPin, OUTPUT);
  pinMode(ledVerdePin, OUTPUT);

  digitalWrite(ledRojoPin, LOW);
  digitalWrite(ledAmarilloPin, LOW);
  digitalWrite(ledVerdePin, LOW);

  dht.begin();

}

void loop(){
  Serial.println(pushed);
  if (pushed == 0) {
    if (!digitalRead(btnPin) == HIGH) {
      Serial.println(F("Iniciando sensor DHT11..."));
      pushed = 1;
    }
  }

  if (pushed == 1) {
    // Esperar 2 segundos entre lecturas

    float humedad = dht.readHumidity();

    float temperatura = dht.readTemperature(); // En °C por defecto

    if (isnan (humedad) || isnan (temperatura)) {

      Serial.println(F("Error al leer el sensor DHT11!"));

    } else {
      Serial.print(F("Humedad: "));

      Serial.print(humedad);

      Serial.print(F("% Temperatura: "));

      Serial.print(temperatura);

      Serial.println(F("°C"));

      if (temperatura >= 15 && temperatura < 26) {

        digitalWrite(ledVerdePin, HIGH);
        digitalWrite(ledAmarilloPin, LOW);
        digitalWrite(ledRojoPin, LOW);

      } else if (temperatura >= 26 && temperatura < 35) {

        digitalWrite(ledVerdePin, LOW);
        digitalWrite(ledAmarilloPin, HIGH);
        digitalWrite(ledRojoPin, LOW);

      } else if (temperatura >= 35) {

        digitalWrite(ledVerdePin, LOW);
        digitalWrite(ledAmarilloPin, LOW);
        digitalWrite(ledRojoPin, HIGH);

      } else {

        digitalWrite(ledVerdePin, LOW);
        digitalWrite(ledAmarilloPin, LOW);
        digitalWrite(ledRojoPin, LOW);
        
      }
    }
  }
  delay(1000);
}
