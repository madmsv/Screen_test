idf_component_register(SRCS "Display.c" "../../sh1106-esp-idf/esp_lcd_panel_sh1106.c" #"../../../sh1106-esp-idf/esp_lcd_panel_sh1106.c"
                    INCLUDE_DIRS "include" "../../sh1106-esp-idf/include"
                    REQUIRES freertos driver esp_lcd)

