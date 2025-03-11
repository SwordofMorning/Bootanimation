#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>

class PNGReader
{
private:
    uint32_t width{0};
    uint32_t height{0};
    std::vector<uint32_t> imageData;
    bool loaded{false};
    std::string lastError;

    void cleanup();
    void setError(const std::string& error);

public:
    PNGReader();
    ~PNGReader();

    PNGReader(const PNGReader&) = delete;
    PNGReader& operator=(const PNGReader&) = delete;

    bool loadPNG(const std::string& filepath);

    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
    
    // Get BGRA888
    const std::vector<uint32_t>& getData() const { return imageData; }
    std::vector<uint32_t>& getData() { return imageData; }

    bool isLoaded() const { return loaded; }

    std::string getLastError() const { return lastError; }
};