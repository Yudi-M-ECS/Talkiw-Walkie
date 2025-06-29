#include <Arduino.h>
#include <driver/i2s.h>

#define I2S_WS   25  // Word Select (L/R Clock)
#define I2S_BCLK 26  // Bit Clock (SCK)
#define I2S_SD   21  // Serial Data (DATA)

#define SAMPLE_RATE 16000  // Sampling rate 16kHz
#define BUFFER_SIZE 120    // Buffer untuk menyimpan sampel audio

void setupI2S() {
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = BUFFER_SIZE,
        .use_apll = false,
        .tx_desc_auto_clear = false,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_SD
    };
    
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void setup() {
    Serial.begin(115200);
    setupI2S();
    Serial.println("ESP32 INMP441 Mic Test");
}

void loop() {
    int32_t samples[BUFFER_SIZE];
    size_t bytesRead;
    
    i2s_read(I2S_NUM_0, &samples, sizeof(samples), &bytesRead, portMAX_DELAY);
    int numSamples = bytesRead / sizeof(int32_t);
    
    for (int i = 0; i < numSamples; i += 10) {  // Mengurangi jumlah sampel yang dicetak
        Serial.println(samples[i] / 5000);  // Skalakan agar lebih kecil di Serial Plotter
    }
  delay(1000);
}
