/*
 * ESP32 - Auth Service WebSocket Client
 * 
 * Este código conecta tu ESP32 al servidor de reconocimiento facial
 * y recibe notificaciones automáticas cuando se detecta un rostro.
 * 
 * INSTALACIÓN DE LIBRERÍAS (Arduino IDE):
 * 1. Sketch → Include Library → Manage Libraries
 * 2. Busca e instala:
 *    - WebSocketsClient by Markus Sattler
 *    - ArduinoJson by Benoit Blanchon (v6.x)
 *    - LiquidCrystal I2C by Frank de Brabander
 */

#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h> 

// ========================================
// CONFIGURACIÓN - MODIFICA ESTOS VALORES
// ========================================

// WiFi
const char* WIFI_SSID = "AP-UTN";
const char* WIFI_PASSWORD = "";

// Servidor WebSocket
// const char* WS_HOST = "192.168.100.14"; // IP de tu servidor (donde corre Node.js)
const char* WS_HOST = "10.1.38.120";
const uint16_t WS_PORT = 5000; // Puerto del servidor
const char* WS_PATH = "/ws/esp32";

// Credenciales del servicio (obtén estos datos de tu dashboard)
const char* SERVICE_UUID = "5220d5ad-1c2e-452f-b3e5-2870ee3707ed";
const char* ACCESS_KEY = "bac4069193fc66b07cfae90614c6439f";
const char* ACCESS_PASSWORD = "4cb4ac4a8fc583ed10fd8295d520c5b4";

// ========================================
// CONFIGURACIÓN DE PINES
// ========================================
#define LED_GREEN_BEHIND 2   // LED verde para acceso concedido
#define LED_RED_BEHIND 1     // LED rojo para acceso denegado
#define BUZZER_PIN 4         // Buzzer ACTIVO para alertas
#define LED_BLUE_BEHIND 42   // LED azul para indicar conexión/autenticación exitosa
#define LED_SERVO 5          // Servo motor pin
#define LED_RED_FRONTAL 40   // Led frontal rojo
#define LED_GREEN_FRONTAL 41 // Led frontal verde

Servo servo1;
const int servo_open = 120;
const int servo_closed = 0;
const int servo_steps = 1;
int servo_status = 0;

// ========================================
// VARIABLES GLOBALES
// ========================================
WebSocketsClient webSocket;
bool isAuthenticated = false;
unsigned long lastPingTime = 0;
const unsigned long PING_INTERVAL = 30000; // 30 segundos

// Buzzer activo: no requiere generación de tono, solo encendido/apagado



// ========================================
// FUNCIONES DE BUZZER ACTIVO
// ========================================

void buzzerActiveBeep(int onMs, int offMs) {
    digitalWrite(BUZZER_PIN, HIGH);
    unsigned long startOn = millis();
    while (millis() - startOn < (unsigned long)onMs) {
        webSocket.loop();
        delay(5);
    }
    digitalWrite(BUZZER_PIN, LOW);
    unsigned long startOff = millis();
    while (millis() - startOff < (unsigned long)offMs) {
        webSocket.loop();
        delay(5);
    }
}

void buzzerMelodySuccess() {
    // 2 beeps cortos
    buzzerActiveBeep(120, 80);
    buzzerActiveBeep(200, 0);
}

void buzzerMelodyAlert() {
    // 3 beeps de alerta
    for (int i = 0; i < 3; i++) {
        buzzerActiveBeep(200, 120);
    }
}

// ========================================
// FUNCIONES DE CONTROL DE HARDWARE
// ========================================

void grantAccess() {
    Serial.println("✅ ACCESO CONCEDIDO - Abriendo puerta...");
    
    // Indicadores visuales
    digitalWrite(LED_GREEN_BEHIND, HIGH);
    digitalWrite(LED_RED_BEHIND, LOW);
    //120
    
    // Activar relé para abrir puerta
    
    // Melodía de éxito
    buzzerMelodySuccess();
    
    // Mantener abierto 5 segundos (no bloqueante)
    unsigned long startTime = millis();
    while (millis() - startTime < 5000) {
        webSocket.loop();
        delay(10);
    }

    if (servo_status == 0) {
        digitalWrite(LED_GREEN_FRONTAL, HIGH);
        digitalWrite(LED_RED_FRONTAL, LOW);
        servo_status = 1;
        for (int i = servo_closed; i < servo_open; i+=servo_steps) {
            servo1.write(i);
            delay(25);
        }
    }
    // Cerrar
    // digitalWrite(LED_GREEN_BEHIND, LOW);
}

void denyAccess() {
    Serial.println("❌ ACCESO DENEGADO - Activando alerta...");
    
    // Indicadores visuales
    digitalWrite(LED_RED_BEHIND, HIGH);
    digitalWrite(LED_GREEN_BEHIND, LOW);
    
    // Melodía de alerta
    buzzerMelodyAlert();

    digitalWrite(LED_GREEN_FRONTAL, LOW);
    digitalWrite(LED_RED_FRONTAL, HIGH);
    
    // Mantener LED rojo 2 segundos
    unsigned long startTime = millis();
    while (millis() - startTime < 2000) {
        webSocket.loop();
        delay(10);
    }
    
    digitalWrite(LED_RED_BEHIND, LOW);
}

void close_door() {
    if (servo_status == 1) {
        digitalWrite(LED_GREEN_FRONTAL, LOW);
        digitalWrite(LED_RED_FRONTAL, LOW);
        // servo_status = 0;
        for (int i = servo_open; i > servo_closed; i-=servo_steps) {
            servo1.write(i);
            delay(25);
        }
    }
}

// ========================================
// FUNCIONES WEBSOCKET
// ========================================

void authenticateWebSocket() {
    Serial.println("[WS] Enviando autenticación...");
    
    StaticJsonDocument<256> doc;
    doc["type"] = "auth";
    doc["serviceUUID"] = SERVICE_UUID;
    doc["access_key"] = ACCESS_KEY;
    doc["access_password"] = ACCESS_PASSWORD;
    
    String output;
    serializeJson(doc, output);
    webSocket.sendTXT(output);
}

void sendPing() {
    if (!isAuthenticated) return;
    
    StaticJsonDocument<64> doc;
    doc["type"] = "ping";
    
    String output;
    serializeJson(doc, output);
    webSocket.sendTXT(output);
    
    Serial.println("[WS] Ping enviado");
}

void handleWebSocketMessage(uint8_t* payload, size_t length) {
    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    
    if (error) {
        Serial.print("[WS] Error parseando JSON: ");
        Serial.println(error.c_str());
        return;
    }
    
    const char* type = doc["type"];
    Serial.printf("[WS] Mensaje recibido: %s\n", type);
    
    // ===== CONEXIÓN INICIAL =====
    if (strcmp(type, "connected") == 0) {
        const char* clientId = doc["clientId"];
        Serial.println("========================================");
        Serial.println("  🔌 CONECTADO AL SERVIDOR");
        Serial.printf("  Client ID: %s\n", clientId);
        Serial.println("========================================");
    }
    
    // ===== AUTENTICACIÓN EXITOSA =====
    else if (strcmp(type, "auth_success") == 0) {
        isAuthenticated = true;
        Serial.println("========================================");
        Serial.println("  ✅ AUTENTICADO EXITOSAMENTE");
        Serial.printf("  Servicio: %s\n", SERVICE_UUID);
        Serial.println("  Esperando notificaciones...");
        Serial.println("========================================");
        
        // Indicador visual
        digitalWrite(LED_GREEN_BEHIND, HIGH);
        delay(1000);
        digitalWrite(LED_GREEN_BEHIND, LOW);

        // LED azul encendido brevemente para indicar autenticación exitosa
        digitalWrite(LED_BLUE_BEHIND, HIGH);
        delay(1500);
        // digitalWrite(LED_BLUE_BEHIND, LOW);
    }
    
    // ===== PONG (RESPUESTA A PING) =====
    else if (strcmp(type, "pong") == 0) {
        Serial.println("[WS] Pong recibido - Conexión activa");
    }
    
    // ===== 🎯 RECONOCIMIENTO DE ROSTRO (NOTIFICACIÓN AUTOMÁTICA) =====
    else if (strcmp(type, "recognition_match") == 0) {
        
        // Verificar que match existe
        if (!doc.containsKey("match")) {
            Serial.println("[WS] Error: match data missing");
            return;
        }
        
        // Extraer datos del match con validación NULL
        const char* name = doc["match"]["name"] | "Unknown";
        bool valid = doc["match"]["valid"] | false;
        const char* message = doc["match"]["message"] | "";
        long timestamp = doc["match"]["timestamp"] | 0;
        
        // MOSTRAR EN SERIAL
        Serial.println("\n\n");
        Serial.println("╔════════════════════════════════════════╗");
        Serial.println("║   🎯 ROSTRO DETECTADO EN WEBAPP      ║");
        Serial.println("╠════════════════════════════════════════╣");
        Serial.printf("║  Persona: %-28s║\n", name);
        Serial.printf("║  Acceso:  %-28s║\n", valid ? "✅ CONCEDIDO" : "❌ DENEGADO");
        
        if (message && strlen(message) > 0) {
            Serial.printf("║  Mensaje: %-28s║\n", message);
        }
        
        Serial.println("╚════════════════════════════════════════╝");
        Serial.println("\n");
        
        // CONTROLAR HARDWARE
        
        if (valid) {
            grantAccess();
        } else {
            denyAccess();
        }
    }
    
    // ===== ERROR =====
    else if (strcmp(type, "error") == 0) {
        const char* errorMsg = doc["error"];
        Serial.println("========================================");
        Serial.println("  ❌ ERROR DEL SERVIDOR");
        Serial.printf("  %s\n", errorMsg);
        Serial.println("========================================");
        
        // Si es error de autenticación, parpadear LED rojo
        if (strstr(errorMsg, "credential") || strstr(errorMsg, "auth")) {
            for(int i = 0; i < 5; i++) {
                digitalWrite(LED_RED_BEHIND, HIGH);
                delay(200);
                digitalWrite(LED_RED_BEHIND, LOW);
                delay(200);
            }
        }
    }
    
    // ===== TIPO DESCONOCIDO =====
    else {
        Serial.printf("[WS] Tipo de mensaje no manejado: %s\n", type);
    }
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("[WS] ❌ Desconectado");
            isAuthenticated = false;
            digitalWrite(LED_RED_BEHIND, HIGH);
            delay(1000);
            digitalWrite(LED_RED_BEHIND, LOW);
            digitalWrite(LED_BLUE_BEHIND, LOW);
            break;
            
        case WStype_CONNECTED:
            Serial.printf("[WS] ✅ Conectado a: wss://%s:%d%s\n", WS_HOST, WS_PORT, WS_PATH);
            // Autenticar inmediatamente
            authenticateWebSocket();
            break;
            
        case WStype_TEXT:
            handleWebSocketMessage(payload, length);
            break;
            
        case WStype_ERROR:
            Serial.println("[WS] ⚠️ Error en WebSocket");
            break;
    }
}

// ========================================
// SETUP
// ========================================

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n");
    Serial.println("========================================");
    Serial.println("  ESP32 - Auth Service Client");
    Serial.println("========================================");
    
    // Configurar pines
    pinMode(LED_GREEN_BEHIND, OUTPUT);
    pinMode(LED_RED_BEHIND, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_BLUE_BEHIND, OUTPUT);
    pinMode(LED_RED_FRONTAL, OUTPUT);
    pinMode(LED_GREEN_FRONTAL, OUTPUT);
    
    // Asegurarse de que todo esté apagado
    digitalWrite(LED_GREEN_BEHIND, LOW);
    digitalWrite(LED_RED_BEHIND, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_BLUE_BEHIND, LOW);

    servo1.attach(LED_SERVO, 500, 2400);
    servo1.write(servo_closed);
    
    delay(1000);
    
    // Conectar WiFi
    Serial.printf("Conectando a WiFi: %s\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        digitalWrite(LED_RED_BEHIND, !digitalRead(LED_RED_BEHIND)); // Parpadeo
        attempts++;
    }
    
    digitalWrite(LED_RED_BEHIND, LOW);
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n❌ Error: No se pudo conectar a WiFi");
        Serial.println("Verifica SSID y contraseña, luego reinicia el ESP32");
        while(1) {
            digitalWrite(LED_RED_BEHIND, HIGH);
            delay(200);
            digitalWrite(LED_RED_BEHIND, LOW);
            delay(200);
        }
    }
    
    Serial.println("\n✅ WiFi Conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    // Indicador visual de WiFi conectado
    for(int i = 0; i < 3; i++) {
        digitalWrite(LED_GREEN_BEHIND, HIGH);
        delay(100);
        digitalWrite(LED_GREEN_BEHIND, LOW);
        delay(100);
    }
    
    delay(2000);
    
    // Configurar WebSocket seguro (WSS)
    Serial.printf("\nConectando a WebSocket: wss://%s:%d%s\n", WS_HOST, WS_PORT, WS_PATH);

    // Nota: Algunas versiones de WebSocketsClient no incluyen setInsecure().
    // Si tu TLS falla por certificado, usa beginSSL con huella (fingerprint)
    // o setCACert(...) según soporte de tu librería.
    webSocket.beginSSL(WS_HOST, WS_PORT, WS_PATH);
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000); // Reconectar cada 5 segundos si se desconecta
    
    Serial.println("\n✅ Sistema iniciado correctamente");
    Serial.println("Esperando conexión al servidor...\n");
}

// ========================================
// LOOP
// ========================================

void loop() {
    // Mantener conexión WebSocket activa
    webSocket.loop();
    
    // Enviar ping periódico (keepalive)
    if (isAuthenticated && (millis() - lastPingTime > PING_INTERVAL)) {
        sendPing();
        lastPingTime = millis();
    }
    
    // La pantalla se mantiene encendida sin actualizaciones periódicas
    // Solo se actualiza cuando hay cambios de estado reales
    
    // Aquí puedes agregar más lógica si necesitas
    // Por ejemplo: leer sensores, controlar otros dispositivos, etc.
}

/*
 * ========================================
 * INSTRUCCIONES DE USO
 * ========================================
 * 
 * 1. OBTENER CREDENCIALES:
 *    - Abre tu dashboard en: http://[TU_IP]:3000/dashboard
 *    - Busca tu servicio de reconocimiento facial
 *    - Copia: UUID, access_key, access_password
 * 
 * 2. CONFIGURAR ESTE CÓDIGO:
 *    - Modifica WIFI_SSID y WIFI_PASSWORD
 *    - Modifica WS_HOST con la IP del servidor
 *    - Pega las credenciales del servicio
 * 
 * 3. VERIFICAR CONEXIONES:
 *    - Sube el código al ESP32
 *    - Abre el Serial Monitor (115200 baud)
 *    - Deberías ver: WiFi conectado → WebSocket conectado → Autenticado
 * 
 * 4. PROBAR:
 *    - Abre la WebApp del servicio
 *    - En el panel "ESP32 Conectados" deberías ver tu dispositivo
 *    - Activa la cámara y detección
 *    - Cuando detecte un rostro, el ESP32 recibirá la notificación
 * 
 * 5. CONEXIONES DE HARDWARE (OPCIONAL):
 *    - LED Verde → GPIO 2 → GND (con resistencia 220Ω)
 *    - LED Rojo → GPIO 4 → GND (con resistencia 220Ω)
 *    - Relé → GPIO 5 (módulo relé 5V)
 *    - Buzzer → GPIO 15 (buzzer activo o pasivo)
 * 
 * ========================================
 * TROUBLESHOOTING
 * ========================================
 * 
 * ❌ "No se pudo conectar a WiFi"
 *    → Verifica SSID y contraseña
 *    → Asegúrate que el ESP32 esté cerca del router
 * 
 * ❌ "Desconectado" constantemente
 *    → Verifica que WS_HOST sea la IP correcta
 *    → Verifica que el servidor Node.js esté corriendo
 *    → Ping a la IP del servidor desde tu PC
 * 
 * ❌ "Invalid credentials"
 *    → Verifica SERVICE_UUID, ACCESS_KEY y ACCESS_PASSWORD
 *    → Cópialos exactamente del dashboard
 * 
 * ❌ No recibo notificaciones
 *    → Verifica que el ESP32 esté "Autenticado" en el Serial
 *    → Verifica que aparezca en el panel "ESP32 Conectados" de la WebApp
 *    → Asegúrate de tener targets configurados en la WebApp
 * 
 * ========================================
 */
