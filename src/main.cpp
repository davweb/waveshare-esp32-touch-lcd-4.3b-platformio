#include <Arduino.h>
#include <DebugLog.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include <lvgl_v8_port.h>
#include <Board.h>
#include <Wireless.h>
#include <ClockTools.h>

using namespace esp_panel::drivers;

uint8_t grid_row = 0;
lv_obj_t * grid_container = NULL;
lv_obj_t* timeValue = NULL;
lv_obj_t* touchValue = NULL;

lv_obj_t* add_row(const char *  labelText, const char * valueText) {
    LOG_DEBUG("Adding row", labelText, valueText);
    lv_obj_t * label = lv_label_create(grid_container);
    lv_label_set_text(label, labelText);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, grid_row, 1);

    lv_obj_t * value = lv_label_create(grid_container);
    lv_label_set_text(value, valueText);
    lv_color_t grey = lv_color_make(100, 100, 100);
    lv_obj_set_style_text_color(value, grey, 0);
    lv_obj_set_style_text_font(value, &lv_font_montserrat_26, 0);
    lv_obj_set_grid_cell(value, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, grid_row, 1);

    grid_row += 1;

    return value;
}

lv_obj_t* add_row(const char *  labelText, String valueText) {
    return add_row(labelText, valueText.c_str());
}

static void set_object_opacity(void * obj, int32_t v) {
    lv_obj_set_style_bg_opa((lv_obj_t *)obj, (lv_opa_t)v, 0);
}

static void animation_delete_object(lv_anim_t * a) {
    lv_obj_del((lv_obj_t *)a->var);
}

static void grid_touch_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED) {
        lv_indev_t * indev = lv_indev_get_act();

        if (indev == NULL) {
            return;
        }

        lv_point_t point;
        lv_indev_get_point(indev, &point);

        char coordinates[8];
        sprintf(coordinates, "%d,%d", point.x, point.y);

        LOG_DEBUG("Touch at", coordinates);

        lvgl_port_lock(-1);
        lv_label_set_text(touchValue, coordinates);

        lv_obj_t * circle = lv_obj_create(lv_scr_act());
        lv_obj_set_size(circle, 50, 50);
        lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(circle, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_set_style_border_width(circle, 0, 0);
        lv_obj_set_pos(circle, point.x - 25, point.y - 25);
        lv_obj_clear_flag(circle, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(circle, LV_OBJ_FLAG_CLICKABLE);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, circle);
        lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
        lv_anim_set_time(&a, 2000);
        lv_anim_set_exec_cb(&a, set_object_opacity);
        lv_anim_set_ready_cb(&a, animation_delete_object);
        lv_anim_start(&a);

        lvgl_port_unlock();
    }
}

void create_grid_widget(void)
{
    static lv_coord_t col_dsc[] = {200, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    grid_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(grid_container, lv_pct(100), lv_pct(100));
    lv_obj_center(grid_container);
    lv_obj_set_layout(grid_container, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(grid_container, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(grid_container, row_dsc, 0);
    lv_obj_clear_flag(grid_container, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(grid_container, grid_touch_event_cb, LV_EVENT_PRESSED, NULL);
}

String getPanelVersion() {
    char panel_version[9];
    sprintf(panel_version, "%d.%d.%d", ESP_PANEL_VERSION_MAJOR, ESP_PANEL_VERSION_MINOR, ESP_PANEL_VERSION_PATCH);
    return String(panel_version);
}

String getLvglVersion() {
    char lvgl_version[9];
    sprintf(lvgl_version, "%d.%d.%d", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    return String(lvgl_version);
}

void setup()
{
    Serial.begin(115200);

    // Wait for serial port to connect
    delay(5000);

    Board *board = initialiseBoard();

    startWiFi();

    setRtcClock();

    LOG_DEBUG("Initializing LVGL");
    lvgl_port_init(board->getLCD(), board->getTouch());

    LOG_DEBUG("Creating UI");
    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    create_grid_widget();

    add_row("Board", board->getConfig().name);
    add_row("Chip Model", ESP.getChipModel() + String(" revision ") + String(ESP.getChipRevision()));
    add_row("SDK Version", ESP.getSdkVersion());
    add_row("Core Version", ESP.getCoreVersion());
    add_row("Driver Version", getPanelVersion());
    add_row("LVGL Version", getLvglVersion());
    add_row("MAC Address", getMacAddress());
    add_row("IP Address", getLocalIpAddress());
    add_row("Timezone", getTimezone());
    timeValue = add_row("Date and Time", getCurrentDateTime());
    touchValue = add_row("Last Touch", "");

    /* Release the mutex */
    lvgl_port_unlock();
}

void loop()
{
    lvgl_port_lock(-1);
    lv_label_set_text(timeValue, getCurrentDateTime().c_str());
    lvgl_port_unlock();

    LOG_TRACE("Idle loop");
    delay(1000);
}

