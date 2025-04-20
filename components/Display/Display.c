#include <stdio.h>
#include "Display.h"

#define TAG_SETUP "DISPLAY SETUP"
#define TAG_ONGOING "DISPLAY ONGOING"
#define TAG_CLOSING "DISPLAY CLOSING"

// static void example_lvgl_flush_cb(lv_disp_t *disp, const lv_area_t *area, uint8_t *px_map);

// static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t io_panel, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
// {
//     lv_disp_t *disp = (lv_disp_t *)user_ctx;
//     lv_display_flush_ready(disp);
//     return false;
// }

// static void example_increase_lvgl_tick(void *arg)
// {
//     /* Tell LVGL how many milliseconds has elapsed */
//     lv_tick_inc(5); //exemple code has this as a macro "EXAMPLE_LVGL_TICK_PERIOD_MS = 5"
// }

// static uint8_t oled_buffer[64 * 132 / 8]; //not flexable

// static _lock_t lvgl_api_lock;

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

    //! this is not the drives i need for my screen
    // esp_lcd_panel_ssd1306_config_t ssd1306_config = {
    //     .height = cfg->height_res,
    // };
    // panel_config.vendor_config = &ssd1306_config;
    // ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    // ESP_ERROR_CHECK(esp_lcd_new_panel_sh1106(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_new_panel_sh1106(io_handle, &panel_config, &panel_handle));
    
    ESP_LOGI(TAG_SETUP, "Doing a round of \"reset\", \"init\" and \"turn on\" for the display");
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    
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
    lv_disp_set_rotation(new_disp, LV_DISP_ROT_NONE);
    disp_handle handle = (disp_handle)malloc(sizeof(Display_t));
    handle->disp = new_disp;
    ESP_LOGI(TAG_SETUP, "Done with setup");
    return handle;
}

// static void example_lvgl_flush_cb(lv_disp_t *disp, const lv_area_t *area, uint8_t *px_map)
// {
//     esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);

//     // This is necessary because LVGL reserves 2 x 4 bytes in the buffer, as these are assumed to be used as a palette. Skip the palette here
//     // More information about the monochrome, please refer to https://docs.lvgl.io/9.2/porting/display.html#monochrome-displays
//     px_map += EXAMPLE_LVGL_PALETTE_SIZE;

//     uint16_t hor_res = lv_display_get_physical_horizontal_resolution(disp);
//     // ESP_LOGW("HOR_RES: ", "%d", hor_res);
//     int x1 = area->x1;
//     int x2 = area->x2;
//     int y1 = area->y1;
//     int y2 = area->y2;

//     for (int y = y1; y <= y2; y++) {
//         for (int x = x1; x <= x2; x++) {
//             /* The order of bits is MSB first
//                         MSB           LSB
//                bits      7 6 5 4 3 2 1 0
//                pixels    0 1 2 3 4 5 6 7
//                         Left         Right
//             */
//             bool chroma_color = (px_map[(hor_res >> 3) * y  + (x >> 3)] & 1 << (7 - x % 8));

//             /* Write to the buffer as required for the display.
//             * It writes only 1-bit for monochrome displays mapped vertically.*/
//             uint8_t *buf = oled_buffer + hor_res * (y >> 3) + (x);
//             if (chroma_color) {
//                 (*buf) &= ~(1 << (y % 8));
//             } else {
//                 (*buf) |= (1 << (y % 8));
//             }
//         }
//     }
//     // pass the draw buffer to the driver
//     esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, oled_buffer);
// }

/*
const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);
    
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = cfg->width_res * cfg->height_res,
        .double_buffer = true,
        .hres = cfg->height_res,
        .vres = cfg->width_res,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };

    ESP_LOGI(TAG_SETUP, "Creating display handle");
    lv_disp_t *new_disp = lvgl_port_add_disp(&disp_cfg);
*/

/*
lv_init();
    // create a lvgl display
    lv_disp_t *display = lv_display_create(cfg->height_res, cfg->width_res);
    // associate the i2c panel handle to the display
    lv_display_set_user_data(display, panel_handle);
    // create draw buffer
    void *buf = NULL;
    ESP_LOGI(TAG_SETUP, "Allocate separate LVGL draw buffers");
    // LVGL reserves 2 x 4 bytes in the buffer, as these are assumed to be used as a palette.
    size_t draw_buffer_sz = cfg->height_res * cfg->width_res / 8 + EXAMPLE_LVGL_PALETTE_SIZE;
    buf = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf);

    // LVGL9 suooprt new monochromatic format.
    lv_display_set_color_format(display, 7); // 7 = enum "LV_COLOR_FORMAT_I1"
    // initialize LVGL draw buffers
    lv_display_set_buffers(display, buf, NULL, draw_buffer_sz, 2); // 2 = enum "LV_DISPLAY_RENDER_MODE_FULL"
    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(display, example_lvgl_flush_cb);

    ESP_LOGI(TAG_SETUP, "Register io panel event callback for LVGL flush ready notification");
    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = example_notify_lvgl_flush_ready,
    };
    // Register done callback
    esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, display);

    ESP_LOGI(TAG_SETUP, "Use esp_timer as LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG_SETUP, "Create LVGL task");
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL);

    ESP_LOGI(TAG_SETUP, "Display LVGL Scroll Text");
    // Lock the mutex due to the LVGL APIs are not thread-safe
    _lock_acquire(&lvgl_api_lock);
    example_lvgl_demo_ui(display);
    _lock_release(&lvgl_api_lock);

*/