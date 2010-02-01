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
// Name          : CTcpResSender.cpp
// Part of       : ConnectionMgr
// Version       : SIP/4.0 
//



#include "SipAssert.h"
#include "CTcpResSender.h"
#include "CSIPServerResolver.h"
#include "CSipConnection.h"
#include "COutgoingData.h"
#include "sipuri.h"
#include "sipviaheader.h"
#include "siphostport.h"
#include "CTransportBase.h"
#include "MSIPResolvingResult.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"
#include "siperr.h"

const TInt CTcpResponseSender::iSenderOffset =
	_FOFF(CTcpResponseSender, iSenderLink);


// -----------------------------------------------------------------------------
// CTcpResponseSender::CTcpResponseSender
// -----------------------------------------------------------------------------
//
CTcpResponseSender::CTcpResponseSender( const TSIPTransportParams& aParams,
                                        CSIPServerResolver& aHostResolver, 
							 		    CSipConnection& aOwner ) :
    iTransportParams( aParams ),
    iHostResolver( aHostResolver ),
    iOwner( &aOwner ),
    iSending( EFalse ),
    iResolving( EFalse )
	{
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::~CTcpResponseSender
// -----------------------------------------------------------------------------
//
CTcpResponseSender::~CTcpResponseSender()
	{
	if ( iResolving && iData )
		{
		CompleteRequest( iData->Status(), KErrCouldNotConnect );
		}
	iHostResolver.Cancel( this );		    
	iResultArray.ResetAndDestroy();
	delete iData;
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::ConnectionFailedL
// -----------------------------------------------------------------------------
//
void CTcpResponseSender::ConnectionFailedL()
	{
	// Safety check, although shouldn't ever exceed array bounds at this point
	if ( IndexWithinBounds() )
	    {
	    iResultArray[ iCurrentIndex ]->SetFailed( ETrue );
	    }
	SendL();
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::Connected
// -----------------------------------------------------------------------------
//
void CTcpResponseSender::Connected( const TInetAddr& aAddr )	
	{
	if ( iSending )
		{
		if ( IndexWithinBounds() && 
		     iResultArray[ iCurrentIndex ]->Address().CmpAddr( aAddr ) )
			{
			iOwner->RemoveTcpSender( this );
			}
		}
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::IsInProgress
// -----------------------------------------------------------------------------
//
TBool CTcpResponseSender::IsInProgress( COutgoingData& aData )
	{
	return iData && aData.Status() == iData->Status();
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::ResolveL
// -----------------------------------------------------------------------------
//
void CTcpResponseSender::ResolveL( COutgoingData& aData )
	{
	if ( !iResolving )
		{
		if ( aData.Message().HasHeader( 
		    	SIPStrings::StringF( SipStrConsts::EViaHeader ) ) )
			{
			TSglQueIter< CSIPHeaderBase > iter = aData.Message().Headers( 
				SIPStrings::StringF( SipStrConsts::EViaHeader ) );
			CSIPHeaderBase* header = iter;
			CSIPViaHeader* via = static_cast<CSIPViaHeader*>( header );
			iHostResolver.GetByViaL( *via, iResultArray, this );
			iResolving = ETrue;
			}
		}
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::CancelSend
// -----------------------------------------------------------------------------
//
TBool CTcpResponseSender::CancelSend( TRequestStatus &aStatus )
	{
	if ( iResolving && iData && &aStatus == iData->Status() )
		{
		CompleteRequest( iData->Status(), KErrSIPTransportFailure );
		iHostResolver.Cancel( this );
		iResolving = EFalse;
		return ETrue;
		}
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::SetData
// -----------------------------------------------------------------------------
//    
void CTcpResponseSender::SetData( COutgoingData* aData )
	{
	__SIP_ASSERT_RETURN( !iData, KErrAlreadyExists );
	iData = aData;
	}

// -----------------------------------------------------------------------------
// CTcpResponseSender::CompletedL
// -----------------------------------------------------------------------------
//   
void CTcpResponseSender::CompletedL()
    {
    iResolving = EFalse;
	SendL();
    }

// -----------------------------------------------------------------------------
// CTcpResponseSender::ErrorOccured
// -----------------------------------------------------------------------------
//	
void CTcpResponseSender::ErrorOccured( TInt /*aError*/ )
    {
    iResolving = EFalse;
	if ( iData )
		{
		CompleteRequest( iData->Status(), KErrCouldNotConnect );
		}
	iOwner->RemoveTcpSender( this );
    }
    
// -----------------------------------------------------------------------------
// CTcpResponseSender::SendL
// -----------------------------------------------------------------------------
//
void CTcpResponseSender::SendL()
	{
	__SIP_ASSERT_LEAVE( iData, KErrNotFound );

	MSIPResolvingResult* result = FirstUnfailedResult();
	if ( result )
		{
		CTransportBase* transport;
		transport = iOwner->FindTransportL( iTransportParams,
		                                    &iData->Message(),
			                                result->Address() );	
		if ( transport )
			{
			iSending = ETrue;
			transport->Send( iTransportParams,
			                 static_cast<CSIPResponse&>( iData->Message() ), 
				             result->Address(), 
						     *iData->Status() );
			}
		else
			{
			iOwner->CreateTcpTransportPointL( iTransportParams,
			                                  result->Address() );
			                                  
			transport = iOwner->FindTransportL( iTransportParams,
			                                    &iData->Message(), 
				                                result->Address() );		
			if ( transport )
				{
				iSending = ETrue;
				transport->Send( iTransportParams,
				                 static_cast<CSIPResponse&>( iData->Message() ), 
					             result->Address(), 
								 *iData->Status() );
				}
			else
				{
				CompleteRequest( iData->Status(), KErrCouldNotConnect );
				iOwner->RemoveTcpSender( this );
				}
			}
		}
	else
		{
		// Sending to every resolved address failed, resolve more
		ResolveL( *iData );
		}
	}
    
// -----------------------------------------------------------------------------
// CTcpResponseSender::CompleteRequest
// -----------------------------------------------------------------------------
//
void CTcpResponseSender::CompleteRequest( TRequestStatus* aStatus, TInt aReason )
	{
	if ( aStatus != 0 && *aStatus == KRequestPending )
		{
		User::RequestComplete( aStatus, aReason );
		}
	}
	
// -----------------------------------------------------------------------------
// CTcpResponseSender::FirstUnfailedResult
// -----------------------------------------------------------------------------
//
MSIPResolvingResult* CTcpResponseSender::FirstUnfailedResult()
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

// -----------------------------------------------------------------------------
// CTcpResponseSender::IndexWithinBounds
// -----------------------------------------------------------------------------
//
TBool CTcpResponseSender::IndexWithinBounds()
    {
    return ( iCurrentIndex >= 0 && iCurrentIndex < iResultArray.Count() );
    }
