#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h> 

// ===================================
// ===== Configuración de Pines
// ===================================

// Pines de tu proyecto
#define SERVO1_PIN 4
#define BOTON1_PIN 14
#define LED_VERDE 2
#define LED_ROJO 19

// ===== Configuración LCD I2C =====
// ¡REVISA ESTA DIRECCIÓN! Prueba 0x27, si no funciona, cambia a 0x3F.
#define LCD_ADDR 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

// ===================================
// ===== Variables
// ===================================

// Objeto para el LCD I2C
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLUMNS, LCD_ROWS);

// Servo
Servo servo1;
int posCerrada = 10;
int posAbierta = 120;
bool servo1Abierto = false;
bool moviendoServo1 = false;
int posActual1 = 0;
int objetivo1 = 0;

// Tiempos para control No Bloqueante
unsigned long ultimoPasoServo1 = 0;
unsigned long ultimoBlinkLed = 0;
bool estadoLed = false;
const int retardoFijo = 15; // Velocidad del servo restaurada a 15ms

// ===================================
// ===== Funciones de Control
// ===================================

// ===== Iniciar movimiento del servo 1 =====
void iniciarMovimientoServo1(int objetivo) {
  objetivo1 = objetivo;
  moviendoServo1 = true;
  ultimoPasoServo1 = millis();

  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_ROJO, LOW);

  // Mensaje en LCD al INICIAR movimiento
  lcd.clear();
  lcd.setCursor(0, 0);
  if (objetivo == posAbierta) {
    lcd.print("Abriendo Puerta...");
  } else {
    lcd.print("Cerrando Puerta...");
  }
  lcd.setCursor(0, 1);
  lcd.print("Moviendo...");
}

// ===== Movimiento sin bloqueo Servo 1 (Usa retardo fijo de 15ms) =====
void moverServo1NoBloqueante() {
  if (!moviendoServo1) return;
  unsigned long ahora = millis();

  // Usa el retardo fijo (15ms)
  if (ahora - ultimoPasoServo1 >= retardoFijo) { 
    ultimoPasoServo1 = ahora;

    // Parpadeo LED
    if (ahora - ultimoBlinkLed >= 100) {
      ultimoBlinkLed = ahora;
      estadoLed = !estadoLed;

      if (objetivo1 > posActual1)
        digitalWrite(LED_VERDE, estadoLed); 
      else
        digitalWrite(LED_ROJO, estadoLed);  
    }

    // Mover servo 1 grado por paso
    if (posActual1 < objetivo1) posActual1++;
    if (posActual1 > objetivo1) posActual1--;

    servo1.write(posActual1);

    // Terminar movimiento
    if (posActual1 == objetivo1) {
      moviendoServo1 = false;
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_ROJO, LOW);

      // Mensaje en LCD al FINALIZAR movimiento
      lcd.clear();
      lcd.setCursor(0, 0);
      
      if (posActual1 == posAbierta) {
        // Puerta ABIERTA
        lcd.print("Acceso Correcto!");
        lcd.setCursor(0, 1);
        lcd.print("Bienvenido.");
      } else {
        // Puerta CERRADA
        lcd.print("Puerta Cerrada");
        lcd.setCursor(0, 1);
        lcd.print("Hasta luego!"); 
      }
    }
  }
}

// ===================================
// ===== SETUP y LOOP
// ===================================

void setup() {
  Serial.begin(115200);

  // Botón
  pinMode(BOTON1_PIN, INPUT_PULLUP);

  // LEDs
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_ROJO, OUTPUT);

  // Servo
  servo1.attach(SERVO1_PIN, 500, 2400);
  servo1.write(posCerrada);
  posActual1 = posCerrada;
  
  // ===== Inicializar LCD I2C =====
  lcd.init();
  lcd.backlight(); 

  // Mensaje de inicio
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistema de Acceso");
  lcd.setCursor(0, 1);
  lcd.print("Listo!");
  
  Serial.println("Sistema listo");
}

void loop() {
  static int ultimoBot1 = HIGH;
  int lectura1 = digitalRead(BOTON1_PIN);

  // Lógica del botón 
  if (lectura1 == LOW && ultimoBot1 == HIGH && !moviendoServo1) {
    servo1Abierto = !servo1Abierto;

    if (servo1Abierto)
      iniciarMovimientoServo1(posAbierta);
    else
      iniciarMovimientoServo1(posCerrada);
  }

  ultimoBot1 = lectura1;

  // Ejecutar movimiento no bloqueante
  moverServo1NoBloqueante();
}
