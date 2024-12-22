--========== Copyleft © 2010, Team Sandbox, Some rights reserved. ===========--
--
-- Purpose: Initialize the base scripted weapon.
--
--===========================================================================--

SWEP.printname				= "MEDKIT"
SWEP.viewmodel				= "models/weapons/v_medkit.mdl"
SWEP.playermodel			= "models/weapons/w_medkit.mdl"
SWEP.anim_prefix			= "anim"
SWEP.bucket					= 5
SWEP.bucket_position		= 3

SWEP.clip_size				= 30
SWEP.clip2_size				= -1
SWEP.default_clip			= 30
SWEP.default_clip2			= -1
SWEP.primary_ammo			= "ar2"
SWEP.secondary_ammo			= "None"

SWEP.weight					= 7
SWEP.item_flags				= 0

SWEP.damage					= 75

SWEP.SoundData				=
{
	empty					= "",
	single_shot				= "HealthKit.Touch"
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
	{ ACT_HL2MP_IDLE,					ACT_HL2MP_IDLE_PISTOL,					false },
	{ ACT_HL2MP_RUN,					ACT_HL2MP_RUN_PISTOL,					false },
	{ ACT_HL2MP_IDLE_CROUCH,			ACT_HL2MP_IDLE_CROUCH_PISTOL,			false },
	{ ACT_HL2MP_WALK_CROUCH,			ACT_HL2MP_WALK_CROUCH_PISTOL,			false },
	{ ACT_HL2MP_GESTURE_RANGE_ATTACK,	ACT_HL2MP_GESTURE_RANGE_ATTACK_PISTOL,	false },
	{ ACT_HL2MP_GESTURE_RELOAD,			ACT_HL2MP_GESTURE_RELOAD_PISTOL,		false },
	{ ACT_HL2MP_JUMP,					ACT_HL2MP_JUMP_PISTOL,					false },
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_PISTOL,				false },
};

function SWEP:Initialize()
	self.m_bReloadsSingly	= false;
	self.m_bFiresUnderwater	= true;
end

function SWEP:Precache()
end

function SWEP:PrimaryAttack()
	local pPlayer = self:GetOwner();

	if ( ToBaseEntity( pPlayer ) == NULL ) then
		return;
	end

	if pPlayer:GetHealth() >= 100 then
		print("100 hp")
		return
	end

	if ( self.m_iClip1 <= 0 ) then
		if ( not self.m_bFireOnEmpty ) then
			self:Reload();
		else
			self:WeaponSound( 0 );
			self.m_flNextPrimaryAttack = 0.15;
		end

		return;
	end

	self:WeaponSound( 1 );

	self:SendWeaponAnim( 180 );
	pPlayer:SetAnimation( 5 );
	ToHL2MPPlayer(pPlayer):DoAnimationEvent( 0 );

	self.m_flNextPrimaryAttack = gpGlobals.curtime() + 0.25;
	self.m_flNextSecondaryAttack = gpGlobals.curtime() + 0.25;

	self.m_iClip1 = self.m_iClip1 - 1;

	pPlayer:SetHealth(pPlayer:GetHealth() + 5)
	
	-- fallback
	if pPlayer:GetHealth() >= 100 then
		pPlayer:SetHealth(100)
	end

	if ( self.m_iClip1 == 0 and pPlayer:GetAmmoCount( self.m_iPrimaryAmmoType ) <= 0 ) then
		-- HEV suit - indicate out of ammo condition
		pPlayer:SetSuitUpdate( "!HEV_AMO0", 0, 0 );
	end
end

function SWEP:SecondaryAttack()
end

function SWEP:Reload()
	local fRet = self:DefaultReload( self:GetMaxClip1(), self:GetMaxClip2(), 182 );
	if ( fRet ) then
--		self:WeaponSound( 6 );
		ToHL2MPPlayer(self:GetOwner()):DoAnimationEvent( 3 );
	end
	return fRet;
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
