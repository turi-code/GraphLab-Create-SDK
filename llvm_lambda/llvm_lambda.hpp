#include <graphlab/flexible_type/flexible_type.hpp>
#include "llvm/IR/Module.h"
#include "llvm/ExecutionEngine/Interpreter.h"

namespace graphlab {

class llvm_lambda {

public:

static void init_llvm();
static void load_external_library(const std::string library_path);
static void shutdown_llvm();

llvm_lambda(const std::string& ir_code);

~llvm_lambda() = default;

flex_type_enum get_output_type();

void apply(const flexible_type& input, flexible_type& output);

private:

std::unique_ptr<llvm::Module> parse_module(const std::string ir_code); 

std::string get_name_of_first_function(llvm::Module*);

void set_function(const std::string function_name); 

void init_engine(std::unique_ptr<llvm::Module>); 

void typecheck();

template<typename OutputType>
void apply_impl(const flexible_type& input, flexible_type& output);

private:

llvm::Function* f;
llvm::Type* m_input_type;
llvm::Type* m_ret_type;
std::unique_ptr<llvm::ExecutionEngine> engine;
std::function<void(const flexible_type&, flexible_type&)> apply_visitor;
};


} // end of graphlab
