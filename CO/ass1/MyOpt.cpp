// --------------- Assumptions -----------------------
// 1. Only int, bool as basic types
// 2. ...

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/ADT/IntervalMap.h"
#include "llvm/IR/LLVMContext.h"
#include <bits/stdc++.h>

using namespace llvm;
using namespace std;

string randomStr(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string s(len, '0');
    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
}

namespace {
  // Hello - The first implementation, without getAnalysisUsage.
  struct MyOpt: public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    MyOpt(): FunctionPass(ID) {}

    bool runOnFunction(Function & func) override {
      for (Function::iterator B = func.begin(); B != func.end(); ++B) {
        //BasicBlock* Blk = B;
        // map<Value, int> valNum;
        map<string, int> valNum;
        map<int, int> nameTable;
        int vn = 1;
        for (BasicBlock::iterator I = B->begin(); I != B->end(); ++I) {
          I->print(errs());
          errs() << "\n";
          int firstValue, secondValue;
          auto getNewRName = [&]() -> string {
            string ret = randomStr(5);
            while (valNum.count(ret)) {
              ret = randomStr(5);
            }
            return ret;
          };
          auto setIName = [&]() -> string {
            string name;
            if (I->hasName()) {
              name = (I->getName()).str();
              // errs() << "This things has name: " << (I->set)
            } else {
              name = getNewRName();
              I->setName(name);
            }
            return name;
          };
          auto constantPropagation = [&] (int c, Value *second, bool isFirst) {
            Value *first = ConstantInt::get(second->getType(), c); 
            if (!isFirst) {
              swap(first, second);
            }
            Instruction *newinstr;
            if (I->getOpcode() == Instruction::Mul) {
              newinstr = BinaryOperator::Create(Instruction::Mul, first, second);
            } else if (I->getOpcode() == Instruction::Add) {
              newinstr = BinaryOperator::Create(Instruction::Add, first, second);
            } else if (I->getOpcode() == Instruction::Sub) {
              newinstr = BinaryOperator::Create(Instruction::Sub, first, second);
            } else if (I->getOpcode() == Instruction::UDiv) {
              newinstr = BinaryOperator::Create(Instruction::UDiv, first, second);
            } else if (I->getOpcode() == Instruction::SDiv) {
              newinstr = BinaryOperator::Create(Instruction::SDiv, first, second);
            } else if (I->getOpcode() == Instruction::URem) {
              newinstr = BinaryOperator::Create(Instruction::URem, first, second);
            } else if (I->getOpcode() == Instruction::SRem) {
              newinstr = BinaryOperator::Create(Instruction::SRem, first, second);
            } // else {
            //   throw invalid_argument("Received something unexpected\n");
            // }
            ReplaceInstWithInst(B->getInstList(), I, newinstr);
            errs() << "Instruction after Constant propagation: " << *newinstr << "\n";
          };
          auto constantFolding = [&] (Value *first) {
            Value *cmp;
            int res = -1;
            if (I->getOpcode() == Instruction::Mul) {
              res = firstValue * secondValue;
            } else if (I->getOpcode() == Instruction::Add) {
              res = firstValue + secondValue;
            } else if (I->getOpcode() == Instruction::Sub) {
              res = firstValue - secondValue;
            } else if (I->getOpcode() == Instruction::UDiv) {
              res = firstValue / secondValue;
            } else if (I->getOpcode() == Instruction::SDiv) {
              res = firstValue / secondValue;
            } else if (I->getOpcode() == Instruction::URem) {
              res = firstValue % secondValue;
            } else if (I->getOpcode() == Instruction::SRem) {
              res = firstValue % secondValue;
            } // else {
              // throw invalid_argument("Received something unexpected\n");
            // }
            cmp = ConstantInt::get(first->getType(), res);
            Value *zero = ConstantInt::get(first->getType(), 0); 
            Instruction *newinstr = BinaryOperator::Create(Instruction::Add, cmp, zero);
            ReplaceInstWithInst(B->getInstList(), I, newinstr);
            string name = setIName();
            valNum[name] = vn++;
            nameTable[vn - 1] = res;
            errs() << "Instruction after Constant folding: " << *newinstr << "\n";
          };
          if (I->getOpcode() == Instruction::Store) {
            bool ok = false;
            int valint = -1;
            for (auto it = I->op_begin(); it != I->op_end(); it++) {
              Value *val = it->get();
              if (val->hasName()) {
                if (nameTable.count(valNum[(val->getName()).str()])) {
                  valint = nameTable[valNum[(val->getName()).str()]];
                  ok = true;
                }
              } else if (ConstantInt *CI = dyn_cast<ConstantInt> (val)) {
                ok = true;
                valint = CI->getSExtValue();
              } else if (ok) {
                string name;
                if (val->hasName()) {
                  name = (val->getName()).str();
                } else {
                  name = getNewRName();
                  val->setName(name);
                }
                assert((val->getName()).str() == name);
                valNum[name] = vn++;
                nameTable[vn - 1] = valint;
              }
            }
          }
          if (I->getOpcode() == Instruction::Load) {
            string name = setIName();
            for (auto it = I->op_begin(); it != I->op_end(); it++) {
              Value *val = it->get();
              valNum[name] = valNum[(val->getName()).str()];
            }
          }
          if (I->isBinaryOp()) {
            errs() << "Bin statement\n";
            // op[0] = I->getOperand(0);
            // op[1] = I->getOperand(1);
            // Try constant propagation
            for (int i = 0; i < 2; i++) {
              Value *at = I->getOperand(i);
              Value *next = I->getOperand((i + 1) % 2);
              if (at->hasName()) {
                int atvn = valNum[(at->getName()).str()];
                if (nameTable.count(atvn)) {
                  constantPropagation(nameTable[atvn], next, i == 0);
                }
              }
            }
            // Try constant folding
            if (ConstantInt *CIF = dyn_cast<ConstantInt> (I->getOperand(0))) {
              if (ConstantInt *CIS = dyn_cast<ConstantInt> (I->getOperand(1))) {
                firstValue = CIF->getSExtValue();
                secondValue = CIS->getSExtValue();
                constantFolding(I->getOperand(0));
              }
            } else {
              errs() << "Hm...\n";
            }
            // Try mul 2 optimisation
            if (I->getOpcode() == Instruction::Mul) {
              for (int i = 0; i < 2; i++) {
                Value *at = I->getOperand(i);
                Value *next = I->getOperand((i + 1) % 2);
                if (ConstantInt *CI = dyn_cast<ConstantInt> (at)) {
                  int atv = CI->getSExtValue();
                  if (atv == 2) {
                    errs() << "Multiplication by 2 optimisation\n";
                    Value *one = ConstantInt::get(next->getType(), 1);
                    Instruction *newaddinstr = BinaryOperator::Create(Instruction::Shl, next, one);
                    ReplaceInstWithInst(B->getInstList(), I, newaddinstr);
                    errs() << *newaddinstr << "\n";
                    break;
                  }
                }
              }
            }
            // if (ConstantInt *CI = dyn_cast<ConstantInt> (second)) {
            //   secondValue = CI->getSExtValue();
            //   if (first->hasName()) {
            //     firstValue = nameTable[valNum[(first->getName()).str()]];
            //     constantFolding(first);
            //   } else if (secondValue == 2 and I->getOpcode() == Instruction::Mul) {
            //     errs() << "Multiplication by 2 optimisation\n";
            //     Value *one = ConstantInt::get(second->getType(), 1);
            //     Instruction *newaddinstr = BinaryOperator::Create(Instruction::Shl, first, one);
            //     ReplaceInstWithInst(B->getInstList(), I, newaddinstr);
            //     errs() << *newaddinstr << "\n";
            //   }
            // } else if (ConstantInt *CI = dyn_cast<ConstantInt> (first)) {
            //   firstValue = CI->getSExtValue();
            //   if (second->hasName()) {
            //     secondValue = nameTable[valNum[(second->getName()).str()]];
            //     constantFolding(first);
            //   } else if (firstValue == 2 and I->getOpcode() == Instruction::Mul) {
            //     errs() << "Multiplication by 2 optimisation\n";
            //     Value *one = ConstantInt::get(second->getType(), 1);
            //     Instruction *newaddinstr = BinaryOperator::Create(Instruction::Shl, second, one);
            //     ReplaceInstWithInst(B->getInstList(), I, newaddinstr);
            //     errs() << *newaddinstr << "\n";
            //   }
          }
        } 
      }
      return true;
    }
  };
}
char MyOpt::ID = 0;
static RegisterPass < MyOpt >
  Y("myopt", "My optimisations");