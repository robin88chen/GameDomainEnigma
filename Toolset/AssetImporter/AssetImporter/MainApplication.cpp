#include <windows.h>
#include "MainForm.h"
#include "Platforms/MemoryMacro.h"
#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
    meInitMemoryCheck();
    //_CrtSetBreakAlloc(2350);
    AssetImporter::MainForm form;
    form.initialize();
    form.caption("Asset Importer");
    form.size(nana::size(1024, 768));
    form.move(nana::point(60, 60));
    form.show();

    nana::exec();

    return 0;
}