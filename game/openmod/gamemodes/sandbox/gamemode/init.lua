--========== Copyleft © 2010, Team Sandbox, Some rights reserved. ===========--
--
-- Purpose:
--
--===========================================================================--

include( "shared.lua" )

local tSpawnPointClassnames = {
  "info_player_deathmatch",
  "info_player_combine",
  "info_player_rebel",
  "info_player_terrorist",
  "info_player_counterterrorist",
  "info_player_axis",
  "info_player_allies",
  "info_player_start"
}

function GM:AddLevelDesignerPlacedObject( pEntity )
  return false
end

function GM:GiveDefaultItems( pPlayer )
  pPlayer:GiveNamedItem( "weapon_physgun" )
  pPlayer:GiveNamedItem("weapon_crowbar")
  pPlayer:GiveNamedItem("weapon_stunstick")
  pPlayer:GiveNamedItem("weapon_pistol")
  pPlayer:GiveNamedItem("weapon_357")
     
  pPlayer:GiveNamedItem("weapon_smg1")
  pPlayer:GiveNamedItem("weapon_ar2")
     
  pPlayer:GiveNamedItem("weapon_shotgun")
  pPlayer:GiveNamedItem("weapon_frag")
     
  pPlayer:GiveNamedItem("weapon_crossbow")
     
  pPlayer:GiveNamedItem("weapon_rpg")
     
  pPlayer:GiveNamedItem("weapon_slam")
     
  pPlayer:GiveNamedItem("weapon_physcannon")
  pPlayer:GiveNamedItem("weapon_physgun")

  pPlayer:GiveNamedItem("weapon_bugbait")
  
  self.BaseClass.GiveDefaultItems( self, pPlayer ) -- fallback..
end

function GM:ItemShouldRespawn( pItem )
  pItem:AddSpawnFlags( 2^30 )
  -- return 6
end

function GM:PlayerEntSelectSpawnPoint( pHL2MPPlayer )
  local tSpawnPoints = {}
  local pSpot = NULL
  for _, classname in ipairs( tSpawnPointClassnames ) do
    pSpot = gEntList.FindEntityByClassname( NULL, classname )
    while ( pSpot ~= NULL ) do
      table.insert( tSpawnPoints, pSpot )
      pSpot = gEntList.FindEntityByClassname( pSpot, classname )
    end
  end
  return tSpawnPoints[ math.random( 1, #tSpawnPoints ) ]
end

function GM:PlayerPickupObject( pHL2MPPlayer, pObject, bLimitMassAndSize )
end
