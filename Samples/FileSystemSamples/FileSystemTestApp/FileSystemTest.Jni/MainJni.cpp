/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include "PlatformLayerUtilities.h"
#include "AndroidBridge.h"
#include "FileSystem.h"
#include "MemoryMacro.h"
#include <iostream>

class androidbuf : public std::streambuf {
public:
    enum { bufsize = 128 }; // ... or some other suitable buffer size
    androidbuf() { this->setp(buffer, buffer + bufsize - 1); }

private:
    int overflow(int c)
    {
        if (c == traits_type::eof()) {
            *this->pptr() = traits_type::to_char_type(c);
            this->sbumpc();
        }
        return this->sync() ? traits_type::eof() : traits_type::not_eof(c);
    }

    int sync()
    {
        int rc = 0;
        if (this->pbase() != this->pptr()) {
            char writebuf[bufsize + 1];
            memcpy(writebuf, this->pbase(), this->pptr() - this->pbase());
            writebuf[this->pptr() - this->pbase()] = '\0';

            rc = __android_log_write(ANDROID_LOG_INFO, "std", writebuf) > 0;
            this->setp(buffer, buffer + bufsize - 1);
        }
        return rc;
    }

    char buffer[bufsize];
};

using namespace Enigma::FileSystem;
using namespace Enigma::Platforms;

static std::string s_filename[10] =
{
    "mb7013.max", "hga0010101.tga", "msjh.ttf",
    "mb7013.max", "hga0010101.tga", "msjh.ttf",
    "mb7013.max", "hga0010101.tga", "msjh.ttf",
    "mb7013.max",
};

constexpr unsigned int FILE_MAX = 10;
IFilePtr files[FILE_MAX];
std::vector<char> read_buf[FILE_MAX];
IFilePtr writeFiles[FILE_MAX];
/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"
JNIEXPORT void JNICALL
Java_com_FileSystemTestApp_FileSystemTestApp_initBridgeCallback( JNIEnv* env, jobject thiz )
{
    //std::cout.rdbuf(new androidbuf);
    // Enter code here
}

extern "C"
JNIEXPORT void JNICALL
Java_com_FileSystemTestApp_FileSystemTestApp_initAdditionalMountPath(JNIEnv * env, jobject thiz)
{
    //std::string path = Enigma::AndroidBridge::GetDataFilePath();
    //DebugPrintf("Data File Path %s", path.c_str());
    Debug::Printf("Test Message Begin");
    /*char u8s[] = u8"Testing 測試";
    char ss[1024];
    sprintf(ss, u8"Testing 測試");
    DebugPrintf(ss);
    DebugPrintf("Test Message End");
    if (Enigma::FileSystem::Instance())
    {
        Enigma::FileSystem::Instance()->AddMountPath(new Enigma::AndroidMountPath("entities/", Enigma::GeometryData::GetDataFilePathID()));
    }*/
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_FileSystemTestApp_FileSystemTestApp_getDataFilePath(JNIEnv * env, jobject thiz)
{
    std::string strPath = AndroidBridge::GetDataFilePath();
    std::string msg = "data file path : " + strPath;
    return env->NewStringUTF(msg.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_FileSystemTestApp_FileSystemTestApp_startStdioTest(JNIEnv * env, jobject thiz)
{
    FileSystem::Create();
    std::string data_path = AndroidBridge::GetDataFilePath();

    FutureFile futureFile[FILE_MAX];
    for (int i = 0; i < FILE_MAX; i++)
    {
        Debug::Printf("Open file %d %s", i, s_filename[i % 10].c_str());
        std::cout << "Open file " << i << " " << s_filename[i % 10] << "\n";
        std::string filepath = data_path + "/" + s_filename[i % 10];
        futureFile[i] = FileSystem::Instance()->AsyncOpenFile(filepath, "rb", "");
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
                //std::cout << "File " << i << " Opening.\n";
            }
            else
            {
                //std::cout << "File " << i << " Ready.\n";
                files[i] = futureFile[i].get();
                if (files[i]) fileSize[i] = files[i]->Size();
                Debug::Printf("File %d of size %d ready.", i, fileSize[i]);
                ready_files++;
            }
        }
    }
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
                Debug::Printf("File %d Still reading.\n", i);
            }
            else
            {
                auto buff = futureRead[i].get();
                if (buff)
                {
                    read_buf[i] = buff.value();
                    Debug::Printf("File %d  Read %d bytes done.", i, read_buf[i].size());
                }
                else
                {
                    Debug::Printf("File %d Read fail %s.\n", i, files[i]->LastError().message().c_str());
                }
                read_done++;
                FileSystem::Instance()->CloseFile(files[i]);
            }
        }
    }
    for (int i = 0; i < FILE_MAX; i++)
    {
        char filename[256];
        sprintf(filename, "%s/writetest%d.dat",data_path.c_str(), i);
        Debug::Printf("Open file %d %s", i, filename);
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
                Debug::Printf("File %d Still writing.\n", i);
            }
            else
            {
                Debug::Printf("File  %d Write %d bytes done.\n", i, futureWrite[i].get());
                write_done++;
                FileSystem::Instance()->CloseFile(writeFiles[i]);
            }
        }
    }

    std::string strPath = AndroidBridge::GetDataFilePath();
    std::string msg = "data file path : " + strPath;
    return env->NewStringUTF(msg.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_FileSystemTestApp_FileSystemTestApp_stopStdioTest(JNIEnv * env, jobject thiz)
{
    for (int i = 0; i < FILE_MAX; i++)
    {
        read_buf[i].clear();
    }
    for (int i = 0; i < FILE_MAX; i++)
    {
        files[i] = nullptr;
        writeFiles[i] = nullptr;
    }
    delete FileSystem::Instance();
}