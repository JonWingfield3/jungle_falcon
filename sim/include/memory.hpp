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
  MemoryBase() : size_(0) {}

  mem_addr_t Size() const { return size_; }

  virtual uint8_t ReadByte(mem_addr_t addr) = 0;
  virtual void WriteByte(mem_addr_t addr, uint8_t data) = 0;

  virtual uint16_t ReadHalfWord(mem_addr_t addr) = 0;
  virtual void WriteHalfWord(mem_addr_t addr, uint16_t data) = 0;

  virtual uint32_t ReadWord(mem_addr_t addr) = 0;
  virtual void WriteWord(mem_addr_t addr, uint32_t data) = 0;

  void CoreDump(mem_addr_t start_addr, mem_addr_t end_addr = 0,
                std::ostream& output_stream = std::cout,
                std::size_t width = 4) {
    output_stream << "\n\nMemory Dump\nMemory Size = " << size_ << std::endl;
    const mem_addr_t actual_end_addr = (end_addr == 0) ? size_ : end_addr;
    CHECK(start_addr < size_) << "Start address is out of bounds";
    CHECK(end_addr < size_) << "End address is out of bounds";

    for (std::size_t ii = start_addr; ii < actual_end_addr - width;
         ii += width) {
      int word = static_cast<int>(ReadWord(ii));
      if (ii > program_size_) {
        char* ptr = reinterpret_cast<char*>(&word);
        char temp = ptr[0];
        ptr[0] = ptr[3];
        ptr[3] = temp;
        temp = ptr[1];
        ptr[1] = ptr[2];
        ptr[2] = temp;
      }

      output_stream << std::hex << std::showbase << ii << "\t"
                    << std::setfill('0') << std::setw(8) << word << "  "
                    << std::endl;
    }
    output_stream << std::endl;
  }

 protected:
  mem_addr_t size_;
  mem_addr_t program_size_;
};

using MemoryPtr = std::shared_ptr<MemoryBase>;

class MainMemory : public MemoryBase {
 public:
  MainMemory(const std::string& image_name = "", std::size_t stack_size = 0)
      : MemoryBase() {
    if (image_name != "") {
      LoadImage(image_name, stack_size);
    }
  }

  uint8_t ReadByte(mem_addr_t addr) { return Read<uint8_t>(addr); }

  void WriteByte(mem_addr_t addr, uint8_t data) { Write<uint8_t>(addr, data); }

  uint16_t ReadHalfWord(mem_addr_t addr) { return Read<uint16_t>(addr); }
  void WriteHalfWord(mem_addr_t addr, uint16_t data) {
    Write<uint16_t>(addr, data);
  }

  uint32_t ReadWord(mem_addr_t addr) { return Read<uint32_t>(addr); }
  void WriteWord(mem_addr_t addr, uint32_t data) {
    Write<uint32_t>(addr, data);
  }

 private:
  void LoadImage(const std::string& image_name, std::size_t stack_size = 1024);

  template <typename data_t>
  data_t Read(mem_addr_t mem_addr) const {
    CHECK(mem_addr < size_)
        << "Attempting to read from invalid address: " << std::hex
        << std::showbase << mem_addr;

    const data_t* read_ptr =
        reinterpret_cast<const data_t*>(mem_.data() + mem_addr);
    const data_t data = *read_ptr;
    VLOG(5) << "Read " << std::hex << std::showbase << data << " from address "
            << mem_addr;
    return data;
  }

  template <typename data_t>
  void Write(mem_addr_t mem_addr, data_t data) {
    CHECK(mem_addr < size_)
        << "Attempting to write to invalid address: " << std::hex
        << std::showbase << mem_addr;

    CHECK(mem_addr >= program_size_)
        << "Error! Writing to program memory, at address " << std::hex
        << std::showbase << mem_addr;

    data_t* write_ptr = reinterpret_cast<data_t*>(mem_.data() + mem_addr);
    *write_ptr = data;
    VLOG(5) << std::hex << std::showbase << "Writing " << static_cast<int>(data)
            << " to address " << mem_addr;
  }

  static constexpr std::size_t kDefaultMemSize = 65535;
  std::vector<uint8_t> mem_;
};

class Cache : public MemoryBase {
 public:
  Cache();

 private:
};
