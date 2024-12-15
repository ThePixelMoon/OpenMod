//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//===========================================================================//

#include "cbase.h"
#include "team_objectiveresource.h"
#include "team_control_point_master.h"
#include "teamplayroundbased_gamerules.h"

#if defined ( TF_DLL )
#include "tf_gamerules.h"
#endif

BEGIN_DATADESC( CTeamControlPointMaster )
	DEFINE_KEYFIELD( m_bDisabled, FIELD_BOOLEAN, "StartDisabled" ),
	DEFINE_KEYFIELD( m_iszCapLayoutInHUD, FIELD_STRING, "caplayout" ),
	DEFINE_KEYFIELD( m_iInvalidCapWinner, FIELD_INTEGER, "cpm_restrict_team_cap_win" ),
	DEFINE_KEYFIELD( m_bSwitchTeamsOnWin, FIELD_BOOLEAN, "switch_teams" ),
	DEFINE_KEYFIELD( m_bScorePerCapture, FIELD_BOOLEAN, "score_style" ),
	DEFINE_KEYFIELD( m_bPlayAllRounds, FIELD_BOOLEAN, "play_all_rounds" ),

	DEFINE_KEYFIELD( m_flPartialCapturePointsRate, FIELD_FLOAT, "partial_cap_points_rate" ),

	DEFINE_KEYFIELD( m_flCustomPositionX, FIELD_FLOAT, "custom_position_x" ),
	DEFINE_KEYFIELD( m_flCustomPositionY, FIELD_FLOAT, "custom_position_y" ),

//	DEFINE_FIELD( m_ControlPoints, CUtlMap < int , CTeamControlPoint * > ),
//	DEFINE_FIELD( m_bFoundPoints, FIELD_BOOLEAN ),
//	DEFINE_FIELD( m_ControlPointRounds, CUtlVector < CTeamControlPointRound * > ),
//	DEFINE_FIELD( m_iCurrentRoundIndex, FIELD_INTEGER ),
//	DEFINE_ARRAY( m_iszTeamBaseIcons, FIELD_STRING, MAX_TEAMS ),
//	DEFINE_ARRAY( m_iTeamBaseIcons, FIELD_INTEGER, MAX_TEAMS ),
//	DEFINE_FIELD( m_bFirstRoundAfterRestart, FIELD_BOOLEAN ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),

	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetWinner", InputSetWinner ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetWinnerAndForceCaps", InputSetWinnerAndForceCaps ),
	DEFINE_INPUTFUNC( FIELD_VOID, "RoundSpawn", InputRoundSpawn ),
	DEFINE_INPUTFUNC( FIELD_VOID, "RoundActivate", InputRoundActivate ),
	DEFINE_INPUTFUNC( FIELD_STRING, "SetCapLayout", InputSetCapLayout ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetCapLayoutCustomPositionX", InputSetCapLayoutCustomPositionX ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetCapLayoutCustomPositionY", InputSetCapLayoutCustomPositionY ),

	DEFINE_FUNCTION( CPMThink ),

	DEFINE_OUTPUT( m_OnWonByTeam1,	"OnWonByTeam1" ),
	DEFINE_OUTPUT( m_OnWonByTeam2,	"OnWonByTeam2" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( team_control_point_master, CTeamControlPointMaster );

ConVar mp_time_between_capscoring( "mp_time_between_capscoring", "30", FCVAR_GAMEDLL, "Delay between scoring of owned capture points.", true, 1, false, 0 );

// sort function for the list of control_point_rounds (we're sorting them by priority...highest first)
int ControlPointRoundSort( CTeamControlPointRound* const *p1, CTeamControlPointRound* const *p2 )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: init
//-----------------------------------------------------------------------------
CTeamControlPointMaster::CTeamControlPointMaster()
{
	m_flPartialCapturePointsRate = 0.0f;
	m_flCustomPositionX = -1.f;
	m_flCustomPositionY = -1.f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::Spawn( void )
{
	Precache();

	SetTouch( NULL );
	m_bFoundPoints = false;
	SetDefLessFunc( m_ControlPoints );

	m_iCurrentRoundIndex = -1;
  	m_bFirstRoundAfterRestart = true;
	m_flLastOwnershipChangeTime = -1;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::UpdateOnRemove( void )
{
	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::KeyValue( const char *szKeyName, const char *szValue )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::Activate( void )
{
	BaseClass::Activate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::RoundRespawn( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::Reset( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::FindControlPoints( void )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::SetBaseControlPoints( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::FindControlPointRounds( void )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::IsInRound( CTeamControlPoint *pPoint )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamControlPointMaster::NumPlayableControlPointRounds( void )
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::SelectSpecificRound( void )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::RegisterRoundBeingPlayed( void )
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::GetControlPointRoundToPlay( void )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Called every 0.1 seconds and checks the status of all the control points
// if one team owns them all, it gives points and resets
// Think also gives the time based points at the specified time intervals
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::CPMThink( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::CheckWinConditions( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputSetWinner( inputdata_t &input )
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputSetWinnerAndForceCaps( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InternalSetWinner( int iTeam )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::HandleRandomOwnerControlPoints( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputRoundSpawn( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputRoundActivate( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputSetCapLayout( inputdata_t &inputdata )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputSetCapLayoutCustomPositionX( inputdata_t &inputdata )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputSetCapLayoutCustomPositionY( inputdata_t &inputdata )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::FireTeamWinOutput( int iWinningTeam )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::FireRoundStartOutput( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::FireRoundEndOutput( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTeamControlPointMaster::PointLastContestedAt( int point )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: This function returns the team that owns all the cap points. 
//			If its not the case that one team owns them all, it returns 0.
//			CPs are broken into groups. A team can win by owning all flags within a single group.
//			
//			Can be passed an overriding team. If this is not null, the passed team
//			number will be used for that cp. Used to predict if that CP changing would
//			win the game.
//-----------------------------------------------------------------------------
int CTeamControlPointMaster::TeamOwnsAllPoints( CTeamControlPoint *pOverridePoint /* = NULL */, int iOverrideNewTeam /* = TEAM_UNASSIGNED */ )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::WouldNewCPOwnerWinGame( CTeamControlPoint *pPoint, int iNewOwner )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamControlPointMaster::IsBaseControlPoint( int iPointIndex )
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Get the control point for the specified team that's at their end of
//			the control point chain.
//-----------------------------------------------------------------------------
int	CTeamControlPointMaster::GetBaseControlPoint( int iTeam )
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputEnable( inputdata_t &input )
{ 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamControlPointMaster::InputDisable( inputdata_t &input )
{ 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------	
int CTeamControlPointMaster::GetNumPointsOwnedByTeam( int iTeam )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: returns how many more mini-rounds it will take for specified team
//			to win, if they keep winning every mini-round
//-----------------------------------------------------------------------------	
int CTeamControlPointMaster::CalcNumRoundsRemaining( int iTeam )
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTeamControlPointMaster::GetPartialCapturePointRate( void )
{
	return 0;
}
