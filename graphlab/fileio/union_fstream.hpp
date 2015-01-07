/**
 * Copyright (C) 2015 Dato, Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#ifndef FILEIO_UNION_FSTREAM_HPP
#define FILEIO_UNION_FSTREAM_HPP
#include <iostream>

namespace graphlab {

/**
 * A simple union of std::fstream and graphlab::hdfs::fstream.
 * Also performs S3 downloading, uploading, and Curl downloading automatically.
 */
class union_fstream {

 public:
  enum stream_type {HDFS, STD, CACHE};
  /**
   * Constructs a union fstream from a filename. Based on the filename
   * (whether it begins with hdfs://, or cache://)
   * an appropriate stream type (HDFS, STD, or CACHE) is created.
   *
   * If the the filename begins with s3:// it downloads the file into local if opened as
   * input stream. For output stream, it will upload to s3 on close.
   *
   * Throws an std::io_base_failure exception if fail to construct the stream.
   */
  union_fstream(std::string url,
                std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out,
                std::string proxy="");

  /// Destructor
  ~union_fstream();

  /// Returns the current stream type. Whether it is a HDFS, STD, or cache stream
  stream_type get_type() const;

  std::istream* get_istream();

  std::ostream* get_ostream();

  // /**
  //  * Returns a pointer to a standard stream if the current stream type
  //  * is a standard stream. Assertion failure otherwise.
  //  */
  // std::fstream* get_std_fstream();

  // /**
  //  * \overload
  //  */
  // const std::fstream* get_std_fstream() const;

  // /**
  //  * Returns a pointer to a hdfs stream if the current stream type
  //  * is a standard stream. Assertion failure otherwise.
  //  */
  // graphlab::hdfs::fstream* get_hdfs_fstream();

  // /**
  //  * \overload
  //  */
  // const graphlab::hdfs::fstream* get_hdfs_fstream() const;

  /**
   * Returns the filename used to construct the union_fstream
   */
  std::string get_name() const;

  /**
   * Register a callback function that will be called on close.
   */
  void register_close_callback(std::function<void()> fn);

  /**
   * Closes the file stream. Finish all registered callback functions.
   *
   * Exception happend during callback functions are propagated.
   */
  void close();

  /**
   * Returns the file size of the opened file. 
   * Returns (size_t)(-1) if there is no file opened, or if there is an
   * error obtaining the file size.
   */
  size_t file_size();

  /**
   * Returns the number of bytes read from the file so far.
   * Note that due to buffering, this may be an overestimate of the current
   * file position.
   */
  size_t get_bytes_read();

  /**
   * Returns the number of bytes written to the file so far.
   * Note that due to buffering, this may be an underestimate of the current
   * file position.
   */
  size_t get_bytes_written();

 private:
  union_fstream(const union_fstream& other) = delete;

 private:

  stream_type type;
  std::string localfile;

  std::istream* input_stream;
  std::ostream* output_stream;
  std::function<void()> close_callback;
};

} // namespace graphlab
#endif
