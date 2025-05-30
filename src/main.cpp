#include "xdrm/xdrm.h"
#include <iostream>
#include <thread>
#include <filesystem>
#include "utils/parser.h"
#include "utils/pngreader.h"

struct modeset_dev *panel;
uint32_t image_data[1080 * 1920];

void panel_func(int width, int height, int offset_x, int offset_y)
{
    int fd = xDRM_Init(&panel, CONN_ID_DSI1, CRTC_ID_DSI1, PLANE_ID_DSI1, width, height, offset_x, offset_y);
    xDRM_Draw(fd, panel);
    xDRM_Exit(fd, panel);
}

int get_frame_count(const std::string &part_path);

int main()
{
    /* ----- Step 1 : Init ----- */

    // Bootanimation path
    std::string path{"/oemven/bootanimation/"};

    // Read config
    auto config = Parser::readConfig(path + "desc.ini");

    // DRM thread
    std::thread th_panel = std::thread(panel_func, config->panel.width, config->panel.height, config->panel.offset_x, config->panel.offset_y);

    // PNG reader
    auto pngReader = std::make_unique<PNGReader>();

    // Wait to finish initialize
    usleep(100 * 1000);

    /* ----- Step 2 : Display Images ----- */

    /* Traverse all parts */
    for (int i = 0; i < config->getPartsCount(); ++i)
    {
        /* Part[i] is anime */
        if (!config->parts[i].isStatic())
        {
            int part_frames = get_frame_count(path + "part" + std::to_string(i) + "/");

            /* Part[i] loop times */
            for (int j = 0; j < config->parts[i].getLoopCount(); ++j)
            {
                /* Traverse each frame in one part */
                for (int k = 0; k < part_frames; ++k)
                {
                    std::string file = path + "part" + std::to_string(i) + "/" + "frame" + std::to_string(k) + ".png";
                    pngReader->loadPNG(file);
                    xDRM_Push(panel, pngReader->getData().data(), config->panel.width * config->panel.height * sizeof(uint32_t));
                    usleep(1000 * 1000 / config->parts[i].fps);
                }
            }
        }
        /* Part[i] is static */
        else
        {
            std::string file = path + "part" + std::to_string(i) + "/" + "frame" + std::to_string(0) + ".png";
            pngReader->loadPNG(file);
            xDRM_Push(panel, pngReader->getData().data(), config->panel.width * config->panel.height * sizeof(uint32_t));
        }
    }

    /* ----- Step 3 : Exit ----- */
    if (th_panel.joinable())
        th_panel.join();

    return 0;
}

int get_frame_count(const std::string &part_path)
{
    std::vector<std::string> png_files;

    if (!std::filesystem::exists(part_path) || !std::filesystem::is_directory(part_path))
    {
        std::cerr << "Directory not found: " << part_path << std::endl;
        return 0;
    }

    for (const auto &entry : std::filesystem::directory_iterator(part_path))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".png")
        {
            png_files.push_back(entry.path().filename().string());
        }
    }

    return png_files.size();
}