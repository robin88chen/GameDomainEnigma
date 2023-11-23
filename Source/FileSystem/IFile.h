/********************************************************************
 * \file   IFile.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2022
 *********************************************************************/
#ifndef _FILE_INTERFACE_H
#define _FILE_INTERFACE_H

#include <string>
#include <future>
#include <optional>
#include <vector>

namespace Enigma::FileSystem
{
    using error = std::error_code;
    enum class ErrorCode;
    class IFile
    {
    public:
        using FutureRead = std::future<std::optional<std::vector<unsigned char>>>;
        using FutureWrite = std::future<size_t>;
    public:
        IFile();
        IFile(const IFile&) = delete;
        IFile(IFile&&) = delete;
        virtual ~IFile();
        IFile& operator=(const IFile&) = delete;
        IFile& operator=(IFile&&) = delete;

        virtual std::string GetFullPath() = 0;

        virtual std::optional<std::vector<unsigned char>> Read(size_t offset, size_t size_request) = 0;
        virtual FutureRead AsyncRead(size_t offset, size_t size_request);
        virtual size_t  Write(size_t offset, const std::vector<unsigned char>& in_buff) = 0;
        virtual FutureWrite AsyncWrite(size_t offset, const std::vector<unsigned char>& in_buff);

        virtual size_t Size() = 0;
        /** 檔案時間 (最後修改時間) \n 一個長整數，用ctime, localtime函式去取得其他格式的時間表示法 */
        virtual time_t FileTime() = 0;

        virtual bool IsExisted() = 0;
        virtual bool IsWritable() = 0;

        error LastError() const { return m_lastError; }
    protected:
        friend class FileSystem;
        virtual error open() = 0;
        virtual error close() = 0;

        error MakeErrorCode(ErrorCode error_code);
        ErrorCode LastErrorCode();

    protected:
        error m_lastError;
    };
    using IFilePtr = std::shared_ptr<IFile>;
    using FutureFile = std::future<IFilePtr>;
}

#endif // !_FILE_INTERFACE_H
