--========== Copyleft © 2010, Team Sandbox, Some rights reserved. ===========--
--
-- Purpose:
--
--===========================================================================--

-- ent_create prop_light

local Warning = dbg.Warning
local Msg = dbg.Msg

local light -- to set

function ENT:Initialize()
	if ( not _CLIENT ) then
		local allowPrecache = self.IsPrecacheAllowed();
		self.SetAllowPrecache( true );
		self.PrecacheModel( "models/props_junk/watermelon01.mdl" );
		self:SetModel( "models/props_junk/watermelon01.mdl" )
		self.SetAllowPrecache( allowPrecache );
		local pPhysicsObject = self:VPhysicsInitNormal( 6, 0, false );

		if ( not pPhysicsObject ) then
			self:SetSolid( 0 );
			self:SetMoveType( 0 );
			Warning("ERROR!: Can't create physics object for " .. self:GetModelName() .. "\n" );
		end
	end
	
	-- get keyvalue color
	local r = self:GetKeyValue("red") or 255
	local g = self:GetKeyValue("green") or 255
	local b = self:GetKeyValue("blue") or 255
	
	-- coloring
	self:SetRenderColor(r,g,b,0)
	
	-- lights
	local lightValue = string.format("%d %d %d 200", r, g, b)
	light = CreateEntityByName("light_dynamic")
	light:KeyValue("_light", lightValue)
	light:KeyValue("brightness", "2")
    light:KeyValue("distance", "256")
	light:SetParent(self)
	light:Spawn()
	light:Activate()
end

function ENT:StartTouch( pEntity )
end

function ENT:Touch( pEntity )
end

function ENT:EndTouch( pEntity )
end

function ENT:VPhysicsUpdate( pPhysics )
	-- get keyvalue color
	local r = self:GetKeyValue("red") or 255
	local g = self:GetKeyValue("green") or 255
	local b = self:GetKeyValue("blue") or 255
	
	-- coloring
	self:SetRenderColor(r,g,b,0)
	
	local lightValue = string.format("%d %d %d 200", r, g, b)
	light:KeyValue("_light", lightValue)
end
