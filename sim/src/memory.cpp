#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <vector>

#include <glog/logging.h>

#include <memory.hpp>

////////////////////////////////////////////////////////////////////////////////
MemoryBase::MemoryBase(std::size_t size, std::size_t latency)
    : HardwareObject(), size_(size), latency_(latency), last_latency_(0) {}

////////////////////////////////////////////////////////////////////////////////
void MemoryBase::ExecuteCycle() { HardwareObject::ExecuteCycle(); }

////////////////////////////////////////////////////////////////////////////////
void MemoryBase::Reset() {
  last_latency_ = 0;
  HardwareObject::Reset();
}

////////////////////////////////////////////////////////////////////////////////
std::size_t MemoryBase::GetLatency() { return latency_; }

////////////////////////////////////////////////////////////////////////////////
std::size_t MemoryBase::GetAccessLatency() { return last_latency_; }

////////////////////////////////////////////////////////////////////////////////
void MemoryBase::CoreDump(mem_addr_t start_addr, mem_addr_t end_addr,
                          std::ostream& output_stream, std::size_t width) {
  output_stream << "\n\nMemory Dump\nMemory Size = " << size_ << std::endl;
  const mem_addr_t actual_end_addr = (end_addr == 0) ? size_ : end_addr;
  CHECK(start_addr < size_) << "Start address is out of bounds";
  CHECK(end_addr < size_) << "End address is out of bounds";
  const int width_scaler = sizeof(instr_t) * width;

  const int num_loops = static_cast<int>((float)(actual_end_addr - start_addr) /
                                         (float)width_scaler);
  for (std::size_t ii = start_addr; ii < start_addr + num_loops; ++ii) {
    const int offset = (ii - start_addr) * width_scaler + start_addr;
    output_stream << std::hex << "[0x" << offset << "]\t";
    for (int jj = 0; jj < width_scaler; jj += sizeof(instr_t)) {
      int word = static_cast<int>(ReadWord(offset + jj));
      std::cout << "0x" << std::setw(8) << std::setfill('0') << word << "  ";
    }
    std::cout << std::endl;
  }
  output_stream << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
MainMemoryBase::MainMemoryBase(std::size_t size, std::size_t first_word_latency)
    : MemoryBase(size, first_word_latency) {
  mem_.resize(size_, 0);
}

////////////////////////////////////////////////////////////////////////////////
uint8_t MainMemoryBase::ReadByte(mem_addr_t addr) {
  uint8_t data;
  Read<uint8_t>(addr, data);
  return data;
}

////////////////////////////////////////////////////////////////////////////////
void MainMemoryBase::WriteByte(mem_addr_t addr, uint8_t data) {
  Write<uint8_t>(addr, data);
}

////////////////////////////////////////////////////////////////////////////////
uint16_t MainMemoryBase::ReadHalfWord(mem_addr_t addr) {
  uint16_t data;
  Read<uint16_t>(addr, data);
  return data;
}

////////////////////////////////////////////////////////////////////////////////
void MainMemoryBase::WriteHalfWord(mem_addr_t addr, uint16_t data) {
  Write<uint16_t>(addr, data);
}

////////////////////////////////////////////////////////////////////////////////
uint32_t MainMemoryBase::ReadWord(mem_addr_t addr) {
  uint32_t data;
  Read<uint32_t>(addr, data);
  return data;
}

////////////////////////////////////////////////////////////////////////////////
void MainMemoryBase::WriteWord(mem_addr_t addr, uint32_t data) {
  Write<uint32_t>(addr, data);
}

////////////////////////////////////////////////////////////////////////////////
InstructionMemory::InstructionMemory(const std::string& image_name,
                                     std::size_t latency, std::size_t size)
    : MainMemoryBase(size, latency) {
  LoadImage(image_name);
}

////////////////////////////////////////////////////////////////////////////////
void InstructionMemory::LoadImage(const std::string& image_name) {
  CHECK(!image_name.empty()) << "Please provide image!";

  std::ifstream image_stream =
      std::ifstream(image_name, (std::ios::in | std::ios::binary));
  CHECK(image_stream.is_open()) << "Couldn't open image!";

  image_stream.seekg(0, std::ios::end);
  const std::size_t bin_file_size = image_stream.tellg();
  VLOG(2) << "Binary file of size: " << bin_file_size;
  CHECK(bin_file_size <= size_) << "Image too large for instruction memory!";

  image_stream.seekg(0, std::ios::beg);
  for (std::size_t ii = 0; ii < bin_file_size; ++ii) {
    char buf = 0;
    CHECK(image_stream.read(&buf, 1)) << "Image stream read failed!";
    VLOG(5) << std::hex << std::showbase << "Address: " << ii << ": "
            << static_cast<int>(buf);
    mem_.at(ii) = buf;
  }
}

////////////////////////////////////////////////////////////////////////////////
DataMemory::DataMemory(std::size_t latency, std::size_t size)
    : MainMemoryBase(size, latency) {}

////////////////////////////////////////////////////////////////////////////////
CacheBase::CacheBase(MemoryPtr mem, std::size_t line_size_bytes,
                     std::size_t num_lines, std::size_t set_associativity,
                     std::size_t latency, std::size_t subsequent_latency)
    : MemoryBase((line_size_bytes_ * num_lines_), latency),
      main_mem_(mem),
      line_size_bytes_(line_size_bytes),
      num_lines_(num_lines / set_associativity),
      set_associativity_(set_associativity),
      subsequent_latency_(subsequent_latency) {
  swapin_counter_max_ = line_size_bytes / sizeof(word_t) * subsequent_latency;
  caches_.resize(
      set_associativity_,
      std::vector<CacheLine>(num_lines_, CacheLine(line_size_bytes_)));
}

////////////////////////////////////////////////////////////////////////////////
void CacheBase::ExecuteCycle() {
  for (auto& cache : caches_) {
    for (auto& line : cache) {
      if (line.swapin_counter < swapin_counter_max_) {
        ++line.swapin_counter;
      }
    }
  }
  MemoryBase::ExecuteCycle();
}

////////////////////////////////////////////////////////////////////////////////
void CacheBase::Reset() {
  caches_.clear();
  caches_.resize(
      set_associativity_,
      std::vector<CacheLine>(num_lines_, CacheLine(line_size_bytes_)));
  num_hits_ = 0;
  num_misses_ = 0;
  MemoryBase::Reset();
}

////////////////////////////////////////////////////////////////////////////////
uint8_t CacheBase::ReadByte(mem_addr_t addr) {
  uint8_t read_data = 0;
  Read<uint8_t>(addr, read_data);
  return read_data;
}

////////////////////////////////////////////////////////////////////////////////
void CacheBase::WriteByte(mem_addr_t addr, uint8_t data) {
  Write<uint8_t>(addr, data);
}

////////////////////////////////////////////////////////////////////////////////
uint16_t CacheBase::ReadHalfWord(mem_addr_t addr) {
  uint16_t read_data = 0;
  Read<uint16_t>(addr, read_data);
  return read_data;
}

////////////////////////////////////////////////////////////////////////////////
void CacheBase::WriteHalfWord(mem_addr_t addr, uint16_t data) {
  Write<uint16_t>(addr, data);
}

////////////////////////////////////////////////////////////////////////////////
uint32_t CacheBase::ReadWord(mem_addr_t addr) {
  uint32_t read_data = 0;
  Read<uint32_t>(addr, read_data);
  return read_data;
}

////////////////////////////////////////////////////////////////////////////////
void CacheBase::WriteWord(mem_addr_t addr, uint32_t data) {
  Write<uint32_t>(addr, data);
}

////////////////////////////////////////////////////////////////////////////////
std::size_t CacheBase::GetTag(mem_addr_t addr) const {
  const std::size_t tag_offset = __builtin_ctz(line_size_bytes_ * num_lines_);
  const std::size_t tag = addr >> tag_offset;
  return tag;
}

////////////////////////////////////////////////////////////////////////////////
std::size_t CacheBase::GetLineIndex(mem_addr_t addr) const {
  const std::size_t line_idx_offset = __builtin_ctz(line_size_bytes_);
  const std::size_t line_idx_mask = num_lines_ - 1;
  const std::size_t line_idx = (addr >> line_idx_offset) & line_idx_mask;
  return line_idx;
}

////////////////////////////////////////////////////////////////////////////////
std::size_t CacheBase::GetLineOffset(mem_addr_t addr) const {
  const std::size_t line_offset_mask = line_size_bytes_ - 1;
  const std::size_t line_offset = addr & line_offset_mask;
  return line_offset;
}

////////////////////////////////////////////////////////////////////////////////
mem_addr_t CacheBase::GetAddress(std::size_t tag, std::size_t line_index,
                                 std::size_t line_offset) {
  const mem_addr_t mem_addr =
      ((tag << __builtin_ctz(num_lines_ * line_size_bytes_)) |
       (line_index << __builtin_ctz(line_size_bytes_)) | (line_offset));
  return mem_addr;
}

////////////////////////////////////////////////////////////////////////////////
mem_addr_t CacheBase::GetBaseAddress(mem_addr_t mem_addr) const {
  const mem_addr_t base_mem_addr =
      GetTag(mem_addr) << __builtin_ctz(num_lines_ * line_size_bytes_);
  return base_mem_addr;
}

////////////////////////////////////////////////////////////////////////////////
CacheBase::CacheLine& CacheBase::LocateLine(mem_addr_t mem_addr) {
  std::size_t set = 0;
  const bool hit = FindLine(mem_addr, set);
  if (!hit) {
    VLOG(3) << "Cache miss!";
    set = HandleCacheMiss(mem_addr);
    ++num_misses_;
    last_latency_ = main_mem_->GetLatency() + latency_;
  } else {
    ++num_hits_;
    last_latency_ = latency_;
  }
  CacheLine& line = Line(set, mem_addr);
  // Update timestamp (used for LRU)
  line.timestamp = cycle_counter_;
  if (line.swapin_counter < swapin_counter_max_) {
    last_latency_ += subsequent_latency_;
  }
  return line;
}

////////////////////////////////////////////////////////////////////////////////
bool CacheBase::FindLine(mem_addr_t mem_addr, std::size_t& set) const {
  const std::size_t line_idx = GetLineIndex(mem_addr);
  const std::size_t tag = GetTag(mem_addr);
  for (set = 0; set < set_associativity_; ++set) {
    if (caches_.at(set).at(line_idx).tag == tag &&
        caches_.at(set).at(line_idx).valid_bit) {
      break;
    }
  }
  return (set != set_associativity_);
}

////////////////////////////////////////////////////////////////////////////////
const CacheBase::CacheLine& CacheBase::Line(std::size_t set,
                                            mem_addr_t mem_addr) const {
  const std::size_t line_idx = GetLineIndex(mem_addr);
  return caches_.at(set).at(line_idx);
}

////////////////////////////////////////////////////////////////////////////////
CacheBase::CacheLine& CacheBase::Line(std::size_t set, mem_addr_t mem_addr) {
  const std::size_t line_idx = GetLineIndex(mem_addr);
  return caches_.at(set).at(line_idx);
}

////////////////////////////////////////////////////////////////////////////////
void CacheBase::ReadLine(mem_addr_t mem_addr, CacheLine& cache_line) {
  // determine base address of line
  //  VLOG(1) << "Read MemAddr: " << mem_addr;
  const mem_addr_t line_base_addr = (mem_addr & ~(line_size_bytes_ - 1));
  const mem_addr_t line_end_addr = line_base_addr + line_size_bytes_;
  VLOG(1) << "Reading in line from addresses: " << line_base_addr << " - "
          << line_end_addr - 1;
  for (mem_addr_t line_addr = 0; line_addr < line_size_bytes_; ++line_addr) {
    const uint8_t next_byte = main_mem_->ReadByte(line_addr + line_base_addr);
    cache_line.line.at(line_addr) = next_byte;
  }
  cache_line.tag = GetTag(mem_addr);
  cache_line.dirty_bit = false;
  cache_line.valid_bit = true;
}

////////////////////////////////////////////////////////////////////////////////
void CacheBase::WriteLine(mem_addr_t mem_addr,
                          const CacheLine& cache_line) const {
  // determine base address of line
  mem_addr_t line_addr = (mem_addr & ~(line_size_bytes_ - 1));
  for (const auto& byte : cache_line.line) {
    main_mem_->WriteByte(line_addr++, byte);
  }
}

////////////////////////////////////////////////////////////////////////////////
std::size_t CacheBase::HandleCacheMiss(mem_addr_t mem_addr) {
  std::size_t new_set = EvictLine(mem_addr);
  CacheLine& new_line = Line(new_set, mem_addr);
  new_line.swapin_counter = 0;
  const std::size_t new_tag = GetTag(mem_addr);
  const std::size_t new_line_index = GetLineIndex(mem_addr);
  const std::size_t new_line_offset = 0;
  const mem_addr_t new_mem_addr =
      GetAddress(new_tag, new_line_index, new_line_offset);
  ReadLine(new_mem_addr, new_line);
  return new_set;
}

////////////////////////////////////////////////////////////////////////////////
DirectlyMappedCache::DirectlyMappedCache(MemoryPtr main_mem,
                                         std::size_t line_size_bytes,
                                         std::size_t num_lines,
                                         std::size_t latency,
                                         std::size_t subsequent_latency)
    : CacheBase(main_mem, line_size_bytes, num_lines, 1, latency,
                subsequent_latency) {}

////////////////////////////////////////////////////////////////////////////////
std::size_t DirectlyMappedCache::EvictLine(mem_addr_t new_addr) {
  // grabs line in first (and only) directly-mapped cache
  CacheLine& evict_line = Line(0, new_addr);
  const std::size_t tag = evict_line.tag;
  const std::size_t line_index = GetLineIndex(new_addr);
  const std::size_t line_offset = 0;
  const mem_addr_t wb_mem_addr = GetAddress(tag, line_index, line_offset);
  if (evict_line.valid_bit && evict_line.dirty_bit) {
    WriteLine(wb_mem_addr, evict_line);
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
LRUCache::LRUCache(MemoryPtr main_mem, std::size_t line_size_bytes,
                   std::size_t num_lines, std::size_t set_associativity,
                   std::size_t latency, std::size_t subsequent_latency)
    : CacheBase(main_mem, line_size_bytes, num_lines, set_associativity,
                latency, subsequent_latency) {}

////////////////////////////////////////////////////////////////////////////////
std::size_t LRUCache::EvictLine(mem_addr_t new_addr) {
  std::size_t min_set = 0;
  std::size_t min_ticks = std::numeric_limits<std::size_t>::max();

  for (std::size_t set_idx = 0; set_idx < set_associativity_; ++set_idx) {
    const std::vector<CacheBase::CacheLine>& next_set = caches_.at(set_idx);
    const CacheLine& next_min_tick_line =
        *std::min_element(next_set.cbegin(), next_set.cend(),
                          [](const CacheLine& lhs, const CacheLine& rhs) {
                            return lhs.timestamp < rhs.timestamp;
                          });
    if (next_min_tick_line.timestamp < min_ticks) {
      min_ticks = next_min_tick_line.timestamp;
      min_set = set_idx;
    }
  }

  CacheLine& evict_line = Line(min_set, new_addr);
  const std::size_t tag = evict_line.tag;
  const std::size_t line_index = GetLineIndex(new_addr);
  const std::size_t line_offset = 0;
  const mem_addr_t wb_mem_addr = GetAddress(tag, line_index, line_offset);
  if (evict_line.valid_bit && evict_line.dirty_bit) {
    WriteLine(wb_mem_addr, evict_line);
  }

  VLOG(1) << "Evicting: " << min_set << ", Line index: " << line_index;
  return min_set;
}
