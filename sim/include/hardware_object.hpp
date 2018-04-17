#pragma once

#include <memory>

class HardwareObject;
using HardwareObjPtr = std::shared_ptr<HardwareObject>;

class HardwareObject {
 public:
  HardwareObject() : cycle_counter_(0) {}
  virtual ~HardwareObject() {}

  virtual void ExecuteCycle() { ++cycle_counter_; }
  virtual void Reset() { cycle_counter_ = 0; }
  std::size_t GetCycles() { return cycle_counter_; }

 protected:
  std::size_t cycle_counter_;
};
