--========== Copyleft © 2010, Team Sandbox, Some rights reserved. ===========--
--
-- Purpose: Initialize the base scripted weapon.
--
--===========================================================================--

SWEP.printname				= "FLECHETTE GUN"
SWEP.viewmodel				= "models/weapons/v_smg1.mdl"
SWEP.playermodel			= "models/weapons/w_smg1.mdl"
SWEP.anim_prefix			= "smg2"
SWEP.bucket					= 2
SWEP.bucket_position		= 2

SWEP.clip_size				= -1
SWEP.clip2_size				= -1
SWEP.default_clip			= -1
SWEP.default_clip2			= -1
SWEP.primary_ammo			= "ar2"
SWEP.secondary_ammo			= "None"

SWEP.weight					= 7
SWEP.item_flags				= 0

SWEP.damage					= 0

SWEP.SoundData				=
{
	empty					= "",
	single_shot				= ""
}

SWEP.ShowUsageHint			= false
SWEP.AutoSwitchTo			= true
SWEP.AutoSwitchFrom			= true
SWEP.BuiltRightHanded		= true
SWEP.AllowFlipping			= true
SWEP.MeleeWeapon			= false
SWEP.UseHands				= true

SWEP.m_acttable            =
{
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_SMG1,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_SMG1,						false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_SMG1,				false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_SMG1,				false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_SMG1,					false },
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_SMG1,					false },
};

function SWEP:Initialize()
	self.m_bReloadsSingly	= false;
	self.m_bFiresUnderwater	= true;
end

function SWEP:Precache()
end

function SWEP:PrimaryAttack()
	local pPlayer = self:GetOwner()

	self.m_flNextPrimaryAttack = gpGlobals.curtime() + 0.07;
	self.m_flNextSecondaryAttack = gpGlobals.curtime() + 0.07;
	
	self:SendWeaponAnim( 180 );
	pPlayer:SetAnimation( 5 );
	ToHL2MPPlayer(pPlayer):DoAnimationEvent( 0 );

	-- fire the pooting bullets
	local eye = pPlayer:EyeAngles()
	local shootpos = pPlayer:EyePosition()
	
	local forward, right, up = pPlayer:GetVectors()
	
	local ent = CreateEntityByName( "hunter_flechette" )
	ent:SetAbsOrigin( shootpos + forward * 32 )
	ent:SetAbsAngles( eye )
	ent:SetOwnerEntity( pPlayer )
	ent:Spawn()
	ent:Activate()
	
	ent:SetAbsVelocity(forward * 2000)
end

function SWEP:SecondaryAttack()
end

function SWEP:Reload()
	return true;
end

function SWEP:Think()
end

function SWEP:CanHolster()
end

function SWEP:Deploy()
end

function SWEP:GetDrawActivity()
	return 171;
end

function SWEP:Holster( pSwitchingTo )
end

function SWEP:ItemPostFrame()
end

function SWEP:ItemBusyFrame()
end

function SWEP:DoImpactEffect()
end
