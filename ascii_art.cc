#include <fstream>
#include <iostream>
#include <vector>

#define HEADERSIZE 54

/*
This function will give the next multiple of alignment given the value. This function only works with a multiple of 2

Example: Next multiple of 4 with input value 10 is 12:
value = 10          10  = 1010
alignment = 4       4   =  100
value + alignment - 1   = 1101

(1101) AND (NOT(0011))  = 1100 = 12
*/

template <typename T> constexpr T alignMultipleOfTwo(const T &value, const T &alignment) {
  return (value + alignment - 1) & ~(alignment - 1);
}

int main() {
  std::ifstream picture("test1.bmp", std::ios::binary);
  if (!picture.is_open()) {
    std::cerr << "Unable to open picture." << std::endl;
    exit(1);
  }

  std::vector<char> header(HEADERSIZE);
  picture.read(header.data(), HEADERSIZE);

  /* Shifts for little endian */
  // Offset to pixel array (image data)
  uint32_t offset = (static_cast<uint32_t>(header[10]) << 0) + (static_cast<uint32_t>(header[11]) << 8) +
                    (static_cast<uint32_t>(header[12]) << 16) + (static_cast<uint32_t>(header[13]) << 24);
  // Width (pixels)
  uint32_t width = (static_cast<uint32_t>(header[18]) << 0) + (static_cast<uint32_t>(header[19]) << 8) +
                   (static_cast<uint32_t>(header[20]) << 16) + (static_cast<uint32_t>(header[21]) << 24);
  // Height (pixels)
  uint32_t height = (static_cast<uint32_t>(header[22]) << 0) + (static_cast<uint32_t>(header[23]) << 8) +
                    (static_cast<uint32_t>(header[24]) << 16) + (static_cast<uint32_t>(header[25]) << 24);

  // Disgard everything before the actual data
  picture.ignore(offset - HEADERSIZE);

  int sizeOfData = alignMultipleOfTwo(width * 3, (uint32_t)4) * height;

  std::vector<char> imageData;
  // Reads the data
  try {
    imageData.resize(sizeOfData);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  picture.read(imageData.data(), sizeOfData);
  // Done with the ifstream
  picture.close();

  size_t count = 0;

  // RGB not in correct order, but it does not matter for the sake of this program, order is: BGR
  for (int32_t i = width*3 * height ; i >= 0 ; i -= 3) {
    // Add all colors together
    uint32_t RGB = static_cast<uint32_t>(imageData[i]) + static_cast<uint32_t>(imageData[i + 1]) +
                   static_cast<uint32_t>(imageData[i + 2]);

    if (RGB < 50) {
      std::cout << " ";
    }
    if (RGB > 50 && RGB < 100) {
      std::cout << ".";
    }
    if (RGB > 100 && RGB < 150) {
      std::cout << ",";
    }
    if (RGB > 150 && RGB < 200) {
      std::cout << "\"";
    }
    if (RGB > 200 && RGB < 250) {
      std::cout << "¤";
    }
    if (RGB > 250 && RGB < 300) {
      std::cout << "#";
    }
    if (RGB > 300 && RGB < 350) {
      std::cout << "@";
    }
    if (RGB > 350) {
      std::cout << "$";
    }

    // Creates new lines when width is reached
    if (count >= width) {
      std::cout << '\n';
      count = 0;
    }
    ++count;
  }
  std::cout << '\n';
}