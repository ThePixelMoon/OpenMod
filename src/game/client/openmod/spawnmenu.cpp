//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//
#include "cbase.h"
#include <stdio.h>
#include <vgui/IVGui.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Controls.h>
#include <vgui_controls/MenuButton.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/PanelListPanel.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>

#include "filesystem.h"
#include "spawnmenu.h"
#include "game_controls/basemodel_panel.h"

#include "tier0/memdbgon.h"

using namespace vgui;

ConVar cl_spawnmenu("cl_spawnmenu", "0", FCVAR_CLIENTDLL, "Spawn Menu");

ImageButton::ImageButton(vgui::Panel* parent, const char* panelName, const char* normalImage, const char* mouseOverImage, const char* mouseClickImage, const char* pCmd)
	: ImagePanel(parent, panelName)
{
	m_pParent = parent;

	SetParent(parent);

	if (pCmd != NULL)
	{
		Q_strncpy(command, pCmd, 100);
		hasCommand = true;
	}
	else
		hasCommand = false;

	// Защищаемся от Buffer Overflow
	Q_strncpy(m_normalImage, normalImage, 100);

	i_normalImage = vgui::scheme()->GetImage(m_normalImage, true);

	if (mouseOverImage != NULL)
	{
		Q_strcpy(m_mouseOverImage, mouseOverImage);
		i_mouseOverImage = vgui::scheme()->GetImage(m_mouseOverImage, true);
		hasMouseOverImage = true;
	}
	else
		hasMouseOverImage = false;

	if (mouseClickImage != NULL)
	{
		Q_strcpy(m_mouseClickImage, mouseClickImage);
		i_mouseClickImage = vgui::scheme()->GetImage(m_mouseClickImage, true);
		hasMouseClickImage = true;
	}
	else
		hasMouseClickImage = false;

	SetNormalImage();
}

void ImageButton::OnCursorEntered()
{
	if (hasMouseOverImage)
		SetMouseOverImage();
}

void ImageButton::OnCursorExited()
{
	if (hasMouseOverImage)
		SetNormalImage();
}

void ImageButton::OnMouseReleased(vgui::MouseCode code)
{
	m_pParent->OnCommand(command);

	if ((code == MOUSE_LEFT) && hasMouseClickImage)
		SetNormalImage();
}

void ImageButton::OnMousePressed(vgui::MouseCode code)
{
	if ((code == MOUSE_LEFT) && hasMouseClickImage)
		SetMouseClickImage();
}

void ImageButton::SetNormalImage(void)
{
	SetImage(i_normalImage);
	Repaint();
}

void ImageButton::SetMouseOverImage(void)
{
	SetImage(i_mouseOverImage);
	Repaint();
}

void ImageButton::SetMouseClickImage(void)
{
	SetImage(i_mouseClickImage);
	Repaint();
}

void ImageButton::SetImage(vgui::IImage* image)
{
	BaseClass::SetImage(image);
}

SMModels::SMModels( vgui::Panel *parent, const char *panelName ) : BaseClass( parent, panelName )
{
	vgui::ivgui()->AddTickSignal( GetVPanel(), 250 );
	box = new ComboBox( this, "ComboBox", 50, false);
	mdl = new CMDLPanel( this, "MDLPanel" );
	LoadControlSettings( "menu/resource/ui/smmodels.res" );
}

void SMModels::InitModels( Panel *panel, const char *modeltype, const char *modelfolder, const char *mdlPath )
{
	FileFindHandle_t fh;
	char const *pModel = g_pFullFileSystem->FindFirst( mdlPath, &fh );
	
	while ( pModel )
	{
		if ( pModel[0] != '.' )
		{
			char ext[ 10 ];
			Q_ExtractFileExtension( pModel, ext, sizeof( ext ) );
			if ( !Q_stricmp( ext, "mdl" ) )
			{
				char file[MAX_PATH];
				Q_FileBase( pModel, file, sizeof( file ) );
			
				if ( pModel && pModel[0] )
				{
					char modelname[MAX_PATH], 
						 entspawn[MAX_PATH], 
						 modelfile[MAX_PATH];

					Q_snprintf( modelname, sizeof(modelname), "%s/%s", modelfolder, file );
					Q_snprintf( entspawn, sizeof(entspawn), "%s_create %s", modeltype, modelname );
					Q_snprintf( modelfile, sizeof( modelfile ), "models/%s.mdl", modelname );
					
					box->AddItem( modelname, NULL );
				}
			}
		}
		pModel = g_pFullFileSystem->FindNext( fh );
	}
	g_pFullFileSystem->FindClose( fh );
}

const char *SMModels::GetText()
{
	box->GetText( sz_mdlname, 260 );
#ifdef DEBUG	
	printf( "%s\n", sz_mdlname );
#endif
	return sz_mdlname;
}

void SMModels::OnCommand( const char *command )
{
	BaseClass::OnCommand( command );

	if(!Q_stricmp(command, "spawn"))
	{
		char spawncommand[260];

		Q_snprintf( spawncommand, sizeof( spawncommand ), "prop_physics_create %s", GetText() );
		engine->ClientCmd( spawncommand );
	}
	else if ( !Q_stricmp( command, "preview")  )
	{	
		char mdlname[260];
		Q_snprintf( mdlname, sizeof( mdlname ), "models/%s.mdl", GetText() );
		mdl->SetMDL( mdlname );
	}
}

SMList::SMList( vgui::Panel *parent, const char *pName ) : BaseClass( parent, pName )
{
	SetBounds( 0, 0, 800, 640 );
}

void SMList::OnTick( void )
{
	BaseClass::OnTick();

	if ( !IsVisible() )
		return;

	int c = m_LayoutItems.Count();
	for ( int i = 0; i < c; i++ )
	{
		vgui::Panel *p = m_LayoutItems[ i ];
		p->OnTick();
	}
}

void SMList::OnCommand( const char *command )
{
	engine->ClientCmd( (char *)command );
}

void SMList::PerformLayout()
{
	BaseClass::PerformLayout();

	int w = 64;
	int h = 64;
	int x = 5;
	int y = 5;

	for ( int i = 0; i < m_LayoutItems.Count(); i++ )
	{	
		vgui::Panel *p = m_LayoutItems[ i ];
		p->SetBounds( x, y, w, h );

		x += ( w + 2 );
		if ( x >= GetWide() - w )
		{
			y += ( h + 2 );
			x = 5;
		}	
	}
}

void SMList::AddImageButton( PanelListPanel *panel, const char *image, const char *hover, const char *command )
{
	ImageButton *btn = new ImageButton( panel, image, image, hover, NULL, command );
	m_LayoutItems.AddToTail( btn );
	panel->AddItem( NULL, btn );
}

void SMList::AddModelPanel( PanelListPanel *panel, const char *mdlname, const char *cmd )
{
	CMDLPanel *mdl = new CMDLPanel( panel, "MDLPanel" );
	mdl->SetMDL( mdlname );
	m_LayoutItems.AddToTail( mdl );
	panel->AddItem( NULL, mdl );
}
	
void SMList::InitEntities( KeyValues *kv, PanelListPanel *panel, const char *enttype )
{
	for ( KeyValues *control = kv->GetFirstSubKey(); control != NULL; control = control->GetNextKey() )
	{
		const char *entname;
		
		if ( !Q_strcasecmp( control->GetName(), "entity" ) )
		{
			entname = control->GetString();
		}
		
		if( Q_strncmp( entname, enttype, Q_strlen(enttype) ) == 0 )
		{
			if ( entname && entname[0] )
			{
				char entspawn[MAX_PATH], normalImage[MAX_PATH], vtf[MAX_PATH], vtf_without_ex[MAX_PATH], vmt[MAX_PATH], file[MAX_PATH];
				
				Q_snprintf( entspawn, sizeof(entspawn), "ent_create %s", entname );
				Q_snprintf( normalImage, sizeof(normalImage), "smenu/%s", entname );
				Q_snprintf( vtf, sizeof( vtf ), "materials/vgui/smenu/%s.vtf", entname );
				Q_snprintf( vtf_without_ex, sizeof(vtf_without_ex), "vgui/smenu/%s", entname );
				Q_snprintf( vmt, sizeof( vmt ), "materials/vgui/smenu/%s.vmt", entname );

				if ( filesystem->FileExists( vtf ) && filesystem->FileExists( vmt ) )
				{
					AddImageButton( panel, normalImage, NULL, entspawn );
					continue;
				}
			}
		}
	}		
}

void SMList::InitModels( PanelListPanel *panel, const char *modeltype, const char *modelfolder, const char *mdlPath )
{
	FileFindHandle_t fh;
	char const *pModel = g_pFullFileSystem->FindFirst( mdlPath, &fh );
	
	while ( pModel )
	{
		if ( pModel[0] != '.' )
		{
			char ext[ 10 ];
			Q_ExtractFileExtension( pModel, ext, sizeof( ext ) );
			if ( !Q_stricmp( ext, "mdl" ) )
			{
				char file[MAX_PATH];
				Q_FileBase( pModel, file, sizeof( file ) );
			
				if ( pModel && pModel[0] )
				{
					char modelname[MAX_PATH], 
						 entspawn[MAX_PATH], 
						 modelfile[MAX_PATH];
					Q_snprintf( modelname, sizeof(modelname), "%s/%s", modelfolder, file );
					Q_snprintf( entspawn, sizeof(entspawn), "%s_create %s", modeltype, modelname );
					Q_snprintf( modelfile, sizeof( modelfile ), "models/%s.mdl", modelname );
					
					DevMsg("Added: %s\n", modelfile);
					AddModelPanel( panel, modelfile, entspawn );
				}
			}
		}
		pModel = g_pFullFileSystem->FindNext( fh );
	}
	g_pFullFileSystem->FindClose( fh );
}

CSMenu::CSMenu( vgui::VPANEL *parent, const char *panelName ) : BaseClass( NULL, "SMenu" )
{
	SetTitle( "SMenu", true );
	SetScheme(vgui::scheme()->LoadSchemeFromFile("menu/resource/sch.res", "SourceScheme"));

	SetProportional(true);
	
	int w = 750;
	int h = 450;

	if (IsProportional())
	{
		w = scheme()->GetProportionalScaledValueEx(GetScheme(), w);
		h = scheme()->GetProportionalScaledValueEx(GetScheme(), h);
	}

	SetSize(w, h);
		
	KeyValues *kv = new KeyValues( "SMenu" );
	if ( kv )
	{
		if ( kv->LoadFromFile(g_pFullFileSystem, "menu/entitylist.txt") )
		{
			SMList *npces = new SMList( this, "EntityPanel");
			npces->InitEntities( kv, npces, "npc_" );
			SMList *weapons = new SMList( this, "EntityPanel");
			weapons->InitEntities( kv, weapons, "weapon_" );
			weapons->InitEntities( kv, weapons, "item_");
			weapons->InitEntities( kv, weapons, "ammo_");
			AddPage( npces, "NPCs" );
			AddPage( weapons, "Weapons");
		}
		kv->deleteThis();
	}
	
	SMList *mdl = new SMList( this, "PropMenu" );
	FileFindHandle_t fh;
	for ( const char *pDir = filesystem->FindFirstEx( "models/*", "GAME", &fh ); pDir && *pDir; pDir = filesystem->FindNext( fh ) )
	{			
		if ( Q_strncmp( pDir, "props_", Q_strlen("props_") ) == 0 ) {
			if ( filesystem->FindIsDirectory( fh ) )
			{
				char dir[MAX_PATH];
				char file[MAX_PATH];
				Q_FileBase( pDir, file, sizeof( file ) );
				Q_snprintf( dir, sizeof( dir ), "models/%s/*.mdl", file );
				printf("%s\n", pDir );
				mdl->InitModels( mdl, "prop_physics", file, dir );
			}
		}
	}

	AddPage( mdl, "Props" );
	
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
	GetPropertySheet()->SetTabWidth(72);
	SetMoveable( true );
	SetVisible( true );
	SetSizeable( true );
}

void CSMenu::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_spawnmenu.GetBool());
}

void CSMenu::OnCommand( const char *command )
{
	BaseClass::OnCommand( command );
		
	if (!Q_stricmp(command, "Close"))	
	{
		cl_spawnmenu.SetValue(0);
	}
}

class CSMPanelInterface : public SMPanel
{
private:
	CSMenu *SMPanel;
public:
	CSMPanelInterface()
	{
		SMPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		SMPanel = new CSMenu(&parent, "SMenu");
	}
	void Destroy()
	{
		if (SMPanel)
		{
			SMPanel->SetParent((vgui::Panel *)NULL);
			delete SMPanel;
		}
	}
	void Activate(void)
	{
		if (SMPanel)
		{
			SMPanel->Activate();
		}
	}
};
static CSMPanelInterface g_SMPanel;
SMPanel* smenu = (SMPanel*)&g_SMPanel;
