#include "esp_camera.h"
#include <WiFi.h>

// ===========================
// Select camera model in board_config.h
// ===========================
#include "board_config.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char *ssid = "MEGACABLE-2.4G-77A5";
const char *password = "dB44s6td42";

void startCameraServer();
void setupLedFlash();

void setup() {
  // 1. Iniciar el Serial
  Serial.begin(115200);
  delay(1000); 
  Serial.println("--- PRUEBA DE ARRANQUE (MODO DRAM FORZADO) ---");

  // 2. Definir la configuración de la cámara
  Serial.println("Definiendo configuracion de camara...");
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  // Dejamos el resto de los ajustes por defecto por ahora...
  config.pixel_format = PIXFORMAT_JPEG;


  // ================================================================
  // !! PRUEBA IMPORTANTE !!
  // Forzamos el uso de la memoria interna (DRAM) en lugar de PSRAM.
  // Esto limitará la calidad, pero probará si la PSRAM es el fallo.
  // ================================================================
  Serial.println("!!! PRUEBA: FORZANDO MODO DRAM (SIN PSRAM) !!!");
  config.frame_size = FRAMESIZE_SVGA; // (800x600) Tamaño más pequeño
  config.fb_location = CAMERA_FB_IN_DRAM; // Usar memoria interna
  config.jpeg_quality = 12;
  config.fb_count = 1; // Solo un búfer en DRAM

  
  // 3. INTENTAR INICIAR LA CÁMARA
  Serial.println("Intentando esp_camera_init()...");
  esp_err_t err = esp_camera_init(&config);
  
  if (err != ESP_OK) {
    Serial.printf("!!! Camera init falló con error 0x%x\n", err);
    Serial.println("REINICIO EN 10 SEGUNDOS...");
    delay(10000);
    ESP.restart(); // Reiniciar si la cámara falla
  }

  // 4. Si llegamos aquí, la cámara funcionó.
  Serial.println("¡¡¡ Camera init OK !!!");

  // El resto de tu código...
  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);
    s->set_brightness(s, 1);
    s->set_saturation(s, -2);
  }
  
  #if defined(CAMERA_MODEL_ESP32S3_EYE)
    s->set_vflip(s, 1);
  #endif
  
  // 5. Intentar conectar al WiFi
  Serial.println("Conectando a WiFi...");
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡WiFi conectado!");

  // 6. Iniciar el servidor
  startCameraServer();
  Serial.print("Servidor de camara listo! Entra en: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Dejarlo vacío está bien
  delay(10000);
}
