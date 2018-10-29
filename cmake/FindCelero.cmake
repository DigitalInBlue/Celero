# Try to find Celero headers and libraries.
#
# Usage of this module as follows:
#
#     find_package(Celero)
#
# Variables used by this module, they can change the default behaviour and need
# to be set before calling find_package:
#
#  CELERO_PREFIX         Set this variable to the root installation of
#                        celero if the module has problems finding the
#                        proper installation path.
#
# Note that you can set the CELERO_PREFIX variable as environment variable.
#
# Variables defined by this module:
#
#  CELERO_FOUND              System has the Celero library and headers
#  CELERO_LIBRARIES          The Celero library
#  CELERO_INCLUDE_DIRS       The location of Celero headers

if(NOT CELRO_PREFIX)
	set(CELERO_PREFIX $ENV{CELERO_PREFIX})
endif()

set(CELERO_HINTS ${CELERO_PREFIX} $ENV{CELERO_PREFIX})
find_path(CELERO_ROOT
  NAMES include/celero/Celero.h
	HINTS "${CELERO_HINTS}"
)

find_library(CELERO_LIBRARIES
    NAMES libcelero.so
    HINTS ${CELERO_ROOT}/lib ${CELERO_ROOT}/lib64 
)

find_path(CELERO_INCLUDE_DIRS
    NAMES celero/Celero.h
    HINTS ${CELERO_ROOT}/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CELERO DEFAULT_MSG
    CELERO_LIBRARIES
    CELERO_INCLUDE_DIRS
)

mark_as_advanced(
    CELERO_LIBRARIES
    CELERO_INCLUDE_DIRS
    CELERO_ROOT
)
