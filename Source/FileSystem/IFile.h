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

namespace Enigma::FileSystem
{
    using error = std::error_code;
    enum class ErrorCode;
    class IFile
    {
    public:
        using FutureRead = std::future<size_t>;
        using FutureWrite = std::future<size_t>;
    public:
        IFile();
        IFile(const IFile&) = delete;
        IFile(IFile&&) = delete;
        virtual ~IFile();
        IFile& operator=(const IFile&) = delete;
        IFile& operator=(IFile&&) = delete;

        virtual std::string GetFullPath() = 0;

        virtual size_t Read(size_t offset, void* out_buff, size_t size) = 0;
        virtual FutureRead AsyncRead(size_t offset, void* out_buff, size_t size);
        virtual size_t  Write(size_t offset, void const* in_buff, size_t size) = 0;
        virtual FutureWrite AsyncWrite(size_t offset, void const* in_buff, size_t size);

        virtual size_t Size() = 0;
        /** 檔案時間 (最後修改時間) \n 一個長整數，用ctime, localtime函式去取得其他格式的時間表示法 */
        virtual time_t FileTime() = 0;

        virtual bool IsExisted() = 0;
        virtual bool IsWritable() = 0;

        error LastError() const { return m_lastError; }
    protected:
        friend class FileSystem;
        virtual error Open() = 0;
        virtual error Close() = 0;

        error MakeErrorCode(ErrorCode error_code);
    protected:
        error m_lastError;
    };
    using IFilePtr = std::shared_ptr<IFile>;
    using FutureFile = std::future<IFilePtr>;
}

#endif // !_FILE_INTERFACE_H
