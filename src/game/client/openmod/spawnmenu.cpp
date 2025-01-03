//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
// 
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "spawnmenu.h"

using namespace vgui;

#ifdef OMOD
extern ConVar hl2_mounted;
extern ConVar tf_mounted;
extern ConVar portal_mounted;
extern ConVar css_mounted;
extern ConVar hl1_mounted;
#endif

ConVar spawn( "cl_propmenu", "0", FCVAR_CLIENTDLL, "spawn menu" );

CSMLMenu* CSMLMenu::s_Instance = nullptr;

CSMLButton::CSMLButton( Panel* parent, const char* panelName, const char* text )
    : MenuButton( parent, panelName, text ), m_pMenu( nullptr ) {}

CSMLButton::~CSMLButton() {
    if( m_pMenu ) {
        delete m_pMenu;
        m_pMenu = nullptr;
    }
}

CSMLCommandButton::CSMLCommandButton( Panel* parent, const char* panelName, const char* labelText, const char* command )
    : Button( parent, panelName, labelText ) {
    AddActionSignalTarget( this );
    SetCommand( command );
}

void CSMLCommandButton::OnCommand( const char* command ) {
    engine->ClientCmd( (char*)command );
}

CSMLCommandButton::~CSMLCommandButton() {}

CSMLPage::CSMLPage( Panel* parent, const char* panelName )
    : PropertyPage( parent, panelName ) {
    vgui::ivgui()->AddTickSignal( GetVPanel(), 250 );
}

CSMLPage::~CSMLPage() {
    for( int i = 0; i < m_LayoutItems.Count(); ++i ) {
        Panel* p = m_LayoutItems[i];
        delete p;
    }
    m_LayoutItems.RemoveAll();
}

void CSMLPage::OnTick() {
    if( !IsVisible() ) return;

    for( int i = 0; i < m_LayoutItems.Count(); ++i ) {
        Panel* p = m_LayoutItems[i];
        p->OnTick();
    }
}

void CSMLPage::PerformLayout() {
    int x = 5, y = 5, w = 150, h = 18, gap = 2;
    int c = m_LayoutItems.Count();
    int tall = GetTall();

    for( int i = 0; i < c; ++i ) {
        Panel* p = m_LayoutItems[i];
        p->SetBounds( x, y, w, h );
        y += (h + gap);
        if( y >= tall - h ) {
            x += (w + gap);
            y = 5;
        }
    }
}

void CSMLPage::Init( KeyValues* kv ) {
    for( KeyValues* control = kv->GetFirstSubKey(); control != nullptr; control = control->GetNextKey() )
    {
        const char* m = control->GetString( "#", "" );
        if( m && m[0] )
        {
            CreateButtonX( this, control->GetName(), m );
        }
    }
}

void CSMLPage::CreateButtonX( CSMLPage* page, const char* label, const char* command ) {
    if( label && command && label[0] && command[0] ) {
#ifdef OMOD
        if( Q_strlen( label ) > 4 &&
            (!Q_stricmp( label + Q_strlen( label ) - 5, "(EP2)" ) ||
              !Q_stricmp( label + Q_strlen( label ) - 5, "(EP1)" ) ||
              !Q_stricmp( label + Q_strlen( label ) - 12, "(Lost Coast)" ) ||
              !Q_stricmp( label + Q_strlen( label ) - 7, "(Portal)" ) ||
              !Q_stricmp( label + Q_strlen( label ) - 5, "(TF2)" ) ||
              !Q_stricmp( label + Q_strlen( label ) - 5, "(HL1)" )) )
        {
            if( !hl2_mounted.GetBool() &&
                (!Q_stricmp( label + Q_strlen( label ) - 5, "(EP2)" ) ||
                  !Q_stricmp( label + Q_strlen( label ) - 5, "(EP1)" ) ||
                  !Q_stricmp( label + Q_strlen( label ) - 12, "(Lost Coast)" )) )
            {
                DevMsg( "HL2 not mounted, skipping..\n" );
                return;
            }
            else if( !portal_mounted.GetBool() &&
                     !Q_stricmp( label + Q_strlen( label ) - 7, "(Portal)" ) )
            {
                DevMsg( "Portal not mounted, skipping..\n" );
                return;
            }
            else if( !tf_mounted.GetBool() &&
                     !Q_stricmp( label + Q_strlen( label ) - 5, "(TF2)" ) )
            {
                DevMsg( "TF2 not mounted, skipping..\n" );
                return;
            }
            else if( !hl1_mounted.GetBool() &&
                     !Q_stricmp( label + Q_strlen( label ) - 5, "(HL1)" ) )
            {
                DevMsg( "HL1 not mounted, skipping..\n" );
                return;
            }
        }
#endif

        CSMLCommandButton* btn = new CSMLCommandButton( page, label, label, command );
        page->m_LayoutItems.AddToTail( btn );
    }
}

CSMLMenu::CSMLMenu( vgui::VPANEL* parent, const char* panelName )
    : PropertyDialog( nullptr, "SMLenu" ) {
    SetTitle( "SPAWN MENU", true );
    SetPos( 0, 0 );

    /* size */
    SetWide( ScreenWidth() );
    SetTall( ScreenHeight() );

    KeyValues* kv = new KeyValues( "SMLenu" );
    if( kv )
    {
        if( kv->LoadFromFile( g_pFullFileSystem, "scripts/props.txt" ) )
        {
            for( KeyValues* dat = kv->GetFirstSubKey(); dat != nullptr; dat = dat->GetNextKey() )
            {
                if( !Q_strcasecmp( dat->GetName(), "width" ) )
                {
                    SetWide( dat->GetInt() );
                }
                else if( !Q_strcasecmp( dat->GetName(), "height" ) )
                {
                    SetTall( dat->GetInt() );
                }
                else
                {
                    if( !Q_stricmp( dat->GetName(), "Portal" ) && !portal_mounted.GetBool() )
                        continue;

                    if( !Q_stricmp( dat->GetName(), "TF2" ) && !tf_mounted.GetBool() )
                        continue;

                    if( !Q_stricmp( dat->GetName(), "CSS" ) && !css_mounted.GetBool() )
                        continue;

                    CSMLPage* page = new CSMLPage( this, dat->GetName() );
                    page->Init( dat );
                    AddPage( page, dat->GetName() );
                }
            }
        }
        kv->deleteThis();
    }

    vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
    GetPropertySheet()->SetTabWidth( 72 );
    SetVisible( true );
    SetMouseInputEnabled( true );
    SetKeyBoardInputEnabled( false ); /* allow movement */
    SetSizeable( false ); /* don't allow resizing */
}

CSMLMenu::~CSMLMenu() {
    for( auto& entry : m_PageRegistry ) {
        delete entry.second;
    }
    m_PageRegistry.clear();
}

void CSMLMenu::CreateButton( const char* pageName, const char* label, const char* command ) {
    if( !s_Instance ) {
        return;
    }

    CSMLPage* page = s_Instance->FindOrCreatePage( pageName );
    if( page ) {
        CSMLPage::CreateButtonX( page, label, command );
    }
}

CSMLPage* CSMLMenu::FindOrCreatePage( const char* pageName ) {
    if( !pageName || !pageName[0] ) {
        return nullptr;
    }

    auto it = m_PageRegistry.find( pageName );
    if( it != m_PageRegistry.end() ) {
        return it->second;
    }

    CSMLPage* newPage = new CSMLPage( this, pageName );
    m_PageRegistry[pageName] = newPage;
    AddPage( newPage, pageName );
    return newPage;
}

void CSMLMenu::OnTick() {
    SetWide( ScreenWidth() );
    SetTall( ScreenHeight() );
    SetVisible( spawn.GetBool() );
}

CSMLPanelInterface::CSMLPanelInterface() : SMLPanel( nullptr ) {}

CSMLPanelInterface::~CSMLPanelInterface() {
    Destroy();
}

void CSMLPanelInterface::Create( vgui::VPANEL parent ) {
    if( !SMLPanel ) {
        SMLPanel = new CSMLMenu( &parent, "SMenu" );
        CSMLMenu::s_Instance = SMLPanel;
    }
}

void CSMLPanelInterface::Destroy() {
    if( SMLPanel ) {
        SMLPanel->SetParent( nullptr );
        delete SMLPanel;
        SMLPanel = nullptr;
        CSMLMenu::s_Instance = nullptr;
    }
}

void CSMLPanelInterface::Activate() {
    if( SMLPanel ) {
        SMLPanel->Activate();
    }
}

void CreateButton( const char* pageName, const char* label, const char* command ) {
    if( !CSMLMenu::s_Instance ) {
        return;
    }

    CSMLPage* page = CSMLMenu::s_Instance->FindOrCreatePage( pageName );
    if( page ) {
        CSMLPage::CreateButtonX( page, label, command );
    }
}

static CSMLPanelInterface g_SMLPanel;
SMLPanel* smlmenu = (SMLPanel*)&g_SMLPanel;