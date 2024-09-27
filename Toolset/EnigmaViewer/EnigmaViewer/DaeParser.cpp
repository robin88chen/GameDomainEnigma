#include "DaeParser.h"
#include "DaeSchema.h"
#include "DaeAnimationParser.h"
#include "DaeSceneParser.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "ViewerCommands.h"

using namespace EnigmaViewer;
using namespace Enigma::FileSystem;


void DaeParser::loadDaeFile(const std::string& filename)
{
    IFilePtr file = FileSystem::instance()->openFile(filename, read | binary, "");
    if (!file) return;
    unsigned file_size = static_cast<unsigned>(file->size());
    auto read_buf = file->read(0, file_size);
    FileSystem::instance()->closeFile(file);
    if (!read_buf)
    {
        outputLog(filename + " read fail!!");
        return;
    }
    pugi::xml_document dae_doc;
    pugi::xml_parse_result res = dae_doc.load_buffer(&((read_buf.value())[0]), file_size);
    if (res.status != pugi::xml_parse_status::status_ok)
    {
        outputLog(filename + " parse error!!");
        return;
    }
    else
    {
        outputLog(filename + " parse done.");
    }
    m_filename = filename;
    pugi::xml_node collada_root = dae_doc.root().child("COLLADA");
    if (!collada_root)
    {
        outputLog(filename + " not a COLLADA file!!");
    }
    DaeSchema::clear();
    DaeSceneParser scene_parser([=](auto s) { outputLog(s); });
    std::error_code er = scene_parser.parseScene(collada_root, filename);
    if (er)
    {
        outputLog("parse scene error!!");
        return;
    }
    //parseScene(collada_root);
    DaeAnimationParser animation_parser([=](auto s) { outputLog(s); });
    er = animation_parser.parseAnimations(collada_root, scene_parser.getModelName());
    //parseAnimations(collada_root);
    if (!er)
    {
        animation_parser.persistAnimator(scene_parser.getModelId());
        scene_parser.persistModel(animation_parser.getAnimatorId());
    }
}

void DaeParser::outputLog(const std::string& msg)
{
    std::make_shared<OutputMessage>(msg)->enqueue();
}
