/**
 * Copyright (C) 2014 GraphLab Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#ifndef GRAPHLAB_UNITY_FLEXIBLE_TYPE_RECORD_HPP
#define GRAPHLAB_UNITY_FLEXIBLE_TYPE_RECORD_HPP
#include <new>
#include <vector>
#include <utility>
#include <graphlab/flexible_type/flexible_type.hpp>
#include <graphlab/flexible_type/flexible_type_registry.hpp>
#include <boost/range.hpp>
namespace graphlab {

/**
 * \ingroup unity
 * The flexible_type_record represents a collection of (field-id, flexible_type) 
 * pairs.
 *
 * The internal represention of the flexible type record varies depending on 
 * the number of fields it contains. When it contains just a single field, 
 * it is a single 16-byte entry which represents a flexible_type with the field 
 * information held in the tag in the flexible_type. When the number of 
 * fields is larger than one, it maintained a pair of (size_t, flexible_type*)
 * which is also 16 bytes, and represents a length, and a pointer to an
 * array of flexible_type objects. Once again the field id is 
 * maintained in the tag in the flexible_type.
 */
class flexible_type_record {
 public:
  flexible_type_record();
  ~flexible_type_record();

  flexible_type_record(const flexible_type_record& other);
  flexible_type_record& operator=(const flexible_type_record& other);

  /**
   * @brief Adds field to record, specifying the data for the field.
   *
   * @param field_id field_id returned from flex_type_registry 
   * @param data field data
   *
   * @return (size_t)(-1) if field already exists. Otherwise returns the field_id specified.
   */
  field_id_type add_field(field_id_type field_id, flexible_type data);

  /**
   * @brief Remove field previously registered.
   *
   * @param field_id field_id from prior addition
   */
  void remove_field(field_id_type field_id);

  /// Serializer
  void save(graphlab::oarchive& oarc) const;

  /// Deserializer
  void load(graphlab::iarchive& iarc);


  /**
   * Returns the number of fields in this record
   */
  inline size_t size() const {
    return get_ptr().first;
  }

  /**
   * Returns an boost range iterating over the contents of the record.
   * The tag of each flexible type is the field ID.
   */
  inline boost::iterator_range<flexible_type*> fields() {
    auto len_and_ptr = get_ptr();
    return boost::make_iterator_range(len_and_ptr.second,
                                      len_and_ptr.second + len_and_ptr.first);
  }

  /**
   *
   * Returns an boost range iterating over the contents of the record.
   * The tag of each flexible type is the field ID. const overload.
   */
  inline boost::iterator_range<const flexible_type*> fields() const {
    auto len_and_ptr = get_ptr();
    return boost::make_iterator_range(len_and_ptr.second,
                                      len_and_ptr.second + len_and_ptr.first);
  }

  /** 
   * makes the record represent an empty record. 
   * cleans up existing contents.
   */
  void clear() {
    if (is_single()) {
      // call destructor explicitly.
      values.single.~flexible_type();
    } else if (is_array()) {
      // delete everything in the array
      for (size_t i = 0;i < values.ptr.first; ++i) {
        values.ptr.second[i].~flexible_type();
      }
      free(values.ptr.second);
    }
    set_empty();
  }


  /**
   * Returns true if the field exists. False otherwise.
   */
  inline bool has_field(size_t s) const {
    std::pair<size_t, const flexible_type*> ptr = get_ptr();
    for (size_t i = 0;i < ptr.first; ++i) {
      if (ptr.second[i].tag() == s) return true;
    } 
    return false;
  }

  /**
   * Returns a pointer to a field if found;
   * Otherwise, returns a NULL pointer
   */
  inline flexible_type* find(field_id_type s) {
    std::pair<size_t, flexible_type*> ptr = get_ptr();
    for (size_t i = 0;i < ptr.first; ++i) {
      if (ptr.second[i].tag() == s) return &(ptr.second[i]);
    }
    return NULL;
  }


  /**
   * Returns a pointer to a field if found;
   * Otherwise, returns a NULL pointer. Const overload.
   */
  inline const flexible_type* find(field_id_type s) const {
    std::pair<size_t, const flexible_type*> ptr = get_ptr();
    for (size_t i = 0;i < ptr.first; ++i) {
      if (ptr.second[i].tag() == s) return &(ptr.second[i]);
    }
    return NULL;
  }


  /**
   * Returns the value of a field if the field exists.
   * Otherwise, creates a new field with an empty value.
   *
   * \note When it creates a new field, the flexible_type is
   * unaware of the actual type, and is thus undefined. Some care needed
   * if you are uncertain whether the field exists in the record.
   */
  inline flexible_type& operator[](field_id_type s) {
    std::pair<size_t, flexible_type*> ptr = get_ptr();
    for (size_t i = 0;i < ptr.first; ++i) {
      if (ptr.second[i].tag() == s) return ptr.second[i];
    }
    // no existing element. create a new one
    flexible_type* newel = add_one_element();
    newel->tag() = s;
    return (*newel);
  }


  /**
   * const overload of the operator[] function.
   * Returns the value of a field if the field exists.
   * Otherwise, fails.
   */
  inline const flexible_type& operator[](field_id_type s) const {
    std::pair<size_t, const flexible_type*> ptr = get_ptr();
    for (size_t i = 0;i < ptr.first; ++i) {
      if (ptr.second[i].tag() == s) return ptr.second[i];
    }
    ASSERT_MSG(false, "Field not found");
    static flexible_type t;
    return t;
  }

  /** 
   * Returns true if this flexible_type_record contains all the fields in
   * "other" and matches all values; returns false otherwise.
   */
  inline bool is_superset_of(const flexible_type_record& other) const {
    for (const auto& f : other.fields()) {
      if (!has_field(f.tag()) || (*this)[f.tag()] != f) return false;
    }
    return true;
  }


  /** 
   * Returns true if this flexible_type_record contains all the fields in
   * "other" and matches all values; returns false otherwise. Like is_superset_of
   * but allows UNDEFINED matches everything. i.e. if either UNDEFINED is 
   * defined to be equal to everything.
   */
  inline bool is_superset_of_undefined_wildcard(const flexible_type_record& other) const {
    for (const auto& f : other.fields()) {
      if (!has_field(f.tag()) || 
          (f.get_type() != flex_type_enum::UNDEFINED && 
          (*this)[f.tag()].get_type() != flex_type_enum::UNDEFINED &&
          (*this)[f.tag()] != f)) return false;
    }
    return true;
  }

  /** 
   * Returns true if this flexible_type_record contains all the fields in
   * "fields" ; returns false otherwise.
   */
  inline bool contains_fields(const std::set<field_id_type>& fields) const {
    for(field_id_type f : fields) {
      if (!has_field(f)) return false;
    }
    return true;
  }


 private:
  /*
   * we use the lower two bits of the union to identify if there are no elements,
   * one element, or more than one element in the record. Specifically,
   * if the lower two bits are 
   * 00 --> the contents of the records is an array of elements
   * 01 --> There are no elements in the record
   * 11 --> There is one element in the record
   *
   * This is safe because the the lower 3 bytes of the flexible_type is not 
   * used, and a malloc of the flexible type will necessarily require 8 byte
   * alignment thus forcing the lower 3 bits to be 0.
   */
  union union_type {
    union_type() : words{0,0} { }
    ~union_type() { }
    flexible_type single; // stores a single element
    std::pair<size_t, flexible_type*> ptr; //stores an array of elements
    std::pair<size_t, size_t> words; // used to get direct access to the record memory
  } values;

  /// returns true if the record is empty
  inline bool is_empty() const {
    return (values.words.second & 3) == 1;
  }

  /// returns true if the record contains only one element
  inline bool is_single() const {
    return (values.words.second & 3) == 3;
  }

  /// returns true if the record contains an array of elements
  inline bool is_array() const {
    return (values.words.second & 3) == 0;
  }


  /// Sets the bits to indicate the record is empty
  inline void set_empty() {
    values.words.first = 0;
    values.words.second = 0;
    values.words.second = (values.words.second & ~(size_t)3) | 1;
  }

  /// Sets the bits to indicate the record contains only one element
  inline void set_single() {
    values.words.second |= 3;
  }

  /**
   * Returns a pair of length of pointer representing the contents of this
   * record. This function is valid regardless of how record is being 
   * represented.
   */
  inline std::pair<size_t, flexible_type*> get_ptr() {
    if (is_empty()) {
      return {0, (flexible_type*)NULL};
    } else if (is_single()) {
      return {1, &values.single};
    } else {
      return values.ptr;
    }
  }

  /**
   * Returns a pair of length of pointer representing the contents of this
   * record. This function is valid regardless of how record is being 
   * represented. Const overload.
   */
  inline std::pair<size_t, const flexible_type*> get_ptr() const {
    if (is_empty()) {
      return {0, (flexible_type*)NULL};
    } else if (is_single()) {
      return {1, &values.single};
    } else {
      return values.ptr;
    }
  }

  /**
   * Resizes the number of elements in the record, clearing all existing
   * contents.
   */
  void resize_and_clear(size_t numel);

  /**
   * Inserts one element to the tail of the array, returning a pointer
   * to the new element.
   */
  flexible_type* add_one_element();
}; // class record


inline std::ostream& operator<<(std::ostream& os, const flexible_type_record& record) {
  os << "[";
  for(const auto& field: record.fields()) {
    os << (size_t)field.tag() << ":" << field << " ";
  }
  os << "]\n";
  return os;
}


} // namespace graphlab
#endif
