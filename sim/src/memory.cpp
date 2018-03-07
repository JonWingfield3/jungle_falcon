#include <memory.hpp>

#include <glog/logging.h>
#include <algorithm>
#include <memory>
#include <vector>

void MainMemory::LoadImage(const std::string& image_name) {
  std::ifstream image_stream =
      std::ifstream(image_name, (std::ios::in | std::ios::binary));
  CHECK(image_stream.is_open()) << "Couldn't open image!";

  image_stream.seekg(0, std::ios::end);
  const std::size_t bin_file_size = image_stream.tellg();
  VLOG(2) << "Binary file of size: " << bin_file_size;
  CHECK(bin_file_size <= size_) << "Binary too large!";

  image_stream.seekg(0, std::ios::beg);
  for (std::size_t ii = 0; ii < bin_file_size; ++ii) {
    char buf = 0;
    CHECK(image_stream.read(&buf, 1)) << "Image stream read failed!";
    VLOG(5) << std::hex << std::showbase << "Address: " << ii << ": " << buf;
    mem_.at(ii) = buf;
  }
  size_ = bin_file_size;
}
