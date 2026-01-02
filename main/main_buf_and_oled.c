#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"

#include "oled_basic.h"
#include "oled_text.h"
#include "buff.h"

/* =======================
 *  DEFINICJE
 * ======================= */
#define TAG "MAIN"

#define SDA_PIN   21
#define SCL_PIN   22
#define I2C_PORT  I2C_NUM_0
#define I2C_FRQ   400000

/* =======================
 *  I2C INIT
 * ======================= */
static esp_err_t i2c_master_init(i2c_port_t i2c_num,
                                 gpio_num_t sda_io,
                                 gpio_num_t scl_io,
                                 uint32_t clk_speed)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_io,
        .scl_io_num = scl_io,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = clk_speed,
    };

    ESP_ERROR_CHECK(i2c_param_config(i2c_num, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_num, conf.mode, 0, 0, 0));
    return ESP_OK;
}

/* =======================
 *  MAIN
 * ======================= */
void app_main(void)
{
    ESP_ERROR_CHECK(i2c_master_init(I2C_PORT, SDA_PIN, SCL_PIN, I2C_FRQ));

    ESP_LOGI(TAG, "OLED init");
    ESP_ERROR_CHECK(oled_init(I2C_PORT));
    ESP_ERROR_CHECK(oled_text_init());

    ESP_ERROR_CHECK(oled_text_clear());
    ESP_ERROR_CHECK(oled_text_flush(I2C_PORT));

    clear_buff();  // czyścimy bufor, choć tu go nie używamy

    ESP_LOGI(TAG, "Live typing demo");

    const char text[] = "hello world";

    while (1) {
        for (int i = 0; text[i] != '\0'; i++) {
            send_buff(text[i]);                 // ⬅️ LIVE typing
            vTaskDelay(pdMS_TO_TICKS(400));     // prędkość „pisania”
        }

        vTaskDelay(pdMS_TO_TICKS(3000));

        ESP_ERROR_CHECK(oled_text_clear());
        ESP_ERROR_CHECK(oled_text_flush(I2C_PORT));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
