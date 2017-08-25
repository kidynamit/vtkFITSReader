# - Find CFitsIO
# Find the native CFitsIO headers and libraries.
#
#  CFITSIO_INCLUDE_DIRS - where to find fitsio.h, etc.
#  CFITSIO_LIBRARIES    - List of libraries when using CFitsIO.
#  CFITSIO_FOUND        - True if CFitsIO found.
#  CFITSIO_VERSION      - Version of the CFitsIO if found

IF (DEFINED ENV{CFITSIO_ROOT_DIR})
    SET(CFITSIO_ROOT_DIR "$ENV{CFITSIO_ROOT_DIR}")
ENDIF()
SET(CFITSIO_ROOT_DIR
    "${CFITSIO_ROOT_DIR}"
    CACHE
    PATH
    "Root directory to search for CFITSIO")

# Look for the header file.
FIND_PATH(CFITSIO_INCLUDE_DIRS NAMES fitsio.h HINTS 
	${CFITSIO_ROOT_DIR}/include )

# Determine architecture
IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
	IF(MSVC)
		SET(_CFITSIO_LIB_ARCH "x64")
	ENDIF()
ELSE()
	IF(MSVC)
		SET(_CFITSIO_LIB_ARCH "Win32")
	ENDIF()
ENDIF()

# Look for the library.
FIND_LIBRARY(CFITSIO_LIBRARY NAMES cfitsio HINTS ${CFITSIO_ROOT_DIR} 
                                                      ${CFITSIO_ROOT_DIR}/lib
                                                    )
MARK_AS_ADVANCED(CFITSIO_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set CFITSIO_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CFITSIO DEFAULT_MSG CFITSIO_LIBRARIES CFITSIO_INCLUDE_DIRS)

MARK_AS_ADVANCED(CFITSIO_LIBRARIES CFITSIO_INCLUDE_DIRS)