#include <stdio.h>
#include "Display.h"

#define TAG_SETUP "DISPLAY SETUP"
#define TAG_ONGOING "DISPLAY ONGOING"
#define TAG_CLOSING "DISPLAY CLOSING"

disp_handle init_display(Display_config *cfg)
{
    if(cfg->master_bus_handle == NULL){
        ESP_LOGW(TAG_SETUP, "Did not find a master bus inside of the config, so creating a new one");
        i2c_master_bus_handle_t bus_handle = NULL;
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

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = cfg->dev_addr,
        .scl_speed_hz = (400 * 1000),
        .control_phase_bytes = 1,   // refer to LCD spec
        .dc_bit_offset = 6,         // refer to LCD spec
        .lcd_cmd_bits = 8,          // refer to LCD spec
        .lcd_param_bits = 8,        // refer to LCD spec
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    /*
    ESP_LOGI(TAG, "Install SSD1306 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = PIN_NUM_RST,
    };

    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    
    
    
    ESP_LOGI("DEBUG", "hello\n\n");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_LOGI("DEBUG", "hello2\n\n");

    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * LCD_V_RES,
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };
    lv_disp_t *disp = lvgl_port_add_disp(&disp_cfg);
    lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);
    screen_handle screen = (screen_handle)malloc(sizeof(screen_t));
    screen->disp = disp;
    screen->label = NULL;
    return screen;
    */
}
