//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "team_train_watcher.h"
#include "trigger_area_capture.h"
#include "player.h"
#include "teamplay_gamerules.h"
#include "team.h"
#include "team_objectiveresource.h"
#include "team_control_point_master.h"
#include "teamplayroundbased_gamerules.h"

extern ConVar mp_capstyle;
extern ConVar mp_blockstyle;
extern ConVar mp_capdeteriorate_time;

IMPLEMENT_AUTO_LIST( ITriggerAreaCaptureAutoList );

BEGIN_DATADESC(CTriggerAreaCapture)

	// Touch functions
	DEFINE_FUNCTION( CTriggerAreaCaptureShim::Touch ),

	// Think functions
	DEFINE_THINKFUNC( CaptureThink ),

	// Keyfields
	DEFINE_KEYFIELD( m_iszCapPointName,	FIELD_STRING,	"area_cap_point" ),
	DEFINE_KEYFIELD( m_flCapTime,		FIELD_FLOAT,	"area_time_to_cap" ),

//	DEFINE_FIELD( m_iCapMode, FIELD_INTEGER ),
//	DEFINE_FIELD( m_bCapturing, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_nCapturingTeam, FIELD_INTEGER ),
//	DEFINE_FIELD( m_nOwningTeam, FIELD_INTEGER ),
//	DEFINE_FIELD( m_nTeamInZone, FIELD_INTEGER ),
//	DEFINE_FIELD( m_fTimeRemaining, FIELD_FLOAT ),
//	DEFINE_FIELD( m_flLastReductionTime, FIELD_FLOAT ),
//	DEFINE_FIELD( m_bBlocked, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_TeamData, CUtlVector < perteamdata_t > ),
//	DEFINE_FIELD( m_Blockers, CUtlVector < blockers_t > ),
//	DEFINE_FIELD( m_bActive, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_hPoint, CHandle < CTeamControlPoint > ),
//	DEFINE_FIELD( m_bRequiresObject, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_iCapAttemptNumber, FIELD_INTEGER ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_VOID, "RoundSpawn", InputRoundSpawn ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetTeamCanCap", InputSetTeamCanCap ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetControlPoint", InputSetControlPoint ),
	DEFINE_INPUTFUNC( FIELD_VOID, "CaptureCurrentCP", InputCaptureCurrentCP ),

	// Outputs
	DEFINE_OUTPUT( m_OnStartTeam1,	"OnStartTeam1" ),
	DEFINE_OUTPUT( m_OnStartTeam2,	"OnStartTeam2" ),
	DEFINE_OUTPUT( m_OnBreakTeam1,	"OnBreakTeam1" ),
	DEFINE_OUTPUT( m_OnBreakTeam2,	"OnBreakTeam2" ),
	DEFINE_OUTPUT( m_OnCapTeam1,	"OnCapTeam1" ),
	DEFINE_OUTPUT( m_OnCapTeam2,	"OnCapTeam2" ),

	DEFINE_OUTPUT( m_StartOutput,	"OnStartCap" ),
	DEFINE_OUTPUT( m_BreakOutput,	"OnBreakCap" ),
	DEFINE_OUTPUT( m_CapOutput,		"OnEndCap" ),

	DEFINE_OUTPUT( m_OnNumCappersChanged, "OnNumCappersChanged" ),
	DEFINE_OUTPUT( m_OnNumCappersChanged2, "OnNumCappersChanged2" ),

END_DATADESC();

LINK_ENTITY_TO_CLASS( trigger_capture_area, CTriggerAreaCapture );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTriggerAreaCapture::CTriggerAreaCapture()
{
	m_TeamData.SetSize( GetNumberOfTeams() );
	m_bStartTouch = false;
	m_hTrainWatcher = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::Spawn( void )
{
	BaseClass::Spawn();

	AddSpawnFlags( SF_TRIGGER_ALLOW_CLIENTS );

	InitTrigger();
	
	Precache();

	SetTouch ( &CTriggerAreaCaptureShim::Touch );		
	SetThink( &CTriggerAreaCapture::CaptureThink );
	SetNextThink( gpGlobals->curtime + AREA_THINK_TIME );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTriggerAreaCapture::KeyValue( const char *szKeyName, const char *szValue )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::Precache( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTriggerAreaCapture::IsActive( void )
{
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::StartTouch(CBaseEntity *pOther)
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::EndTouch(CBaseEntity *pOther)
{
	BaseClass::EndTouch( pOther );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTriggerAreaCapture::CaptureModeScalesWithPlayers() const
{
	return mp_capstyle.GetBool();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::AreaTouch( CBaseEntity *pOther )
{
}

ConVar mp_simulatemultiplecappers( "mp_simulatemultiplecappers", "1", FCVAR_CHEAT );

#define MAX_CAPTURE_TEAMS 8

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::CaptureThink( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::SetCapTimeRemaining( float flTime )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::SetOwner( int team )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::ForceOwner( int team )
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::HandleRespawnTimeAdjustments( int oldTeam, int newTeam )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::StartCapture( int team, int capmode )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::GetNumCappingPlayers( int team, int &numcappers, int *cappingplayers )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::EndCapture( int team )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::BreakCapture( bool bNotEnoughPlayers )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::IncrementCapAttemptNumber( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::InputRoundSpawn( inputdata_t &inputdata )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::InputSetTeamCanCap( inputdata_t &inputdata )
{
}

void CTriggerAreaCapture::InputCaptureCurrentCP( inputdata_t &inputdata )
{
}

void CTriggerAreaCapture::InputSetControlPoint( inputdata_t &inputdata )
{
}

//-----------------------------------------------------------------------------
// Purpose: Check if this player's death causes a block
// return FALSE if the player is not in this area
// return TRUE otherwise ( eg player is in area, but his death does not cause break )
//-----------------------------------------------------------------------------
bool CTriggerAreaCapture::CheckIfDeathCausesBlock( CBaseMultiplayerPlayer *pVictim, CBaseMultiplayerPlayer *pKiller )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::UpdateNumPlayers( bool bBlocked /*= false */ )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::UpdateOwningTeam( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::UpdateCappingTeam( int iTeam )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::UpdateTeamInZone( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::UpdateBlocked( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTriggerAreaCapture::SetNumCappers( int nNumCappers, bool bBlocked /* = false */ )
{
}