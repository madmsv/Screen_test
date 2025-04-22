idf_component_register(SRCS "Display.c"
                    INCLUDE_DIRS "include" "../sh1106/include"
                    REQUIRES freertos driver esp_lcd)

