#include "PawnLoader.h"
#include "LevelEditorErrors.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"

using namespace LevelEditor;
using namespace Enigma::FileSystem;

PawnLoader::PawnLoader()
{

}

PawnLoader::~PawnLoader()
{

}

error PawnLoader::StartLoadingPawn(const std::string& full_path)
{
    if (full_path.empty()) return ErrorCode::emptyFilePath;
    auto file = FileSystem::Instance()->OpenFile(full_path, Read | Binary);
    if (!file) return ErrorCode::fileIOError;

    return ErrorCode::ok;
}

