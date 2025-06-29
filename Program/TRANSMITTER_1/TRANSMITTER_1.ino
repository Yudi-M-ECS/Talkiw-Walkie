#include <WiFi.h>
#include <esp_now.h>
#include "driver/i2s.h"

#define I2S_SCK 26
#define I2S_WS 25
#define I2S_SD 21

#define PUSH_BUTTON 5 

// Buffer size for audio data
#define BUFFER_SIZE 120
int16_t audioBuffer[BUFFER_SIZE];

// ESP-NOW broadcast address (alamat MAC penerima)
uint8_t broadcastAddress[] = {0xCC, 0xDB, 0xA7, 0x96, 0x26, 0x78};

void setupI2S() {
  Serial.println("Setting up I2S...");
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
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
    .data_out_num = -1,
    .data_in_num = I2S_SD
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

void setupESPNow() {
  Serial.println("Setting up ESP-NOW...");
  
  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW initialized successfully.");
  } else {
    Serial.println("Error initializing ESP-NOW.");
    return; 
  }
  
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    Serial.println("Peer added successfully.");
  } else {
    Serial.println("Failed to add peer.");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Sender...");
  setupI2S();
  setupESPNow();

  pinMode(PUSH_BUTTON, INPUT_PULLUP);
}

void loop() {
  size_t bytesRead;

  if(digitalRead(PUSH_BUTTON) == LOW){

  // Membaca data dari INMP441
    i2s_read(I2S_NUM_0, &audioBuffer, BUFFER_SIZE * sizeof(int16_t), &bytesRead, portMAX_DELAY);

    if (bytesRead > 0) {
      // Menambahkan gain dan membatasi nilai agar tidak terlalu tinggi
      for (int i = 0; i < BUFFER_SIZE; i++) {
        audioBuffer[i] = constrain(audioBuffer[i] * 2, -32768, 32767);  // Peningkatan gain dan membatasi nilai
      }

      Serial.print("Bytes read: ");
      Serial.println(bytesRead);
      
      // Mengirim data melalui ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)audioBuffer, bytesRead);
      if (result == ESP_OK) {
        Serial.println("Data sent successfully.");
      } else {
        Serial.print("Error sending data: ");
        Serial.println(result);
      }
    } else {
      Serial.println("No data read from I2S.");
    }
  }

  delay(10); // Memberi jeda agar proses tidak terlalu cepat
}

