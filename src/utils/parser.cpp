#include "parser.h"

bool PartConfig::isStatic() const
{
    return fps == 0;
}

int PartConfig::getLoopCount() const 
{
    return isStatic() ? 1 : loop;
}

size_t AnimationConfig::getPartsCount() const
{
    return parts.size(); 
}

void AnimationConfig::printConfig() const
{
    std::cout << "Loaded animation configuration:\n"
              << "Panel: " << panel.width << "x" << panel.height << "\n"
              << "Offset: " << panel.offset_x << "@" << panel.offset_y << "\n"
              << "Parts count: " << parts.size() << "\n";

    for (size_t i = 0; i < parts.size(); ++i)
    {
        std::cout << "Part" << i << " @" << parts[i].fps << "fps"
                  << " (loop=" << parts[i].getLoopCount() << ")\n";
    }
}

std::string Parser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::pair<std::string, std::string> Parser::parseKeyValue(const std::string& line) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return {"", ""};
    }
    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    return {key, value};
}

std::unique_ptr<AnimationConfig> Parser::readConfig(const std::string& filepath)
{
    auto config = std::make_unique<AnimationConfig>();
    
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file " << filepath << std::endl;
        return nullptr;
    }

    bool inPanel = false;
    bool inAnimation = false;
    int currentPart = -1;
    uint8_t paramsRead = 0;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == ';') {
            continue;
        }

        line = trim(line);

        if (line[0] == '[') {
            inPanel = (line == "[panel]");
            inAnimation = (line == "[animation]");
            
            if (line.substr(0, 5) == "[part") {
                try {
                    std::string partNum = line.substr(5, line.length() - 6);
                    currentPart = std::stoi(partNum);
                    if (currentPart >= 0 && 
                        static_cast<size_t>(currentPart) < config->parts.size()) {
                        continue;
                    }
                } catch (...) {
                    currentPart = -1;
                }
            }
            continue;
        }

        auto [key, value] = parseKeyValue(line);
        if (key.empty() || value.empty()) {
            continue;
        }

        try {
            if (inPanel) {
                if (key == "width") {
                    config->panel.width = std::stoi(value);
                    paramsRead |= 1;
                } else if (key == "height") {
                    config->panel.height = std::stoi(value);
                    paramsRead |= 2;
                } else if (key == "offset_x") {
                    config->panel.offset_x = std::stoi(value);
                    paramsRead |= 4;
                } else if (key == "offset_y") {
                    config->panel.offset_y = std::stoi(value);
                    paramsRead |= 8;
                }
            } else if (inAnimation) {
                if (key == "parts") {
                    int partsCount = std::stoi(value);
                    config->parts.resize(partsCount);
                    paramsRead |= 16;
                }
            } else if (currentPart >= 0 && 
                      static_cast<size_t>(currentPart) < config->parts.size()) {
                if (key == "fps") {
                    config->parts[currentPart].fps = std::stoi(value);
                } else if (key == "loop") {
                    int loopCount = std::stoi(value);
                    if (loopCount < 1) {
                        std::cerr << "Invalid loop count: " << loopCount << std::endl;
                        return nullptr;
                    }
                    config->parts[currentPart].loop = loopCount;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing value: " << e.what() << std::endl;
            return nullptr;
        }
    }

    if ((paramsRead & 31) != 31) {
        std::cerr << "Not all required parameters were read\n";
        return nullptr;
    }

    config->printConfig();
    return config;
}