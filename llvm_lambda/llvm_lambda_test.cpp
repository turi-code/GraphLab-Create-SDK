#include "llvm_lambda.hpp"
#include <string>
#include <fstream>
#include <streambuf>

const char* lib_path = "/Users/haijieg/anaconda/lib/libpython2.7.dylib";
using namespace graphlab;

void usage(char** argv) {
  std::cerr << "Usage: " << argv[0] << " [LLVM_IR_FILE] [INPUT]" << std::endl;
  std::cerr << "Example: " << argv[0] << " ir_examples/add1.ll 100" << std::endl;
} 

int main(int argc, char** argv) {
  if (argc != 3) {
    usage(argv);
    exit(0);
  }

  std::ifstream code_stream(argv[1]);
  std::string code((std::istreambuf_iterator<char>(code_stream)),
                    std::istreambuf_iterator<char>());

  llvm_lambda::init_llvm();
  llvm_lambda::load_external_library(lib_path);
  flexible_type input = atoi(argv[2]);
  flexible_type output;
  llvm_lambda lambda(code);
  lambda.apply(input, output);
  std::cout << "output: " << output << std::endl;
  llvm_lambda::shutdown_llvm();
}
