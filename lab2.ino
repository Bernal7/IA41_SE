/ Ejemplo: AnalogReadSerial adaptado para sensor LM35
// Lee A0, muestra ADC, voltaje y temperatura en grados Celsius.

const int pinLM35 = A0; // Pin analógico donde está conectado el sensor LM35

const int ledVerde = 13;
const int ledAmarillo = 12;
const int ledRojo = 11;

float temperatura;

void setup() {
