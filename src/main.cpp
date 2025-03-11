#include "xdrm/xdrm.h"
#include <iostream>
#include <thread>
#include "utils/parser.h"

struct modeset_dev *panel;
uint32_t image_data[1080 * 1920];

void panel_func(int width, int height, int offset_x, int offset_y)
{
    int fd = xDRM_Init(&panel, CONN_ID_DSI1, CRTC_ID_DSI1, PLANE_ID_DSI1, width, height, offset_x, offset_y);
    xDRM_Draw(fd, panel);
    xDRM_Exit(fd, panel);
}

int main()
{
    auto config = Parser::readConfig("/oem/bootanimation/desc.ini");

    std::thread th_panel = std::thread(panel_func, config->panel.width, config->panel.height, config->panel.offset_x, config->panel.offset_y);

    // wait to finish initialize
    usleep(300 * 1000);

    int count = 0;
    while (1)
    {
        xDRM_Pattern(image_data, config->panel.width, config->panel.height, count++);
        xDRM_Push(panel, image_data, config->panel.width * config->panel.height * sizeof(uint32_t));
        usleep(1000 * 1000 / config->parts[0].fps);
    }

    if (th_panel.joinable())
        th_panel.join();

    return 0;
}