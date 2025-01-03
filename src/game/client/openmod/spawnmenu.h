//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef SPAWNMENU_H
#define SPAWNMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "basemenu.h"
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/MenuButton.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/MenuItem.h>
#include "filesystem.h"
#include <map>
#include <string>

using namespace vgui;

class SMLPanel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void		Activate(void) = 0;
};

extern SMLPanel* smlmenu;

class CSMLButton : public MenuButton {
public:
    CSMLButton( Panel* parent, const char* panelName, const char* text );
    ~CSMLButton() override;

private:
    Menu* m_pMenu;
};

class CSMLCommandButton : public Button {
public:
    CSMLCommandButton( Panel* parent, const char* panelName, const char* labelText, const char* command );
    void OnCommand( const char* command ) override;
    ~CSMLCommandButton() override;
};

class CSMLPage : public PropertyPage {
public:
    CSMLPage( Panel* parent, const char* panelName );
    ~CSMLPage() override;
    void OnTick() override;
    void PerformLayout() override;
    void Init( KeyValues* kv );
    static void CreateButtonX( CSMLPage* page, const char* label, const char* command );

private:
    CUtlVector<Panel*> m_LayoutItems;
};

class CSMLMenu : public PropertyDialog {
public:
    CSMLMenu( vgui::VPANEL* parent, const char* panelName );
    ~CSMLMenu() override;
    static void CreateButton( const char* pageName, const char* label, const char* command );
    CSMLPage* FindOrCreatePage( const char* pageName );
    void OnTick() override;

public:
    static CSMLMenu* s_Instance;
    std::map<std::string, CSMLPage*> m_PageRegistry;
};

class CSMLPanelInterface : public SMLPanel {
public:
    CSMLPanelInterface();
    ~CSMLPanelInterface();
    void Create( vgui::VPANEL parent );
    void Destroy();
    void Activate();

private:
    CSMLMenu* SMLPanel;
};

void CreateButton( const char* pageName, const char* label, const char* command );

#endif