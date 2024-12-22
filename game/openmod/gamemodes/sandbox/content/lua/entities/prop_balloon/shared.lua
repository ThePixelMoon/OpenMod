--========== Copyleft © 2010, Team Sandbox, Some rights reserved. ===========--
--
-- Purpose:
--
--===========================================================================--

-- ent_create prop_balloon

local Warning = dbg.Warning
local Msg = dbg.Msg

local physObj
local velocity = 2

function ENT:Initialize()
	if ( not _CLIENT ) then
		local allowPrecache = self.IsPrecacheAllowed();
		self.SetAllowPrecache( true );
		self.PrecacheModel( "models/props_junk/watermelon01.mdl" );
		self:SetModel( "models/props_junk/watermelon01.mdl" )
		self.SetAllowPrecache( allowPrecache );
		physObj = self:VPhysicsInitNormal( 6, 0, false );
		if ( not physObj ) then
			self:SetSolid( 0 );
			self:SetMoveType( 0 );
			Warning("ERROR!: Can't create physics object for " .. self:GetModelName() .. "\n" );
		end
		
		-- ballooonnzzzz		
		physObj:SetMass(100)
		physObj:Wake()
		physObj:EnableGravity(false)
		
		self:SetRenderColor(255,0,0,255)
	end
end

function ENT:StartTouch( pEntity )
end

function ENT:Touch( pEntity )
end

function ENT:EndTouch( pEntity )
end

function ENT:VPhysicsUpdate( pPhysics )
	-- i know this will be unoptimized, but as far as i know this is the only way to do this
	velocity = velocity + 2
	if velocity >= 256 then
		velocity = 256
	end
	
	physObj:SetVelocity(Vector(0,0,velocity), Vector(0,0,0))
end
