project(dolphin-dropbox-plugin)

FIND_PACKAGE( KDE4 REQUIRED )
#FIND_PACKAGE(LibKonq REQUIRED)
INCLUDE( KDE4Defaults )

include_directories( ${QT_INCLUDES} ${KDE4_INCLUDES} ${LIBKONQ_INCLUDE_DIR} )
add_subdirectory(i18n)

add_subdirectory(QJson)

kde4_add_plugin(dolphin-dropbox-plugin  src/dolphin-dropbox-plugin.cpp)
target_link_libraries(dolphin-dropbox-plugin qjson ${KDE4_KIO_LIBS})

install(FILES dolphin-dropbox-plugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(TARGETS dolphin-dropbox-plugin DESTINATION ${PLUGIN_INSTALL_DIR})
