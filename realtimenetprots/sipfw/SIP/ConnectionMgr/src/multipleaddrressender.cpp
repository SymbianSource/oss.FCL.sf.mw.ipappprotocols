// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : multipleaddrressender.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//




#include "multipleaddrressender.h"
#include "sipresponse.h"
#include "multiaddrsenderowner.h"
#include "CSIPServerResolver.h"
#include "sipuri.h"
#include "sipviaheader.h"
#include "siphostport.h"
#include "SipLogs.h"
#include "MSIPResolvingResult.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"
#include "siperr.h"


const TInt CMultipleAddrResSender::iMultisenderOffset =
	_FOFF(CMultipleAddrResSender, iMultisenderLink);

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::CMultipleAddrResSender
// -----------------------------------------------------------------------------
//
CMultipleAddrResSender::CMultipleAddrResSender( 
                                      const TSIPTransportParams& aParams,
                                      MMultiAddrSenderOwner& aOwner,
                                      MTimerManager& aTimer, 
                                      CSIPServerResolver& aServerResolver,
                                      TUint aICMPErrorWaitTime
                                       ) :
	CActive( CActive::EPriorityStandard ), 
	iTransportParams( aParams ),
    iOwner( &aOwner ), 
    iTimer( aTimer ),
    iServerResolver( aServerResolver ),
    iICMPErrorWaitTime( aICMPErrorWaitTime ),
    iInUse( EFalse ),
    iState( EInitialized ),
    iICMPErrorOccuredWhileActive( EFalse )
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::~CMultipleAddrResSender
// -----------------------------------------------------------------------------
//
CMultipleAddrResSender::~CMultipleAddrResSender()
	{
	CompleteClientRequest( KErrSIPTransportFailure );
	iTimer.Stop( iTimerId );
	iServerResolver.Cancel( this );
	Cancel();
	iResultArray.ResetAndDestroy();
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::SendL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::SendL ( CSIPResponse& aResponse,
                                     const TInetAddr& aAddr, 
                                     TRequestStatus& aStat )
	{
	__ASSERT_ALWAYS( !iInUse, User::Leave( KErrInUse ) );
	
	iFirstAddr = aAddr;
	iResponse = &aResponse;
	iInUse = ETrue;
	
	iOwner->SendToNetL( iTransportParams, aResponse, aAddr, iStatus);
	
	iState = CMultipleAddrResSender::ESendingFirst;	
	aStat = KRequestPending;	
	iClientStatus = &aStat;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::ICMPErrorL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::ICMPErrorL(
								CSipConnectionMgr::TICMPError /*aICMPError*/,
                                const TInetAddr& aErrAddress )
	{
	if ( iState == CMultipleAddrResSender::ESendingList &&
	     IndexWithinBounds() &&
	     iResultArray[ iCurrentIndex ]->Address().CmpAddr( aErrAddress ) )
	    {
	    iResultArray[ iCurrentIndex ]->SetFailed(ETrue);
		ICMPErrorWithCurrentAddressL();
	    }
    else if ( iState == CMultipleAddrResSender::ESendingFirst )
        {
    	if ( iFirstAddr.Match(aErrAddress) && 
    	     iFirstAddr.Port() == aErrAddress.Port() )
    		{
    		ICMPErrorWithCurrentAddressL();
    		}
        }
    else
        {
        // Required by PC-Lint
        }
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::RunL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::RunL()
	{
	CompleteClientRequest( iStatus.Int() );

	if ( iStatus == KErrNone )
		{
		switch ( iState )
			{
			case CMultipleAddrResSender::ESendingFirst: 
			case CMultipleAddrResSender::ESendingList: 
				WaitForICMPErrorsL(); 
				break;
			default: 
				break; 
			}
		}
	else
		{
		iOwner->Remove( this );	
		}
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::CancelResponse
// -----------------------------------------------------------------------------
//
TBool CMultipleAddrResSender::CancelResponse( TRequestStatus& aStat )
	{
	if ( &aStat == iClientStatus && 
	     iState == CMultipleAddrResSender::ESendingFirst )
		{
		CompleteClientRequest( KErrSIPTransportFailure );
		if ( IsActive() )
			{
			iOwner->CancelRequest( iStatus );			
			}
		else
			{
			iTimer.Stop( iTimerId );
			iOwner->Remove( this );
			}
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::DoCancel
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::DoCancel()
	{
	iOwner->CancelRequest( iStatus );
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::RunError
// -----------------------------------------------------------------------------
//
TInt CMultipleAddrResSender::RunError( TInt aError )
	{
	if ( aError != KErrNoMemory )
		{
		return KErrNone;
		}
	return aError;
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::ICMPErrorWithCurrentAddressL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::ICMPErrorWithCurrentAddressL()
	{
	if ( !IsActive() )
		{
		iTimer.Stop( iTimerId );
		SendNextL();		
		}
	else
		{
		iICMPErrorOccuredWhileActive = ETrue;	
		}	
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::WaitForICMPErrorsL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::WaitForICMPErrorsL()
	{
	if ( iICMPErrorOccuredWhileActive )
		{
		SendNextL();
		iICMPErrorOccuredWhileActive = EFalse;	
		}
	else
		{		
		iTimerId = iTimer.StartL( this, iICMPErrorWaitTime );
		}	
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::SendNextL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::SendNextL()
	{
	if ( IsActive() )
		{
		return;
		}
		
	if ( iState == CMultipleAddrResSender::ESendingFirst )
		{
		ResolveL();
		iState = CMultipleAddrResSender::EResolving;
		return;
		}
		
    if ( iState == CMultipleAddrResSender::EResolving || 
         iState == CMultipleAddrResSender::ESendingList )
		{
		MSIPResolvingResult* result = FirstUnfailedResult();
		if ( result )
		    {
		    SendToNetL( *result );
		    iState = CMultipleAddrResSender::ESendingList;
		    }
		else
		    {
		    // Sending to every resolved address failed, resolve more
		    ResolveL();
		    iState = CMultipleAddrResSender::EResolving;
		    }
		}		
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::TimerExpiredL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::TimerExpiredL (TTimerId /*aTimerId*/,
                                            TAny* /*aTimerParam*/)
	{
	__SIP_LOG( "CMultipleAddrResSender::TimerExpiredL" )
	iOwner->Remove( this );
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::Has2xxResponse
// -----------------------------------------------------------------------------
//	
TBool CMultipleAddrResSender::Has2xxResponse() const
    {
    return ( iResponse && iResponse->Type() == CSIPResponse::E2XX );
    }

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::CompletedL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::CompletedL()
    {
    SendNextL();
    }
 
// -----------------------------------------------------------------------------
// CMultipleAddrResSender::ErrorOccured
// -----------------------------------------------------------------------------
//  
void CMultipleAddrResSender::ErrorOccured( TInt /*aError*/ )
    {   
    iOwner->Remove( this );	
    }

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::SendToNetL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::SendToNetL( MSIPResolvingResult& aResult )
	{
	iOwner->SendToNetL( iTransportParams, *iResponse, aResult.Address(), iStatus );
	SetActive();	
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::ResolveL
// -----------------------------------------------------------------------------
//
void CMultipleAddrResSender::ResolveL()
	{
	if ( iResponse->HasHeader( SIPStrings::StringF(SipStrConsts::EViaHeader) ) )
		{
		CSIPHeaderBase* header = iResponse->Header( 
		                    SIPStrings::StringF(SipStrConsts::EViaHeader), 0 );
		CSIPViaHeader* via = static_cast<CSIPViaHeader*>( header );
		iServerResolver.GetByViaL( *via, iResultArray, this );
		}
	}

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::CompleteClientRequest
// -----------------------------------------------------------------------------
//	
void CMultipleAddrResSender::CompleteClientRequest(TInt aReason)
	{
	if ( iState == CMultipleAddrResSender::ESendingFirst )
		{
		TRequestStatus* stat = iClientStatus;
		if ( stat && *stat == KRequestPending )
			{
			User::RequestComplete( stat, aReason );
			}
		iClientStatus = 0;
		}
	}
	
// -----------------------------------------------------------------------------
// CMultipleAddrResSender::IndexWithinBounds
// -----------------------------------------------------------------------------
//
TBool CMultipleAddrResSender::IndexWithinBounds()
    {
    return ( iCurrentIndex >= 0 && iCurrentIndex < iResultArray.Count() );
    }

// -----------------------------------------------------------------------------
// CMultipleAddrResSender::FirstUnfailedResult
// -----------------------------------------------------------------------------
//
MSIPResolvingResult* CMultipleAddrResSender::FirstUnfailedResult()
    {
    for ( TInt i = 0; i < iResultArray.Count(); i++ )
        {
        MSIPResolvingResult* result = iResultArray[ i ];
        if ( !result->Failed() )
            {
            iCurrentIndex = i;
            return result;
            }
        }
    iCurrentIndex = KErrNotFound;
    return 0;
    }
