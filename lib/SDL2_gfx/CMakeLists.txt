add_library(SDL2_gfx SDL2_gfxPrimitives.c SDL2_rotozoom.c)

IF (WIN32)
    target_link_libraries(SDL2_gfx mingw32 SDL2main SDL2)
else ()
    target_link_libraries(SDL2_gfx SDL2main SDL2)
ENDIF (WIN32)