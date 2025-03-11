#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <sstream>

class PanelConfig
{
public:
    int width{0};
    int height{0};
    int offset_x{0};
    int offset_y{0};
};

class PartConfig
{
public:
    int fps{0};
    int loop{1};

    bool isStatic() const;
    int getLoopCount() const;
};

class AnimationConfig
{
public:
    PanelConfig panel;
    std::vector<PartConfig> parts;

    size_t getPartsCount() const;
    void printConfig() const;
};

class Parser
{
private:
    static std::string trim(const std::string &str);
    static std::pair<std::string, std::string> parseKeyValue(const std::string &line);

public:
    static std::unique_ptr<AnimationConfig> readConfig(const std::string &filepath);
};