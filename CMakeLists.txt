cmake_minimum_required(VERSION 2.6.2)

# ==============================================================================
#
#  Lubyk
#
# ==============================================================================

project (LUBYK)
set(LUBYK_VERSION "0.5")
set(LUBYK_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# ==============================================================================
#
#  Options
#
# ==============================================================================
set(QT_SRC_PATH "" CACHE STRING    "Use static qt build (provide qt src path)")

# ==============================================================================
#
#  Platform guessing
#
# ==============================================================================

if(UNIX)
  add_definitions(-DLUA_USE_LINUX)
  if(APPLE)
    set(LUBYK_LIB /Applications/Lubyk.app/Contents/Resources)
    set(PLAT "macosx")
    # add_definitions(-DLUA_USE_MACOSX)
    add_definitions(-D__MACOSX_CORE__)
    # CoreFoundation needed by Midi and cannot be added in dylib
#    set(PLAT_LINK "-all_load -flat_namespace -lreadline -framework Accelerate -framework CoreFoundation -framework OpenGL -framework GLUT -framework Cocoa -lobjc") # -framework Cocoa -lobjc ===> planet_gui.mm
    set(PLAT_LINK "-all_load -flat_namespace -lreadline -framework Accelerate -framework CoreFoundation -framework OpenGL -framework GLUT -lobjc")
    set(PLAT_OBJ_FLAGS "-flat_namespace -undefined suppress -dynamic -bundle")

    if (RELEASE)
      set(CMAKE_OSX_ARCHITECTURES "ppc;i386;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
    endif(RELEASE)
  else(APPLE)
    set(LUBYK_LIB /usr/local/lib/lubyk)
    set(LINUX true)
    set(PLAT "linux")
    add_definitions(-D__LINUX_ALSASEQ__)
    #set(PLAT_LINK "-lavahi-client -lrt -lreadline -lasound")
    set(PLAT_LINK "rt")
    set(PLAT_OBJ_FLAGS "-dynamic")
  endif(APPLE)
else(UNIX)
  if(WIN32)
    set(PLAT "win32")
  else(WIN32)
    set(PLAT "unsupported")
  endif(WIN32)
endif(UNIX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fPIC")
if (RELEASE)
  add_definitions(-O2 -DNDEBUG)
endif(RELEASE)
add_definitions(-D__${PLAT}__ -DUSE_READLINE)

# ==============================================================================
#
#  Build all modules in 'modules'
#
# ==============================================================================
#add_custom_target(modules ALL)
add_custom_target(modules)
add_subdirectory(modules)

# ==============================================================================
#
#  Build lua binary (not mandatory: used to have debug info)
#
# ==============================================================================
# we do not shipt with a lua binary yet
#add_subdirectory(lua)

# ==============================================================================
#
#  Build Mac OS X dmg
#
# ==============================================================================
add_custom_target(dmg
  COMMAND hdiutil create -srcfolder ${LUBYK_SOURCE_DIR}/deploy/macosx/Lubyk_dmg -format UDZO -volname 'Install Lubyk' ${LUBYK_SOURCE_DIR}/deploy/macosx/Lubyk.dmg
  )

# ==============================================================================
#
#  Test all
#
# ==============================================================================
add_custom_target(test_all cd .. && ./ltest)
add_dependencies(test_all modules)


message (STATUS "===========================================================================")
message (STATUS "   LUBYK build configuration settings                                      ")
message (STATUS "===========================================================================")
message (STATUS "   Target folder     = ${LUBYK_LIB}")
message (STATUS "   Platform          = ${PLAT}")
message (STATUS "   Release           = ${RELEASE}")
if(QT_SRC_PATH STREQUAL "")
message (STATUS "   QT_SRC_PATH       (build using Qt frameworks)")
else(QT_SRC_PATH STREQUAL "")
message (STATUS "   QT_SRC_PATH       = ${QT_SRC_PATH}")
endif(QT_SRC_PATH STREQUAL "")
message (STATUS "   Type: 'ccmake <path to lubyk>' to change settings")
message (STATUS "===========================================================================")
