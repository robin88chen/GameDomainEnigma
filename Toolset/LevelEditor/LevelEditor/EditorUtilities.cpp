#include "EditorUtilities.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IMountPath.h"
#include "Frameworks/TokenVector.h"

using namespace Enigma;

void LevelEditor::PasteTextureImageToButton(const std::string& filepath, nana::button* btn, const unsigned int btn_size)
{
    nana::paint::graphics graph(nana::size(btn_size, btn_size * 6));
    nana::paint::image img(filepath); // PNG 要另外編譯support 版本
    nana::rectangle src_rc(img.size());
    img.stretch(src_rc, graph, { 0, 0, btn_size, btn_size });
    img.stretch(src_rc, graph, { 0, (int)btn_size, btn_size, btn_size });
    img.stretch(src_rc, graph, { 0, (int)btn_size * 2, btn_size, btn_size });
    img.stretch(src_rc, graph, { 0, (int)btn_size * 3, btn_size, btn_size });
    img.stretch(src_rc, graph, { 0, (int)btn_size * 4, btn_size, btn_size });
    img.stretch(src_rc, graph, { 0, (int)btn_size * 5, btn_size, btn_size });

    nana::element::bground bground;
    bground.image(graph, true, {});
    btn->set_bground(bground);
}

std::string LevelEditor::FilePathCombinePathID(const std::filesystem::path& path, const std::string& path_id)
{
    std::list<FileSystem::IMountPathPtr> mount_paths = FileSystem::FileSystem::Instance()->GetMountPathsWithPathID(path_id);
    if (mount_paths.empty()) return path.filename().string();
    std::filesystem::path parent_path = path.parent_path().parent_path(); // 暫時只往上一層尋找
    for (auto mp : mount_paths)
    {
        if (mp->EqualMountPath(parent_path))
        {
            return (--(--path.end()))->string() + "/" + path.filename().string(); // +"@" + mp->GetPathID();
        }
    }
    return path.filename().string();
}

std::tuple<MathLib::Vector3, bool> LevelEditor::ParseTextToVector3(const std::string& value)
{
    TokenVector tokens = En_Split_Token(value, ", ");
    if (tokens.size() < 3) return { MathLib::Vector3::ZERO, false };
    MathLib::Vector3 vec;
    for (unsigned int i = 0; i < 3; i++)
    {
        vec[i] = (float)std::atof(tokens[i].c_str());
    }
    return { vec, true };
}

std::tuple<MathLib::Vector2, bool> LevelEditor::ParseTextToVector2(const std::string& value)
{
    TokenVector tokens = En_Split_Token(value, ", ");
    if (tokens.size() < 2) return { MathLib::Vector2::ZERO, false };
    MathLib::Vector2 vec;
    for (unsigned int i = 0; i < 2; i++)
    {
        vec[i] = (float)std::atof(tokens[i].c_str());
    }
    return { vec, true };
}

