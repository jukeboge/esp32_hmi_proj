idf_component_register(SRCS "ui.c"
                    INCLUDE_DIRS "include"
                    REQUIRES lvgl font_parse unigbk
                    WHOLE_ARCHIVE)
