#include <stdio.h>
#include <esp_lcd_panel_ops.h>
#include <string.h>
#include "taskMonitor.h"
#include "tgx_c.h"
#include "esp_log.h"
#include "timeProbe.h"
#include "lcd.h"


uint16_t frameBuffer[240 * 240];

TaskHandle_t handle_taskFlush;

void taskFlush(void *parm) {

    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        uint8_t *frame = (uint8_t *) getFrame();
        for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; ++i) {
            ((uint8_t *) frameBuffer)[i * 2] = frame[i * 2 + 1];
            ((uint8_t *) frameBuffer)[i * 2 + 1] = frame[i * 2];
        }
        esp_lcd_panel_draw_bitmap(lcd_panel_handle, 0, 0, LCD_WIDTH, LCD_HEIGHT, frameBuffer);

    }

}

void app_main(void) {
    startTaskMonitor(10000);
    if (ESP_OK != init_lcd()) {
        ESP_LOGE("LCD", "LCD init fail");
        while (1);
    }
    xTaskCreatePinnedToCore(taskFlush, "taskFlush", 4 * 1024, NULL, 5, &handle_taskFlush, 1);

    tgx_setup();
    uint8_t fps_count = {0};
    timeProbe_t fps;
    timeProbe_start(&fps);
    int64_t lastFrameTime = 0;
    while (1) {
        fps_count++;
        if (fps_count == 0) {
            ESP_LOGI("fps", "fps: %f", 256.0f * 1000.0f / (timeProbe_stop(&fps) / 1000.0));
            timeProbe_start(&fps);
        }


        {//限制fps
            int64_t current = esp_timer_get_time();
            int64_t shouldFlushTime = lastFrameTime + (1000 * 1000 / 50);
            if (shouldFlushTime > current) {
                vTaskDelay(pdMS_TO_TICKS((shouldFlushTime - current) >> 10));
                lastFrameTime = shouldFlushTime;
            } else {
                lastFrameTime = current;
            }
        }

        tgx_next();
        xTaskNotifyGive(handle_taskFlush);
    }

}
