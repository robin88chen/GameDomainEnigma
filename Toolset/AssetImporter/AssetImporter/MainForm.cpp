#include "MainForm.h"
#include "SchemeColorDef.h"

using namespace AssetImporter;

MainForm::MainForm()
{

}

MainForm::~MainForm()
{

}

void MainForm::initialize()
{
    UISchemeColors::ApplySchemaColors(scheme());
}
