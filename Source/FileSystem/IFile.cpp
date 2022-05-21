#include "IFile.h"
#include "FileSystemErrors.h"

using namespace Enigma::FileSystem;

IFile::IFile()
{
    m_lastError = ErrorCode::ok;
}

IFile::~IFile()
{
}

IFile::FutureRead IFile::AsyncRead(size_t offset, size_t size_request)
{
    return std::async(std::launch::async, &IFile::Read, this, offset, size_request);
}

IFile::FutureWrite IFile::AsyncWrite(size_t offset, void const* in_buff, size_t size)
{
    return std::async(std::launch::async, &IFile::Write, this, offset, in_buff, size);
}

error IFile::MakeErrorCode(ErrorCode error_code)
{
    m_lastError = make_error_code(error_code);
    return m_lastError;
}
ErrorCode IFile::LastErrorCode()
{
    return static_cast<ErrorCode>(m_lastError.value());
}