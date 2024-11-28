--========= Copyleft © 2010-2013, Team Sandbox, Some rights reserved. ============--
--
-- Purpose:
--
-- $NoKeywords: $
--===========================================================================--

include( "../includes/extensions/table.lua" );
include( "../includes/extensions/vgui.lua" );

local vgui = vgui;

local CStartGame = {
	m_pStartButton = INVALID_PANEL
}

-------------------------------------------------------------------------------
-- Purpose: Basic help dialog
-------------------------------------------------------------------------------
function CStartGame:Init(parent, panelName)
--	self:SetDeleteSelfOnClose(true);
	self:SetBounds(0, 0, 512, 406);
	self:SetSizeable( false );

	self:SetTitle("START GAME", true);
	
	self.m_pStartButton = vgui.Button( 
		self, 
		"StartGame", 
		"Start Game" );

--	self:SetApplyButtonVisible(true);
	self:LoadControlSettings("resource\\CStartGame.res");
end

-------------------------------------------------------------------------------
-- Purpose: Opens the dialog
-------------------------------------------------------------------------------
function CStartGame:Run()
	self:SetTitle("START GAME", true);
	self:Activate();
end

vgui.register( CStartGame, "CStartGame", "Frame" )
