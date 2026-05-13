if (CONFIG_HANES_DRIVER)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/hanes_driver.c)
endif()
zephyr_include_directories(include)
