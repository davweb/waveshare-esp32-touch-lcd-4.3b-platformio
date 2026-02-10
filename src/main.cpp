#include <Arduino.h>
#include <DebugLog.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include <lvgl_v8_port.h>
#include <Board.h>
#include <Wireless.h>

using namespace esp_panel::drivers;

uint8_t grid_row = 0;
lv_obj_t * grid_container = NULL;


void add_row(const char *  labelText, const char * valueText) {
    LOG_DEBUG("Adding row", labelText, valueText);
    lv_obj_t * label = lv_label_create(grid_container);
    lv_label_set_text(label, labelText);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, grid_row, 1);

    lv_obj_t * value = lv_label_create(grid_container);
    lv_label_set_text(value, valueText);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_24, 0);
    lv_obj_set_grid_cell(value, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, grid_row, 1);

    grid_row += 1;
}

void add_row(const char *  labelText, String valueText) {
    add_row(labelText, valueText.c_str());
}

void create_grid_widget(void)
{
    static lv_coord_t col_dsc[] = {350, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    grid_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(grid_container, lv_pct(100), lv_pct(100));
    lv_obj_center(grid_container);
    lv_obj_set_layout(grid_container, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(grid_container, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid_container, row_dsc, 0);
}

void setup()
{
    Serial.begin(115200);

    // Wait for serial port to connect
    delay(5000);

    Board *board = initialiseBoard();

    startWiFi();

    LOG_DEBUG("Initializing LVGL");
    lvgl_port_init(board->getLCD(), board->getTouch());

    // Collect values
    char panel_version[9];
    sprintf(panel_version, "%d.%d.%d", ESP_PANEL_VERSION_MAJOR, ESP_PANEL_VERSION_MINOR, ESP_PANEL_VERSION_PATCH);
    char lvgl_version[9];
    sprintf(lvgl_version, "%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);

    LOG_DEBUG("Creating UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    create_grid_widget();

    add_row("SDK Version", ESP.getSdkVersion());
    add_row("Core Version", ESP.getCoreVersion());
    add_row("ESP32_Display_Panel Version", panel_version);
    add_row("LVGL Version", lvgl_version);
    add_row("MAC Address", getMacAddress());
    add_row("IP Address", getLocalIpAddress());
    add_row("Chip Model", ESP.getChipModel() + String(" revision ") + String(ESP.getChipRevision()));

    /* Release the mutex */
    lvgl_port_unlock();
}

void loop()
{
    LOG_DEBUG("Idle loop");
    delay(1000);
}

