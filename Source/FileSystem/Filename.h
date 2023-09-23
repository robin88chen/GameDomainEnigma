/*****************************************************************//**
 * \file   Filename.h
 * \brief  
 * 
 * \author jongb
 * \date   September 2022
 *********************************************************************/
#ifndef _FILESYSTEM_FILENAME_H
#define _FILESYSTEM_FILENAME_H

#include <cstdlib>
#include <string>
/** @file Filename.h */

namespace Enigma::FileSystem
{
    /** Filename class */
    class Filename
    {
    public:
        Filename();
        Filename(const std::string& full_path);
        Filename(const std::string& file_path, const std::string& path_id);
        Filename(const Filename& filename);

        /** Contains the path of directories, not including the drive designator or the actual file name.
        The trailing slash is optional, and either a forward slash (/) or a backslash (\) or both might be used in a single dir argument.
        If a trailing slash (/ or \) is not specified, it is inserted automatically. */
        void SetDir(const std::string& dir);

        /** Contains a letter (A, B, and so on) corresponding to the desired drive and an optional trailing colon.
        inserts the colon automatically in the composite path if it is missing. */
        void SetDrive(const std::string& drive);

        /** Contains the actual file name extension, with or without a leading period (.).
        inserts the period automatically if it does not appear in ext. */
        void SetExt(const std::string& ext);

        /** Contains the base file name without any extensions.  */
        void SetBaseFileName(const std::string& fname);

        /** Contains the file name with extensions.  */
        void SetFileName(const std::string& filename);

        /** The mount path id */
        void SetMountPathID(const std::string& path_id);

        const std::string& GetDir() const;  ///< Optional directory path, including trailing slash. Forward slashes ( / ), backslashes ( \ ), or both may be used.
        const std::string& GetDrive() const; ///< Optional drive letter, followed by a colon (:)
        const std::string& GetExt() const; ///< Optional filename extension, including leading period (.)
        const std::string& GetBaseFileName() const; ///< Base filename (no extension)
        const std::string& GetFileName() const; ///< filename (with extension)

        std::string GetFullPath() const;
        std::string GetPath() const;  ///< get file path, like : "C:\Game\Media\"
        const std::string& GetMountPathID() const; ///< get mount path ID
        std::string GetFullFileNameAtPath() const; ///< return "fullpath@path_id"
        std::string GetFileNameAtPath() const; ///< return "filename(with extension)@path_id"
        const std::string& GetSubPathFileName() const; ///< return the filepath when using filepath constructor, 必須提供這個, 否則跟PathID 組合後, 子目錄路徑會被組合掉
        std::string GetSubPathFileNameAtPath() const;

        bool HasPathID() const;

    private:
        void SplitPath(const std::string& str);

    private:
        std::string m_dir;
        std::string m_drive;
        std::string m_ext;
        std::string m_fname;
        std::string m_filename;
        std::string m_pathID;
        std::string m_subPathFilename; ///< 用 filepath 建構子時的 filepath
    };
};

#endif // !_FILESYSTEM_FILENAME_H
