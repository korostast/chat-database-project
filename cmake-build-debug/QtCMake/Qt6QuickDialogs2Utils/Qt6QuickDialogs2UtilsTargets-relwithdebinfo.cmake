#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Qt6::QuickDialogs2Utils" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickDialogs2Utils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(Qt6::QuickDialogs2Utils PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libQt6QuickDialogs2Utils.a"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/Qt6QuickDialogs2Utils.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS Qt6::QuickDialogs2Utils )
list(APPEND _IMPORT_CHECK_FILES_FOR_Qt6::QuickDialogs2Utils "${_IMPORT_PREFIX}/lib/libQt6QuickDialogs2Utils.a" "${_IMPORT_PREFIX}/bin/Qt6QuickDialogs2Utils.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
