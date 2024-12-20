//========= Copyright OpenMod, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "spawnmenu.h"
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

using namespace vgui;

#ifdef OMOD
extern ConVar hl2_mounted;
extern ConVar tf_mounted;
extern ConVar portal_mounted;
extern ConVar css_mounted;
#endif

class CSMLButton : public MenuButton
{
    typedef MenuButton BaseClass;

public:
    CSMLButton(Panel* parent, const char* panelName, const char* text)
        : BaseClass(parent, panelName, text), m_pMenu(nullptr)
    {}

private:
    Menu* m_pMenu;
};

class CSMLCommandButton : public Button
{
    typedef Button BaseClass;

public:
    CSMLCommandButton(Panel* parent, const char* panelName, const char* labelText, const char* command)
        : BaseClass(parent, panelName, labelText)
    {
        AddActionSignalTarget(this);
        SetCommand(command);
    }

    void OnCommand(const char* command) override
    {
        engine->ClientCmd((char*)command);
    }
};

class CSMLPage : public PropertyPage
{
    typedef PropertyPage BaseClass;

public:
    CSMLPage(Panel* parent, const char* panelName)
        : BaseClass(parent, panelName)
    {
        vgui::ivgui()->AddTickSignal(GetVPanel(), 250);
    }

    void OnTick() override
    {
        BaseClass::OnTick();

        if (!IsVisible())
            return;

        for (int i = 0; i < m_LayoutItems.Count(); ++i)
        {
            Panel* p = m_LayoutItems[i];
            p->OnTick();
        }
    }

    void PerformLayout() override
    {
        BaseClass::PerformLayout();

        int x = 5, y = 5, w = 150, h = 18, gap = 2;
        int c = m_LayoutItems.Count();
        int tall = GetTall();

        for (int i = 0; i < c; ++i)
        {
            Panel* p = m_LayoutItems[i];
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
        for (KeyValues* control = kv->GetFirstSubKey(); control != nullptr; control = control->GetNextKey())
        {
            const char* m = control->GetString("#", "");
            if (m && m[0])
            {
                CreateButton(control->GetName(), m);
            }
        }
    }

    void CreateButton(const char* label, const char* command)
    {
        if (label && command && label[0] && command[0])
        {
#ifdef OMOD
            if (Q_strlen(label) > 4 &&
                (!Q_stricmp(label + Q_strlen(label) - 5, "(EP2)") || 
                 !Q_stricmp(label + Q_strlen(label) - 5, "(EP1)") || 
                 !Q_stricmp(label + Q_strlen(label) - 12, "(Lost Coast)") || 
                 !Q_stricmp(label + Q_strlen(label) - 7, "(Portal)") ||
                 !Q_stricmp(label + Q_strlen(label) - 5, "(TF2)")))
            {
                if (!hl2_mounted.GetBool() == true && 
                    !Q_stricmp(label + Q_strlen(label) - 5, "(EP2)") && 
                    !Q_stricmp(label + Q_strlen(label) - 5, "(EP1)") && 
                    !Q_stricmp(label + Q_strlen(label) - 12, "(Lost Coast)"))
                {
                    DevMsg("HL2 not mounted, skipping..\n");
                    return;
                }
                else if (!portal_mounted.GetBool() == true && 
                         !Q_stricmp(label + Q_strlen(label) - 7, "(Portal)"))
                {
                    DevMsg("Portal not mounted, skipping..\n");
                    return;
                }
                else if (!tf_mounted.GetBool() == true && 
                         !Q_stricmp(label + Q_strlen(label) - 5, "(TF2)"))
                {
                    DevMsg("TF2 not mounted, skipping..\n");
                    return;
                }
            }
#endif

            CSMLCommandButton* btn = new CSMLCommandButton(this, label, label, command);
            m_LayoutItems.AddToTail(btn);
        }
    }

private:
    CUtlVector<Panel*> m_LayoutItems;
};

ConVar spawn("cl_propmenu", "0", FCVAR_CLIENTDLL, "spawn menu");

class CSMLMenu : public PropertyDialog
{
    typedef PropertyDialog BaseClass;

public:
    CSMLMenu(vgui::VPANEL* parent, const char* panelName)
        : BaseClass(nullptr, "SMLenu")
    {
        SetTitle("SPAWN MENU", true);
        SetPos(0, 0);

        /* size */
        SetWide(ScreenWidth());
        SetTall(ScreenHeight());

        KeyValues* kv = new KeyValues("SMLenu");
        if (kv)
        {
            if (kv->LoadFromFile(g_pFullFileSystem, "scripts/props.txt"))
            {
                for (KeyValues* dat = kv->GetFirstSubKey(); dat != nullptr; dat = dat->GetNextKey())
                {
                    if (!Q_strcasecmp(dat->GetName(), "width"))
                    {
                        SetWide(dat->GetInt());
                    }
                    else if (!Q_strcasecmp(dat->GetName(), "height"))
                    {
                        SetTall(dat->GetInt());
                    }
                    else
                    {
                        if (!Q_stricmp(dat->GetName(), "Portal") && !portal_mounted.GetBool())
                            continue;

                        if (!Q_stricmp(dat->GetName(), "TF2") && !tf_mounted.GetBool())
                            continue;

                        if (!Q_stricmp(dat->GetName(), "CSS") && !tf_mounted.GetBool())
                            continue;

                        CSMLPage* page = new CSMLPage(this, dat->GetName());
                        page->Init(dat);
                        AddPage(page, dat->GetName());
                    }
                }
            }
            kv->deleteThis();
        }

        vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
        GetPropertySheet()->SetTabWidth(72);
        SetVisible(true);
        SetMouseInputEnabled(true);
        SetKeyBoardInputEnabled(false); /* allow movement */
        SetSizeable(false); /* don't allow resizing */
    }

    void OnTick() override
    {
        BaseClass::OnTick();

        /* size */
        SetWide(ScreenWidth());
        SetTall(ScreenHeight());

        SetVisible(spawn.GetBool());
    }

    void OnCommand(const char* command) override
    {
        BaseClass::OnCommand(command);
    }
};

class CSMLPanelInterface : public SMLPanel
{
private:
    CSMLMenu* SMLPanel;

public:
    CSMLPanelInterface() : SMLPanel(nullptr) {}

    void Create(vgui::VPANEL parent)
    {
        if (!SMLPanel)
        {
            SMLPanel = new CSMLMenu(&parent, "SMenu");
        }
    }

    void Destroy()
    {
        if (SMLPanel)
        {
            SMLPanel->SetParent(nullptr);
            delete SMLPanel;
            SMLPanel = nullptr;
        }
    }

    void Activate()
    {
        if (SMLPanel)
        {
            SMLPanel->Activate();
        }
    }
};

static CSMLPanelInterface g_SMLPanel;
SMLPanel* smlmenu = (SMLPanel*)&g_SMLPanel;