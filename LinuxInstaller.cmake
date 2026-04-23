# Define installation path.
set(CPACK_SET_DESTDIR "on")
set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/conducteo")

# Define CPak options.
set(CPACK_BINARY_DEB "ON")
set(CPACK_GENERATOR "DEB")
set(CPACK_BINARY_DEB "1")

# Define package options.
set(CPACK_DEBIAN_PACKAGE_NAME "conducteo")
set(CPACK_PACKAGE_DESCRIPTION "Logiciel de calcul des ponts thermiques linéiques")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Logiciel conducteö")
set(CPACK_PACKAGE_VERSION_MAJOR "${CONDUCTEO_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CONDUCTEO_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${CONDUCTEO_VERSION_PATCH}")

# Define package architecture.
set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

# Define package maintainer.
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "C. Marcel - NWANDA <clement.marcel@nwanda.fr>")
set(CPACK_PACKAGE_VENDOR "C. Marcel - NWANDA <clement.marcel@nwanda.fr>")
set(CPACK_PACKAGE_CONTACT "C. Marcel - NWANDA <clement.marcel@nwanda.fr>")

# Define package section.
set(CPACK_DEBIAN_PACKAGE_SECTION "web")

# Define package priority.
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# Package output name.
set(CPACK_PACKAGE_FILE_NAME "conducteo-${CONDUCTEO_VERSION_MAJOR}.${CONDUCTEO_VERSION_MINOR}.${CONDUCTEO_VERSION_PATCH}")

# Strip binaries.
set(CMAKE_INSTALL_DO_STRIP "TRUE")
set(CPACK_STRIP_FILES "1")

# Package dependencies.
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5gui5, libqt5printsupport5", "libqt5xml5", "libzip4")

# conducteo.
install(TARGETS conducteo RUNTIME DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX})

# Documentation.
install(FILES ${CMAKE_SOURCE_DIR}/install/documentation.pdf DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX})
install(FILES ${CMAKE_SOURCE_DIR}/install/tutorial.pdf DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX})
install(FILES ${CMAKE_SOURCE_DIR}/install/quickstart.pdf DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX})

# Validation.
install(FILES ${CMAKE_SOURCE_DIR}/install/validations.pdf DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX})

file(GLOB_RECURSE validations_data_en10211 "${CMAKE_SOURCE_DIR}/validations/data/en10211/*.c2d")
file(GLOB_RECURSE validations_data_common  "${CMAKE_SOURCE_DIR}/validations/data/common/*.c2d")
install(FILES ${validations_data_en10211} DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/exemples)
install(FILES ${validations_data_common} DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/exemples)

file(GLOB_RECURSE validations_data_en10077 "${CMAKE_SOURCE_DIR}/validations/data/en10077/*.c2d")
install(FILES ${validations_data_en10077} DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/exemples)

file(GLOB_RECURSE tutorial_data "${CMAKE_SOURCE_DIR}/documentation/tutorial/*.c2d")
install(FILES ${tutorial_data} DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/exemples)

# Conducteo databases.
install(FILES ${CMAKE_SOURCE_DIR}/install/rt2012.db DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX})

# Examples.
file(GLOB_RECURSE c2d_examples "${CMAKE_SOURCE_DIR}/install/exemples/*.c2d")
install(FILES ${c2d_examples} DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/exemples)

# Docx template.
install(FILES ${CMAKE_SOURCE_DIR}/install/model_fr.docx DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/docx)
install(FILES ${CMAKE_SOURCE_DIR}/install/model_2_fr.docx DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/docx)

install(FILES ${CMAKE_SOURCE_DIR}/install/model_en.docx DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/docx)
install(FILES ${CMAKE_SOURCE_DIR}/install/model_2_en.docx DESTINATION ${CPACK_PACKAGING_INSTALL_PREFIX}/docx)

include (CPack)
