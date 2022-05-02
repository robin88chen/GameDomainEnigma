#include "IFile.h"

using namespace Enigma::FileSystem;

IFile::IFile()
{
}

IFile::~IFile()
{
}

IFile::FutureRead IFile::AsyncRead(size_t offset, void* out_buff, size_t size)
{
    return std::async(std::launch::async, &IFile::Read, this, offset, out_buff, size);
}

IFile::FutureWrite IFile::AsyncWrite(size_t offset, void const* in_buff, size_t size)
{
    return std::async(std::launch::async, &IFile::Write, this, offset, in_buff, size);
}
