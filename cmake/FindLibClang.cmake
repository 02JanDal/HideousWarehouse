find_path(LIBCLANG_INCLUDE_DIR Index.h PATH_SUFFIXES clang-c)
find_library(LIBCLANG_LIBRARY NAMES clang)

set(LibClang_LIBRARIES ${LIBCLANG_LIBRARY})
set(LibClang_INCLUDE_DIRS ${LIBCLANG_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibClang DEFAULT_MSG LIBCLANG_LIBRARY LIBCLANG_INCLUDE_DIR)

mark_as_advanced(LIBCLANG_LIBRARY LIBCLANG_INCLUDE_DIR)
