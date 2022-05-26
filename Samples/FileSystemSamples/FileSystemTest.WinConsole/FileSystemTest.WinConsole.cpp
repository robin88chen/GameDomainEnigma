// FileSystemTest.WinConsole.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include "FileSystem.h"
#include "MemoryMacro.h"
#include "IFile.h"

using namespace Enigma::FileSystem;

static std::string s_filename[10] =
{
    "mb7013.max", "hga0010101.tga", "msjh.ttf",
    "mb7013.max", "hga0010101.tga", "msjh.ttf",
    "mb7013.max", "hga0010101.tga", "msjh.ttf",
    "mb7013.max",
};

int main()
{
    constexpr unsigned int FILE_MAX = 50;
    std::cout << "Hello World!\n";
    meInitMemoryCheck();
    FileSystem::Create();

    FutureFile futureFile[FILE_MAX];
    IFilePtr files[FILE_MAX];
    for (int i = 0; i < FILE_MAX; i++)
    {
        std::cout << "Open file " << i << " " <<  s_filename[i % 10] << "\n";
        futureFile[i] = FileSystem::Instance()->AsyncOpenFile(s_filename[i % 10], "rb", "");
    }
    unsigned int fileSize[FILE_MAX];
    int ready_files = 0;
    while (ready_files < FILE_MAX)
    {
        for (int i = 0; i < FILE_MAX; i++)
        {
            if (!futureFile[i].valid()) continue;
            if ((futureFile[i].wait_for(std::chrono::milliseconds(1)) != std::future_status::ready))
            {
                std::cout << "File " << i << " Opening.\n";
            }
            else
            {
                std::cout << "File " << i << " Opened.\n";
                files[i] = futureFile[i].get();
                if (files[i])
                {
                    fileSize[i] = files[i]->Size();
                    std::cout << "File " << i << "Size " << fileSize[i] << ".\n";
                }
                ready_files++;
            }
        }
    }
    constexpr unsigned int BUFFER_SIZE = 5000 * 1024;
    std::vector<char> read_buf[FILE_MAX];
    IFile::FutureRead futureRead[FILE_MAX];
    for (int i = 0; i < FILE_MAX; i++)
    {
        if (files[i]) futureRead[i] = files[i]->AsyncRead(0, fileSize[i]); // BUFFER_SIZE);
    }
    int read_done = 0;
    while (read_done < FILE_MAX)
    {
        for (int i = 0; i < FILE_MAX; i++)
        {
            if (!futureRead[i].valid()) continue;
            if (futureRead[i].wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)
            {
                std::cout << "File " << i << " Still reading.\n";
            }
            else
            {
                auto buff = futureRead[i].get();
                if (buff)
                {
                    read_buf[i] = buff.value();
                    std::cout << "File " << i << " Read " << read_buf[i].size() << " bytes done.\n";
                }
                else
                {
                    std::cout << "File " << i << " Read fail " << files[i]->LastError() << "\n";
                }
                read_done++;
                FileSystem::Instance()->CloseFile(files[i]);
            }
        }
    }
    IFilePtr writeFiles[FILE_MAX];
    for (int i = 0; i < FILE_MAX; i++)
    {
        char filename[256];
        sprintf_s(filename, "writetest%d.dat", i);
        std::cout << "Open file " << i << " " << filename << "\n";
        writeFiles[i] = FileSystem::Instance()->OpenFile(filename, "w+b", "");
    }
    IFile::FutureWrite futureWrite[FILE_MAX];
    for (int i = 0; i < FILE_MAX; i++)
    {
        if (writeFiles[i]) futureWrite[i] = writeFiles[i]->AsyncWrite(0, read_buf[i]); // BUFFER_SIZE);
    }
    int write_done = 0;
    while (write_done < FILE_MAX)
    {
        for (int i = 0; i < FILE_MAX; i++)
        {
            if (!futureWrite[i].valid()) continue;
            if (futureWrite[i].wait_for(std::chrono::milliseconds(1)) != std::future_status::ready)
            {
                std::cout << "File " << i << " Still writing.\n";
            }
            else
            {
                std::cout << "File " << i << " Write " << futureWrite[i].get() << " bytes done.\n";
                write_done++;
                FileSystem::Instance()->CloseFile(writeFiles[i]);
            }
        }
    }
    for (int i = 0; i < FILE_MAX; i++)
    {
        files[i] = nullptr;
        writeFiles[i] = nullptr;
    }

    delete FileSystem::Instance();
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
