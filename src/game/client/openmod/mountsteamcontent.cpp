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

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

// from HL2SB, because i am a lazy ass
typedef struct
{
	const char* m_pPathName;
	int m_nAppId;
} gamePaths_t;

gamePaths_t g_GamePaths[8] =
{
	{ "hl2",		220    },
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

			DevMsg("mounting path: %s\n", VarArgs("%s/%s", szInstallDir, g_GamePaths[i].m_pPathName));
			g_pFullFileSystem->AddSearchPath(VarArgs("%s/%s", szInstallDir, g_GamePaths[i].m_pPathName), "GAME");

			if (!pFilename)
				continue;

			if ( iVal == 360 )
			{
				do
				{
					char filef[1024];
					Q_strncpy(filef, pFilename, sizeof(filef));

					filef[strlen(filef) - 8] = '\0';

					char aPath[1024];
					Q_snprintf(aPath, sizeof(aPath), "%s/hl1/hl1_pak", szInstallDir);

					Q_strcat(aPath, "_dir.vpk", sizeof(aPath));
					DevMsg("mounting path: %s\n", aPath);

					g_pFullFileSystem->AddSearchPath(aPath, "GAME");

				} while (g_pFullFileSystem->FindNext(hFind));
			}

			if (iVal == 220) // workaround for hl2 anniversary update
			{
				struct EpisodeInfo
				{
					const char* folder;
					const char* pakPrefix;
				};

				EpisodeInfo episodes[] = {
					{"ep2", "ep2_pak"},
					{"episodic", "ep1_pak"},
					{"lostcoast", "lostcoast_pak"}
				};

				for (const auto& episode : episodes)
				{
					g_pFullFileSystem->AddSearchPath(VarArgs("%s/%s", szInstallDir, episode.folder), "GAME");

					do
					{
						char aPath[1024];
						Q_snprintf(aPath, sizeof(aPath), "%s/%s/%s_dir.vpk", szInstallDir, episode.folder, episode.pakPrefix);

						DevMsg("mounting path: %s\n", aPath);
						g_pFullFileSystem->AddSearchPath(aPath, "GAME");
					} while (g_pFullFileSystem->FindNext(hFind));
				}
			}

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