#pragma once

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <glog/logging.h>

#include <hardware_object.hpp>
#include <riscv_defs.hpp>

class MemoryBase;
using MemoryPtr = std::shared_ptr<MemoryBase>;

////////////////////////////////////////////////////////////////////////////////
class MemoryBase : public HardwareObject {
 public:
  MemoryBase(std::size_t size, std::size_t latency);
  virtual ~MemoryBase() {}

  virtual void ExecuteCycle();
  virtual void Reset();

  virtual uint8_t ReadByte(mem_addr_t addr) = 0;
  virtual void WriteByte(mem_addr_t addr, uint8_t data) = 0;

  virtual uint16_t ReadHalfWord(mem_addr_t addr) = 0;
  virtual void WriteHalfWord(mem_addr_t addr, uint16_t data) = 0;

  virtual uint32_t ReadWord(mem_addr_t addr) = 0;
  virtual void WriteWord(mem_addr_t addr, uint32_t data) = 0;

  virtual void CoreDump(mem_addr_t start_addr, mem_addr_t end_addr = 0,
                        std::ostream& output_stream = std::cout,
                        std::size_t width = 4);

  std::size_t GetLatency();
  std::size_t GetAccessLatency();

 protected:
  mem_addr_t size_;
  std::size_t latency_;
  std::size_t last_latency_;
};

////////////////////////////////////////////////////////////////////////////////
class MainMemoryBase : public MemoryBase {
 public:
  MainMemoryBase(std::size_t size, std::size_t latency = 0);
  ~MainMemoryBase() override = default;

  uint8_t ReadByte(mem_addr_t addr);
  void WriteByte(mem_addr_t addr, uint8_t data);

  uint16_t ReadHalfWord(mem_addr_t addr);
  void WriteHalfWord(mem_addr_t addr, uint16_t data);

  uint32_t ReadWord(mem_addr_t addr);
  void WriteWord(mem_addr_t addr, uint32_t data);

 protected:
  std::vector<uint8_t> mem_;

 private:
  template <typename data_t>
  void Read(mem_addr_t mem_addr, data_t& data) const;

  template <typename data_t>
  void Write(mem_addr_t mem_addr, data_t data);
};

////////////////////////////////////////////////////////////////////////////////
class InstructionMemory : public MainMemoryBase {
 public:
  InstructionMemory(const std::string& image, std::size_t latency = 0,
                    std::size_t size = kDefaultMemSize);

 private:
  static constexpr std::size_t kDefaultMemSize{1 << 15};  // 32k
  void LoadImage(const std::string& image_name);
};

////////////////////////////////////////////////////////////////////////////////
class DataMemory : public MainMemoryBase {
 public:
  DataMemory(std::size_t latency = 0, std::size_t size = kDefaultDataSize);

 private:
  static constexpr std::size_t kDefaultDataSize{1 << 12};  // 4k
};

////////////////////////////////////////////////////////////////////////////////
class CacheBase : public MemoryBase {
 public:
  CacheBase(MemoryPtr main_mem, std::size_t line_size_bytes = 16,
            std::size_t num_lines = 16, std::size_t set_associativity = 1,
            std::size_t latency = 0);
  ~CacheBase() override = default;

  uint8_t ReadByte(mem_addr_t addr);
  void WriteByte(mem_addr_t addr, uint8_t data);

  uint16_t ReadHalfWord(mem_addr_t addr);
  void WriteHalfWord(mem_addr_t addr, uint16_t data);

  uint32_t ReadWord(mem_addr_t addr);
  void WriteWord(mem_addr_t addr, uint32_t data);

 protected:
  struct CacheLine {
    CacheLine(std::size_t line_size_bytes) : line(line_size_bytes, 0) {}
    CacheLine(int tag, std::vector<uint8_t> line_vals)
        : tag(tag), line(line_vals) {}

    std::size_t tag = 0;
    std::vector<uint8_t> line;
    bool dirty_bit = false;
    bool valid_bit = false;
    std::size_t timestamp = 0;
  };

  template <typename data_t>
  void Read(mem_addr_t mem_addr, data_t& data);

  template <typename data_t>
  void Write(mem_addr_t mem_addr, data_t data);

  std::size_t GetTag(mem_addr_t addr) const;
  std::size_t GetLineIndex(mem_addr_t addr) const;
  std::size_t GetLineOffset(mem_addr_t addr) const;
  mem_addr_t GetAddress(std::size_t tag, std::size_t line_index,
                        std::size_t line_offset);

  bool SearchCache(mem_addr_t mem_addr, std::size_t& set) const;
  std::size_t HandleCacheMiss(mem_addr_t addr);

  const CacheLine& Line(std::size_t set, mem_addr_t mem_addr) const;
  CacheLine& Line(std::size_t set, mem_addr_t mem_addr);

  void ReadLine(mem_addr_t mem_addr, CacheLine& cache_line);
  void WriteLine(mem_addr_t mem_addr, const CacheLine& cache_line) const;

  virtual std::size_t EvictLine(mem_addr_t new_addr) = 0;

  MemoryPtr main_mem_;
  std::size_t line_size_bytes_;
  std::size_t num_lines_;
  std::size_t set_associativity_;
  std::vector<std::vector<CacheLine>> lines_;
  std::size_t num_misses_ = 0;
  std::size_t num_hits_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
class DirectlyMappedCache : public CacheBase {
 public:
  DirectlyMappedCache(MemoryPtr main_mem, std::size_t line_size_bytes = 16,
                      std::size_t num_lines = 16, std::size_t latency = 0);

 private:
  std::size_t EvictLine(mem_addr_t new_addr) final;
};

////////////////////////////////////////////////////////////////////////////////
class LRUCache : public CacheBase {
 public:
  LRUCache(MemoryPtr main_mem, std::size_t line_size_bytes = 16,
           std::size_t num_lines = 16, std::size_t set_associativity = 1,
           std::size_t latency = 0);

 private:
  std::size_t EvictLine(mem_addr_t new_addr) final;
};

////////////////////////////////////////////////////////////////////////////////
class RandomCache : public CacheBase {
 public:
  RandomCache(MemoryPtr main_mem, std::size_t line_size_bytes = 16,
              std::size_t num_lines = 16, std::size_t set_associativity = 1,
              std::size_t latency = 0);

 private:
  std::size_t EvictLine(mem_addr_t new_addr) final;
};

//
// Template implementations for MainMemoryBase class
// Included in header to help compiler
//

////////////////////////////////////////////////////////////////////////////////
template <typename data_t>
void MainMemoryBase::Read(mem_addr_t mem_addr, data_t& data) const {
  CHECK(mem_addr < size_) << "Attempting to read from invalid address: "
                          << std::hex << std::showbase << mem_addr;

  const data_t* read_ptr =
      reinterpret_cast<const data_t*>(mem_.data() + mem_addr);
  data = *read_ptr;
  VLOG(5) << "Read " << std::hex << std::showbase << data << " from address "
          << mem_addr;
}

////////////////////////////////////////////////////////////////////////////////
template <typename data_t>
void MainMemoryBase::Write(mem_addr_t mem_addr, data_t data) {
  CHECK(mem_addr < size_) << "Attempting to write to invalid address: "
                          << std::hex << std::showbase << mem_addr;

  data_t* write_ptr = reinterpret_cast<data_t*>(mem_.data() + mem_addr);
  *write_ptr = data;
  VLOG(5) << std::hex << std::showbase << "Writing " << static_cast<int>(data)
          << " to address " << mem_addr;
}

////////////////////////////////////////////////////////////////////////////////
template <typename data_t>
void CacheBase::Read(mem_addr_t mem_addr, data_t& data) {
  std::size_t set = 0;
  last_latency_ = 0;
  if (!SearchCache(mem_addr, set)) {
    VLOG(2) << "Cache miss!";
    set = HandleCacheMiss(mem_addr);
    ++num_misses_;
    last_latency_ += main_mem_->GetLatency();
  } else {
    ++num_hits_;
  }
  last_latency_ += latency_;
  CacheLine& cache_line = Line(set, mem_addr);
  cache_line.timestamp = cycle_counter_;  // only used in LRU cache
  const std::size_t line_offset = GetLineOffset(mem_addr);
  const data_t* data_ptr =
      reinterpret_cast<const data_t*>(cache_line.line.data() + line_offset);
  data = *data_ptr;
}

////////////////////////////////////////////////////////////////////////////////
template <typename data_t>
void CacheBase::Write(mem_addr_t mem_addr, data_t data) {
  std::size_t set = 0;
  last_latency_ = 0;
  if (!SearchCache(mem_addr, set)) {
    VLOG(2) << "Cache miss!";
    set = HandleCacheMiss(mem_addr);
    ++num_misses_;
    last_latency_ += main_mem_->GetLatency();
  } else {
    ++num_hits_;
  }
  last_latency_ += latency_;
  CacheLine& cache_line = Line(set, mem_addr);
  cache_line.dirty_bit = true;
  cache_line.timestamp = cycle_counter_;  // only used in LRU cache
  const std::size_t line_offset = GetLineOffset(mem_addr);
  data_t* data_ptr =
      reinterpret_cast<data_t*>(cache_line.line.data() + line_offset);
  *data_ptr = data;
}
