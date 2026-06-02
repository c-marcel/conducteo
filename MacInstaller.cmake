# Bundle icon.
set(bundle_icon "${CMAKE_SOURCE_DIR}/resources/conducteo.icns")
set_source_files_properties(${bundle_icon} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

# Documentation.
file (GLOB pdf_docs "${CMAKE_SOURCE_DIR}/install/documentation/documentation.pdf" )
set_source_files_properties(${pdf_docs} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/documentation)

file (GLOB pdf_docs_2 "${CMAKE_SOURCE_DIR}/install/documentation/validations.pdf" )
set_source_files_properties(${pdf_docs_2} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/documentation)

file (GLOB pdf_docs_3 "${CMAKE_SOURCE_DIR}/install/documentation/tutorial.pdf" )
set_source_files_properties(${pdf_docs_3} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/documentation)

file (GLOB pdf_docs_4 "${CMAKE_SOURCE_DIR}/install/documentation/quickstart.pdf" )
set_source_files_properties(${pdf_docs_4} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/documentation)

file(GLOB_RECURSE validations_data_en10211 "${CMAKE_SOURCE_DIR}/validations/data/en10211/*.c2d")
file(GLOB_RECURSE validations_data_common  "${CMAKE_SOURCE_DIR}/validations/data/common/*.c2d")

set_source_files_properties(${validations_data_en10211} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/examples)
set_source_files_properties(${validations_data_common} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/examples)

file(GLOB_RECURSE validations_data_en10077 "${CMAKE_SOURCE_DIR}/validations/data/en10077/*.c2d")
set_source_files_properties(${validations_data_en10077} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/examples)

file(GLOB_RECURSE tutorial_data "${CMAKE_SOURCE_DIR}/documentation/tutorial/*.c2d")
set_source_files_properties(${tutorial_data} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/examples)

# RT 2012 database.
file (GLOB rt_db "${CMAKE_SOURCE_DIR}/install/data/rt2012.db" )
set_source_files_properties(${rt_db} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/data)

# Docx template.
file (GLOB mac_docx_template "${CMAKE_SOURCE_DIR}/install/templates/model_fr.docx" )
set_source_files_properties(${mac_docx_template} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/docx)

file (GLOB mac_docx_template_2 "${CMAKE_SOURCE_DIR}/install/templates/model_2_fr.docx" )
set_source_files_properties(${mac_docx_template_2} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/docx)

file (GLOB mac_docx_template_3 "${CMAKE_SOURCE_DIR}/install/templates/model_en.docx" )
set_source_files_properties(${mac_docx_template_3} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/docx)

file (GLOB mac_docx_template_4 "${CMAKE_SOURCE_DIR}/install/templates/model_2_en.docx" )
set_source_files_properties(${mac_docx_template_4} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/docx)

file(GLOB mac_translations "${CMAKE_SOURCE_DIR}/install/translations/*.json")
set_source_files_properties(${mac_translations}
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources/translations
)

# Examples.
file (GLOB examples "${CMAKE_SOURCE_DIR}/install/examples/*.c2d" )
set_source_files_properties(${examples} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/examples)

set(MACOSXResources ${bundle_icon} ${pdf_docs} ${pdf_docs_2} ${pdf_docs_4} ${rt_db} ${mac_translations} ${examples} ${mac_docx_template} ${mac_docx_template_2} ${mac_docx_template_3} ${mac_docx_template_4} ${validations_data_en10211} ${validations_data_en10077} ${validations_data_common} ${tutorial_data} ${pdf_docs_3})

set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0")

# set bundle informations.
set( MACOSX_BUNDLE_INFO_STRING          "conducteö"                                                                                         )
set( MACOSX_BUNDLE_ICON_FILE            "conducteo.icns"                                                                                    )
set( MACOSX_BUNDLE_GUI_IDENTIFIER       "conducteo"    																						)
set( MACOSX_BUNDLE_LONG_VERSION_STRING  "${CONDUCTEO_VERSION_MAJOR}.${CONDUCTEO_VERSION_MINOR}.${CONDUCTEO_VERSION_PATCH}"                  )
set( MACOSX_BUNDLE_BUNDLE_NAME          "conducteö"                                                                                         )
set( MACOSX_BUNDLE_SHORT_VERSION_STRING "${CONDUCTEO_VERSION_MAJOR}.${CONDUCTEO_VERSION_MINOR}"                                             )
set( MACOSX_BUNDLE_BUNDLE_VERSION       "${CONDUCTEO_VERSION_MAJOR}.${CONDUCTEO_VERSION_MINOR}.${CONDUCTEO_VERSION_PATCH}"                  )
set( MACOSX_BUNDLE_COPYRIGHT            "C. MARCEL - 2009-2019"                                                                             )
	