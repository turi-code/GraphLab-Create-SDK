/**
 * Copyright (C) 2014 GraphLab Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#ifndef GRAPHLAB_FLEXIBLE_TYPE_STRING_ESCAPE_HPP
#define GRAPHLAB_FLEXIBLE_TYPE_STRING_ESCAPE_HPP
#include <string>
namespace graphlab {

/**
 * Take the second character of an escaped character and generate its
 * matching unescaped character. For instance, given "n" this returns "\n";
 */
inline static std::string unescape_char(char c) noexcept {
  switch(c) {
   case '\'':
     return std::string("\'");
   case '\"':
     return std::string("\"");
   case '\\':
     return std::string("\\");
   case '/':
     return std::string("/");
   case 't':
     return std::string("\t");
   case 'b':
     return std::string("\b");
   case 'r':
     return std::string("\r");
   case 'n':
     return std::string("\n");
   case '0':
     return std::string("\0");
   default:
     return std::string("\\") + c;
  }
}

/**
 * Escapes a string from val into output.
 * The length of the output string is in returned in output_len.
 * Note that output.length() may be greater than the output_len.
 *
 * \param val The string to escape
 * \param escape_char The escape character to use (recommended '\\')
 * \param quote_char The quote character to use. (recommended '\"')
 * \param use_quote_char If the output string should be quoted
 * \param double_quote If double quotes are converted to single quotes.
 */
void escape_string(const std::string& val, 
                   char escape_char,
                   char quote_char,
                   bool use_quote_char,
                   bool double_quote,
                   std::string& output, 
                   size_t& output_len);
} // namespace graphlab

#endif
