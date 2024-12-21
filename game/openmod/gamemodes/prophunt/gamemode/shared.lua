--========== Copyleft © 2010, Team Sandbox, Some rights reserved. ===========--
--
-- Purpose:
--
--===========================================================================--

GM.Name       = "Prop Hunt"
GM.Homepage   = ""
GM.Developer  = "YourLocalMoon"
GM.Manual     = nil

function GM:Initialize()
end

function GM:Shutdown()
end

function GM:CalcPlayerView( pPlayer, eyeOrigin, eyeAngles, fov )
end

function GM:CheckGameOver()
end

function GM:ClientSettingsChanged( pPlayer )
end

function GM:CreateStandardEntities()
end

function GM:DeathNotice( pVictim, info )
end

function GM:FlWeaponRespawnTime( pWeapon )
end

function GM:FlWeaponTryRespawn( pWeapon )
end

function GM:GetGameDescription()
  return self.Name
end

function GM:GetMapRemainingTime()
  return 0;
end

function GM:GoToIntermission()
end

function GM:IsIntermission()
end

function GM:IsTeamplay()
  return false
end

function GM:LevelShutdown()
  self:Shutdown()
end

function GM:OnEntityCreated( pEntity )
end

function GM:PlayerKilled( pVictim, info )
end

function GM:PlayerPlayFootStep( pPlayer, vecOrigin, fvol, force )
end

function GM:PlayerRelationship( pPlayer, pTarget )
end

function GM:PlayerTraceAttack( info, vecDir, ptr )
end

function GM:PlayerUse( pPlayer )
end

function GM:Precache()
  _R.CBaseEntity.PrecacheScriptSound( "AlyxEmp.Charge" );
end

function GM:ShouldCollide( collisionGroup0, collisionGroup1 )
end

function GM:Think()
end

function GM:VecWeaponRespawnSpot( pWeapon )
end
