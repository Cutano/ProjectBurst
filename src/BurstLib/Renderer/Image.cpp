#include "Image.h"

#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Burst
{
    Image::Image(int width, int height) : m_Width(width), m_Height(height)
    {
        m_Data = new Pixel[width * height];
    }

    Image::~Image()
    {
        delete m_Data;
    }

    Pixel& Image::At(unsigned int row, unsigned int col)
    {
        assert((row + 1) * (col + 1) <= m_Width * m_Height);
        return m_Data[row * m_Width + col];
    }

    Pixel* Image::GetRawData()
    {
        return m_Data;
    }

    bool Image::SaveAsPNG(std::string path)
    {
        return stbi_write_png(path.c_str(), m_Width, m_Height, 3, m_Data, m_Width * 3);
    }

} // namespace Burst
