# Binary.
install(TARGETS conducteo RUNTIME DESTINATION bin)

# Data.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/install/data/
        DESTINATION share/conducteo/data
        OPTIONAL)
        
# Documentation.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/install/documentation/
        DESTINATION share/doc/conducteo
        OPTIONAL)
        
install(DIRECTORY ${CMAKE_SOURCE_DIR}/install/documentation/
        DESTINATION share/conducteo/docs
        OPTIONAL)
        
# Examples.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/install/examples/
        DESTINATION share/conducteo/examples
        OPTIONAL)
        
# Translations.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/install/translations/
        DESTINATION share/conducteo/translations
        OPTIONAL)

# Templates.
install(DIRECTORY ${CMAKE_SOURCE_DIR}/install/templates/
        DESTINATION share/conducteo/templates
        OPTIONAL)
        
# Validations.
file(GLOB_RECURSE validations_data_en10211 "${CMAKE_SOURCE_DIR}/validations/data/en10211/*.c2d")
file(GLOB_RECURSE validations_data_common  "${CMAKE_SOURCE_DIR}/validations/data/common/*.c2d")
file(GLOB_RECURSE validations_data_en10077 "${CMAKE_SOURCE_DIR}/validations/data/en10077/*.c2d")
install(FILES ${validations_data_en10211} DESTINATION share/conducteo/examples OPTIONAL)
install(FILES ${validations_data_common} DESTINATION share/conducteo/examples OPTIONAL)
install(FILES ${validations_data_en10077} DESTINATION share/conducteo/examples OPTIONAL)

# Tutorial.
file(GLOB_RECURSE tutorial_data "${CMAKE_SOURCE_DIR}/documentation/tutorial/*.c2d")
install(FILES ${tutorial_data} DESTINATION share/conducteo/examples OPTIONAL)

# Link.
install(FILES ${CMAKE_SOURCE_DIR}/debian/conducteo.desktop
        DESTINATION share/applications)
        
# Icon.
install(FILES ${CMAKE_SOURCE_DIR}/debian/conducteo.png
        DESTINATION share/icons/hicolor/128x128/apps)
        
# Mime type.
install(FILES ${CMAKE_SOURCE_DIR}/debian/conducteo.xml
        DESTINATION share/mime/packages)
        
