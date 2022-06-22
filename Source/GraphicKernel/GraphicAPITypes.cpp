#include "GraphicAPITypes.h"

using namespace Enigma::Graphics;
unsigned int GraphicFormat::ColorChannelBits() const
{
    switch (fmt)
    {
    case FMT_R8G8B8:
        return 8;
    case FMT_A8R8G8B8:
        return 8;
    case FMT_A8B8G8R8:
        return 8;
    case FMT_X8R8G8B8:
        return 8;
    case FMT_R5G6B5:
        return 5;
    case FMT_X1R5G5B5:
        return 5;
    case FMT_A1R5G5B5:
        return 5;
    case FMT_A4R4G4B4:
        return 4;
    case FMT_R3G3B2:
        return 2;
    case FMT_A8R3G3B2:
        return 2;
    case FMT_X4R4G4B4:
        return 4;
    case FMT_A2B10G10R10:
        return 10;
    case FMT_A2R10G10B10:
        return 10;
    default:
        return 0;
    }
}

unsigned int GraphicFormat::AlphaChannelBits() const
{
    switch (fmt)
    {
    case FMT_R8G8B8:
        return 0;
    case FMT_A8R8G8B8:
        return 8;
    case FMT_A8B8G8R8:
        return 8;
    case FMT_X8R8G8B8:
        return 0;
    case FMT_R5G6B5:
        return 0;
    case FMT_X1R5G5B5:
        return 0;
    case FMT_A1R5G5B5:
        return 1;
    case FMT_A4R4G4B4:
        return 4;
    case FMT_R3G3B2:
        return 0;
    case FMT_A8R3G3B2:
        return 8;
    case FMT_X4R4G4B4:
        return 0;
    case FMT_A2B10G10R10:
        return 2;
    case FMT_A2R10G10B10:
        return 2;
    default:
        return 0;
    }
}

GraphicFormat GraphicFormat::ColorFormat(int r, int g, int b, int a)
{
    if ((r == 8) && (g == 8) && (b == 8) && (a == 8))
    {
        return GraphicFormat(FMT_A8B8G8R8); //(FMT_A8R8G8B8);
    }
    else if ((r == 8) && (g == 8) && (b == 8) && (a == 0))
    {
        return GraphicFormat(FMT_R8G8B8);
    }
    else if ((r == 4) && (g == 4) && (b == 4) && (a == 4))
    {
        return GraphicFormat(FMT_A4R4G4B4);
    }
    else if ((r == 5) && (g == 6) && (b == 5) && (a == 0))
    {
        return GraphicFormat(FMT_R5G6B5);
    }
    else if ((r == 5) && (g == 5) && (b == 5) && (a == 1))
    {
        return GraphicFormat(FMT_A1R5G5B5);
    }
    else if ((r == 10) && (g == 10) && (b == 10) && (a == 2))
    {
        return GraphicFormat(FMT_A2R10G10B10);
    }
    return GraphicFormat(FMT_UNKNOWN);
}

unsigned int GraphicFormat::PixelBits() const
{
    switch (fmt)
    {
    case FMT_R8G8B8:
        return 24;
    case FMT_A8R8G8B8:
        return 32;
    case FMT_A8B8G8R8:
        return 32;
    case FMT_X8R8G8B8:
        return 32;
    case FMT_R5G6B5:
        return 16;
    case FMT_X1R5G5B5:
        return 16;
    case FMT_A1R5G5B5:
        return 16;
    case FMT_A4R4G4B4:
        return 16;
    case FMT_R3G3B2:
        return 8;
    case FMT_A8R3G3B2:
        return 16;
    case FMT_X4R4G4B4:
        return 16;
    case FMT_A2B10G10R10:
        return 32;
    case FMT_A2R10G10B10:
        return 32;
    default:
        return 0;
    }
}

unsigned int GraphicFormat::DepthBits() const
{
    switch (fmt)
    {
    case FMT_D16:
        return 16;
    case FMT_D15S1:
        return 15;
    case FMT_D24X8:
        return 24;
    case FMT_D24S8:
        return 24;
    case FMT_D24X4S4:
        return 24;
    case FMT_D32:
        return 32;
    default:
        return 0;
    }
}

unsigned int GraphicFormat::StencilBits() const
{
    switch (fmt)
    {
    case FMT_D16:
        return 0;
    case FMT_D15S1:
        return 1;
    case FMT_D24X8:
        return 0;
    case FMT_D24S8:
        return 8;
    case FMT_D24X4S4:
        return 4;
    case FMT_D32:
        return 0;
    default:
        return 0;
    }
}

GraphicFormat GraphicFormat::DepthFormat(int d, int s)
{
    if ((d == 16) && (s == 0))
    {
        return GraphicFormat(FMT_D16);
    }
    else if ((d == 15) && (s == 1))
    {
        return GraphicFormat(FMT_D15S1);
    }
    else if ((d == 24) && (s == 8))
    {
        return GraphicFormat(FMT_D24S8);
    }
    else if ((d == 24) && (s == 4))
    {
        return GraphicFormat(FMT_D24X4S4);
    }
    else if ((d == 32) && (s == 0))
    {
        return GraphicFormat(FMT_D32);
    }
    return GraphicFormat(FMT_UNKNOWN);
}