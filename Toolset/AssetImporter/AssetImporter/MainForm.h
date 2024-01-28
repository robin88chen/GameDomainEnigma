/*********************************************************************
 * \file   MainForm.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef MAIN_FORM_H
#define MAIN_FORM_H

#include "nana/gui.hpp"

namespace AssetImporter
{
    class MainForm : public nana::form
    {
    public:
        MainForm();
        ~MainForm();

        void initialize();
    };
}

#endif // MAIN_FORM_H
