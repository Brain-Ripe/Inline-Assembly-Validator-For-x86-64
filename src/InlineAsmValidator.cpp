#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"
#include <set>
#include <regex>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace clang;
using namespace clang::ast_matchers;

const std::set<std::string> X86_64_REGS = {
    "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
    "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp",
    "al", "bl", "cl", "dl", "ah", "bh", "ch", "dh"
};

const std::set<std::string> X86_64_MNEMONICS = {
    "mov", "add", "sub", "mul", "div", "push", "pop", 
    "jmp", "call", "ret", "nop", "lea", "inc", "dec"
};

namespace {

class AsmCallback : public MatchFinder::MatchCallback {
int StackBalance=0;
public:
    virtual void run(const MatchFinder::MatchResult &Result) override {
    const auto *AS = Result.Nodes.getNodeAs<GCCAsmStmt>("asmStmt");
    if (!AS) return;

    std::string AsmString = AS->getAsmString();
    SourceManager &SM = *Result.SourceManager;
    unsigned Line = SM.getSpellingLineNumber(AS->getBeginLoc());

    // 1. Build a lookup set of declared clobbers
    std::set<std::string> DeclaredClobbers;
    for (unsigned i = 0; i < AS->getNumClobbers(); ++i) {
        // Direct insertion works for std::string in your Clang version
        DeclaredClobbers.insert(AS->getClobber(i));
    }

    llvm::outs() << "\n[Found asm(...)] at Line " << Line << ": " << AsmString << "\n";

    // 2. Validate Mnemonic & Track Stack Alignment
    std::stringstream ss(AsmString);
    std::string mnemonic;
    ss >> mnemonic; 
    
    // Clean mnemonic of punctuation (e.g., 'mov,' or 'push:')
    mnemonic.erase(std::remove_if(mnemonic.begin(), mnemonic.end(), 
                   [](unsigned char c){ return std::ispunct(c); }), mnemonic.end());

    if (X86_64_MNEMONICS.find(mnemonic) == X86_64_MNEMONICS.end()) {
        llvm::errs() << "  !!! MNEMONIC ERROR: '" << mnemonic << "' is not a recognized instruction.\n";
    } else {
        // Stack Alignment Logic
        if (mnemonic == "push") {
            StackBalance++;
            llvm::outs() << "  [Stack] PUSH detected. Depth: " << StackBalance << "\n";
        } else if (mnemonic == "pop") {
            StackBalance--;
            llvm::outs() << "  [Stack] POP detected. Depth: " << StackBalance << "\n";
            if (StackBalance < 0) {
                llvm::errs() << "  !!! STACK ERROR: Stack underflow (POP without PUSH)!\n";
            }
        }
    }

    // 3. Validate Registers & Clobber Matching
    std::regex RegRegex("%%([a-zA-Z0-9]+)");
    auto RegBegin = std::sregex_iterator(AsmString.begin(), AsmString.end(), RegRegex);
    auto RegEnd = std::sregex_iterator();

    for (std::sregex_iterator i = RegBegin; i != RegEnd; ++i) {
        std::string reg = (*i)[1].str();

        // Rule A: Is it a real x86-64 register?
        if (X86_64_REGS.find(reg) == X86_64_REGS.end()) {
            llvm::errs() << "  !!! REGISTER ERROR: '" << reg << "' is not a valid x86-64 register.\n";
        } 
        // Rule B: If it's real, is it correctly clobbered?
        else if (DeclaredClobbers.find(reg) == DeclaredClobbers.end()) {
            llvm::errs() << "  !!! CLOBBER ERROR: '" << reg << "' used but not declared in clobber list.\n";
        } 
        else {
            // llvm::outs() << "  [OK] Register '" << reg << "' validated and clobbered.\n";
        }
    }
    llvm::outs() << "------------------------------------------\n";
}
};

class AsmConsumer : public ASTConsumer {
public:
    AsmConsumer() {
        // FIX 2: Using the most generic matcher name to satisfy the compiler
        Matcher.addMatcher(asmStmt().bind("asmStmt"), &Handler);
    }

    void HandleTranslationUnit(ASTContext &Context) override {
        Matcher.matchAST(Context);
    }

private:
    AsmCallback Handler;
    MatchFinder Matcher;
};

class AsmValidatorAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                   llvm::StringRef) override {
        return std::make_unique<AsmConsumer>();
    }

    bool ParseArgs(const CompilerInstance &CI,
                   const std::vector<std::string> &args) override {
        return true;
    }
};

} // namespace

static FrontendPluginRegistry::Add<AsmValidatorAction>
    X("asm-validator", "Validates x86-64 inline assembly constraints");