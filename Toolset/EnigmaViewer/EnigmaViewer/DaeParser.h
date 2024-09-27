/*********************************************************************
 * \file   DaeParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _DAE_PARSER_H
#define _DAE_PARSER_H

#include <string>

namespace EnigmaViewer
{
    class DaeParser
    {
    public:
        DaeParser() = default;

        void loadDaeFile(const std::string& filename);

    private:
        void outputLog(const std::string& msg);

    private:
        std::string m_filename;
    };
}
#endif // DAEPARSER_H
