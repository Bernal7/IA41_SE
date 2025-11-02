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
