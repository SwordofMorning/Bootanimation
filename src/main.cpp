#include "xdrm/xdrm.h"
#include <iostream>
#include <thread>
#include "utils/parser.h"

struct modeset_dev *panel;
uint32_t image_data[640 * 512];
int panel_width{0}, panel_height{0}, offset_x{0}, offset_y{0};

void panel_func()
{
    int fd = xDRM_Init(&panel, CONN_ID_DSI1, CRTC_ID_DSI1, PLANE_ID_DSI1, 640, 512, 0, 0);
    xDRM_Draw(fd, panel);
    xDRM_Exit(fd, panel);
}

int main()
{
    // auto config = Parser::readConfig("/oem/bootanimation/desc.ini");
    // panel_width = config->panel.width;
    // panel_height = config->panel.height;
    // offset_x = config->panel.offset_x;
    // offset_y = config->panel.offset_y;

    std::thread th_panel = std::thread(panel_func);

    // wait to finish initialize
    sleep(1);
    // usleep(300 * 1000);

    int count = 0;
    while (1)
    {
        xDRM_Pattern(image_data, 640, 512, count++);
        xDRM_Push(panel, image_data, sizeof(image_data));
        usleep(16 * 1000);
    }

    if (th_panel.joinable())
        th_panel.join();

    return 0;
}