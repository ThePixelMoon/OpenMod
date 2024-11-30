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
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/MenuButton.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/MenuItem.h>
#include "vgui_int.h"
#ifdef _WIN32
#pragma warning( disable : 4005 )
#include <windows.h>
#undef CreateFont
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

static CDllDemandLoader g_GameUIDLL( "GameUI" );

RootPanel *guiroot = NULL;

void OverrideRootUI(void)
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
    sprintf( filePath, "%s/html/index.html", gameDir );

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

	virtual void OnClose() override
	{
		SetVisible(false);
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
			engine->ClientCmd_Unrestricted("disconnect");
			engine->ClientCmd_Unrestricted("wait");
			engine->ClientCmd_Unrestricted("wait");
			engine->ClientCmd_Unrestricted("sv_lan 1");
			engine->ClientCmd_Unrestricted("setmaster enable");
			engine->ClientCmd_Unrestricted("progress_enable");

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

// loading screen

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMapLoadBG::CMapLoadBG( char const* panelName ) : EditablePanel( NULL, panelName )
{
	VPANEL toolParent = enginevgui->GetPanel(PANEL_GAMEUIDLL);
	SetParent(toolParent);
	LoadControlSettings("resource/loadingdialog.res");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CMapLoadBG::~CMapLoadBG()
{}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMapLoadBG::ApplySchemeSettings( IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	int iWide, iTall;
	surface()->GetScreenSize(iWide, iTall);
	SetSize(iWide, iTall);
}

//-----------------------------------------------------------------------------
// Purpose: Sets a new background on demand
//-----------------------------------------------------------------------------
void CMapLoadBG::SetEnable( bool enabled)
{
	int wide, tall;
	vgui::surface()->GetScreenSize(wide, tall);

#if 0
	HHPanel = new vgui::HTML( this, "HHPanel" );
	HHPanel->SetScrollbarsEnabled( false );
	HHPanel->SetContextMenuEnabled( false );
	HHPanel->SetViewSourceEnabled( false );
	HHPanel->SetBounds( 0, 0, wide, tall );
	HHPanel->OpenURL( "https://google.com/", nullptr, true );
	HHPanel->SetEnabled( enabled );
#endif
}

// spawnmenu
class CSMLButton : public MenuButton
{
	typedef MenuButton BaseClass;
public:
	CSMLButton(Panel* parent, const char* panelName, const char* text);

private:
	Menu* m_pMenu;
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
		int x = 5;
		int y = 5;
		int w = 150;
		int h = 18;
		int gap = 2;

		int c = m_LayoutItems.Count();
		int tall = GetTall();

		for (int i = 0; i < c; i++)
		{
			vgui::Panel* p = m_LayoutItems[i];
			p->SetBounds(x, y, w, h);

			y += (h + gap);
			if (y >= tall - h)
			{
				x += (w + gap);
				y = 5;
			}
		}
	}

	void Init(KeyValues* kv)
	{
		for (KeyValues* control = kv->GetFirstSubKey(); control != NULL; control = control->GetNextKey())
		{
			int iType = control->GetInt("imagebutton", 0);

			if (iType == 1)
			{
				const char* a, * b, * c, * t;
				//command
				t = control->GetString("#", "");
				a = control->GetString("normal", "");
				b = control->GetString("overimage", "");
				c = control->GetString("mouseclick", "");

				if (t, a, b, c && t[0], a[0], b[0], c[0])
				{

				}
			}
			else
			{
				const char* m;

				//command
				m = control->GetString("#", "");
				if (m && m[0])
				{
					CSMLCommandButton* btn = new CSMLCommandButton(this, "CommandButton", control->GetName(), m);
					m_LayoutItems.AddToTail(btn);
					continue;
				}
			}
		}
	}
private:
	CUtlVector< vgui::Panel* >		m_LayoutItems;
};

ConVar spawn("cl_propmenu", "0", FCVAR_CLIENTDLL, "spawn menu");

class CSMLMenu : public vgui::PropertyDialog
{
	typedef vgui::PropertyDialog BaseClass;
public:

	CSMLMenu(vgui::VPANEL* parent, const char* panelName)
		: BaseClass(NULL, "SMLenu")
	{
		SetTitle("SPAWN MENU", true);
		SetPos(325, 100);

		KeyValues* kv = new KeyValues("SMLenu");
		if (kv)
		{
			if (kv->LoadFromFile(g_pFullFileSystem, "scripts/props.txt"))
			{
				for (KeyValues* dat = kv->GetFirstSubKey(); dat != NULL; dat = dat->GetNextKey())
				{
					if (!Q_strcasecmp(dat->GetName(), "width"))
					{
						SetWide(dat->GetInt());
						continue;
					}
					else if (!Q_strcasecmp(dat->GetName(), "height"))
					{
						SetTall(dat->GetInt());
						continue;
					}

					CSMLPage* page = new CSMLPage(this, dat->GetName());
					page->Init(dat);

					AddPage(page, dat->GetName());
				}
			}
			kv->deleteThis();
		}

		vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
		//oh ok
		GetPropertySheet()->SetTabWidth(72);
		SetVisible(true);
		SetMouseInputEnabled(true);
		SetSizeable(true);
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

		//if (!Q_stricmp(command, "Close"))
		//{
		//	spawn.SetValue(0);
		//}
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

// commands
void openURL(const CCommand& args)
{
	// FIXME::
#ifdef _WIN32
	if (args.ArgC() < 2)
	{
		Msg("usage: open <URL>\n");
		return;
	}

	const char* url = args.Arg(1);

	size_t commandSize = strlen("/C start ") + strlen(url) + strlen(" && exit") + 1;
	char* command = new char[commandSize];

	strcpy(command, "/C start ");
	strcat(command, url);
	strcat(command, " && exit");
	
	ShellExecuteA(NULL, "open", "cmd.exe", command, NULL, SW_SHOWMINIMIZED);

	delete[] command;
#else
	// WIP
#endif
}

CON_COMMAND(open, "haha open url go brr")
{
	openURL(args);
}