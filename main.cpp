#include <iostream>
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>

using namespace llvm;

enum Token {
  tok_eof = -1,
  tok_add = -2, tok_number = -3,
  tok_error = -4,
};

static double numVal;
static std::string identifierStr;

static int getTok(){
  static int lastChar = ' ';

  while(isspace(lastChar))
    lastChar = getchar();
  
  if(isalpha(lastChar)){
    identifierStr = lastChar;
    while (isalnum((lastChar = getchar())))
      identifierStr += lastChar;

    if(identifierStr != "add")
      return tok_error;

    return tok_add;
  }

  if (isdigit(lastChar)){
      std::string numStr;
      do {
        numStr += lastChar;
        lastChar = getchar();
      } while (isdigit(lastChar));

      numVal = strtod(numStr.c_str(), nullptr);
      return tok_number;
  }

  if(lastChar == EOF)
    return tok_eof;

  lastChar = getchar();
  return tok_error;
} 

//headerparse
namespace {

class ExprAST {
public:
  virtual ~ExprAST() = default;

  virtual Value *codegen() = 0;
};

class NumberExprAST : public ExprAST {
  double Val;

public:
  NumberExprAST(double Val) : Val(Val) {}

  Value *codegen() override;

};

class FuncExprAST : public ExprAST {
  public:
    std::string func;
    std::unique_ptr<ExprAST> num1, num2;

    FuncExprAST(std::string func, std::unique_ptr<ExprAST>num1, std::unique_ptr<ExprAST> num2) : func(func), num1(std::move(num1)), num2(std::move(num2)) {}

    Module *createModAdd(llvm::LLVMContext &context);

    Value *rightVal();
    Value *leftVal();

    Value *codegen() override;
};

}



//parseeeeeeeeeeeeeeeeeeeeeeeeeeeeee

static int curTok;
static int getNextToken() { return curTok = getTok(); }

std::unique_ptr<ExprAST> LogError(const char *Str) {
  fprintf(stderr, "Error: %s", Str);
  return nullptr;
}
std::unique_ptr<FuncExprAST> LogErrorF(const char *Str) {
  LogError(Str);
  return nullptr;
}

static std::unique_ptr<ExprAST> ParseNumberExpression(){
  auto Result = std::make_unique<NumberExprAST>(numVal);
  return std::move(Result);
}

static std::unique_ptr<FuncExprAST> ParseNumb(){
  std::string fnName = identifierStr; 
  if(curTok != tok_number)
    return LogErrorF("expected number \n");
  auto number1 = ParseNumberExpression();
  getNextToken();
  if(curTok != tok_number)
    return LogErrorF("expected number \nready>");
  auto number2 = ParseNumberExpression();
  
  return std::make_unique<FuncExprAST>(fnName, std::move(number1), std::move(number2));  
}

static std::unique_ptr<FuncExprAST> ParseAdd(){
  getNextToken();
  return ParseNumb();
}


//codegen

LLVMContext context;
std::unique_ptr<Module> TheModule;
std::unique_ptr<IRBuilder<>> Builder;
std::map<std::string, Value *> NamedValues;

Value *LogErrorV(const char *Str) {
  LogError(Str);
  return nullptr;
}

Value *NumberExprAST::codegen() {
  return ConstantFP::get(context, APFloat(Val));
}

Value *FuncExprAST::codegen(){
  return nullptr;
}

Value *FuncExprAST::leftVal(){
  Value *L = num1->codegen();
  return L;
}

Value *FuncExprAST::rightVal(){
  Value *R = num2->codegen();
  return R;
}




Module *FuncExprAST::createModAdd(llvm::LLVMContext &context){

  Module *TheModule = new llvm::Module("my cool jit", context);

  llvm::FunctionCallee function = TheModule->getOrInsertFunction("main",
      llvm::Type::getDoubleTy(context));
  llvm::Function *func = llvm::cast<llvm::Function>(function.getCallee());
  
  func->setCallingConv(llvm::CallingConv::C);
  llvm::Function::arg_iterator args = func->arg_begin();
  llvm::Value *x = FuncExprAST::leftVal();
  llvm::Value *y = FuncExprAST::rightVal();
  llvm::BasicBlock *bb = llvm::BasicBlock::Create(context, "entry", func);
  llvm::IRBuilder<>builder(bb);
  llvm::Value *tmp = builder.CreateBinOp(llvm::Instruction::Add, x, y, "tmp");
  builder.CreateRet(tmp);

  return TheModule;


}

//top level

static void InitializeModule() {
  // Open a new context and module.
  LLVMContext context;

}

static void handleAdd(){
  if(auto e = ParseAdd()){
    if (Module *FnIR = e->createModAdd(context)) {
      errs() << *FnIR;
      fprintf(stderr, "\n");
    }
  }else{
    getNextToken();
  }
}

static void handleError(){
  getNextToken();
}

static void handleNumb(){
  getNextToken();
}


/// top ::= definition | external | expression | ';'
static void MainLoop() {
  while (true) {
    fprintf(stderr, "ready>");
    switch (curTok) {
    case tok_eof:
      return;
    case tok_add:
      handleAdd();
      break;
    case tok_error:
      handleError();
      break;
    case tok_number:
      handleNumb();
      break;
    default:
      break;
    }
  }
}


int main(){
  fprintf(stderr, "ready>");
  getNextToken();

  InitializeModule();

  MainLoop();

  TheModule->print(errs(), nullptr);

  return 0;
}
