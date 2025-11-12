/*
  * ESP32 + LED + 220 OHMS Resistor
*/

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";
const int ledPin = 2;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\nConectando al WiFi...");
  WiFi.begin(ssid, password);

  int intento = 0;
  while (WiFi.status() != WL_CONNECTED && intento < 20) {
    delay(1000);
    Serial.print(".");
    intento++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Conectado!");
    Serial.print("IP asignada: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ No se pudo conectar al WiFi");
  }

  pinMode(ledPin, OUTPUT);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "https://google.com/"; // Tu URL original

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      Serial.printf("Código HTTP: %d\n", httpCode);

      // Si el servidor devuelve redirección (301 o 302)
      if (httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_FOUND) {
        String newUrl = http.getLocation();
        Serial.print("Redirigiendo a: ");
        Serial.println(newUrl);

        http.end(); // Termina la conexión anterior
        http.begin(newUrl); // Nueva petición a la URL redirigida
        httpCode = http.GET(); // Ejecuta GET de nuevo
      }

      if (httpCode == HTTP_CODE_OK) {

        digitalWrite(ledPin, HIGH);
        Serial.println("✅ 200 OK → LED ENCENDIDO");
      } else {
        digitalWrite(ledPin, LOW);
        Serial.printf("⚠️ Código %d, LED apagado\n", httpCode);
      }
    } else {
      Serial.printf("Error en conexión: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    delay(1000);

    digitalWrite(ledPin, LOW);
  }
  delay(5000);
}
