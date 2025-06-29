#include <driver/i2s.h>

#define I2S_DATA_OUT_PIN 22   // Pin untuk data I2S
#define I2S_BCLK_PIN   19     // Pin untuk bit clock I2S
#define I2S_LRCK_PIN   23     // Pin untuk word select I2S

// Setup konfigurasi I2S
void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_TX,  // Mode Master dan Transmit
    .sample_rate = 44100,                   // Sample rate 44.1 kHz
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, // 1 channel (mono)
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1 // Perbaiki urutan penugasan
  };

  // Perbaikan penggunaan pin config
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK_PIN,   // Pin untuk bit clock
    .ws_io_num = I2S_LRCK_PIN,    // Pin untuk word select
    .data_out_num = I2S_DATA_OUT_PIN,  // Pin untuk data out
    .data_in_num = I2S_PIN_NO_CHANGE  // Tidak ada input data
  };

  // Inisialisasi I2S
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
}

void setup() {
  Serial.begin(115200);
  setupI2S();
  delay(1000);
}

void loop() {
  int16_t audioData[128];  // Buffer untuk data audio
  size_t bytesRead;
  
  // Membaca data audio dari mikrofon (misalnya dari input analog)
  // Di sini kita membaca dari pin analog (misalnya pin 34)
  for (int i = 0; i < 128; i++) {
    audioData[i] = analogRead(34) - 512;  // Baca dan offsetkan nilai
    delayMicroseconds(22);  // Sesuaikan dengan sample rate
  }
  
  // Kirim data audio ke MAX98357A melalui I2S
  i2s_write(I2S_NUM_0, (const char *)audioData, sizeof(audioData), &bytesRead, portMAX_DELAY);
}
