/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Represents an SRTP stream.
*
*/




// INCLUDES
#include "srtpstreamout.h"
#include "tsrtpstreamoutstatebase.h"
#include "tsrtpstreamoutstateuninit.h"
#include "tsrtpstreamoutstatenormal.h"
#include "srtpsession.h"
#include "srtpcryptohandler.h"
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtputils.h"

// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses default cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamOut* CSRTPStreamOut::NewL( CSRTPSession& aSession, TUint aSSRC )
    {
    CSRTPStreamOut* self = new( ELeave )CSRTPStreamOut( aSession, aSSRC);
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );        
    return self;
    }

// ---------------------------------------------------------------------------
// Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamOut* CSRTPStreamOut::NewL( CSRTPSession& aSession,
                                       TUint aSSRC,
                                       CSRTPCryptoContext* aCon,
                                       MSRTPReKeyingObserver& aObs )
    {
    CSRTPStreamOut* self = new( ELeave )CSRTPStreamOut( aSession, aSSRC, 
                                                      aCon, aObs);
    CleanupStack::PushL( self );    
    TRAPD( err, self->ConstructL() );
    if ( err )
        {
        self->iStrmSpecificCrypto = EFalse; // Cannot take ownership
        User::Leave( err );
        }
    CleanupStack::Pop( self );        
    return self;
    }	

// ---------------------------------------------------------------------------
// Used when crypto context will be set later to this stream
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamOut* CSRTPStreamOut::NewL( CSRTPSession& aSession,
                                       TUint aSSRC,
                                       MSRTPReKeyingObserver& aObs )
    {
    CSRTPStreamOut* self = new( ELeave )CSRTPStreamOut( aSession, aSSRC, 
                                                       aObs);
         
    return self;
    }	

// ---------------------------------------------------------------------------
// CSRTPStreamOut::CSRTPStreamOut
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamOut::CSRTPStreamOut( CSRTPSession& aSession,
                                      TUint aSSRC)
    :   CSRTPStream(aSession, aSSRC, EFalse),
        iStates(MSRTPStreamOutContext::EMaxStates),
        iCurrentRTPState(ESRTPStreamOutUninitialized),
        iCurrentRTCPState(ESRTPStreamOutUninitialized)
    {
    
    }

// ---------------------------------------------------------------------------
// CSRTPStreamOut::CSRTPStreamOut
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamOut::CSRTPStreamOut( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs )
    :   CSRTPStream(aSession, aSSRC, aCon, aObs,  EFalse),
        iStates(MSRTPStreamOutContext::EMaxStates),
        iCurrentRTPState(ESRTPStreamOutUninitialized),      
        iCurrentRTCPState(ESRTPStreamOutUninitialized)    
    {
    
    }
    
// ---------------------------------------------------------------------------
// CSRTPStreamOut::CSRTPStreamOut
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamOut::CSRTPStreamOut( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      MSRTPReKeyingObserver& aObs )
    :   CSRTPStream(aSession, aSSRC, aObs,  EFalse),
        iStates(MSRTPStreamOutContext::EMaxStates),
        iCurrentRTPState(ESRTPStreamOutUninitialized),      
        iCurrentRTCPState(ESRTPStreamOutUninitialized)    
    {
    
    }    
	
// -----------------------------------------------------------------------------
// CSRTPStreamOut::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPStreamOut::ConstructL()
	{
	CreateCryptoHandlerSRTPL();
    CreateCryptoHandlerSRTCPL();
    InitializeStatesL();
	}

// -----------------------------------------------------------------------------
// CSRTPStreamOut::SetCryptoOutL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CSRTPStreamOut::SetCryptoOutL(CSRTPCryptoContext* aCon)
	{
	//checking if there is one crytpocontext existed
	//construct only when stream has its own cryto handler
	if( !aCon)
	    {
	    User::Leave( KErrArgument );
	    }
	if (!iRekey )
		{
		delete iHandlerRTP; iHandlerRTP=NULL;    
    	delete iHandlerRTCP;iHandlerRTCP=NULL; 
    	delete iContext; iContext=NULL;
		iContext= aCon;
		iStrmSpecificCrypto= ETrue;
    	iStates.Reset(); 
    	//should create handler and initial state
    	TRAPD( err, 
    	    {
    	    CreateCryptoHandlerSRTPL();
        	CreateCryptoHandlerSRTCPL();
        	InitializeStatesL();
    	    } );
    	if ( err )
    	    {
    	    iContext = NULL; // Cannot take ownership if leave occurs
    	    User::Leave( err );
    	    }
    	ChangeRTPState(MSRTPStreamOutContext::ESRTPStreamOutUninitialized);
    	ChangeRTCPState(MSRTPStreamOutContext::ESRTPStreamOutUninitialized);
		}
	}
	
// ---------------------------------------------------------------------------
// CSRTPStreamOut::ReInitialOutStates
// 
// ---------------------------------------------------------------------------
//
void CSRTPStreamOut::UpdateCryptoAndStatesL()
	{
	if( !iStrmSpecificCrypto)
		{
		UpdateCryptoHandlerL();
		iStates.Reset(); 
		InitializeStatesL();
		ChangeRTPState(MSRTPStreamOutContext::ESRTPStreamOutUninitialized);
		ChangeRTCPState(MSRTPStreamOutContext::ESRTPStreamOutUninitialized);
		}	
	}
// ---------------------------------------------------------------------------
// HBufC8* CSRTPStreamOut::ProtectRtpL(
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSRTPStreamOut::ProtectRtpL( const TDesC8& aPacket )
    {    
    if (IsContextSet())       
    	return (CurrentRTPState().DoProtectRtpL(aPacket));
    //it is using session crypto but the crypto handler is not been generated yet
    //it is stream specific crypto but handler is not set yet	
    User::Leave (KErrArgument);
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// HBufC8* CSRTPStreamOut::ProtectRtcpL(
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSRTPStreamOut::ProtectRtcpL( const TDesC8& aPacket )
    {
    if (IsContextSet())
    	return (CurrentRTCPState().DoProtectRtcpL(aPacket));
    User::Leave (KErrArgument);
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// CSRTPStreamOut::InitializeStatesL
// -----------------------------------------------------------------------------
//	
void CSRTPStreamOut::InitializeStatesL()
    {
	iStates.AppendL(TSRTPStreamOutStateUninit(*this, *iHandlerRTP, *iHandlerRTCP),
					sizeof(TSRTPStreamOutStateUninit));
	iStates.AppendL(TSRTPStreamOutStateNormal(*this, *iHandlerRTP, *iHandlerRTCP),
					sizeof(TSRTPStreamOutStateNormal));
    }

// ---------------------------------------------------------------------------
// CSRTPStreamOut::~CSRTPStreamOut
// ---------------------------------------------------------------------------
//
CSRTPStreamOut::~CSRTPStreamOut( )
    {
    delete iHandlerRTP;   
    delete iHandlerRTCP;
    if (iStrmSpecificCrypto ) 
    	{
    	delete iContext; iContext=NULL;
    	}  
     
	iStates.Reset();
	iHandlerRTP=NULL;  
	iHandlerRTCP=NULL; 
	}    


// -----------------------------------------------------------------------------
// CSRTPStreamOut::ChangeRTPState
// -----------------------------------------------------------------------------
//      
void CSRTPStreamOut::ChangeRTPState(MSRTPStreamOutContext::TContextOutState aNewState)
    {
    SRTP_DEBUG_TINT_VALUE( "CSRTPStreamOut::ChangeRTPState to new State", aNewState);
     
    iCurrentRTPState = aNewState;
    }

// -----------------------------------------------------------------------------
// CSRTPStreamOut::ChangeRTCPState
// -----------------------------------------------------------------------------
//      
void CSRTPStreamOut::ChangeRTCPState(TContextOutState aNewState)
    {
    iCurrentRTCPState = aNewState;
    }

// ----------------------------------------------------------------------------
// CSRTPStreamOut::CurrentRTPState
// ----------------------------------------------------------------------------
//
TSRTPStreamOutStateBase& CSRTPStreamOut::CurrentRTPState()
	{
	SRTP_DEBUG_TINT_VALUE( "CSRTPStreamOut::CurrentRTPState()", iCurrentRTPState);
    SRTP_DEBUG_TINT_VALUE( "CSRTPStreamOut State count", iStates.Count() );    
                
    
    return iStates.At(iCurrentRTPState);
	}

// ----------------------------------------------------------------------------
// CSRTPStreamOut::CurrentRTCPState
// ----------------------------------------------------------------------------
//
TSRTPStreamOutStateBase& CSRTPStreamOut::CurrentRTCPState()
	{
	SRTP_DEBUG_TINT_VALUE( "CSRTPStreamOut::CurrentRTCPState()", iCurrentRTCPState);
    SRTP_DEBUG_TINT_VALUE( "CSRTPStreamOut State count", iStates.Count() );    
                
    return iStates.At(iCurrentRTCPState);
	}
