//===--- Sanitizers.cpp - C Language Family Language Options ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the classes from Sanitizers.h
//
//===----------------------------------------------------------------------===//
#include "clang/Basic/Sanitizers.h"
#include "clang/Basic/LLVM.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/MathExtras.h"

using namespace clang;

SanitizerSet::SanitizerSet() : Mask(0) {}

bool SanitizerSet::has(SanitizerMask K) const {
  assert(llvm::countPopulation(K) == 1);
  return Mask & K;
}

bool SanitizerSet::hasOneOf(SanitizerMask K) const {
  return Mask & K;
}

void SanitizerSet::set(SanitizerMask K, bool Value) {
  assert(llvm::countPopulation(K) == 1);
  Mask = Value ? (Mask | K) : (Mask & ~K);
}

void SanitizerSet::clear() {
  Mask = 0;
}

bool SanitizerSet::empty() const {
  return Mask == 0;
}

SanitizerMask clang::parseSanitizerValue(StringRef Value, bool AllowGroups) {
  SanitizerMask ParsedKind = llvm::StringSwitch<SanitizerMask>(Value)
#define SANITIZER(NAME, ID) .Case(NAME, SanitizerKind::ID)
#define SANITIZER_GROUP(NAME, ID, ALIAS)                                       \
  .Case(NAME, AllowGroups ? SanitizerKind::ID##Group : 0)
#include "clang/Basic/Sanitizers.def"
    .Default(0);
  return ParsedKind;
}

SanitizerMask clang::expandSanitizerGroups(SanitizerMask Kinds) {
#define SANITIZER(NAME, ID)
#define SANITIZER_GROUP(NAME, ID, ALIAS)                                       \
  if (Kinds & SanitizerKind::ID##Group)                                        \
    Kinds |= SanitizerKind::ID;
#include "clang/Basic/Sanitizers.def"
  return Kinds;
}
