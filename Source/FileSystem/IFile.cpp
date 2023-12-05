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

IFile::FutureRead IFile::asyncRead(size_t offset, size_t size_request)
{
    return std::async(std::launch::async, &IFile::read, this, offset, size_request);
}

IFile::FutureWrite IFile::asyncWrite(size_t offset, const std::vector<unsigned char>& in_buff)
{
    return std::async(std::launch::async, &IFile::write, this, offset, in_buff);
}

error IFile::makeErrorCode(ErrorCode error_code)
{
    m_lastError = make_error_code(error_code);
    return m_lastError;
}
ErrorCode IFile::lastErrorCode()
{
    return static_cast<ErrorCode>(m_lastError.value());
}