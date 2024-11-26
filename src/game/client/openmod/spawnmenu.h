#ifndef SPAWNMENU_H
#define SPAWNMENU_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/PanelListPanel.h>
#include <matsys_controls\mdlpanel.h>

class SMToolMenu : public vgui::PropertyPage
{
	typedef vgui::PropertyPage BaseClass;
public:
	SMToolMenu(vgui::Panel* parent, const char* panelName);

	virtual void OnTick(void);
	virtual void OnCommand(const char* pcCommand);
private:
	vgui::TextEntry* m_Red;
	vgui::TextEntry* m_Green;
	vgui::TextEntry* m_Blue;
	vgui::TextEntry* m_Modelscale;
	vgui::TextEntry* m_EntCreate;
};

class SMModels : public vgui::PropertyPage
{
	typedef vgui::PropertyPage BaseClass;

public:
	SMModels(vgui::Panel* parent, const char* panelName);
	const char* GetText();
	virtual void InitModels(Panel* panel, const char* modeltype, const char* modelfolder, const char* mdlPath);
	virtual void OnCommand(const char* command);

private:
	vgui::ComboBox* box;
	CMDLPanel* mdl;

	char sz_mdlname[260];
};

class SMList : public vgui::PanelListPanel
{
public:
	typedef vgui::PanelListPanel BaseClass;

	SMList(vgui::Panel* parent, const char* pName);

	virtual void OnTick(void);
	virtual void OnCommand(const char* command);
	virtual void PerformLayout();
	virtual void AddImageButton(vgui::PanelListPanel* panel, const char* image, const char* hover, const char* command);
	virtual void AddModelPanel(vgui::PanelListPanel* panel, const char* mdlname, const char* cmd);
	virtual void InitEntities(KeyValues* kv, vgui::PanelListPanel* panel, const char* enttype);
	virtual void InitModels(vgui::PanelListPanel* panel, const char* modeltype, const char* modelfolder, const char* mdlPath);
private:
	CUtlVector<vgui::Panel*> m_LayoutItems;
};

class CSMenu : public vgui::PropertyDialog
{
	typedef vgui::PropertyDialog BaseClass;
public:
	CSMenu(vgui::VPANEL* parent, const char* panelName);
	void OnTick();
	void OnCommand(const char* command);
};

class SMPanel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void		Activate(void) = 0;
};

extern SMPanel* smenu;

class ImageButton : public vgui::ImagePanel
{
	typedef vgui::ImagePanel BaseClass;

public:
	ImageButton(vgui::Panel* parent, const char* panelName, const char* normalImage, const char* mouseOverImage, const char* mouseClickImage, const char* pCmd);

	virtual void OnCursorEntered(); // When the mouse hovers over this panel, change images
	virtual void OnCursorExited(); // When the mouse leaves this panel, change back

	virtual void OnMouseReleased(vgui::MouseCode code);
	virtual void OnMousePressed(vgui::MouseCode code);

	void SetNormalImage(void);
	void SetMouseOverImage(void);
	void SetMouseClickImage(void);

private:
	vgui::IImage* i_normalImage; // The image when the mouse isn't over it, and its not being clicked
	vgui::IImage* i_mouseOverImage; // The image that appears as when the mouse is hovering over it
	vgui::IImage* i_mouseClickImage; // The image that appears while the mouse is clicking

	char command[260]; // The command when it is clicked on
	char m_normalImage[260];
	char m_mouseOverImage[260];
	char m_mouseClickImage[260];

	Panel* m_pParent;

	bool hasCommand; // If this is to act as a button
	bool hasMouseOverImage; // If this changes images when the mouse is hovering over it
	bool hasMouseClickImage; // If this changes images when the mouse is clicking it

	virtual void SetImage(vgui::IImage* image); //Private because this really shouldnt be changed
};

#endif