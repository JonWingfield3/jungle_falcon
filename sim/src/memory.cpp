#include <memory.hpp>

#include <glog/logging.h>
#include <algorithm>
#include <memory>
#include <vector>

void MainMemory::LoadImage(const std::string& image_name,
                           std::size_t stack_size) {
  std::ifstream image_stream =
      std::ifstream(image_name, (std::ios::in | std::ios::binary));
  CHECK(image_stream.is_open()) << "Couldn't open image!";

  image_stream.seekg(0, std::ios::end);
  const std::size_t bin_file_size = image_stream.tellg();
  VLOG(2) << "Binary file of size: " << bin_file_size;

  std::size_t total_mem_size = bin_file_size + 2 * stack_size;
  mem_.resize(total_mem_size, 0);
  VLOG(5) << "Set main memory size to: " << total_mem_size;
  VLOG(5) << "Stack size: " << stack_size
          << ", program size: " << bin_file_size;

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
