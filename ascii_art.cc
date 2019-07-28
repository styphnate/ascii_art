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
  std::ifstream picture("test6.bmp", std::ios::binary);
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

  // must be divisable by 3
  uint32_t res = 9;
  constexpr static uint32_t pixelDataWidth = 3;

  std::vector<uint32_t> output(((width * pixelDataWidth * height) / res) + 1, 0);
  uint32_t widthMultiplier = 0;
  size_t tmpCount = 0;

  for (int32_t i = ((width * pixelDataWidth * height) / res); i >= 0; i -= pixelDataWidth) {
    if (tmpCount >= width) {
      // This could cut lines at the end, this is just temporarily
      if ((i - width * pixelDataWidth * res / 3) <= width * pixelDataWidth * res / 3) {
        break;
      }
      i -= width * pixelDataWidth * res / 3;
      tmpCount = 0;
    }
    ++tmpCount;
    for (uint32_t j = 0; j < res; ++j) {
      if (j != 0 && j % (res / 3) == 0) {
        widthMultiplier += width * pixelDataWidth;
      }
      // RGB not in correct order, but it does not matter for the sake of this program, order is: BGR
      output.at(i) +=
          static_cast<uint32_t>(imageData[i + ((j % (res / 3) + 1) * pixelDataWidth) + widthMultiplier]) +
          static_cast<uint32_t>(imageData[i + 1 + ((j % (res / 3) + 1) * pixelDataWidth) + widthMultiplier]) +
          static_cast<uint32_t>(imageData[i + 2 + ((j % (res / 3) + 1) * pixelDataWidth) + widthMultiplier]);
    }
    tmpCount = 0;
    output.at(i) = (output.at(i) / res);
    widthMultiplier = 0;
  }

  size_t count = 0;
  for (auto i : output) {

    if (i < 50) {
      std::cout << " ";
    }
    if (i > 50 && i < 100) {
      std::cout << ".";
    }
    if (i > 100 && i < 150) {
      std::cout << ",";
    }
    if (i > 150 && i < 200) {
      std::cout << "\"";
    }
    if (i > 200 && i < 250) {
      std::cout << "¤";
    }
    if (i > 250 && i < 300) {
      std::cout << "#";
    }
    if (i > 300 && i < 350) {
      std::cout << "@";
    }
    if (i > 350) {
      std::cout << "$";
    }

    // Creates new lines when width is reached
    if (count >= width / (res / 3)) {
      std::cout << '\n';
      count = 0;
    }
    ++count;
  }
  std::cout << '\n';
}