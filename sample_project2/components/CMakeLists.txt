set(src_dirs
            BEEP
            KEY
            LED
            OLED)

set(include_dirs
            BEEP
            KEY
            LED
            OLED)

set(requires
            driver)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format -Wno-format )
