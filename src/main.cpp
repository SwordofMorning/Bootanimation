#include "xdrm/xdrm.h"
#include <iostream>
#include <thread>
#include "utils/parser.h"

struct modeset_dev *panel;
uint32_t image_data[1080 * 1920];

void panel_func(uint32_t width, uint32_t height)
{
    int fd = xDRM_Init(&panel, CONN_ID_DSI1, CRTC_ID_DSI1, PLANE_ID_DSI1, width, height, 0, 0);
    xDRM_Draw(fd, panel);
    xDRM_Exit(fd, panel);
}

int main()
{
    auto config = Parser::readConfig("/oem/bootanimation/desc.ini");

    if (config)
    {
        std::thread th_panel = std::thread(panel_func, config->panel.width, config->panel.height);
        // wait to finish initialize
        usleep(100 * 1000);

        int count = 0;
        while (1)
        {
            xDRM_Pattern(image_data, 1080, 1920, count++);
            xDRM_Push(panel, image_data, sizeof(image_data));
            usleep(16666);
        }

        if (th_panel.joinable())
            th_panel.join();
    }

    return 0;
}