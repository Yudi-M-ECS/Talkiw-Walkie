#include <WiFi.h>
#include <esp_now.h>
#include "driver/i2s.h" 

#define I2S_SCK 26
#define I2S_WS 25
#define I2S_SD 27

#define BUFFER_SIZE 120
int16_t audioBuffer[BUFFER_SIZE];

void setupI2S() {
  Serial.println("Setting up I2S...");

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 512,
    .use_apll = false,
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_SD,
    .data_in_num = -1
  };

  if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) == ESP_OK) {
    Serial.println("I2S driver installed successfully.");
  } else {
    Serial.println("Error installing I2S driver.");
  }

  if (i2s_set_pin(I2S_NUM_0, &pin_config) == ESP_OK) {
    Serial.println("I2S pins configured successfully.");
  } else {
    Serial.println("Error configuring I2S pins.");
  }
}

void onDataReceived(const esp_now_recv_info_t *recvInfo, const uint8_t *data, int len) {
  Serial.print("Data received from: ");
  for (int i = 0; i < 6; ++i) {
    Serial.printf("%02X", recvInfo->src_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  Serial.print("Data length: ");
  Serial.println(len);

  memcpy(audioBuffer, data, len);
  for (int i = 0; i < len / sizeof(int16_t); i++) {
    audioBuffer[i] = constrain(audioBuffer[i] * 2, -32768, 32767); 
  }

  size_t bytesWritten;
  i2s_write(I2S_NUM_0, audioBuffer, len, &bytesWritten, portMAX_DELAY);

  Serial.print("Bytes written to I2S: ");
  Serial.println(bytesWritten);
}


void setupESPNow() {
  Serial.println("Setting up ESP-NOW...");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW initialized successfully.");
  } else {
    Serial.println("Error initializing ESP-NOW.");
    return;
  }

  esp_now_register_recv_cb(onDataReceived);
}
 
void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Receiver...");
  setupI2S();
  setupESPNow();
}

void loop() {
}
