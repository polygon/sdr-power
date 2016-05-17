# - Find AIRSPY
# Find the native AIRSPY includes and library
# This module defines
#  AIRSPY_INCLUDE_DIR, where to find airspy.h, etc.
#  AIRSPY_LIBRARIES, the libraries needed to use AIRSPY.
#  AIRSPY_FOUND, If false, do not try to use AIRSPY.
# also defined, but not for general use are
#  AIRSPY_LIBRARY, where to find the AIRSPY library.

FIND_PATH(AIRSPY_INCLUDE_DIR airspy.h
        ${AIRSPY_DIR}/include/libairspy
        /usr/pkgs64/include/libairspy
        /usr/include/libairspy
        /usr/local/include/libairspy
)

FIND_LIBRARY(AIRSPY_LIBRARY
        NAMES airspy
        PATHS ${AIRSPY_DIR}/lib
        "${AIRSPY_DIR}\\win32\\lib"
        /usr/pkgs64/lib
        /usr/lib64
        /usr/lib
        /usr/local/lib
        NO_DEFAULT_PATH
)

IF (AIRSPY_LIBRARY AND AIRSPY_INCLUDE_DIR)
    SET(AIRSPY_LIBRARIES ${AIRSPY_LIBRARY})
    SET(AIRSPY_FOUND "YES")
ELSE (AIRSPY_LIBRARY AND AIRSPY_INCLUDE_DIR)
    SET(AIRSPY_FOUND "NO")
ENDIF (AIRSPY_LIBRARY AND AIRSPY_INCLUDE_DIR)

IF (AIRSPY_FOUND)
    IF (NOT AIRSPY_FIND_QUIETLY)
        MESSAGE(STATUS "Found AIRSPY: ${AIRSPY_LIBRARIES}")
    ENDIF (NOT AIRSPY_FIND_QUIETLY)
ELSE (AIRSPY_FOUND)
    IF (AIRSPY_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find AIRSPY library")
    ENDIF (AIRSPY_FIND_REQUIRED)
ENDIF (AIRSPY_FOUND)

# Deprecated declarations.
GET_FILENAME_COMPONENT (NATIVE_AIRSPY_LIB_PATH ${AIRSPY_LIBRARY} PATH)

MARK_AS_ADVANCED(
        AIRSPY_LIBRARY
        AIRSPY_INCLUDE_DIR
)