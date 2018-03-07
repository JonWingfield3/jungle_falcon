#pragma once

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <glog/logging.h>

#include <riscv_defs.hpp>

class MemoryBase {
 public:
  MemoryBase(mem_addr_t capacity) : capacity_(capacity), size_(0) {}

  template <typename data_t>
  data_t Read(mem_addr_t mem_addr) const {}

  template <typename data_t>
  void Write(mem_addr_t mem_addr, data_t data) {}

  mem_addr_t Size() const { return size_; }
  mem_addr_t Capacity() const { return capacity_; }

  void CoreDump(std::size_t num_bytes = 0,
                std::ostream& output_stream = std::cout,
                std::size_t width = 4) {
    output_stream << "\n\nMemory Dump\nMemory Size = " << size_ << std::endl;
    for (std::size_t ii = 0; ii < size_ - width; ii += width) {
      output_stream << std::hex << std::showbase << ii << "\t";
      for (std::size_t jj = width; jj > 0; --jj) {
        output_stream << std::hex << std::showbase << std::setw(4)
                      << static_cast<int>(Read<uint8_t>(ii + jj - 1)) << "  ";
      }
      output_stream << std::endl;
    }
    output_stream << std::endl;
  }

 protected:
  mem_addr_t capacity_;
  mem_addr_t size_;
};

using MemoryPtr = std::shared_ptr<MemoryBase>;

class MainMemory : public MemoryBase {
 public:
  MainMemory(const std::string& image_name,
             mem_addr_t capacity = kDefaultMemSize)
      : MemoryBase(capacity) {
    mem_.resize(capacity_);
    std::fill(mem_.begin(), mem_.end(), 0);
    VLOG(5) << "Zero filling main memory.. wrote " << capacity_ << " 0 bytes";
    LoadImage(image_name);
  }

  template <typename data_t>
  data_t Read(mem_addr_t mem_addr) const {
    CHECK(mem_addr < capacity_) << "Attempting to read from invalid address!";

    const uint8_t* base_addr_ptr = mem_.data();
    const uint8_t* read_addr_ptr =
        static_cast<const uint8_t*>(base_addr_ptr + mem_addr);
    const data_t* read_ptr = reinterpret_cast<const data_t*>(read_addr_ptr);
    const data_t data = *read_ptr;

    VLOG(5) << "Read 0x" << std::hex << data << std::dec << " from address 0x"
            << std::hex << mem_addr;
    return data;
  }

  template <typename data_t>
  void Write(mem_addr_t mem_addr, data_t data) {
    CHECK(mem_addr < capacity_) << "Attempting to write to invalid address!";
    VLOG(5) << std::hex << std::showbase << "Writing " << static_cast<int>(data)
            << " to address " << mem_addr;

    uint8_t* base_addr_ptr = static_cast<uint8_t*>(mem_.data());
    uint8_t* write_addr_ptr = static_cast<uint8_t*>(base_addr_ptr + mem_addr);
    data_t* write_ptr = reinterpret_cast<data_t*>(write_addr_ptr);
    *write_ptr = data;
  }

 private:
  void LoadImage(const std::string& image_name);

  template <typename data_t>
  static data_t SwapEndianness(data_t data) {
    // adapted from:
    // https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
    union {
      data_t data;
      std::array<uint8_t, sizeof(data_t)> bytes;
    } source, dest;
    source.data = data;

    for (std::size_t ii = 0; ii < sizeof(data_t); ++ii) {
      dest.bytes.at(ii) = source.bytes.at(sizeof(data_t) - ii - 1);
    }
    return dest.data;
  }

  static constexpr std::size_t kDefaultMemSize = 65535;
  std::vector<uint8_t> mem_;
};

class Cache : public MemoryBase {
 public:
  Cache();

 private:
};
