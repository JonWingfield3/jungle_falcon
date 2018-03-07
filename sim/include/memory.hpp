#pragma once

#include <glog/logging.h>

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <riscv_defs.hpp>

class MemoryUtils;

class MainMemory {
 public:
  MainMemory(mem_addr_t capacity = kDefaultMemSize) : capacity_(capacity) {
    mem_.resize(capacity);
    std::fill(mem_.begin(), mem_.end(), static_cast<uint8_t>(0));
    VLOG(5) << "Zero filling main memory.. wrote " << capacity << " 0 bytes";
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
  friend class MemoryUtils;

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

  static constexpr int kDefaultMemSize = 1024;
  std::vector<uint8_t> mem_;
  mem_addr_t capacity_;
  mem_addr_t size_;
};

class MemoryUtils {
 public:
  static void LoadImage(MainMemory& main_memory,
                        const std::string& image_name) {
    std::ifstream image_stream =
        std::ifstream(image_name, (std::ios::in | std::ios::binary));
    CHECK(image_stream.is_open()) << "Couldn't open image!";

    image_stream.seekg(0, std::ios::end);
    const std::size_t bin_file_size = image_stream.tellg();
    VLOG(2) << "Binary file of size: " << bin_file_size;
    CHECK(bin_file_size <= main_memory.mem_.size()) << "Binary too large!";

    image_stream.seekg(0, std::ios::beg);
    for (std::size_t ii = 0; ii < bin_file_size; ++ii) {
      uint32_t buf = 0;
      CHECK(image_stream.read(reinterpret_cast<char*>(&buf), 1))
          << "Image stream read failed!";
      main_memory.Write(static_cast<mem_addr_t>(ii), buf);
      VLOG(5) << std::hex << std::showbase << "Address: " << ii << ": " << buf;
    }
    main_memory.size_ = bin_file_size;
  }

  static void CoreDump(const MainMemory& main_memory,
                       std::ostream& output_stream = std::cout,
                       std::size_t width = 4) {
    const mem_addr_t mem_size = main_memory.mem_.size();
    VLOG(2) << "Memory size = " << mem_size;

    output_stream << "\n\nMemory Dump:\n\n";
    for (std::size_t ii = 0; ii < mem_size - width; ii += width) {
      output_stream << std::hex << std::showbase << ii << "\t";
      for (std::size_t jj = width; jj > 0; --jj) {
        output_stream << std::hex << std::showbase << std::setw(4)
                      << static_cast<int>(main_memory.mem_.at(ii + jj - 1))
                      << "  ";
      }
      output_stream << std::endl;
    }
    output_stream << std::endl;
  }
};
