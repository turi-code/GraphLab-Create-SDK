/**
 * Copyright (C) 2014 GraphLab Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#ifndef GRAPHLAB_UNITY_FLEXIBLE_TYPE_BASE_TYPES_HPP
#define GRAPHLAB_UNITY_FLEXIBLE_TYPE_BASE_TYPES_HPP
#include <string>
#include <vector>
#include <cstdint>
#include <graphlab/serialization/serialization_includes.hpp>
#include <graphlab/image/image_type.hpp>

namespace graphlab {


class flexible_type;
/**
 * \defgroup group_flexible_type_types Flexible Type Content Types
 * \ingroup group_gl_flexible_type
 * \{
 */
/**
 * A 64 bit integer type that can be stored in a flexible_type. 
 * Corresponds to the type enum \ref flex_type_enum::INTEGER
 */
typedef int64_t flex_int;
/**
 * A 64 bit double type that can be stored in a flexible_type.
 * Corresponds to the type enum \ref flex_type_enum::FLOAT.
 */
typedef double flex_float;
/**
 * A string that can be stored in a flexible_type.
 * Corresponds to the type enum \ref flex_type_enum::STRING.
 */
typedef std::string flex_string;
/**
 * A vector<double> that can be stored in a flexible_type.
 * Corresponds to the type enum \ref flex_type_enum::VECTOR.
 */
typedef std::vector<double> flex_vec;
/**
 * A recursive object vector<flexible_type> that can be stored in a flexible_type.
 * Corresponds to the type enum \ref flex_type_enum::LIST.
 */
typedef std::vector<flexible_type> flex_list;
/**
 * An recursive associate dictionary object vector<std::pair<flexible_type, flexible_type> >
 * Corresponds to the type enum \ref flex_type_enum::DICT.
 */
typedef std::vector<std::pair<flexible_type, flexible_type>> flex_dict;
/**
 * An \ref image_type "image type" that can be stored in a flexible_type.
 * Corresponds to the type enum \ref flex_type_enum::IMAGE. 
 */
typedef image_type flex_image;

/**
 * A date_time object that can be stored in a flexible_type.  
 * Corresponds to the type enum \ref flex_type_enum::DATETIME
 * flex_date_time.first is utc timestamp and flex_date_time.second is the
 * timezone offset. 
 */
struct flex_date_time { 
  flex_date_time() = default;
  /**
   * Constructs a flex_date_time object from a posix timestamp, and and time zone offset.
   * \param _posix_timestamp Timestamp value since 1st Jan 1970
   * \param _half_hour_offset Additional offset for timezone. In integral increments
   *                          of half-hour.
   */
  explicit inline flex_date_time(int64_t _posix_timestamp, int32_t _half_hour_offset = 0) { 
    first = _posix_timestamp;
    second = _half_hour_offset;
  }

  int64_t first : 56;
  int8_t second : 8;

  /**
   * Returns the timestamp value.
   */
  inline int64_t posix_timestamp() const {
    return first;
  }

  /**
   * Returns the time zone value in integral increments of half-hour.
   */
  inline int32_t time_zone_offset() const {
    return second;
  }
};

/**
 * Represents the undefined type in the flexible_type.
 * Corresponds to the type enum \ref flex_type_enum::UNDEFINED.
 *
 * The global static variable \ref FLEX_UNDEFINED can be used when an undefined
 * value is needed. i.e.
 * \code
 * flexible_type f = FLEX_UNDEFINED;
 * \endcode
 * This is equivalent to:
 * \code
 * flexible_type f = flex_undefined();
 * \endcode
 */
struct flex_undefined {
  inline void save(oarchive& oarc) const { }
  inline void load(iarchive& iarc) { }
};

/// \}

/**
 * \ingroup group_gl_flexible_type
 * An enumeration over the possible types the flexible type can contain.
 *
 * \note Renumber this at your own risk. 
 *
 * \see flex_type_enum_from_name
 * \see flex_type_enum_to_name
 */
enum class flex_type_enum: char {
  INTEGER = 0,  /**< Integer type. Stored type is the \ref flex_int */
  FLOAT = 1,    /**< 64-bit floating point type. Stored type is the \ref flex_float */
  STRING = 2,   /**< String type. Stored type is the \ref flex_string */
  VECTOR = 3,   /**< Numeric vector type. Stored type is the \ref flex_vec */
  LIST = 4,     /**< Recursive List type. Stored type is the \ref flex_list */
  DICT = 5,     /**< Recursive Dictionary type. Stored type is the \ref flex_dict */
  DATETIME = 6, /**< Date-Time type. Stored type is the \ref flex_date_time */
  UNDEFINED = 7,/**< Undefined / Missing Value type. Stored type is the \ref flex_undefined */
  IMAGE= 8      /**< Image type. Stored type is the \ref flex_image */
  // types above 127 are reserved
};

/**
 * \ingroup group_gl_flexible_type
 * Check if one flexible type is convertable to the other.
 */
inline bool flex_type_is_convertible(flex_type_enum from, flex_type_enum to) {
  static constexpr bool castable[9][9] =
        // int flt str vec rec dic dtime undef img
/*int*/  {{1,  1,  1,  0,  0,  0,  1,  0,  0},  // integer casts to self, float and string
/*flt*/   {1,  1,  1,  0,  0,  0,  1,  0,  0},  // float casts to integer, self, string
/*str*/   {0,  0,  1,  0,  0,  0,  0,  0,  0},  // string casts to string only
/*vec*/   {0,  0,  1,  1,  1,  0,  0,  0,  0},  // vector casts to string and self and recursive
/*rec*/   {0,  0,  1,  0,  1,  0,  0,  0,  0},  // recursive casts to string and self
/*dic*/   {0,  0,  1,  0,  0,  1,  0,  0,  0},  // dict casts to self
/*dtime*/ {1,  1,  1,  0,  0,  0,  1,  0,  0},  // dtime casts to string and self
/*undef*/ {0,  0,  1,  0,  0,  0,  0,  1,  0},  //UNDEFINED casts to string and UNDEFINED 
/*img*/   {0,  0,  1,  1,  0,  0,  0,  0,  1}}; // img casts to string, vec, and self.
  return castable[static_cast<int>(from)][static_cast<int>(to)];
}


/**
 * \ingroup group_gl_flexible_type
 * Check if numeric operators between these types are supported.
 * if a and b are flexible_types and
 * flex_type_has_binary_op(a.get_type(), b.get_type(), op) is true, this means.
 * that a op= b and a op b is false. op is any of '+','-','*','/','%','<','>','='
 *
 * '<' is the less than operator, and '>' is the greater than operator.
 * '=' is the equality comparison operator.
 * Suceeding on '<' or '>' implies that the less than or equal operator (<=)
 * and the greater than or equal operator (>=) will also succeed
 *
 * For instance given flexible_type a, b
 * \code
 * if (flex_type_has_binary_op(a, b, '+')) {
 *   a += b // will succeed
 *   a + b  // will succeed
 * } else {
 *   a += b // will fail
 *   a + b  // will fail
 * }
 * \endcode
 */
inline bool flex_type_has_binary_op(flex_type_enum left,
                                    flex_type_enum right,
                                    char op) {
  static constexpr bool plus_operator[9][9] =     // '+' operator
        // int flt str vec rec cel dtime undef img
/*int*/  {{1,  1,  0,  0,  0,  0,  0,  0,  0},
/*flt*/   {1,  1,  0,  0,  0,  0,  0,  0,  0},
/*str*/   {0,  0,  1,  0,  0,  0,  0,  0,  0},
/*vec*/   {1,  1,  0,  1,  0,  0,  0,  0,  0},
/*rec*/   {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*cel*/   {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*dtime*/ {1,  0,  0,  0,  0,  0,  0,  0,  0},
/*undef*/ {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*img*/   {0,  0,  0,  0,  0,  0,  0,  0,  0}};

  static constexpr bool other_numeric_operators[9][9] =  // '-','*','/','%' operator
        // int flt str vec rec cel dtime undef img
/*int*/  {{1,  1,  0,  0,  0,  0,  0,  0,  0},
/*flt*/   {1,  1,  0,  0,  0,  0,  0,  0,  0},
/*str*/   {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*vec*/   {1,  1,  0,  1,  0,  0,  0,  0,  0},
/*rec*/   {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*cel*/   {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*dtime*/ {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*undef*/ {0,  0,  0,  0,  0,  0,  0,  0,  0},
/*img*/   {0,  0,  0,  0,  0,  0,  0,  0,  0}};

  static constexpr bool comparison_operators[9][9] =  // < or > operator
        // int flt str vec rec cel dtime undef img
/*int*/  {{1,  1,  0,  0,  0,  0,  1,  0},
/*flt*/   {1,  1,  0,  0,  0,  0,  0,  0},
/*str*/   {0,  0,  1,  0,  0,  0,  0,  0},
/*vec*/   {0,  0,  0,  0,  0,  0,  0,  0},
/*rec*/   {0,  0,  0,  0,  0,  0,  0,  0},
/*cel*/   {0,  0,  0,  0,  0,  0,  0,  0},
/*dtime*/ {1,  0,  0,  0,  0,  0,  1,  0},
/*undef*/ {0,  0,  0,  0,  0,  0,  0,  0},
/*img*/   {0,  0,  0,  0,  0,  0,  0,  0}};

  static constexpr bool equality_operators[9][9] =  // = operator
        // int flt str vec rec cel dtime undef img
/*int*/  {{1,  1,  0,  0,  0,  0,  1,  1, 0},
/*flt*/   {1,  1,  0,  0,  0,  0,  0,  1, 0},
/*str*/   {0,  0,  1,  0,  0,  0,  0,  1, 0},
/*vec*/   {0,  0,  0,  1,  0,  0,  0,  1, 0},
/*rec*/   {0,  0,  0,  0,  0,  0,  0,  1, 0},
/*cel*/   {0,  0,  0,  0,  0,  0,  0,  1, 0},
/*dtime*/ {1,  0,  0,  0,  0,  0,  1,  1, 0},
/*undef*/ {1,  1,  1,  1,  1,  1,  1,  1, 1},
/*img*/   {0,  0,  0,  0,  0,  0,  0,  1, 0}};
  switch(op) {
   case '+':
    return plus_operator[static_cast<int>(left)][static_cast<int>(right)];
   case '-':
   case '*':
   case '/':
   case '%':
    return other_numeric_operators[static_cast<int>(left)][static_cast<int>(right)];
   case '<':
   case '>':
    return comparison_operators[static_cast<int>(left)][static_cast<int>(right)];
   case '=':
    return equality_operators[static_cast<int>(left)][static_cast<int>(right)];
   default:
    return false;
  }
}


/**
 * \ingroup group_gl_flexible_type
 * Identifies the flex_type_enum representing the type,
 * All other non-matching types will return UNDEFINED.
 *
 * type_to_enum<T>::value is the corresponding enumeration value in \ref
 * flex_type_enum if T matches \b exactly one of the flexible_type content 
 * types. type_to_enum<T>::value is flex_type_enum::UNDEFINED otherwise.
 *
 * This is very useful for SFINAE operations.
 *
 * Example:
 * The following lines will both return flex_type_enum::INTEGER
 * \code
 * (flex_type_enum) type_to_enum<flex_int>
 * type_to_enum<flex_int>::value
 * \endcode
 */
template <typename T>
struct type_to_enum {
  static constexpr flex_type_enum value = flex_type_enum::UNDEFINED;
};

template <>
struct type_to_enum<flex_int> {
  static constexpr flex_type_enum value = flex_type_enum::INTEGER;
  constexpr operator flex_type_enum() const { return value; }
};

template <>
struct type_to_enum<flex_date_time> {
  static constexpr flex_type_enum value = flex_type_enum::DATETIME;
  constexpr operator flex_type_enum() const { return value; }
};

template <>
struct type_to_enum<flex_float> {
  static constexpr flex_type_enum value = flex_type_enum::FLOAT;
  constexpr operator flex_type_enum() const { return value; }
};

template <>
struct type_to_enum<flex_string> {
  static constexpr flex_type_enum value = flex_type_enum::STRING;
  constexpr operator flex_type_enum() const { return value; }
};

template <>
struct type_to_enum<flex_vec> {
  static constexpr flex_type_enum value = flex_type_enum::VECTOR;
  constexpr operator flex_type_enum() const { return value; }
};


template <>
struct type_to_enum<flex_list> {
  static constexpr flex_type_enum value = flex_type_enum::LIST;
  constexpr operator flex_type_enum() const { return value; }
};

template <>
struct type_to_enum<flex_dict> {
  static constexpr flex_type_enum value = flex_type_enum::DICT;
  constexpr operator flex_type_enum() const { return value; }
};

template <>
struct type_to_enum<flex_image> {
  static constexpr flex_type_enum value = flex_type_enum::IMAGE;
  constexpr operator flex_type_enum() const { return value; }
};

template <>
struct type_to_enum<flex_undefined> {
  static constexpr flex_type_enum value = flex_type_enum::UNDEFINED;
  constexpr operator flex_type_enum() const { return value; }
};


/**
 * \ingroup group_gl_flexible_type
 *
 * is_valid_flex_type<T>::value is true if T is type which matches \b exactly
 * one of the flexible_type content types. false otherwise. 
 *
 * This is very useful for SFINAE operations.
 *
 * Example:
 * The following lines will both return true
 * \code
 * (bool)type_to_enum<flex_int>
 * type_to_enum<flex_int>::value
 * \endcode
 *
 */
template <typename T>
struct is_valid_flex_type{
  static constexpr bool value = false;
  constexpr operator bool() const { return value; }
};

template <>
struct is_valid_flex_type<flex_int> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};


template <>
struct is_valid_flex_type<flex_date_time> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};

template <>
struct is_valid_flex_type<flex_float> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};

template <>
struct is_valid_flex_type<flex_string> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};

template <>
struct is_valid_flex_type<flex_vec> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};

template <>
struct is_valid_flex_type<flex_dict> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};


template <>
struct is_valid_flex_type<flex_list> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};

template <>
struct is_valid_flex_type<flex_image> {
  static constexpr bool value = true;
  constexpr operator bool() const { return value; }
};


/**
 * \ingroup group_gl_flexible_type
 *
 * enum_to_type<flex_type_enum e>::type is the flexible_type content type
 * corresponding to the enumeration value. This is very useful for SFINAE
 * operations.
 *
 * Example:
 * This will refer to the type flex_int
 * \code
 * type_to_enum<flex_type_enum::INTEGER>::type
 * \endcode
 */
template <flex_type_enum>
struct enum_to_type{
  typedef void type;
};

template <>
struct enum_to_type<flex_type_enum::INTEGER>{
  typedef flex_int type;
};


template <>
struct enum_to_type<flex_type_enum::DATETIME>{
  typedef flex_date_time type;
};

template <>
struct enum_to_type<flex_type_enum::FLOAT>{
  typedef flex_float type;
};

template <>
struct enum_to_type<flex_type_enum::STRING>{
  typedef flex_string type;
};

template <>
struct enum_to_type<flex_type_enum::VECTOR>{
  typedef flex_vec type;
};


template <>
struct enum_to_type<flex_type_enum::LIST>{
  typedef flex_list type;
};

template <>
struct enum_to_type<flex_type_enum::DICT>{
  typedef flex_dict type;
};

template <>
struct enum_to_type<flex_type_enum::IMAGE>{
  typedef flex_image type;
};


template <>
struct enum_to_type<flex_type_enum::UNDEFINED>{
  typedef flex_undefined type;
};

/**
 * \ingroup group_gl_flexible_type
 * Returns a printable name of a flex_type_enum.
 * Reverse of \ref flex_type_enum_from_name
 */
inline const char* flex_type_enum_to_name(flex_type_enum en) {
  switch(en) {
   case flex_type_enum::INTEGER:
     return "integer";
   case flex_type_enum::DATETIME:
     return "datetime";
   case flex_type_enum::FLOAT:
     return "float";
   case flex_type_enum::STRING:
     return "string";
   case flex_type_enum::VECTOR:
     return "array";
   case flex_type_enum::LIST:
     return "list";
   case flex_type_enum::DICT:
     return "dictionary";
   case flex_type_enum::IMAGE:
     return "image";
   default:
     return "NoneType";
  }
}

/**
 * \ingroup group_gl_flexible_type
 * Given the printable name of a flexible_type type, returns its corresponding
 * \ref flex_type_enum enumeration value.
 * Reverse of \ref flex_type_enum_to_name.
 */
inline flex_type_enum flex_type_enum_from_name(const std::string& name) {
  static std::map<std::string, flex_type_enum> type_map{
    {"integer", flex_type_enum::INTEGER},
    {"datetime", flex_type_enum::DATETIME},
    {"float", flex_type_enum::FLOAT},
    {"string", flex_type_enum::STRING},
    {"array", flex_type_enum::VECTOR},
    {"recursive", flex_type_enum::LIST},
    {"dictionary", flex_type_enum::DICT},
    {"image", flex_type_enum::IMAGE},
    {"undefined", flex_type_enum::UNDEFINED}
  };
  if (type_map.count(name) == 0) {
    log_and_throw(std::string("Invalid flexible type name " + name));
  }
  return type_map.at(name);
}


/**
 * \ingroup group_gl_flexible_type
 *
 * has_direct_conversion_to_flexible_type<T>::value is is true if:
 * \code
 * flexible_type f = T();
 * \endcode
 * will succeed. This is very useful for SFINAE operations.
 */
template <typename T>
struct has_direct_conversion_to_flexible_type {
  static constexpr bool value =
      !std::is_same<T, flexible_type>::value && (
      std::is_integral<T>::value || 
      std::is_floating_point<T>::value || 
      std::is_convertible<T, flex_string>::value ||
      std::is_convertible<T, flex_list>::value ||
      std::is_convertible<T, flex_vec>::value ||
      std::is_convertible<T, flex_dict>::value ||
      std::is_convertible<T, flex_date_time>::value ||
      std::is_convertible<T, flex_image>::value ||
      std::is_same<T, flex_undefined>::value);

  static constexpr flex_type_enum desired_type =
      std::is_integral<T>::value ? flex_type_enum::INTEGER :
      std::is_floating_point<T>::value ? flex_type_enum::FLOAT :
      std::is_convertible<T, flex_string>::value ? flex_type_enum::STRING :
      std::is_convertible<T, flex_list>::value ? flex_type_enum::LIST :
      std::is_convertible<T, flex_vec>::value ? flex_type_enum::VECTOR :
      std::is_convertible<T, flex_dict>::value ? flex_type_enum::DICT :
      std::is_convertible<T, flex_date_time>::value ? flex_type_enum::DATETIME :
      std::is_convertible<T, flex_image>::value ? flex_type_enum::IMAGE :
      flex_type_enum::UNDEFINED;
};


} // namespace graphlab

#endif
