/**
 * Copyright (C) 2014 GraphLab Inc.
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
 */

#ifndef GRAPHLAB_FLEXIBLE_TYPE_REGISTRY_HPP
#define GRAPHLAB_FLEXIBLE_TYPE_REGISTRY_HPP
#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex> 
#include <cstdint>
#include <graphlab/flexible_type/flexible_type.hpp>
namespace graphlab {


typedef uint32_t field_id_type;
/**
 * \ingroup unity
 * @brief Type Registry for each field associated with vertices or edges.
 */
class flexible_type_registry {
 public:
  flexible_type_registry():_registered_field_counter(0) {}
  ~flexible_type_registry() {}


  /**
   * @brief Unregister field from registry.
   *
   * @param name Field name to unregister.
   */
  void unregister_field(std::string name);

  /**
   * @brief Retrieve the field_id for the specified field name
   *
   * @param name field name to retrieve id for.
   *
   * @return (field_id_type)(-1) if field name not found, otherwise field_id
   */
  field_id_type get_field_id(std::string name) const;

  /**
   * Returns true if the field name exists. Equivalent to 
   * get_field_id(name) != (field_id_type)(-1)
   *
   * \param name Name of the field to query
   */
  inline bool has_field(std::string name) const {
    return _registry_name.count(name) != 0;
  }

  /**
   * Returns true if the field id exists. 
   *
   * \param id ID of the field to query
   */
  inline bool has_field(field_id_type id) const {
    return _registry_index.count(id) != 0;
  }



  /**
   * Retrieve the fieldname for a given field id
   *
   * @param name field id to retrieve name of
   *
   * @return empty string if id not found. The field name otherwise.
   */
  std::string get_field_name(field_id_type id) const;


  /**
   * Register a field using a value to identify the type.
   *
   * @param name Name of field to register.
   * @param value The value to acquire the field type from
   *
   * @return (field_id_type)(-1) if the field already exists, 
   * otherwise the field_id for the registered field is returned
   */
  field_id_type register_field(std::string name, const flexible_type& value);

  /**
   * Register a field.
   *
   * @param name Name of field to register
   * @param type The type of the field.
   *
   * @return (field_id_type)(-1) if the field already exists, 
   * otherwise the field_id for the registered field is returned
   */      
  field_id_type register_field(std::string name, flex_type_enum type);

  /**
   * Returns all registered field names.
   */
  std::vector<std::string> get_all_field_names() const;

  /**
   * Queries the type of the field with the name "name" 
   *
   * \param name Name of the field to query.
   * 
   * \return If the field is 
   * found, the pair (true, type of field) will be returned. 
   * If the field is not found, the pair (true, [undefined]) will be
   * returned.
   */
  std::pair<bool, flex_type_enum> get_field_type(std::string name) const;


  /**
   * Queries the type of the field with the field id "id"
   *
   * \param id of field to query
   * 
   * \return If the field is 
   * found, the pair (true, type of field) will be returned. 
   * If the field is not found, the pair (true, [undefined]) will be
   * returned.
   */
  std::pair<bool, flex_type_enum> get_field_type(field_id_type name) const;

  /**
   * Register the internal id fields, e.g. "__id", "__src_id", "__dst_id" with "id_type".
   */
  void register_id_fields(flex_type_enum id_type);


  /** Empties the registry */
  inline void clear() {
    _registry_name.clear();
    _registry_index.clear();
    _registry_index_to_name.clear();
  }

  /// Serializer
  inline void save(oarchive& oarc) const {
    oarc << _registry_name << _registry_index_to_name << _registry_index;
  }

  /// Deserializer
  inline void load(iarchive& iarc) {
    iarc >> _registry_name >> _registry_index_to_name >> _registry_index;
  }

 private:
  /// field name -> index map
  std::unordered_map<std::string, field_id_type> _registry_name;
  /// index -> type map
  std::unordered_map<field_id_type, flex_type_enum> _registry_index;

  /// index -> field_name
  std::unordered_map<field_id_type, std::string> _registry_index_to_name;

  field_id_type _registered_field_counter;
};

} // namespace graphlab

#endif
