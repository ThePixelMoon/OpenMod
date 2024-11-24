--========== Copyleft © 2010, Team Sandbox, Some rights reserved. ===========--
--
-- Purpose:
--
--===========================================================================--

GM.Name			= "Sandbox"
GM.Homepage		= "http://www.andrewmcwatters.com/hl2sb/"
GM.Developer	= "Team Sandbox"
GM.Manual		= nil

function GM:Initialize()
if ( not _CLIENT ) then
  engine.ServerCommand( "mp_falldamage 1\n" )
  engine.ServerCommand( "mp_flashlight 1\n" )
else
  -- Andrew; not the right place to put this, but oh well.
  self:CreateDefaultPanels()
end
end

function GM:FlWeaponRespawnTime( pWeapon )
  return -1
end

function GM:FlWeaponTryRespawn( pWeapon )
  return 0
end

function GM:PlayerPlayStepSound( pPlayer, vecOrigin, psurface, fvol, force )
end

function GM:WeaponShouldRespawn( pItem )
  pItem:AddSpawnFlags( 2^30 )
  -- return 2
end
