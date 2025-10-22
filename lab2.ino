/ Ejemplo: AnalogReadSerial adaptado para sensor LM35
// Lee A0, muestra ADC, voltaje y temperatura en grados Celsius.

const int pinLM35 = A0; // Pin analógico donde está conectado el sensor LM35

const int ledVerde = 13;
const int ledAmarillo = 12;
const int ledRojo = 11;

float temperatura;

void setup() {
	// Inicializa la comunicación serial a 9600 baudios
	Serial.begin(9600);
	while (!Serial) {
		; // espera a que el puerto serial se conecte (solo necesario en placas como Leonardo)
	}
	Serial.println("LM35 + LEDs - Indicador de Temperatura");

	pinMode(ledVerde, OUTPUT);
	pinMode(ledAmarillo, OUTPUT);
	pinMode(ledRojo, OUTPUT);
}

void loop() {
	// Lee el valor analógico del pin A0 (0..1023)
	int lecturaADC = analogRead(pinLM35);

	//calcular voltaje (sv de referecncia)
	const float voltaje = lecturaADC * (5 / 1023.0);

	//convertir a temperatura C (LM35 entrega 10 mV/C)
	temperatura = voltaje * 100.0; //voltaje * 100

	//Mostrar datos por monitor serial
	Serial.print("ADC: ");
