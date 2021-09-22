set (IMGUI_HEADERS
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imconfig.h"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.h"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_internal.h"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imstb_rectpack.h"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imstb_textedit.h"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imstb_truetype.h"
    PARENT_SCOPE           
)                          
                           
set (IMGUI_SOURCES         
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp"
    "${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp"
    PARENT_SCOPE
)