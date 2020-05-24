target_sources( 
    TantalusStage1
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/gpio.cpp
        ${CMAKE_CURRENT_LIST_DIR}/lib/HAL/time.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/stdio_controller.cpp
)
