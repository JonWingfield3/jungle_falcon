#include <memory.hpp>

#include <glog/logging.h>
#include <algorithm>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
void MemoryBase::CoreDump(mem_addr_t start_addr, mem_addr_t end_addr,
                          std::ostream& output_stream, std::size_t width) {
  return;
  output_stream << "\n\nMemory Dump\nMemory Size = " << size_ << std::endl;
  const mem_addr_t actual_end_addr = (end_addr == 0) ? size_ : end_addr;
  CHECK(start_addr < size_) << "Start address is out of bounds";
  CHECK(end_addr < size_) << "End address is out of bounds";

  for (std::size_t ii = start_addr; ii < actual_end_addr - width;
       ii += sizeof(uint32_t)) {
    output_stream << std::hex << "0x" << ii << "\t";
    for (std::size_t jj = 0; jj < width; ++jj) {
      int word = static_cast<int>(ReadWord(ii + 4 * jj));
      std::cout << "0x" << std::setw(8) << std::setfill('0') << word << "  ";
    }
    std::cout << std::endl;
  }
  output_stream << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
void MainMemory::LoadImage(const std::string& image_name,
                           std::size_t data_size) {
  std::ifstream image_stream =
      std::ifstream(image_name, (std::ios::in | std::ios::binary));
  CHECK(image_stream.is_open()) << "Couldn't open image!";

  image_stream.seekg(0, std::ios::end);
  const std::size_t bin_file_size = image_stream.tellg();
  VLOG(2) << "Binary file of size: " << bin_file_size;

  std::size_t total_mem_size = bin_file_size + data_size;
  mem_.resize(total_mem_size, 0);
  VLOG(5) << "Set main memory size to: " << total_mem_size;
  VLOG(5) << "Stack size: " << data_size << ", program size: " << bin_file_size;

  image_stream.seekg(0, std::ios::beg);
  for (std::size_t ii = 0; ii < bin_file_size; ++ii) {
    char buf = 0;
    CHECK(image_stream.read(&buf, 1)) << "Image stream read failed!";
    VLOG(5) << std::hex << std::showbase << "Address: " << ii << ": "
            << static_cast<int>(buf);
    mem_.at(ii) = buf;
  }

  program_size_ = bin_file_size;
  size_ = total_mem_size;
}
