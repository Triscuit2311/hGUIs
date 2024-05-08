#pragma once
#include "pch.h"

inline std::vector<uint8_t> ReadFileToByteArray(const std::string& filename)
{
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);

    // Get the length of the file
    file.seekg(0, std::ios::end);
    const std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Create a vector to store the contents of the file
    std::vector<uint8_t> fileData(fileSize);

    // Read the file into the vector
    if (file.read(reinterpret_cast<char*>(fileData.data()), fileSize))
    {
        return fileData;
    }

    return {};
}