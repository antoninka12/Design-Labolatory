idf_component_register(
    SRCS "oled_text.c" "bitmap.c" "oled_basic.c" 
    INCLUDE_DIRS "include"
    REQUIRES driver
    PRIV_REQUIRES flex
)
