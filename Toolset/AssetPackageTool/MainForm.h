#ifndef _MAIN_FORM_H
#define _MAIN_FORM_H

#include "nana/gui.hpp"
#include "nana/gui/widgets/menubar.hpp"
#include "nana/gui/widgets/tabbar.hpp"
#include "AssetPackagePanel.h"

namespace AssetPackageTool
{
	class MainForm : public nana::form
	{
	public:
		MainForm();
		virtual ~MainForm();

		void Initialize();
	private:
		void InitMenu();

		void OnCloseCommand(nana::menu::item_proxy& menu_item);
		void OnCreatePackage(nana::menu::item_proxy& menu_item);
		void OnOpenPackage(nana::menu::item_proxy& menu_item);
	private:
		nana::place* m_place;
		nana::menubar* m_menubar;
		nana::tabbar<int>* m_tabbar;
		std::vector<AssetPackagePanel*> m_assetPanels;
	};
}
#endif // !_MAIN_FORM_H
