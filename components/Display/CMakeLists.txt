idf_component_register(SRCS "Display.c"
                    INCLUDE_DIRS "include" "../../sh1106-esp-idf/include"
                    #PRIV_REQUIRES "../../sh1106-esp-idf/include"
                    REQUIRES freertos driver esp_lcd)

