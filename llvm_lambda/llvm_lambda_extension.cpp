#include <graphlab/sdk/toolkit_function_macros.hpp>
#include <graphlab/sdk/toolkit_class_macros.hpp>
#include <graphlab/flexible_type/flexible_type.hpp>
#include <graphlab/sdk/gl_sarray.hpp>
#include "llvm_lambda.hpp"

using namespace graphlab;

void init_llvm() {
  llvm_lambda::init_llvm();
}

void shutdown_llvm() {
  llvm_lambda::shutdown_llvm();
}

void add_shared_library(const std::string library_path) {
  llvm_lambda::load_external_library(library_path);
}

flexible_type apply_once(const flexible_type& v,
                         const std::string& llvm_ir) {
  llvm_lambda lambda(llvm_ir);
  flex_type_enum output_type = lambda.get_output_type();
  flexible_type ret(output_type);
  lambda.apply(v, ret);
  return ret;
}

gl_sarray apply_sa(gl_sarray sa,
                   const std::string& llvm_ir) {
  llvm_lambda lambda(llvm_ir);
  flex_type_enum output_type = lambda.get_output_type();
  gl_sarray_writer writer(output_type, 1);
  flexible_type ret(output_type);
  for (const auto& v: sa.range_iterator()) {
    lambda.apply(v, ret);
    writer.write(ret, 0);
  }
  return writer.close();
}

BEGIN_FUNCTION_REGISTRATION
REGISTER_FUNCTION(init_llvm);
REGISTER_FUNCTION(shutdown_llvm);
REGISTER_FUNCTION(add_shared_library, "library_path");
REGISTER_FUNCTION(apply_sa, "sa", "llvm_ir");
REGISTER_FUNCTION(apply_once, "v", "llvm_ir");
END_FUNCTION_REGISTRATION
