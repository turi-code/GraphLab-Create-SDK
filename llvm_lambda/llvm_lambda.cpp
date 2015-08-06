#include "llvm/ADT/STLExtras.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/DynamicLibrary.h"

#include "llvm_lambda.hpp"
#include <mutex>

using namespace llvm;
namespace graphlab {

static std::mutex LLVM_Mutex;
static bool LLVM_Initialized = false;

static void llvm_fatal_error_handler(const char* reason) {
  throw(reason);
}

void llvm_lambda::init_llvm() {
  std::lock_guard<std::mutex> lock(LLVM_Mutex);
  if (!LLVM_Initialized) {
    InitializeNativeTarget();
    LLVM_Initialized = true;
     LLVMInstallFatalErrorHandler(llvm_fatal_error_handler);
  }
  LLVM_Initialized = false;
}

void llvm_lambda::shutdown_llvm() {
  std::lock_guard<std::mutex> lock(LLVM_Mutex);
  if (LLVM_Initialized) {
    llvm_shutdown();
  }
}


llvm_lambda::llvm_lambda(const std::string& ir_code) { 
  llvm_lambda::init_llvm();
  std::unique_ptr<Module> m = parse_module(ir_code);
  auto function_name = get_name_of_first_function(m.get());
  init_engine(std::move(m));
  set_function(function_name);
}

void llvm_lambda::apply(const flexible_type& input,
                        flexible_type& output) {
  apply_visitor(input, output);
}

std::unique_ptr<Module> llvm_lambda::parse_module(const std::string ir_code) {
  LLVMContext &ctx = getGlobalContext();
  auto membuffer = MemoryBuffer::getMemBuffer(StringRef(ir_code));
  SMDiagnostic err;
  auto mod = parseIR(membuffer->getMemBufferRef(), err, ctx);
  if (mod == nullptr) {
    throw(err.getMessage().str());
  }
  std::cerr << "Module loaded" << std::endl;
  return mod;
}

std::string llvm_lambda::get_name_of_first_function(Module* mod) {
  auto& func_list = mod->getFunctionList();
  std::string name;
  if (func_list.size() == 0) {
    throw("Module does not contain any functions");
  } else {
    name = func_list.front().getName();
  }
  return name;
}

void llvm_lambda::set_function(const std::string function_name) {
  std::cerr << "Set function to " << function_name << std::endl;
  f = engine->FindFunctionNamed(function_name.c_str());
  if (f == nullptr) {
    throw("function not found in module");
  }
  typecheck();

  if (m_ret_type->isDoubleTy()) {
    apply_visitor = [this](const flexible_type& in, flexible_type& out) {
      apply_impl<flex_float>(in, out);
    };
  } else {
    apply_visitor = [this](const flexible_type& in, flexible_type& out) {
      apply_impl<flex_int>(in, out);
    };
  }
}

void llvm_lambda::typecheck() {
  FunctionType* ftype = f->getFunctionType();
  if (ftype->getNumParams() != 4) {
    throw("Type check failed. Invalid number of arguments");
  }
  // check return type
  m_ret_type = ftype->getParamType(0);
  if (!m_ret_type->isPointerTy()) {
    throw("Type check failed. First argument (rettype) must be pointer type");
  }
  m_ret_type = m_ret_type->getPointerElementType();
  if (!(m_ret_type->isDoubleTy() || m_ret_type->isIntegerTy())) {
    throw("Type check failed. Return type must be int or double");
  }

  // check input type
  Type* m_input_type = ftype->getParamType(3);
  if (!(m_input_type->isDoubleTy() || m_input_type->isIntegerTy())) {
    throw("Type check failed. Input type must be int or double");
  }
}

flex_type_enum llvm_lambda::get_output_type() {
  if (m_ret_type->isDoubleTy()) {
    return flex_type_enum::FLOAT;
  } else if (m_ret_type->isIntegerTy()) {
    return flex_type_enum::INTEGER;
  } else {
    throw("Invalid output type");
  }
}

void llvm_lambda::load_external_library(const std::string library_path) {
  bool success = llvm::sys::DynamicLibrary::LoadLibraryPermanently(library_path.c_str());
  if (!success) {
    std::cerr << "Adding dynamic library " << library_path << " to llvm jit engine" << std::endl; 
  } else {
    throw(std::string("Fail adding dynamic library ") + library_path);
  }
}

void llvm_lambda::init_engine(std::unique_ptr<llvm::Module> mod) {
  std::string errStr;
  engine.reset(EngineBuilder(std::move(mod)).setVerifyModules(true).setErrorStr(&errStr).create());
  if (errStr.empty()) {
    std::cerr << "Engine created" << std::endl;
  } else {
    throw(std::string("Fail creating engine: ") + errStr);
  }
}

template<typename OutputType>
void llvm_lambda::apply_impl(const flexible_type& input,
                             flexible_type& output) {
  // 1st argument is the pointer to the return value object
  // 2nd and 3rd argument are pointer to python execution environment, no use here
  // 4th argument is the input argument
  std::vector<GenericValue> args(4);
  OutputType ret;
  args[0].PointerVal = (void*)(&ret);
  args[1].PointerVal = (void*)(nullptr);
  args[2].PointerVal = (void*)(nullptr);
  if (input.get_type() == flex_type_enum::INTEGER) {
    args[3].IntVal = APInt(64, (flex_int)(input));
  } else if (input.get_type() == flex_type_enum::FLOAT) {
    args[3].DoubleVal = (flex_float)(input);
  }

  // Need exception handling
  engine->runFunction(f, args);

  output = ret;
}

}
