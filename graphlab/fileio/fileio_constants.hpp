/**
 * Copyright (C) 2015 Dato, Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#ifndef GRAPHLAB_FILEIO_FILEIO_CONSTANTS_HPP
#define GRAPHLAB_FILEIO_FILEIO_CONSTANTS_HPP
#include <string>
namespace graphlab {
namespace fileio {

/**
 * The protocol prefix cache:// to identify a cached file.
 */
extern const std::string CACHE_PREFIX;

/**
 * The "directory" (cache://tmp/) which all cached files are located in 
 */
extern const std::string TMP_CACHE_PREFIX;

/**
 * The physical directory (/var/tmp) which all cached files are located in .
 * colon seperated.
 */
extern std::string CACHE_FILE_LOCATIONS;

/**
 * The initial memory capacity assigned to caches
 */
extern const size_t FILEIO_INITIAL_CAPACITY_PER_FILE;

/**
 * The maximum memory capacity assigned to a cached file until it has to 
 * be flushed.
 */
extern size_t FILEIO_MAXIMUM_CACHE_CAPACITY_PER_FILE;

/**
 * The maximum memory capacity used by all cached files.
 * be flushed.
 */
extern size_t FILEIO_MAXIMUM_CACHE_CAPACITY;

/**
 * The default fileio reader buffer size
 */
extern size_t FILEIO_READER_BUFFER_SIZE; 

/**
 * The default fileio writer buffer size
 */
extern size_t FILEIO_WRITER_BUFFER_SIZE;
}
}
#endif
