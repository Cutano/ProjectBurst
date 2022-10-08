#pragma once

#include <string>

namespace Burst
{
    struct Pixel
    {
        unsigned char r{0};
        unsigned char g{0};
        unsigned char b{0};
    };

    class Image
    {
    private:
        Pixel *m_Data{nullptr};
        int m_Width, m_Height;

    public:
        Image(int width, int height);
        ~Image();

        Pixel& At(unsigned int row, unsigned int col);
        Pixel* GetRawData();
        bool SaveAsPNG(const std::string& path) const;

        [[nodiscard]] int GetWidth() const
        {
            return m_Width;
        }

        [[nodiscard]] int GetHeight() const
        {
            return m_Height;
        }
    };
    
} // namespace Burst
