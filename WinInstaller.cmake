# Windows installer.

# CPack/NSIS properties.
set(CPACK_ALL_INSTALL_TYPES Standard)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_NAME "conducteö")
set(CPACK_PACKAGE_VENDOR "C. Marcel <clement.marcel@nwanda.fr>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "conducteö software")
set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\resources\\\\icon.ico")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}\\\\resources\\\\license.txt"   )

set(CPACK_PACKAGE_VERSION_MAJOR "${CONDUCTEO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CONDUCTEO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CONDUCTEO_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION "${CONDUCTEO_VERSION_MAJOR}.${CONDUCTEO_VERSION_MINOR}")

set(CPACK_COMPONENT_APPLICATION_INSTALL_TYPES Standard)
set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME "conducteö")
set(CPACK_COMPONENT_APPLICATION_DESCRIPTION "Logiciel conducteö (calcul des ponts thermiques)")
set(CPACK_COMPONENT_APPLICATION_REQUIRED 1)

set(CPACK_PACKAGE_INSTALL_DIRECTORY "conducteo")
set(CPACK_NSIS_DISPLAY_NAME "conducteö")
set(CPACK_NSIS_MODIFY_PATH OFF)

# Conducteo executable.
install(TARGETS conducteo RUNTIME DESTINATION . COMPONENT application)

# Documentation.
install(FILES ${CMAKE_SOURCE_DIR}/install/documentation.pdf DESTINATION ./documentation COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/tutorial.pdf DESTINATION ./documentation COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/quickstart.pdf DESTINATION ./documentation COMPONENT application)

# Validation.
install(FILES ${CMAKE_SOURCE_DIR}/install/validations.pdf DESTINATION ./documentation COMPONENT application)

file(GLOB_RECURSE validations_data_en10211 "${CMAKE_SOURCE_DIR}/validations/data/en10211/*.c2d")
file(GLOB_RECURSE validations_data_common  "${CMAKE_SOURCE_DIR}/validations/data/common/*.c2d")
install(FILES ${validations_data_en10211} DESTINATION ./exemples COMPONENT application)
install(FILES ${validations_data_common} DESTINATION ./exemples COMPONENT application)

file(GLOB_RECURSE validations_data_en10077 "${CMAKE_SOURCE_DIR}/validations/data/en10077/*.c2d")
install(FILES ${validations_data_en10077} DESTINATION ./exemples COMPONENT application)

file(GLOB_RECURSE tutorial_data "${CMAKE_SOURCE_DIR}/documentation/tutorial/*.c2d")
install(FILES ${tutorial_data} DESTINATION ./exemples COMPONENT application)

# Conducteo databases.
install(FILES ${CMAKE_SOURCE_DIR}/install/rt2012.db DESTINATION . COMPONENT application)

# Examples.
file(GLOB_RECURSE c2d_examples "${CMAKE_SOURCE_DIR}/install/exemples/*.c2d")
install(FILES ${c2d_examples} DESTINATION ./exemples COMPONENT application)

# Docx template.
install(FILES ${CMAKE_SOURCE_DIR}/install/model_fr.docx DESTINATION ./docx COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/model_2_fr.docx DESTINATION ./docx COMPONENT application)

install(FILES ${CMAKE_SOURCE_DIR}/install/model_en.docx DESTINATION ./docx COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/model_2_en.docx DESTINATION ./docx COMPONENT application)

# HACK: set Qt bin/plugins directories.
set( QT_BINARY_DIR    "${Qt6Core_DIR}/../../../bin"     )
set( QT_PLUGINS_DIR   "${Qt6Core_DIR}/../../../plugins" )

# Qt dependencies.
install(FILES ${QT_BINARY_DIR}/Qt6Widgets.dll          DESTINATION . COMPONENT application)
install(FILES ${QT_BINARY_DIR}/Qt6Gui.dll              DESTINATION . COMPONENT application)
install(FILES ${QT_BINARY_DIR}/Qt6Core.dll             DESTINATION . COMPONENT application)
install(FILES ${QT_BINARY_DIR}/Qt6Network.dll          DESTINATION . COMPONENT application)
install(FILES ${QT_BINARY_DIR}/Qt6PrintSupport.dll     DESTINATION . COMPONENT application)
install(FILES ${QT_BINARY_DIR}/Qt6Svg.dll              DESTINATION . COMPONENT application)

install(FILES ${QT_PLUGINS_DIR}/platforms/qwindows.dll DESTINATION ./platforms COMPONENT application)

# images formats support.
install(FILES ${QT_PLUGINS_DIR}/imageformats/qgif.dll DESTINATION ./imageformats COMPONENT application)
install(FILES ${QT_PLUGINS_DIR}/imageformats/qico.dll DESTINATION ./imageformats COMPONENT application)
install(FILES ${QT_PLUGINS_DIR}/imageformats/qjpeg.dll DESTINATION ./imageformats COMPONENT application)
install(FILES ${QT_PLUGINS_DIR}/imageformats/qsvg.dll DESTINATION ./imageformats COMPONENT application)

# Microsoft compiler dependencies.
# Visual c++ 2017 64 bits.
install(FILES ${CMAKE_SOURCE_DIR}/install/windows/msvcp140.dll DESTINATION . COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/windows/msvcp140_1.dll DESTINATION . COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/windows/msvcp140_2.dll DESTINATION . COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/windows/vcruntime140.dll DESTINATION . COMPONENT application)
install(FILES ${CMAKE_SOURCE_DIR}/install/windows/vcruntime140_1.dll DESTINATION . COMPONENT application)

set(CPACK_ALL_INSTALL_TYPES                    Standard                                            )
set(CPACK_COMPONENT_APPLICATION_INSTALL_TYPES  Standard                                            )
set(CPACK_COMPONENT_APPLICATION_DISPLAY_NAME   "conducteö"                                         )
set(CPACK_COMPONENT_APPLICATION_DESCRIPTION    "Simulation des ponts thermiques linéiques"         )
set(CPACK_COMPONENT_APPLICATION_REQUIRED       1                                                   )
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE        1                                                   )
set(CPACK_PACKAGE_NAME                         "conducteo"                                         )
set(CPACK_PACKAGE_VENDOR                       "conducteo"                                         )
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY          "conducteo"                                         )
set(CPACK_PACKAGE_VERSION                      "${CONDUCTEO_VERSION_MAJOR}.${CONDUCTEO_VERSION_MINOR}.${CONDUCTEO_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR                ${CONDUCTEO_VERSION_MAJOR}                          )
set(CPACK_PACKAGE_VERSION_MINOR                ${CONDUCTEO_VERSION_MINOR}                          )
set(CPACK_PACKAGE_VERSION_PATCH                ${CONDUCTEO_VERSION_PATCH}                          )

set(CPACK_PACKAGE_INSTALL_DIRECTORY            "conducteo"                                       )
set(CPACK_NSIS_MENU_LINKS                      "conducteo.exe" 
											   "conducteö ${CONDUCTEO_VERSION_MAJOR}.${CONDUCTEO_VERSION_MINOR}"
											   "documentation/documentation.pdf"
											   "Manuel d'utilisation"                              )
set(CPACK_NSIS_MUI_ICON                        "${CMAKE_SOURCE_DIR}\\\\resources\\\\icon.ico"      )
set(CPACK_NSIS_DISPLAY_NAME                    "conducteö"                                         )
set(CPACK_NSIS_MODIFY_PATH                     OFF                                                 )

set(CPACK_NSIS_CONTACT                         "clement.marcel@nwanda.fr"                          )
set(CPACK_RESOURCE_FILE_LICENSE                "${CMAKE_SOURCE_DIR}\\\\resources\\\\license.txt"   )
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON                                                  )

 # c2d files association.
SET(CPACK_NSIS_DEFINES "!include ${CMAKE_SOURCE_DIR}\\\\FileAssociation.nsh \\n !include ${CMAKE_SOURCE_DIR}\\\\RefreshIcon.nsh ")
SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "\\\${registerExtension} \\\"$INSTDIR\\\\conducteo.exe\\\" \\\".c2d\\\" \\\"Fichier conducteö\\\" \\n Call RefreshShellIcons")

include(CPack)
