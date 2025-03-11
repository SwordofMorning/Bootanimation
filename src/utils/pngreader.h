#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <cstring>

class PNGReader {
public:
    PNGReader();
    ~PNGReader();

    // 禁用拷贝
    PNGReader(const PNGReader&) = delete;
    PNGReader& operator=(const PNGReader&) = delete;

    // 加载PNG文件
    bool loadPNG(const std::string& filepath);

    // 获取图像信息
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }
    
    // 获取ARGB888数据
    const std::vector<uint32_t>& getData() const { return imageData; }

    std::vector<uint32_t>& getData() { return imageData; }

    // 检查是否加载成功
    bool isLoaded() const { return loaded; }

    // 获取最后的错误信息
    std::string getLastError() const { return lastError; }

private:
    uint32_t width{0};
    uint32_t height{0};
    std::vector<uint32_t> imageData;
    bool loaded{false};
    std::string lastError;

    // 清理资源
    void cleanup();
    
    // 设置错误信息
    void setError(const std::string& error);
};