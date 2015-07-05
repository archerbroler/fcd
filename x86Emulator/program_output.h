//
//  program_output.hpp
//  x86Emulator
//
//  Created by Félix on 2015-06-16.
//  Copyright © 2015 Félix Cloutier. All rights reserved.
//

//
// The algorithm used here is based off K. Yakdan, S. Eschweiler, E. Gerhards-Padilla
// and M. Smith's research paper "No More Gotos", accessible from the Internet Society's website:
// http://www.internetsociety.org/doc/no-more-gotos-decompilation-using-pattern-independent-control-flow-structuring-and-semantics
//

#ifndef program_output_cpp
#define program_output_cpp

#include "ast_grapher.h"
#include "ast_nodes.h"
#include "dumb_allocator.h"
#include "llvm_warnings.h"

SILENCE_LLVM_WARNINGS_BEGIN()
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/RegionInfo.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
SILENCE_LLVM_WARNINGS_END()

#include <memory>
#include <unordered_map>

// XXX Make this a legit LLVM backend?
// Doesn't sound like a bad idea, but I don't really know where to start.
class AstBackEnd : public llvm::ModulePass
{
	// cleared on run
	DumbAllocator<> pool;
	std::unique_ptr<AstGrapher> grapher;
	std::unordered_map<const llvm::Function*, Statement*> astPerFunction;
	
	// cleared on runOnFunction
	std::unordered_map<llvm::BasicBlock*, llvm::BasicBlock*> postDomTraversalShortcuts;
	
	bool runOnFunction(llvm::Function& fn);
	bool runOnLoop(llvm::Loop& loop);
	bool runOnRegion(llvm::BasicBlock& entry, llvm::BasicBlock& exit);
	
public:
	static char ID;
	
	inline AstBackEnd() : ModulePass(ID)
	{
	}
	
	inline virtual const char* getPassName() const override
	{
		return "AST Back-End";
	}
	
	virtual void getAnalysisUsage(llvm::AnalysisUsage &au) const override;
	virtual bool runOnModule(llvm::Module& m) override;
	
	const Statement* astForFunction(const llvm::Function& fn) const;
};

#endif /* program_output_cpp */