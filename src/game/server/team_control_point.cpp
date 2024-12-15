//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "team_control_point.h"
#include "player.h"
#include "teamplay_gamerules.h"
#include "teamplayroundbased_gamerules.h"
#include "team.h"
#include "team_control_point_master.h"
#include "mp_shareddefs.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"

#ifdef TF_DLL
#include "tf_shareddefs.h"
#include "tf_gamerules.h"
#endif

#define CONTROL_POINT_UNLOCK_THINK			"UnlockThink"

BEGIN_DATADESC(CTeamControlPoint)
	DEFINE_KEYFIELD( m_iszPrintName,			FIELD_STRING,	"point_printname" ),
	DEFINE_KEYFIELD( m_iCPGroup,				FIELD_INTEGER,	"point_group" ),
	DEFINE_KEYFIELD( m_iDefaultOwner,			FIELD_INTEGER,	"point_default_owner" ),
	DEFINE_KEYFIELD( m_iPointIndex,				FIELD_INTEGER,	"point_index" ),
	DEFINE_KEYFIELD( m_iWarnOnCap,				FIELD_INTEGER,	"point_warn_on_cap" ),
	DEFINE_KEYFIELD( m_iszWarnSound,			FIELD_STRING,	"point_warn_sound" ),

	DEFINE_KEYFIELD( m_iszCaptureStartSound,	FIELD_STRING,	"point_capture_start_sound" ),
	DEFINE_KEYFIELD( m_iszCaptureEndSound,		FIELD_STRING,	"point_capture_end_sound" ),
	DEFINE_KEYFIELD( m_iszCaptureInProgress,	FIELD_STRING,	"point_capture_progress_sound" ),
	DEFINE_KEYFIELD( m_iszCaptureInterrupted,	FIELD_STRING,	"point_capture_interrupted_sound" ),
	DEFINE_KEYFIELD( m_bRandomOwnerOnRestart,	FIELD_BOOLEAN,	"random_owner_on_restart" ),
	DEFINE_KEYFIELD( m_bLocked,					FIELD_BOOLEAN,	"point_start_locked" ),

	DEFINE_FUNCTION( UnlockThink ),

//	DEFINE_FIELD( m_iTeam, FIELD_INTEGER ),
//	DEFINE_FIELD( m_iIndex, FIELD_INTEGER ),
//	DEFINE_FIELD( m_TeamData, CUtlVector < perteamdata_t > ),
//	DEFINE_FIELD( m_bPointVisible, FIELD_INTEGER ),
//	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_iszName, FIELD_STRING ),
//	DEFINE_FIELD( m_bStartDisabled, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_flLastContestedAt, FIELD_FLOAT ),
//	DEFINE_FIELD( m_pCaptureInProgressSound, CSoundPatch ),

	DEFINE_INPUTFUNC( FIELD_INTEGER,	"SetOwner",			InputSetOwner ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"ShowModel",		InputShowModel ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"HideModel",		InputHideModel ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"RoundActivate",	InputRoundActivate ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"SetLocked",		InputSetLocked ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"SetUnlockTime",	InputSetUnlockTime ),

	DEFINE_OUTPUT(	m_OnCapTeam1,		"OnCapTeam1" ),	// these are fired whenever the point changes modes
	DEFINE_OUTPUT(	m_OnCapTeam2,		"OnCapTeam2" ),
	DEFINE_OUTPUT(	m_OnCapReset,		"OnCapReset" ),

	DEFINE_OUTPUT(	m_OnOwnerChangedToTeam1,	"OnOwnerChangedToTeam1" ),	// these are fired when a team does the work to change the owner
	DEFINE_OUTPUT(	m_OnOwnerChangedToTeam2,	"OnOwnerChangedToTeam2" ),

	DEFINE_OUTPUT(	m_OnRoundStartOwnedByTeam1,	"OnRoundStartOwnedByTeam1" ),	// these are fired when a round is starting
	DEFINE_OUTPUT(	m_OnRoundStartOwnedByTeam2,	"OnRoundStartOwnedByTeam2" ),

	DEFINE_OUTPUT(	m_OnUnlocked, "OnUnlocked" ),

	DEFINE_THINKFUNC( AnimThink ),
END_DATADESC();

LINK_ENTITY_TO_CLASS( team_control_point, CTeamControlPoint );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTeamControlPoint::CTeamControlPoint()
{
	m_TeamData.SetSize( GetNumberOfTeams() );
	m_pCaptureInProgressSound = NULL;

	m_bLocked = false;
	m_flUnlockTime = -1;

#ifdef  TF_DLL
	UseClientSideAnimation();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::Spawn( void )
{
	// Validate our default team
	if ( m_iDefaultOwner < 0 || m_iDefaultOwner >= GetNumberOfTeams() )
	{
		Warning( "team_control_point '%s' has bad point_default_owner.\n", GetDebugName() );
		m_iDefaultOwner = TEAM_UNASSIGNED;
	}

	Precache();

	InternalSetOwner( m_iDefaultOwner, false );	//init the owner of this point
	TeamplayRoundBasedRules()->RecalculateControlPointState();

	SetActive( !m_bStartDisabled );

	BaseClass::Spawn();

	SetPlaybackRate( 1.0 );
	SetThink( &CTeamControlPoint::AnimThink );
	SetNextThink( gpGlobals->curtime + 0.1f );

	if ( FBitSet( m_spawnflags, SF_CAP_POINT_HIDE_MODEL ) )
	{
		AddEffects( EF_NODRAW );
	}

	if ( FBitSet( m_spawnflags, SF_CAP_POINT_HIDE_SHADOW ) )
	{
		AddEffects( EF_NOSHADOW );
	}

	m_flLastContestedAt = -1;

	m_pCaptureInProgressSound = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPoint::KeyValue( const char *szKeyName, const char *szValue )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::Precache( void )
{
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CTeamControlPoint::AnimThink( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Used by ControlMaster to this point to its default owner
//-----------------------------------------------------------------------------
void CTeamControlPoint::InputReset( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::HandleScoring( int iTeam )
{
}

//-----------------------------------------------------------------------------
// Purpose: Used by Area caps to set the owner
//-----------------------------------------------------------------------------
void CTeamControlPoint::InputSetOwner( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::InputShowModel( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::InputHideModel( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPoint::GetCurrentHudIconIndex( void )
{
	return m_TeamData[GetOwner()].iIcon;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPoint::GetHudIconIndexForTeam( int iGameTeam )
{
	return m_TeamData[iGameTeam].iIcon;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPoint::GetHudOverlayIndexForTeam( int iGameTeam )
{
	return m_TeamData[iGameTeam].iOverlay;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTeamControlPoint::GetPreviousPointForTeam( int iGameTeam, int iPrevPoint )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::ForceOwner( int iTeam )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::SetOwner( int iCapTeam, bool bMakeSound, int iNumCappers, int *pCappingPlayers )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::CaptureStart( int iCapTeam, int iNumCappingPlayers, int *pCappingPlayers )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::CaptureEnd( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::CaptureInterrupted( bool bBlocked )
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::StopLoopingSounds( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Sets the new owner of the point, plays the appropriate sound and shows the right model
//-----------------------------------------------------------------------------
void CTeamControlPoint::InternalSetOwner( int iCapTeam, bool bMakeSound, int iNumCappers, int *pCappingPlayers )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::SendCapString( int iCapTeam, int iNumCappingPlayers, int *pCappingPlayers )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::CaptureBlocked( CBaseMultiplayerPlayer *pPlayer, CBaseMultiplayerPlayer *pVictim )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPoint::GetOwner( void ) const
{ 
	return m_iTeam;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPoint::GetDefaultOwner( void ) const
{
	return m_iDefaultOwner;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPoint::GetCPGroup( void )
{
	return m_iCPGroup;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the time-based point value of this control point
//-----------------------------------------------------------------------------
int CTeamControlPoint::PointValue( void )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::SetActive( bool active )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::SetCappersRequiredForTeam( int iGameTeam, int iCappers )
{
}


//-----------------------------------------------------------------------------
// Purpose: Return true if this point has ever been contested, false if the enemy has never contested this point yet
//-----------------------------------------------------------------------------
bool CTeamControlPoint::HasBeenContested( void ) const
{
	return m_flLastContestedAt > 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTeamControlPoint::LastContestedAt( void )
{
	return m_flLastContestedAt;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::SetLastContestedAt( float flTime )
{
	m_flLastContestedAt = flTime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::UpdateCapPercentage( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTeamControlPoint::GetTeamCapPercentage( int iTeam )
{
	return 0.0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPoint::DrawDebugTextOverlays( void ) 
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: The specified player took part in capping this point.
//-----------------------------------------------------------------------------
void CTeamControlPoint::PlayerCapped( CBaseMultiplayerPlayer *pPlayer )
{
}

//-----------------------------------------------------------------------------
// Purpose: The specified player blocked the enemy team from capping this point.
//-----------------------------------------------------------------------------
void CTeamControlPoint::PlayerBlocked( CBaseMultiplayerPlayer *pPlayer )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::InputRoundActivate( inputdata_t &inputdata )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::InputSetLocked( inputdata_t &inputdata )
{
	// never lock/unlock the point if we're in waiting for players
	if ( TeamplayRoundBasedRules() && TeamplayRoundBasedRules()->IsInWaitingForPlayers() )
		return;

	bool bLocked = inputdata.value.Int() > 0;
	InternalSetLocked( bLocked );
}
 
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::InternalSetLocked( bool bLocked )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::InputSetUnlockTime( inputdata_t &inputdata )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPoint::UnlockThink( void )
{
}
