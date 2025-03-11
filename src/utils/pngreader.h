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
    void setError(const std::string &error);

public:
    PNGReader();
    ~PNGReader();

    PNGReader(const PNGReader &) = delete;
    PNGReader &operator=(const PNGReader &) = delete;

    bool loadPNG(const std::string &filepath);

    // Get BGRA888
    const std::vector<uint32_t> &getData() const;
    std::vector<uint32_t> &getData();

    uint32_t getWidth() const;
    uint32_t getHeight() const;

    bool isLoaded() const;
    std::string getLastError() const;
};