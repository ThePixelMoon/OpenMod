//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Slider.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/MenuButton.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/Controls.h>

#include "matsys_controls/mdlpanel.h"
#include "filesystem.h"
#include "vgui_int.h"
#include "steam/steam_api.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

class CModelButton : public CMDLPanel
{
	typedef CMDLPanel BaseClass;

public:
	CModelButton(vgui::Panel* parent, const char* panelName, const char* modelPath, const char* command)
		: BaseClass(parent, panelName)
	{
		m_Command = command;
		SetMDL(modelPath);
		LookAtMDL();
	}

	virtual void OnMouseReleased(vgui::MouseCode code)
	{
		if (code == MOUSE_LEFT)
		{
			if (!m_Command.IsEmpty())
			{
				engine->ClientCmd(m_Command.Get());
			}
		}
		BaseClass::OnMouseReleased(code);
	}

private:
	CUtlString m_Command;
};


class CSMLButton : public MenuButton
{
	typedef MenuButton BaseClass;
public:
	CSMLButton(Panel* parent, const char* panelName, const char* text);

private:
	Menu* m_pMenu;
	Frame* m_pFrame;
};

class CSMLCommandButton : public vgui::Button
{
	typedef vgui::Button BaseClass;

public:
	CSMLCommandButton(vgui::Panel* parent, const char* panelName, const char* labelText, const char* command)
		: BaseClass(parent, panelName, labelText)
	{
		AddActionSignalTarget(this);
		SetCommand(command);
	}

	virtual void OnCommand(const char* command)
	{
		engine->ClientCmd((char*)command);
	}

	virtual void OnTick(void)
	{
	}
};

class CSMLPage : public vgui::PropertyPage
{
	typedef vgui::PropertyPage BaseClass;
public:
	CSMLPage(vgui::Panel* parent, const char* panelName)
		: BaseClass(parent, panelName)
	{
		vgui::ivgui()->AddTickSignal(GetVPanel(), 250);
	}

	virtual void OnTick(void)
	{
		BaseClass::OnTick();

		if (!IsVisible())
			return;

		int c = m_LayoutItems.Count();
		for (int i = 0; i < c; i++)
		{
			vgui::Panel* p = m_LayoutItems[i];
			p->OnTick();
		}
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();

		const int xStart = 5;
		const int yStart = 5;
		const int itemWidth = 150;
		const int itemHeight = 150;
		const int gap = 10;

		int x = xStart;
		int y = yStart;

		int itemCount = m_LayoutItems.Count();
		int containerHeight = GetTall();

		for (int i = 0; i < itemCount; ++i)
		{
			vgui::Panel* item = m_LayoutItems[i];
			item->SetBounds(x, y, itemWidth, itemHeight);

			y += (itemHeight + gap);

			if (y + itemHeight > containerHeight)
			{
				x += (itemWidth + gap);
				y = yStart;
			}
		}
	}

	void Init(KeyValues* kv)
	{
		for (KeyValues* control = kv->GetFirstSubKey(); control != nullptr; control = control->GetNextKey())
		{
			const char* command = control->GetString("#", "");
			if (command[0])
			{
				CSMLCommandButton* btn = new CSMLCommandButton(this, "CommandButton", control->GetName(), command);
				m_LayoutItems.AddToTail(btn);
			}
		}
	}

public:
	CUtlVector<vgui::Panel*> m_LayoutItems;
};

ConVar spawn("cl_spawnmenu", "0", FCVAR_CLIENTDLL, "enable spawnmenu");

class CSMLMenu : public vgui::PropertyDialog
{
	typedef vgui::PropertyDialog BaseClass;
public:

	CSMLMenu(vgui::VPANEL* parent, const char* panelName)
		: BaseClass(NULL, "SMLenu")
	{
		SetTitle("SPAWN MENU", true);
		int x, y;
		surface()->GetScreenSize(x, y);
		SetSize(x, y);
		SetPos(0,0);

		vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

		loadButtons();

		GetPropertySheet()->SetTabWidth(72);
		SetMoveable(false);
		SetVisible(true);
		SetKeyBoardInputEnabled(false);
		SetMouseInputEnabled(true);
		SetSizeable(false);
		SetProportional(false);
	}

	void CSMLMenu::loadButtons() {
		const int buttonWidth = 128;
		const int buttonHeight = 128;
		const int startX = 50;
		const int startY = 50;
		const int gap = 10;

		int screenWidth, screenHeight;
		engine->GetScreenSize(screenWidth, screenHeight);
		const int buttonsPerRow = (screenWidth - startX * 2) / (buttonWidth + gap);

		FileFindHandle_t handle;
		const char* modelFile = g_pFullFileSystem->FindFirstEx("models/*.mdl", "GAME", &handle);

		if (modelFile) {
			int i = 0;
			do {
				char fullPath[MAX_PATH];
				Q_snprintf(fullPath, sizeof(fullPath), "models/%s", modelFile);

				CModelButton* btn = new CModelButton(this, modelFile, fullPath, "give weapon_bugbait");

				int xPos = startX + (i % buttonsPerRow) * (buttonWidth + gap);
				int yPos = startY + (i / buttonsPerRow) * (buttonHeight + gap);

				btn->SetBounds(xPos, yPos, buttonWidth, buttonHeight);
				btn->LookAtMDL();

				i++;
			} while ((modelFile = g_pFullFileSystem->FindNext(handle)) != nullptr);

			g_pFullFileSystem->FindClose(handle);
		}
	}

	void Init(KeyValues* kv);

	void OnTick()
	{
		BaseClass::OnTick();
		SetVisible(spawn.GetBool());
	}

	void OnCommand(const char* command)
	{
		BaseClass::OnCommand(command);
	}

};

void CSMLMenu::Init(KeyValues* kv)
{
}

class CSMLPanelInterface : public SMLPanel
{
private:
	CSMLMenu* SMLPanel;
public:
	CSMLPanelInterface()
	{
		SMLPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		SMLPanel = new CSMLMenu(&parent, "SMenu");
	}
	void Destroy()
	{
		if (SMLPanel)
		{
			SMLPanel->SetParent((vgui::Panel*)NULL);
			delete SMLPanel;
		}
	}
	void Activate(void)
	{
		if (SMLPanel)
		{
			SMLPanel->Activate();
		}
	}
};
static CSMLPanelInterface g_SMLPanel;
SMLPanel* smlmenu = (SMLPanel*)&g_SMLPanel;