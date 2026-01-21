idf_component_register(SRCS "main.c" "main_buf_and_oled.c" 
                       INCLUDE_DIRS "."
                       REQUIRES oled flex buff)