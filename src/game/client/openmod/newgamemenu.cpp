//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "newgamemenu.h"
#include "filesystem.h"
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/CheckButton.h>

using namespace vgui;

class CNewGame : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CNewGame, vgui::Frame);

	CNewGame(vgui::VPANEL parent);
	~CNewGame() {};

protected:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

private:
	Button* m_pCloseButton;
	Button* m_pStartButton;
	PropertySheet* m_pPropertySheet;
	TextEntry* m_pHostNameText;
	ComboBox* m_pMaxPlayersComboBox;
	ComboBox* m_pMapComboBox;
	ComboBox* m_pGameModeComboBox;
	Label* m_pHostNameLabel;
	Label* m_pMaxPlayersLabel;
	Label* m_pMapLabel;
	Label* m_pGameModeLabel;
	CheckButton* m_pNoclipCheck;
	Label* m_pNoclipLabel;
};

CNewGame::CNewGame(vgui::VPANEL parent)
	: BaseClass(NULL, "NewGame")
{
	SetParent(parent);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);
	SetProportional(false);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetVisible(true);
	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	LoadControlSettings("resource/UI/newgamemenu.res");
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	m_pPropertySheet = new PropertySheet(this, "PropertySheet");
	m_pPropertySheet->SetPos(0, 32);
	m_pPropertySheet->SetSize(512, 480);

	Panel* pServerTab = new Panel(m_pPropertySheet, "ServerTab");
	m_pPropertySheet->AddPage(pServerTab, "Server");

	m_pHostNameLabel = new Label(pServerTab, "HostNameLabel", "Hostname:");
	m_pHostNameLabel->SetPos(20, 20);
	m_pHostNameLabel->SetSize(100, 20);

	m_pHostNameText = new TextEntry(pServerTab, "HostNameText");
	m_pHostNameText->SetPos(120, 20);
	m_pHostNameText->SetSize(250, 20);
	m_pHostNameText->SetText("OpenMod Server");

	m_pMaxPlayersLabel = new Label(pServerTab, "MaxPlayersLabel", "Max Players:");
	m_pMaxPlayersLabel->SetPos(20, 60);
	m_pMaxPlayersLabel->SetSize(100, 20);

	m_pMaxPlayersComboBox = new ComboBox(pServerTab, "MaxPlayersComboBox", 8, false);
	m_pMaxPlayersComboBox->SetPos(120, 60);
	m_pMaxPlayersComboBox->SetSize(150, 20);

	int maxPlayers[] = { 1, 2, 4, 8, 12, 16, 32, 64 };
	for (int i = 0; i < ARRAYSIZE(maxPlayers); i++)
	{
		m_pMaxPlayersComboBox->AddItem(VarArgs("%d", maxPlayers[i]), NULL);
	}
	m_pMaxPlayersComboBox->ActivateItemByRow(0);

	m_pMapLabel = new Label(pServerTab, "MapLabel", "Map:");
	m_pMapLabel->SetPos(20, 100);
	m_pMapLabel->SetSize(100, 20);

	m_pMapComboBox = new ComboBox(pServerTab, "MapComboBox", 8, false);
	m_pMapComboBox->SetPos(120, 100);
	m_pMapComboBox->SetSize(150, 20);

	FileFindHandle_t findHandle = NULL;
	const char* pszFilename = g_pFullFileSystem->FindFirst("maps/*.bsp", &findHandle);
	while (pszFilename)
	{
		char mapname[256];
		Q_snprintf(mapname, sizeof(mapname), "maps/%s", pszFilename);
		if (!g_pFullFileSystem->FileExists(mapname, "MOD"))
			goto nextFile;

		const char* str = Q_strstr(pszFilename, "maps");
		if (str)
			Q_strncpy(mapname, str + 5, sizeof(mapname) - 1);
		else
			Q_strncpy(mapname, pszFilename, sizeof(mapname) - 1);

		char* ext = Q_strstr(mapname, ".bsp");
		if (ext)
			*ext = 0;

		m_pMapComboBox->AddItem(mapname, NULL);

	nextFile:
		pszFilename = g_pFullFileSystem->FindNext(findHandle);
	}
	g_pFullFileSystem->FindClose(findHandle);

	m_pMapComboBox->ActivateItemByRow(0);

	Panel* pGameplayTab = new Panel(m_pPropertySheet, "GameplayTab");
	m_pPropertySheet->AddPage(pGameplayTab, "Gameplay");

	m_pNoclipLabel = new Label(pGameplayTab, "NoclipLabel", "Allow Noclip:");
	m_pNoclipLabel->SetPos(20, 60);
	m_pNoclipLabel->SetSize(100, 20);

	m_pNoclipCheck = new CheckButton(pGameplayTab, "NoclipCheck", "");
	m_pNoclipCheck->SetPos(120, 60);
	m_pNoclipCheck->SetSize(150, 20);
	m_pNoclipCheck->SetSelected(true);

	m_pGameModeLabel = new Label(pGameplayTab, "GameModeLabel", "Gamemode:");
	m_pGameModeLabel->SetPos(20, 20);
	m_pGameModeLabel->SetSize(100, 20);

	m_pGameModeComboBox = new ComboBox(pGameplayTab, "GameModeComboBox", 8, false);
	m_pGameModeComboBox->SetPos(120, 20);
	m_pGameModeComboBox->SetSize(150, 20);

	FileFindHandle_t gamemodeFindHandle = NULL;
	const char* gamemodeFolder = "gamemodes/*";
	const char* gamemodeName = g_pFullFileSystem->FindFirst(gamemodeFolder, &gamemodeFindHandle);
	while (gamemodeName)
	{
		if (Q_stricmp(gamemodeName, ".") == 0 || Q_stricmp(gamemodeName, "..") == 0)
		{
			gamemodeName = g_pFullFileSystem->FindNext(findHandle);
			continue;
		}

		char gamemodePath[256];
		Q_snprintf(gamemodePath, sizeof(gamemodePath), "gamemodes/%s", gamemodeName);
		if (g_pFullFileSystem->IsDirectory(gamemodePath))
			m_pGameModeComboBox->AddItem(gamemodeName, NULL);

		gamemodeName = g_pFullFileSystem->FindNext(gamemodeFindHandle);
	}
	g_pFullFileSystem->FindClose(gamemodeFindHandle);

	for (int i = 0; i < m_pGameModeComboBox->GetItemCount(); i++)
	{
		char itemText[256];
		m_pGameModeComboBox->GetItemText(i, itemText, sizeof(itemText));
		if (Q_stricmp(itemText, "sandbox") == 0)
		{
			m_pGameModeComboBox->ActivateItem(i);
			break;
		}
	}
	m_pStartButton = new Button(this, "StartButton", "Start", this, "startgame");
	m_pStartButton->SetPos(433, 472);
	m_pStartButton->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pStartButton->SetReleasedSound("ui/buttonclick.wav");

	m_pCloseButton = new Button(this, "Button", "Close", this, "turnoff");
	m_pCloseButton->SetPos(360, 472);
	m_pCloseButton->SetDepressedSound("common/bugreporter_succeeded.wav");
	m_pCloseButton->SetReleasedSound("ui/buttonclick.wav");

	int screenWide, screenTall;
	vgui::surface()->GetScreenSize(screenWide, screenTall);
	int windowWide, windowTall;
	GetSize(windowWide, windowTall);
	SetPos((screenWide - windowWide) / 2, (screenTall - windowTall) / 2);
}

class CNewGameInterface : public NewGame
{
private:
    CNewGame* NewGamePanel;
public:
    CNewGameInterface()
    {
        NewGamePanel = NULL;
    }
    void Create(vgui::VPANEL parent)
    {
        NewGamePanel = new CNewGame(parent);
    }
    void Destroy()
    {
        if (NewGamePanel)
        {
            NewGamePanel->SetParent((vgui::Panel*)NULL);
            delete NewGamePanel;
        }
    }
	void Activate(void)
	{
		if (NewGamePanel)
		{
			NewGamePanel->Activate();
		}
	}
};

static CNewGameInterface g_NewGame;
NewGame* newgame = (NewGame*)&g_NewGame;

ConVar cl_shownewgame("cl_shownewgame", "0", FCVAR_CLIENTDLL, "Sets the state of newgame <state>");

void CNewGame::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_shownewgame.GetBool());
}

CON_COMMAND(opennewgame, "new game menu")
{
	cl_shownewgame.SetValue(1); // hacky hack
	newgame->Activate();
};

void CNewGame::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);

	if (!Q_stricmp(pcCommand, "turnoff"))
		cl_shownewgame.SetValue(0);

	if (!Q_stricmp(pcCommand, "startgame"))
	{

		char szHostName[256];
		m_pHostNameText->GetText(szHostName, sizeof(szHostName));

		int maxPlayersIndex = m_pMaxPlayersComboBox->GetActiveItem();
		char szMaxPlayers[64];
		m_pMaxPlayersComboBox->GetItemText(maxPlayersIndex, szMaxPlayers, sizeof(szMaxPlayers));

		int mapIndex = m_pMapComboBox->GetActiveItem();
		char szMap[256];
		m_pMapComboBox->GetItemText(mapIndex, szMap, sizeof(szMap));

		int gamemodeIndex = m_pGameModeComboBox->GetActiveItem();
		char szGameMode[256];
		m_pGameModeComboBox->GetItemText(gamemodeIndex, szGameMode, sizeof(szGameMode));

		bool allowNoclip = m_pNoclipCheck->IsSelected();

#if 0
		DevMsg("Starting game with:\n");
		DevMsg("Hostname: %s\n", szHostName);
		DevMsg("Max players: %s\n", szMaxPlayers);
		DevMsg("Map: %s\n", szMap);
		DevMsg("Gamemode: %s\n", szGameMode);
#endif

		SetVisible(false);

		// minor commands
		engine->ClientCmd_Unrestricted("disconnect");
		engine->ClientCmd_Unrestricted("wait");
		engine->ClientCmd_Unrestricted("wait");
		engine->ClientCmd_Unrestricted("sv_lan 1");
		engine->ClientCmd_Unrestricted("setmaster enable");
		engine->ClientCmd_Unrestricted("progress_enable");

		// MAJOR commands (get it?)
		engine->ClientCmd_Unrestricted(VarArgs("maxplayers %s", szMaxPlayers));
		engine->ClientCmd_Unrestricted(VarArgs("hostname %s", szHostName));
		engine->ClientCmd_Unrestricted(VarArgs("gamemode %s", szGameMode));
		engine->ClientCmd_Unrestricted(VarArgs("map %s", szMap));
		engine->ClientCmd_Unrestricted(VarArgs("sbox_allow_noclip %d", allowNoclip ? 1 : 0));
		if (Q_stricmp(szGameMode, "sandbox") == 0)
			engine->ClientCmd_Unrestricted("sv_cheats 1");
	}
}