#include "playnoteI2S.h"


// Fréquence de base pour le calcul des fréquences des notes

const i2s_port_t i2sPort = I2S_NUM_0; // Déclaration de i2sPort en dehors de toutes les fonctions


// Fréquence de base pour le calcul des fréquences des notes

bool i2sInitialized = false;

void setupCustomI2S(int bckPin, int lrckPin, int dataOutPin)
{
    i2s_config_t i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX), // Convertir en i2s_mode_t
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    i2s_pin_config_t pinConfig = {
        .bck_io_num = bckPin,
        .ws_io_num = lrckPin,
        .data_out_num = dataOutPin,
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    esp_err_t err = i2s_driver_install(i2sPort, &i2sConfig, 0, NULL);
    if (err != ESP_OK) {
        Serial.println("Erreur lors de l'initialisation du bus I2S");
        return;
    }

    err = i2s_set_pin(i2sPort, &pinConfig);
    if (err != ESP_OK) {
        Serial.println("Erreur lors de la configuration des pins du bus I2S");
        return;
    }

    i2sInitialized = true;
}

// Générer une note de la fréquence spécifiée pendant la durée spécifiée
void playNote(float frequency, int duration, int volumePercent) {
    if (!i2sInitialized) {
        Serial.println("Erreur : bus I2S non initialisé");
        return;
    }

    // Convertir le volume en fraction entre 0 et 1
    float volume = volumePercent / 100.0;

    size_t numSamples = duration * SAMPLE_RATE / 1000;
    size_t bytesToSend = numSamples * sizeof(int16_t) * CHANNEL_NUM;

    int16_t *samples = (int16_t *)malloc(bytesToSend);
    for (int i = 0; i < numSamples; i++) {
        float t = (float)i / SAMPLE_RATE;
        float sample = volume * 0.5 * sin(2 * M_PI * frequency * t);
        samples[i * 2] = (int16_t)(INT16_MAX * sample);
        samples[i * 2 + 1] = (int16_t)(INT16_MAX * sample);
    }

    size_t bytesWritten;
    esp_err_t err = i2s_write(i2sPort, samples, bytesToSend, &bytesWritten, portMAX_DELAY);
    if (err != ESP_OK) {
        Serial.print("Erreur lors de l'écriture sur le bus I2S: ");
        Serial.println(err);
    }

    free(samples);
}
