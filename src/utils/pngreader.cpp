#include "pngreader.h"
#include <png.h>
#include <iostream>

PNGReader::PNGReader() = default;

PNGReader::~PNGReader()
{
    cleanup();
}

void PNGReader::cleanup()
{
    width = 0;
    height = 0;
    imageData.clear();
    loaded = false;
}

void PNGReader::setError(const std::string& error)
{
    lastError = error;
    cleanup();
}

bool PNGReader::loadPNG(const std::string& filepath)
{
    cleanup();

    FILE* fp = fopen(filepath.c_str(), "rb");
    if (!fp) {
        setError("Cannot open file: " + filepath);
        return false;
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        setError("Failed to create PNG read struct");
        return false;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);
        fclose(fp);
        setError("Failed to create PNG info struct");
        return false;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
        setError("Error during PNG reading");
        return false;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    width = png_get_image_width(png, info);
    height = png_get_image_height(png, info);
    png_byte color_type = png_get_color_type(png, info);
    png_byte bit_depth = png_get_bit_depth(png, info);

    if (bit_depth == 16)
        png_set_strip_16(png);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    if (!(color_type & PNG_COLOR_MASK_ALPHA))
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    png_set_bgr(png);

    png_read_update_info(png, info);

    std::vector<uint8_t> tempBuffer(width * height * 4);
    std::vector<png_bytep> row_pointers(height);

    for (uint32_t y = 0; y < height; y++)
    {
        row_pointers[y] = &tempBuffer[y * width * 4];
    }

    png_read_image(png, row_pointers.data());

    imageData.resize(width * height);

    memcpy(imageData.data(), tempBuffer.data(), width * height * 4);

    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);

    loaded = true;
    return true;
}