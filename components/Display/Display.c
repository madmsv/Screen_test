#include <stdio.h>
#include "Display.h"

#define TAG_SETUP "DISPLAY SETUP"
#define TAG_ONGOING "DISPLAY ONGOING"
#define TAG_CLOSING "DISPLAY CLOSING"

disp_handle init_display(Display_config *cfg)
{
    ESP_LOGI(TAG_SETUP, "Starting display setup");
    i2c_master_bus_handle_t bus_handle = NULL;
    if(cfg->master_bus_handle == NULL){
        ESP_LOGW(TAG_SETUP, "Did not find a master bus inside of the config, so creating a new one");
        i2c_master_bus_config_t bus_config = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .scl_io_num = cfg->io_scl,
            .sda_io_num = cfg->io_sda,
            .i2c_port = cfg->port_num,
            .glitch_ignore_cnt = 7,
            .flags.enable_internal_pullup = true,
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));
    }
    else{
        bus_handle = cfg->master_bus_handle;
    }

    ESP_LOGI(TAG_SETUP, "Making i2c io handle");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = cfg->dev_addr,
        .scl_speed_hz = (400 * 1000),
        .control_phase_bytes = 1,   // refer to LCD spec
        .dc_bit_offset = 6,         // refer to LCD spec
        .lcd_cmd_bits = 8,          // refer to LCD spec
        .lcd_param_bits = 8,        // refer to LCD spec
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(bus_handle, &io_config, &io_handle));

    
    ESP_LOGI(TAG_SETUP, "Install panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
    };

    if (CONFIG_LCD_CONTROLLER_SH1106){
        ESP_LOGI(TAG_SETUP, "Creating handle for sh1106");
        ESP_ERROR_CHECK(esp_lcd_new_panel_sh1106(io_handle, &panel_config, &panel_handle));
    }
    // ESP_ERROR_CHECK(panel_sh1106_mirror(&panel_handle, true, false));
    
    ESP_LOGI(TAG_SETUP, "Doing a round of \"reset\", \"init\" and \"turn on\" for the display");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));
    // ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, false));
    // vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));
    
    ESP_LOGI(TAG_SETUP, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);
    
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = cfg->width_res * cfg->height_res,
        .double_buffer = true,
        .hres = cfg->width_res,
        .vres = cfg->height_res,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };

    ESP_LOGI(TAG_SETUP, "Creating display handle");
    lv_disp_t *new_disp = lvgl_port_add_disp(&disp_cfg);
    lv_disp_set_rotation(new_disp, LV_DISP_ROT_180); //LV_DISP_ROT_NONE
    disp_handle handle = (disp_handle)malloc(sizeof(Display_t));
    handle->disp = new_disp;
    ESP_LOGI(TAG_SETUP, "Done with setup");
    return handle;
}

esp_err_t add_lable(disp_handle display, char * txt){
    lv_obj_t *scr = lv_disp_get_scr_act(display->disp);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(label, txt); //"Hello Espressif, Hello LVGL."
    /* Size of the screen (if you use rotation 90 or 270, please use lv_display_get_vertical_resolution) */
    lv_obj_set_width(label, 80);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    return ESP_OK;
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

esp_err_t add_menu(disp_handle display, char *name){
    lv_obj_t *scr = lv_disp_get_scr_act(display->disp);
    lv_obj_t * label;

    lv_obj_t * btn1 = lv_btn_create(scr);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    lv_obj_t * btn2 = lv_btn_create(scr);
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Toggle");
    lv_obj_center(label);
    return ESP_OK;
}



