//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "mountsteamcontent.h"
#include "steam\steam_api.h"
#include "filesystem.h"
#include "KeyValues.h"

// vgui
#include "vgui/IVGui.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/Label.h"
#include "vgui/ISurface.h"

using namespace vgui;

// from HL2SB, because i am a lazy ass
typedef struct
{
	const char* m_pPathName;
	int m_nAppId;
} gamePaths_t;

gamePaths_t g_GamePaths[11] =
{
	{ "hl2",		220    },
	{ "episodic",	220    }, // got merged in base game
	{ "ep2",		220    }, // got merged in base game
	{ "lostcoast",	220    }, // got merged in base game
	{ "cstrike",	240    },
	{ "hl1",		280    },
	{ "dod",		300    },
	{ "hl1mp",		360    },
	{ "portal",		400    },
	{ "hl2mp",		320    },
	{ "tf",			440    }
};
static const int size = sizeof(g_GamePaths) / sizeof(g_GamePaths[0]); // because we can't do g_GamePaths.size()

bool mountContent(int nExtraAppId)
{
    char szInstallDir[1024];
    if (!steamapicontext->SteamApps()->GetAppInstallDir(nExtraAppId, szInstallDir, sizeof(szInstallDir)))
        return false; // fail..

	for (int i = 0; i < size; i++)
    {
		int iVal = g_GamePaths[i].m_nAppId;
		if (iVal == nExtraAppId)
		{
			char szSearchPath[1024];
			Q_snprintf(szSearchPath, sizeof(szSearchPath), "%s/%s/*_dir.vpk", szInstallDir, g_GamePaths[i].m_pPathName);

			FileFindHandle_t hFind;
			const char* pFilename = g_pFullFileSystem->FindFirst(szSearchPath, &hFind);

			if (!pFilename)
				continue;

			do
			{
				char file[1024];
				Q_strncpy(file, pFilename, sizeof(file));

				file[strlen(file) - 8] = '\0';

				char absolutePath[1024];
				Q_snprintf(absolutePath, sizeof(absolutePath), "%s/%s/%s", szInstallDir, g_GamePaths[i].m_pPathName, file);

				Q_strcat(absolutePath, "_dir.vpk", sizeof(absolutePath));
				DevMsg("mounting path: %s\n", absolutePath);

				g_pFullFileSystem->AddSearchPath(absolutePath, "GAME");

			} while (g_pFullFileSystem->FindNext(hFind));

			g_pFullFileSystem->FindClose(hFind);
		}
    }

    return true;
};

void addSearchPathByAppId( int nAppId )
{
	for (int i = 0; i < size; i++)
	{
		int iVal = g_GamePaths[i].m_nAppId;
		char szInstallDir[1024];
		if (!steamapicontext->SteamApps()->GetAppInstallDir(iVal, szInstallDir, sizeof(szInstallDir)))
			return; // fail..

		char absolutePath[1024];

		if (iVal == 360)
		{
			const char* pathName = g_GamePaths[2].m_pPathName;
			Q_snprintf(absolutePath, sizeof(absolutePath), "%s/%s", szInstallDir, pathName);
			g_pFullFileSystem->AddSearchPath(pathName, "GAME");
		}
		if (iVal == nAppId)
		{
			const char* pathName = g_GamePaths[i].m_pPathName;
			Q_snprintf(absolutePath, sizeof(absolutePath), "%s/%s", szInstallDir, pathName);
			g_pFullFileSystem->AddSearchPath(absolutePath, "GAME");
		}
	}
};

void mountGames(void)
{
	KeyValues* pMainFile, * pFileSystemInfo;
	const char* gamePath = engine->GetGameDirectory();

	pMainFile = new KeyValues("gamecontent.txt");
	if (pMainFile->LoadFromFile(filesystem, VarArgs("%s/gamecontent.txt", gamePath), "MOD"))
	{
		pFileSystemInfo = pMainFile->FindKey("FileSystem");
		if (pFileSystemInfo)
		{
			for (KeyValues* pKey = pFileSystemInfo->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey())
			{
				if (strcmp(pKey->GetName(), "appid") == 0)
				{
					int nExtraContentId = pKey->GetInt();
					if (nExtraContentId)
					{
						DevMsg("mounting id %i\n", nExtraContentId);
						addSearchPathByAppId(nExtraContentId);
						mountContent(nExtraContentId);
					}
				}
			}
		}
	}
	pMainFile->deleteThis();
};

// vgui
class CGamesContentMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CGamesContentMenu, vgui::Frame);

public:
	CGamesContentMenu(vgui::Panel* pParent) : BaseClass(pParent, "GamesContentMenu")
	{
		SetSize(400, 450);
		SetSizeable(false);
		SetMoveable(false);
		SetTitle("CONTENT", true);

		int buttonIndex = 0;
		for (int i = 0; i < size; i++)
		{
			const char* gamePathName = g_GamePaths[i].m_pPathName;

			if (!Q_stricmp(gamePathName, "episodic") ||
				!Q_stricmp(gamePathName, "ep2") ||
				!Q_stricmp(gamePathName, "lostcoast"))
			{
				continue;
			}

			vgui::CheckButton* pCheckButton = new vgui::CheckButton(this, VarArgs("GameCheck%d", buttonIndex), gamePathName);
			pCheckButton->SetPos(40, 30 + (buttonIndex * 30));
			pCheckButton->SetSize(pCheckButton->GetWide() + 50, pCheckButton->GetTall());
			m_GameCheckButtons[buttonIndex] = pCheckButton;
			buttonIndex++;
		}

		for (int i = buttonIndex; i < size; i++)
		{
			m_GameCheckButtons[i] = nullptr;
		}

		vgui::Label* pLabel = new vgui::Label(this, "InfoLabel", "Changes to your mounted games take effect when you restart.");
		pLabel->SetPos(20, 30 + (size * 30) + 10);
		pLabel->SetSize(400, 20);

		vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

		int posX = GetWide() - 70;
		int posY = GetTall() - 40;
		vgui::Button* m_ApplyButton = new vgui::Button(this, "ApplyButton", "Apply", this, "press");
		m_ApplyButton->SetPos(posX, posY);

		int wide, tall;
		surface()->GetScreenSize(wide, tall);
		int posXX = (wide - GetWide()) / 2;
		int posYY = (tall - GetTall()) / 2;
		SetPos(posXX, posYY);

		loadAndPreCheckCheckboxes();
	}

	void loadAndPreCheckCheckboxes()
	{
		KeyValues* pMainFile, * pFileSystemInfo;
		const char* gamePath = engine->GetGameDirectory();

		pMainFile = new KeyValues("gamecontent.txt");
		if (pMainFile->LoadFromFile(filesystem, VarArgs("%s/gamecontent.txt", gamePath), "MOD"))
		{
			pFileSystemInfo = pMainFile->FindKey("FileSystem");
			if (pFileSystemInfo)
			{
				for (KeyValues* pKey = pFileSystemInfo->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey())
				{
					if (strcmp(pKey->GetName(), "appid") == 0)
					{
						int nExtraContentId = pKey->GetInt();
						for (int i = 0; i < size; i++)
						{
							if (g_GamePaths[i].m_nAppId == nExtraContentId)
							{
								m_GameCheckButtons[i]->SetSelected(true);
								break;
							}
						}
					}
				}
			}
		}
		pMainFile->deleteThis();
	}

	void OnCommand(const char* pcCommand) override
	{
		BaseClass::OnCommand(pcCommand);

		if (!Q_stricmp(pcCommand, "press"))
		{
			KeyValues* pMainFile = new KeyValues("GameContent");
			const char* gamePath = engine->GetGameDirectory();

			KeyValues* pFileSystemInfo = pMainFile->CreateNewKey();
			pFileSystemInfo->SetName("FileSystem");

			for (int i = 0; i < size; i++)
			{
				vgui::CheckButton* pCheckButton = m_GameCheckButtons[i];
				if (pCheckButton && pCheckButton->IsSelected())
				{
					KeyValues* appid = new KeyValues("appid");
					appid->SetInt(nullptr, g_GamePaths[i].m_nAppId);
					pFileSystemInfo->AddSubKey(appid);
				}
			}

			if (!pMainFile->SaveToFile(filesystem, VarArgs("%s/gamecontent.txt", gamePath), "MOD"))
			{
				Warning("failed to save gamecontent.txt\n");
			}

			pMainFile->deleteThis();
		}
	}

private:
	vgui::CheckButton* m_GameCheckButtons[size];
};

CON_COMMAND(opencontentmenu, "content manager")
{
	static CGamesContentMenu* pCurrentMenu = nullptr;

	if (pCurrentMenu)
	{
		pCurrentMenu->Close();
		pCurrentMenu = nullptr;
	}

	pCurrentMenu = new CGamesContentMenu(nullptr);
	pCurrentMenu->Activate();
}