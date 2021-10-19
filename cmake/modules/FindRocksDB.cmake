# Find the RocksDB libraries
#
# The following variables are optionally searched for defaults
#  ROCKSDB_ROOT_DIR:    Base directory where all RocksDB components are found
#
# The following are set after configuration is done:
#  ROCKSDB_FOUND
#  RocksDB_INCLUDE_DIR
#  RocksDB_LIBRARIES

find_path(RocksDB_INCLUDE_DIR NAMES rocksdb/db.h
                             PATHS ${ROCKSDB_ROOT_DIR} ${ROCKSDB_ROOT_DIR}/include)

find_library(RocksDB_LIBRARIES NAMES rocksdb
                              PATHS ${ROCKSDB_ROOT_DIR} ${ROCKSDB_ROOT_DIR}/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RocksDB DEFAULT_MSG RocksDB_INCLUDE_DIR RocksDB_LIBRARIES)

if(ROCKSDB_FOUND)
    message(STATUS "Found RocksDB  (include: ${RocksDB_INCLUDE_DIR}, library: ${RocksDB_LIBRARIES})")
    mark_as_advanced(RocksDB_INCLUDE_DIR RocksDB_LIBRARIES)

    add_library(sc_machine_rocksdb UNKNOWN IMPORTED)
    set_target_properties(sc_machine_rocksdb PROPERTIES
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
        IMPORTED_LOCATION "${RocksDB_LIBRARIES}"
        INTERFACE_INCLUDE_DIRECTORIES "${RocksDB_INCLUDE_DIR}"
    )
endif()
