/**
 * Copyright (C) 2014 GraphLab Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#ifndef GRAPHLAB_FLEXIBLE_TYPE_STRING_PARSER_HPP
#define GRAPHLAB_FLEXIBLE_TYPE_STRING_PARSER_HPP
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/qi.hpp>
#include <graphlab/flexible_type/string_escape.hpp>

/*
 * Must of this is obtained from 
 * http://boost-spirit.com/home/articles/qi-example/creating-your-own-parser-component-for-spirit-qi/
 */

namespace parser_impl { 

/*
 * \internal
 * The string parsing configuration.
 *
 */
struct parser_config {
  // If any of these character occurs outside of quoted string, 
  // the string will be terminated
  std::string restrictions;
  // The character to use for an escape character
  char escape_char = '\\';
  /* whether double quotes inside of a quote are treated as a single quote.
   * i.e. """hello""" => \"hello\"
   */
  char double_quote = true;
};

BOOST_SPIRIT_TERMINAL_EX(restricted_string); 

} // namespace parser_impl 

namespace boost { 
namespace spirit {


template <typename T1>
struct use_terminal<qi::domain, 
    terminal_ex<parser_impl::tag::restricted_string, fusion::vector1<T1> > >
  : mpl::true_ {};


} } // namespace spirit, boost

namespace parser_impl {

/*
 * \internal
 * This class defines a string parser which allows the parser writer to define
 * a list of characters which are not permitted in unquoted strings. Quoted 
 * strings have no restrictions on what characters they can contain.
 * Usage:
 * \code
 *   parser_impl::parser_config config;
 *   config.[set stuff up]
 *   rule = parser_impl::restricted_string(config);
 * \endcode
 */
struct string_parser
    : boost::spirit::qi::primitive_parser<string_parser> {
  // Define the attribute type exposed by this parser component
  template <typename Context, typename Iterator>
  struct attribute {
    typedef std::string type;
  };

  parser_config config;

  string_parser(){}
  string_parser(parser_config config):config(config) {}

  enum class tokenizer_state {
    START_FIELD, ESCAPED_CHAR, IN_FIELD, IN_QUOTED_FIELD,
    ESCAPE_IN_QUOTED_FIELD
  };
// insert a character into the field buffer. resizing it if necessary

  // This function is called during the actual parsing process
  template <typename Iterator, typename Context, typename Skipper, typename Attribute>
  bool parse(Iterator& first, Iterator const& last, 
             Context&, Skipper const& skipper, Attribute& attr) const {
    boost::spirit::qi::skip_over(first, last, skipper);
    Iterator cur = first;
    std::string ret;

    tokenizer_state state = tokenizer_state::START_FIELD; 
    bool keep_parsing = true;
    char quote_char = 0;
    while(keep_parsing && cur != last) {
      // Next character in file
      char c = *cur;
      if(state != tokenizer_state::IN_QUOTED_FIELD && 
         state != tokenizer_state::ESCAPE_IN_QUOTED_FIELD && 
         config.restrictions.find(c) != std::string::npos) break;
      ++cur;
      switch(state) {
       case tokenizer_state::START_FIELD:
         if (c == '\'' || c == '\"') {
           quote_char = c;
           state = tokenizer_state::IN_QUOTED_FIELD;
         } else if (c == config.escape_char) {
           /* possible escaped character */
           state = tokenizer_state::ESCAPED_CHAR;
         } else {
           /* begin new unquoted field */
           ret = ret + c;
           state = tokenizer_state::IN_FIELD;
         }
         break;

       case tokenizer_state::ESCAPED_CHAR:
         ret = ret + graphlab::unescape_char(c);
         state = tokenizer_state::IN_FIELD;
         break;

       case tokenizer_state::IN_FIELD:
         /* in unquoted field */
         if (c == config.escape_char) {
           /* possible escaped character */
           state = tokenizer_state::ESCAPED_CHAR;
         } else {
           /* normal character - save in field */
           ret = ret + c;
         }
         break;

       case tokenizer_state::IN_QUOTED_FIELD:
         /* in quoted field */
         if (c == config.escape_char) {
           /* Possible escape character */
           state = tokenizer_state::ESCAPE_IN_QUOTED_FIELD;
         }
         else if (c == quote_char) {
           if (c == '\"' && config.double_quote) {
             /* doublequote; " represented by "" */
             // look ahead one character
             if (cur + 1 < last && *cur == quote_char) {
               ret = ret + c;
               ++cur;
               break;
             }
           }
           // we are done.
           keep_parsing = false;
         }
         else {
           /* normal character - save in field */
           ret = ret + c;
         }
         break;

       case tokenizer_state::ESCAPE_IN_QUOTED_FIELD:
         ret = ret + graphlab::unescape_char(c);
         state = tokenizer_state::IN_QUOTED_FIELD;
         break;
      }
    }
    if (cur == first) return false;
    else {
      if (!quote_char) boost::algorithm::trim_right(ret);
      attr = ret;
      first = cur;
    }
    return true;
  }

// This function is called during error handling to create
// a human readable string for the error context.
  template <typename Context>
  boost::spirit::info what(Context&) const {
    return boost::spirit::info("string_parser");
  }
};
} // namespace parser_impl

namespace boost { 
namespace spirit { 
namespace qi {

// This is the factory function object invoked in order to create
// an instance of our iter_pos_parser.
template <typename Modifiers, typename T1>
struct make_primitive<terminal_ex<parser_impl::tag::restricted_string, fusion::vector1<T1>>, Modifiers> {
    typedef parser_impl::string_parser result_type;

    template <typename Terminal>
    result_type operator()(const Terminal& term, unused_type) const {
        return result_type(fusion::at_c<0>(term.args));
    }
};

}}} // namespace qi, spirit, boost

#endif
