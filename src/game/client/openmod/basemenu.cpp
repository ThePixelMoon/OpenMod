//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "basemenu.h"

#include "vgui/ILocalize.h"
#include "vgui/IPanel.h"
#include "vgui/ISurface.h"
#include "vgui/ISystem.h"
#include "vgui/IVGui.h"
#include "ienginevgui.h"
#include <engine/IEngineSound.h>
#include "filesystem.h"
#include "vgui_controls\ListPanel.h"
#include "vgui_controls\Button.h"
#include "vgui_controls\ComboBox.h"
#include "vgui_controls\CheckButton.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

static CDllDemandLoader g_GameUIDLL( "GameUI" );

RootPanel *guiroot = NULL;

void OverrideRootUI()
{
	if( !OverrideUI->GetPanel() )
	{
		OverrideUI->Create(NULL);
	}
	if( guiroot->GetGameUI() )
	{
		guiroot->GetGameUI()->SetMainMenuOverride( guiroot->GetVPanel() );
		return;
	}
}

RootPanel::RootPanel(VPANEL parent) : Panel( NULL, "RootPanel" )
{
	SetParent(parent);
	guiroot = this;

	m_bCopyFrameBuffer = false;
	gameui = NULL;

	LoadGameUI();

	m_ExitingFrameCount = 0;

    m_pHTMLPanel = new vgui::HTML(this, "HTMLPanel");
    m_pHTMLPanel->SetScrollbarsEnabled(false);
	m_pHTMLPanel->SetContextMenuEnabled(false);
	m_pHTMLPanel->SetViewSourceEnabled(false);
#if 0
	m_pHTMLPanel->RunJavascript(
		"document.body.style.userSelect = 'none';"
		"document.body.style.webkitUserSelect = 'none';"
		"document.body.style.mozUserSelect = 'none';"
		"document.body.style.msUserSelect = 'none';"
		"document.body.style.touchAction = 'none';"
	);
	// oh well..
#endif
}

IGameUI *RootPanel::GetGameUI()
{
	if( !gameui )
	{
		if ( !LoadGameUI() )
			return NULL;
	}

	return gameui;
}

bool RootPanel::LoadGameUI()
{
	if( !gameui )
	{
		CreateInterfaceFn gameUIFactory = g_GameUIDLL.GetFactory();
		if ( gameUIFactory )
		{
			gameui = (IGameUI *) gameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);
			if( !gameui )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}

RootPanel::~RootPanel()
{
	gameui = NULL;
	g_GameUIDLL.Unload();
}

void RootPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	// Resize the panel to the screen size
	// Otherwise, it'll just be in a little corner
	int wide, tall;
	vgui::surface()->GetScreenSize(wide, tall);
	SetSize(wide,tall);

    const char *gameDir = engine->GetGameDirectory();
    char filePath[256];
    sprintf( filePath, "%s/html/menu.html", gameDir );

    m_pHTMLPanel->SetBounds(0, 0, wide, tall);
    m_pHTMLPanel->OpenURL(filePath, nullptr, true);
}

class COverrideInterface : public IOverrideInterface
{
private:
	RootPanel *MainMenu;
 
public:
	int UI_Interface( void )
	{
		MainMenu = NULL;
	}
 
	void Create( vgui::VPANEL parent )
	{
		MainMenu = new RootPanel(parent);
	}

	vgui::VPANEL GetPanel( void )
	{
		if ( !MainMenu )
			return NULL;
		return MainMenu->GetVPanel();
	}
 
	void Destroy( void )
	{
		if ( MainMenu )
		{
			MainMenu->SetParent( (vgui::Panel *)NULL );
			delete MainMenu;
		}
	}
 
    virtual RootPanel* GetMenuBase() override
    {
        return MainMenu;
    }
};
 
static COverrideInterface g_SMenu;
IOverrideInterface *OverrideUI = ( IOverrideInterface * )&g_SMenu;

// new game menu
class MapListPanel : public vgui::ListPanel
{
public:
	MapListPanel(vgui::Panel* parent, const char* panelName)
		: vgui::ListPanel(parent, panelName)
	{
		AddActionSignalTarget(this);
	}

	void OnMousePressed(vgui::MouseCode code) override
	{
		vgui::ListPanel::OnMousePressed(code);

		int selectedItem = GetSelectedItem(0);
		if (selectedItem != -1)
		{
			KeyValues* kv = GetItem(selectedItem);
			const char* mapName = kv->GetString("Map");
			if (m_pMapLabel)
			{
				m_pMapLabel->SetText(VarArgs("%s", mapName));
			}
		}
	}

	void SetMapLabel(vgui::Label* label)
	{
		m_pMapLabel = label;
	}

private:
	vgui::Label* m_pMapLabel;
};

class NewGame : public vgui::Frame
{
public:
	NewGame(vgui::Panel* parent, const char* panelName)
		: vgui::Frame(parent, panelName)
	{
		int x, y;
		surface()->GetScreenSize(x, y);
		SetSize(x,y);
		SetTitle("NEW GAME", true);
		SetVisible(true);
		SetMoveable(true);
		SetSizeable(false);

		m_pMapList = new MapListPanel(this, "MapList");
		m_pMapList->SetBounds(10, 30, 200, y - 60);
		m_pMapList->AddColumnHeader(0, "Map", "Map List", 200, 0);

		vgui::ISurface* surface = vgui::surface();
		HFont hFont = surface->CreateFont();
		surface->SetFontGlyphSet(hFont, "Arial", 24, 600, 0, 0, 0);
		m_pMapLabel = new vgui::Label(this, "MapLabel", " ");
		m_pMapLabel->SetPos(220, 30);
		m_pMapLabel->SetSize(200, 30);
		m_pMapLabel->SetFont(hFont);

		m_pMapList->SetMapLabel(m_pMapLabel);

		loadMaps();
		m_pMapList->SetSingleSelectedItem(0);
		int selectedItem = m_pMapList->GetSelectedItem(0);
		if (selectedItem != -1)
		{
			KeyValues* kv = m_pMapList->GetItem(selectedItem);
			const char* mapName = kv->GetString("Map");
			if (m_pMapLabel)
			{
				m_pMapLabel->SetText(VarArgs("%s", mapName));
			}
		} // fallback

		m_pStartButton = new vgui::Button(this, "StartButton", "Start Game");
		m_pStartButton->SetPos(x - 110, y - 50);
		m_pStartButton->SetSize(100, 30);
		m_pStartButton->SetCommand("startgame");

		m_pMaxPlayersComboBox = new vgui::ComboBox(this, "MaxPlayersComboBox", 5, false);
		m_pMaxPlayersComboBox->SetPos(x - 190, 40);
		m_pMaxPlayersComboBox->SetSize(180, 30);
		m_pMaxPlayersComboBox->AddItem("1", nullptr);
		m_pMaxPlayersComboBox->AddItem("2", nullptr);
		m_pMaxPlayersComboBox->AddItem("4", nullptr);
		m_pMaxPlayersComboBox->AddItem("8", nullptr);
		m_pMaxPlayersComboBox->AddItem("16", nullptr);
		m_pMaxPlayersComboBox->AddItem("32", nullptr);
		m_pMaxPlayersComboBox->AddItem("64", nullptr);
		m_pMaxPlayersComboBox->ActivateItem(0);

		m_pEnableCheatsCheckBox = new vgui::CheckButton(this, "EnableCheatsCheckBox", "Enable Cheats");
		m_pEnableCheatsCheckBox->SetPos(x - 190, 80);
		m_pEnableCheatsCheckBox->SetSize(180, 30);
	}

	virtual void Paint() override
	{
		vgui::Panel::Paint();
	}

	virtual void OnCommand(const char* command) override
	{
		if (FStrEq(command, "startgame"))
		{
			KeyValues* kv = m_pMapList->GetItem(m_pMapList->GetSelectedItem(0));

			SetVisible(false); // so we could see progress bar

			// minor commands
			engine->ClientCmd_Unrestricted("disconnect\nwait\nwait\nsv_lan 1\nsetmaster enable\nprogress_enable\n");

			// MAJOR commands (get it?)
			int maxPlayers = m_pMaxPlayersComboBox->GetActiveItem();
			if (maxPlayers != -1)
			{
				char maxPlayersStr[256];
				m_pMaxPlayersComboBox->GetItemText(maxPlayers, maxPlayersStr, sizeof(maxPlayersStr));
				engine->ClientCmd_Unrestricted(VarArgs("maxplayers %s", maxPlayersStr));
			}

			engine->ClientCmd_Unrestricted(VarArgs("map %s", kv->GetString("Map")));
			if (m_pEnableCheatsCheckBox->IsSelected())
				engine->ClientCmd_Unrestricted("sv_cheats 1");
		}
		else
		{
			vgui::Panel::OnCommand(command);
		}
	}

private:
	void loadMaps()
	{
		FileFindHandle_t findHandle = NULL;

		const char* pszFilename = g_pFullFileSystem->FindFirst("maps/*.bsp", &findHandle);
		while (pszFilename)
		{
			char mapname[256];
			Q_snprintf(mapname, sizeof(mapname), "maps/%s", pszFilename);
			if (!g_pFullFileSystem->FileExists(mapname, "MOD"))
			{
				goto nextFile;
			}

			const char* str = Q_strstr(pszFilename, "maps");
			if (str)
			{
				Q_strncpy(mapname, str + 5, sizeof(mapname) - 1);
			}
			else
			{
				Q_strncpy(mapname, pszFilename, sizeof(mapname) - 1);
			}

			char* ext = Q_strstr(mapname, ".bsp");
			if (ext)
			{
				*ext = 0;
			}

			KeyValues* kv = new KeyValues("Map");
			kv->SetString("Map", mapname);
			m_pMapList->AddItem(kv, 0, false, false);
			kv->deleteThis();

			goto nextFile;

		nextFile:
			pszFilename = g_pFullFileSystem->FindNext(findHandle);
		}

		g_pFullFileSystem->FindClose(findHandle);
	}

private:
	MapListPanel* m_pMapList;
	vgui::Button* m_pStartButton;
	vgui::Label* m_pMapLabel;
	vgui::ComboBox* m_pMaxPlayersComboBox;
	vgui::CheckButton* m_pEnableCheatsCheckBox;
};

CON_COMMAND(opennewgame, "new game")
{
	static NewGame* pCurrentMenu = nullptr;

	if (pCurrentMenu)
	{
		pCurrentMenu->Close();
		pCurrentMenu = nullptr;
	}

	pCurrentMenu = new NewGame(nullptr, "NewGame");
	pCurrentMenu->Activate();
}