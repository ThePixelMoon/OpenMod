//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Entity that propagates train data for escort game type
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_team_train_watcher.h"
#include "igameevents.h"
#include "c_team_objectiveresource.h"

#ifdef TF_CLIENT_DLL
#include "tf_shareddefs.h"
#include "teamplayroundbased_gamerules.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_TeamTrainWatcher, DT_TeamTrainWatcher, CTeamTrainWatcher)

	RecvPropFloat( RECVINFO( m_flTotalProgress ) ),
	RecvPropInt( RECVINFO( m_iTrainSpeedLevel ) ),
	RecvPropFloat( RECVINFO( m_flRecedeTime ) ),
	RecvPropInt( RECVINFO( m_nNumCappers ) ),
#ifdef GLOWS_ENABLE
	RecvPropEHandle( RECVINFO( m_hGlowEnt ) ),
#endif // GLOWS_ENABLE

END_RECV_TABLE()

CUtlVector< CHandle<C_TeamTrainWatcher> > g_hTrainWatchers;

C_TeamTrainWatcher::C_TeamTrainWatcher()
{
	// force updates when we get our baseline
	m_iTrainSpeedLevel = -2;
	m_flTotalProgress = -1;
	m_flRecedeTime = -1;

#ifdef GLOWS_ENABLE
	m_pGlowEffect = NULL;
	m_hGlowEnt = NULL;
	m_hOldGlowEnt = NULL;
#endif // GLOWS_ENABLE
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TeamTrainWatcher::~C_TeamTrainWatcher()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TeamTrainWatcher::ClientThink()
{
}

#ifdef GLOWS_ENABLE
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TeamTrainWatcher::UpdateGlowEffect( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TeamTrainWatcher::DestroyGlowEffect( void )
{
}
#endif // GLOWS_ENABLE

void C_TeamTrainWatcher::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

	m_iOldTrainSpeedLevel = m_iTrainSpeedLevel;
	m_flOldProgress = m_flTotalProgress;
	m_flOldRecedeTime = m_flRecedeTime;
	m_nOldNumCappers = m_nNumCappers;
#ifdef GLOWS_ENABLE
	m_hOldGlowEnt = m_hGlowEnt;
#endif // GLOWS_ENABLE
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TeamTrainWatcher::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
#ifdef GLOWS_ENABLE
	if ( m_hOldGlowEnt != m_hGlowEnt )
	{
		UpdateGlowEffect();
	}
#endif // GLOWS_ENABLE
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TeamTrainWatcher::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TeamTrainWatcher::UpdateOnRemove( void )
{
	BaseClass::UpdateOnRemove();
}
