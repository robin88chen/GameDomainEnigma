#include "Filename.h"

using namespace Enigma::FileSystem;

Filename::Filename()
{

}

Filename::Filename(const std::string& file_path, const std::string& path_id)
{
    if (path_id.empty())
    {
        SplitPath(file_path);
    }
    else
    {
        SplitPath(file_path + "@" + path_id);
    }
}

Filename::Filename(const std::string& full_path)
{
    SplitPath(full_path);
}

Filename::Filename(const Filename& filename)
{
    m_dir = filename.m_dir;
    m_drive = filename.m_drive;
    m_ext = filename.m_ext;
    m_fname = filename.m_fname;
    m_filename = filename.m_filename;
    m_pathID = filename.m_pathID;
}

void Filename::SplitPath(const std::string& str)
{
    size_t pos = str.find(':');
    if (pos != std::string::npos)
    {
        m_drive = str.substr(0, pos + 1);
        m_dir = str.substr(pos + 1);
    }
    else
    {
        m_dir = str;
    }
    pos = m_dir.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        m_fname = m_dir.substr(pos + 1);
        m_dir = m_dir.substr(0, pos + 1);
    }
    else
    {
        m_fname = m_dir;
        m_dir = "";
    }
    pos = m_fname.find('@');
    if (pos != std::string::npos)
    {
        m_pathID = m_fname.substr(pos + 1);
        m_fname = m_fname.substr(0, pos);
    }
    pos = m_fname.find_last_of('.');
    if (pos != std::string::npos)
    {
        m_ext = m_fname.substr(pos);
        m_fname = m_fname.substr(0, pos);
    }
    else
    {
        m_ext = "";
    }
    m_filename = m_fname + m_ext;
}

std::string Filename::GetFullPath()
{
    return m_drive + m_dir + m_fname + m_ext;
}

std::string Filename::GetPath()
{
    return m_drive + m_dir;
}

std::string Filename::GetMountPathID()
{
    return m_pathID;
}
const std::string& Filename::GetMountPathID() const
{
    return m_pathID;
}

std::string Filename::GetFullFileNameAtPath()
{
    if (HasPathID())
    {
        return GetFullPath() + "@" + m_pathID;
    }
    return GetFullPath();
}

std::string Filename::GetFileNameAtPath()
{
    if (HasPathID())
    {
        return GetFileName() + "@" + m_pathID;
    }
    return GetFileName();
}

void Filename::SetDir(const std::string& dir)
{
    m_dir = dir;
    if ((m_dir.length() > 0) &&
        ((m_dir[m_dir.length() - 1] != '/') && (m_dir[m_dir.length() - 1] != '\\')))
    {
        m_dir = m_dir + "/";
    }
}

void Filename::SetDrive(const std::string& drive)
{
    m_drive = drive;
    if ((m_drive.length() > 0) && (m_drive[m_drive.length() - 1] != ':'))
    {
        m_drive = m_drive + ":";
    }
}

void Filename::SetExt(const std::string& ext)
{
    m_ext = ext;
    if ((m_ext.length() > 0) && (m_ext[0] != '.'))
    {
        m_ext = "." + m_ext;
    }
    m_filename = m_fname + m_ext;
}

void Filename::SetBaseFileName(const std::string& fname)
{
    m_fname = fname;
    m_filename = m_fname + m_ext;
}

void Filename::SetFileName(const std::string& filename)
{
    m_filename = filename;
    size_t pos = m_filename.find_last_of('.');
    if (pos != std::string::npos)
    {
        m_ext = m_filename.substr(pos);
        m_fname = m_filename.substr(0, pos);
    }
    else
    {
        m_ext = "";
        m_fname = m_filename;
    }
}

void Filename::SetMountPathID(const std::string& path_id)
{
    m_pathID = path_id;
}

std::string Filename::GetDir()
{
    return m_dir;
}

std::string Filename::GetDrive()
{
    return m_drive;
}

std::string Filename::GetExt()
{
    return m_ext;
}

std::string Filename::GetBaseFileName()
{
    return m_fname;
}

std::string Filename::GetFileName()
{
    return m_filename;
}
const std::string& Filename::GetFileName() const
{
    return m_filename;
}

bool Filename::HasPathID()
{
    return m_pathID.length() > 0;
}
