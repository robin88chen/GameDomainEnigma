#include "TextureEgl.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include "GraphicKernel/GraphicErrors.h"
#include "GraphicKernel/GraphicEvents.h"
#include "Frameworks/EventPublisher.h"
#include "MathLib/Rect.h"
#include "FileSystem/IFile.h"
#include <cassert>

using namespace Enigma::Devices;
using ErrorCode = Enigma::Graphics::ErrorCode;
#if TARGET_PLATFORM == PLATFORM_ANDROID
#include "png.h"

#endif