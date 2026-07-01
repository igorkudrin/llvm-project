//===---- KCFI.cpp - Implements Kernel Control-Flow Integrity (KCFI) ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// TODO: Add annotation
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/CFGuardCheckExpand.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBundle.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/IR/Module.h"
#include "llvm/InitializePasses.h"

using namespace llvm;

#define DEBUG_TYPE "cfguard-check-expand"
#define PASS_NAME "Extract CFGuard call checks into a separate instruction"

namespace {
class CFGuardCheckExpand {
public:
  bool run(MachineFunction &MF);

private:
  /// Machine instruction info used throughout the class.
  const TargetInstrInfo *TII = nullptr;

  /// Target lowering for arch-specific parts.
  const TargetLowering *TLI = nullptr;

  /// Splits an indirect call into calling a guard function and an indirect call
  /// itself. \returns true if the instruction was adjusted.
  bool extractCFGuardCall(MachineBasicBlock &MBB,
                          MachineBasicBlock::instr_iterator I) const;
};

class MachineCFGuardCheckExpandLegacy : public MachineFunctionPass {
public:
  static char ID;

  MachineCFGuardCheckExpandLegacy() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return PASS_NAME; }
  bool runOnMachineFunction(MachineFunction &MF) override {
    return CFGuardCheckExpand().run(MF);
  }
};

char MachineCFGuardCheckExpandLegacy::ID = 0;
} // end anonymous namespace

INITIALIZE_PASS(MachineCFGuardCheckExpandLegacy, DEBUG_TYPE, PASS_NAME, false,
                false)

FunctionPass *llvm::createCFGuardCheckExpandPass() {
  return new MachineCFGuardCheckExpandLegacy();
}

PreservedAnalyses MachineCFGuardCheckExpandPass::run(MachineFunction &MF,
                                       MachineFunctionAnalysisManager &MFAM) {
  if (!CFGuardCheckExpand().run(MF))
    return PreservedAnalyses::all();

  PreservedAnalyses PA = getMachineFunctionPassPreservedAnalyses();
  PA.preserveSet<CFGAnalyses>();
  return PA;
}

bool CFGuardCheckExpand::extractCFGuardCall(MachineBasicBlock &MBB,
                     MachineBasicBlock::instr_iterator MBBI) const {
  assert(TII && "Target instruction info was not initialized");
  assert(TLI && "Target lowering was not initialized");

  // If the call instruction is bundled, we can only emit a check safely if
  // it's the first instruction in the bundle.
  //if (MBBI->isBundled() && !std::prev(MBBI)->isBundle())
  //  report_fatal_error("bundled calls are not supported");

  MachineInstr *Check = TLI->EmitCFGuardCheck(MBB, MBBI, TII);

  // Clear the original call's CFI type.
  assert(MBBI->isCall() && "Unexpected instruction type");
  MBBI->clearFlag(MachineInstr::CFGuardMerged);

  // If not already bundled, bundle the check and the call to prevent
  // further changes.
  //if (!MBBI->isBundled())
  //  finalizeBundle(MBB, Check->getIterator(), std::next(MBBI->getIterator()));

  return true;
}

bool CFGuardCheckExpand::run(MachineFunction &MF) {
  const Module *M = MF.getFunction().getParent();
  if (M->getControlFlowGuardMode() != ControlFlowGuardMode::Enabled)
    return false;

  const auto &SubTarget = MF.getSubtarget();
  TII = SubTarget.getInstrInfo();
  TLI = SubTarget.getTargetLowering();

  bool Changed = false;
  for (MachineBasicBlock &MBB : MF) {
    // Use instr_iterator because we don't want to skip bundles.
    for (MachineBasicBlock::instr_iterator MII = MBB.instr_begin(),
                                           MIE = MBB.instr_end();
         MII != MIE; ++MII) {
      if (MII->isCall() && MII->getFlag(MachineInstr::CFGuardMerged))
        Changed |= extractCFGuardCall(MBB, MII);
    }
  }

  return Changed;
}
