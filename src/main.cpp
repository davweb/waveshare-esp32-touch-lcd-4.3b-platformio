#include <Arduino.h>
#include <DebugLog.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include <lvgl_v8_port.h>
#include <Board.h>
#include <Wireless.h>

using namespace esp_panel::drivers;

void setup()
{
    Serial.begin(115200);

    // Wait for serial port to connect
    delay(5000);

    startWiFi();

    Board *board = initialiseBoard();

    LOG_DEBUG("Initializing LVGL");
    lvgl_port_init(board->getLCD(), board->getTouch());

    LOG_DEBUG("Creating UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    /**
     * Create the simple labels
     */
    lv_obj_t *label_1 = lv_label_create(lv_scr_act());
    lv_label_set_text(label_1, "Hello World!");
    lv_obj_set_style_text_font(label_1, &lv_font_montserrat_30, 0);
    lv_obj_align(label_1, LV_ALIGN_CENTER, 0, -20);
    lv_obj_t *label_2 = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(
        label_2, "ESP32_Display_Panel (%d.%d.%d)",
        ESP_PANEL_VERSION_MAJOR, ESP_PANEL_VERSION_MINOR, ESP_PANEL_VERSION_PATCH
    );
    lv_obj_set_style_text_font(label_2, &lv_font_montserrat_16, 0);
    lv_obj_align_to(label_2, label_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_t *label_3 = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(label_3, "LVGL (%d.%d.%d)", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    lv_obj_set_style_text_font(label_3, &lv_font_montserrat_16, 0);
    lv_obj_align_to(label_3, label_2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /* Release the mutex */
    lvgl_port_unlock();
}

void loop()
{
    LOG_DEBUG("Idle loop");
    delay(1000);
}

