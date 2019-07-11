
add_library(ishiiruka_libretro SHARED
  Boot.cpp
  Host.cpp
  Input.cpp
  Log.cpp
  Main.cpp
  Options.cpp
  RetroGLContext.cpp
  Video.cpp
)

if (NOT APPLE)
   target_sources(ishiiruka_libretro PRIVATE
      Vulkan.cpp
   )
endif()

include_directories(${CMAKE_SOURCE_DIR}/Externals/Libretro/Include)
include_directories(${CMAKE_SOURCE_DIR}/Externals/Vulkan/Include)

set(LIBRARY_OUTPUT_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

set_target_properties(ishiiruka_libretro PROPERTIES PREFIX "")
if(ANDROID)
   target_sources(ishiiruka_libretro PRIVATE
   )
   set_target_properties(ishiiruka_libretro PROPERTIES SUFFIX "_android.so")
endif()

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
   set(CLANG 1)
endif()

if(NOT MSVC AND NOT CLANG)
   set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
else()
   target_link_libraries(ishiiruka_libretro PRIVATE
      core
      uicommon
      ${LIBS}
   )
endif()
   
if(LIBRETRO_STATIC)
   target_link_libraries(ishiiruka_libretro PRIVATE
      core
      uicommon
      ${LIBS}
      "-static-libgcc -static-libstdc++"
   )
endif()
