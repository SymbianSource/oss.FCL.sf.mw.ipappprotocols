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
#include "msrtpstreamincontext.h"
#include "srtpstreamin.h"
#include "tsrtpstreaminstatebase.h"
#include "tsrtpstreaminstateuninit.h"
#include "tsrtpstreaminstatelatebind.h"
#include "tsrtpstreaminstatenormal.h"
#include "srtpsession.h"
#include "srtpcryptohandler.h"
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "srtputils.h"

// ---------------------------------------------------------------------------
// Constructor. Used when stream uses default cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamIn* CSRTPStreamIn::NewL( CSRTPSession& aSession,
                                      TUint aSSRC)
    {
    CSRTPStreamIn* self = new( ELeave )CSRTPStreamIn( aSession, aSSRC);
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );        
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamIn* CSRTPStreamIn::NewL( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs )
    {
    CSRTPStreamIn* self = new( ELeave )CSRTPStreamIn( aSession, aSSRC, 
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
// Constructor. Used when stream uses default cryptographic context
// and "late binding". No SSRC is provided by the user.
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamIn* CSRTPStreamIn::NewL( CSRTPSession& aSession)
    {
    CSRTPStreamIn* self = new( ELeave )CSRTPStreamIn( aSession);
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );        
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor. Used when stream uses its own cryptographic context
// and "late binding". No SSRC is provided by the user.
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamIn* CSRTPStreamIn::NewL( CSRTPSession& aSession,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs )
    {
    CSRTPStreamIn* self = new( ELeave )CSRTPStreamIn( aSession, 
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
// Constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamIn* CSRTPStreamIn::NewL( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      MSRTPReKeyingObserver& aObs )
    {
    CSRTPStreamIn* self = new( ELeave )CSRTPStreamIn( aSession, aSSRC,aObs);
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor. Used when stream uses its own cryptographic context
// and "late binding". No SSRC is provided by the user.
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPStreamIn* CSRTPStreamIn::NewL( CSRTPSession& aSession,
                                      MSRTPReKeyingObserver& aObs )
    {
    CSRTPStreamIn* self = new( ELeave )CSRTPStreamIn( aSession, aObs);
    return self;
    }    
// -----------------------------------------------------------------------------
// CSRTPStreamIn::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPStreamIn::ConstructL()
	{
	CreateCryptoHandlerSRTPL();
    CreateCryptoHandlerSRTCPL();
    InitializeStatesL();
	}

// -----------------------------------------------------------------------------
// CSRTPStreamIn::SetCryptoInL()
// -----------------------------------------------------------------------------
//
EXPORT_C void CSRTPStreamIn::SetCryptoInL(CSRTPCryptoContext* aCon)
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
            
    	if (SSRC())
    		{
    		ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInUninitialized);
    		ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInUninitialized);
    		}
    	else
    		{
    		ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInLateBinding);
    		ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInLateBinding);
    		}	
		}
	}

// ---------------------------------------------------------------------------
// CSRTPStreamIn::ReInitialInStates
// 
// ---------------------------------------------------------------------------
//
void CSRTPStreamIn::UpdateCryptoAndStatesL()
	{
	if( !iStrmSpecificCrypto)
	    {
	    UpdateCryptoHandlerL();
		iStates.Reset(); 
		InitializeStatesL();
		if (SSRC())
			{
			ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInUninitialized);
			ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInUninitialized);
			}
		else
			{
			ChangeRTPState(MSRTPStreamInContext::ESRTPStreamInLateBinding);
			ChangeRTCPState(MSRTPStreamInContext::ESRTPStreamInLateBinding);
			}	
		}
	}
// ---------------------------------------------------------------------------
// CSRTPStreamIn::CSRTPStreamIn
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamIn::CSRTPStreamIn(  CSRTPSession& aSession,
                                    TUint aSSRC)
    :   CSRTPStream(aSession, aSSRC, ETrue),
        iStates(MSRTPStreamInContext::EMaxStates),
        iCurrentRTPState(MSRTPStreamInContext::ESRTPStreamInUninitialized),      
        iCurrentRTCPState(MSRTPStreamInContext::ESRTPStreamInUninitialized)
           
    {
    
    }

// ---------------------------------------------------------------------------
// CSRTPStreamIn::~CSRTPStreamIn
// ---------------------------------------------------------------------------
//
CSRTPStreamIn::~CSRTPStreamIn( )
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
// ---------------------------------------------------------------------------
// CSRTPStreamIn::CSRTPStreamIn
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamIn::CSRTPStreamIn( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs )
    :   CSRTPStream(aSession, aSSRC, aCon, aObs, ETrue),
        iStates(MSRTPStreamInContext::EMaxStates),
        iCurrentRTPState(MSRTPStreamInContext::ESRTPStreamInUninitialized),      
        iCurrentRTCPState(MSRTPStreamInContext::ESRTPStreamInUninitialized)
            
    {
    
    }

// ---------------------------------------------------------------------------
// CSRTPStreamIn::CSRTPStreamIn
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamIn::CSRTPStreamIn( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      MSRTPReKeyingObserver& aObs )
    :   CSRTPStream(aSession, aSSRC, aObs, ETrue),
        iStates(MSRTPStreamInContext::EMaxStates),
        iCurrentRTPState(MSRTPStreamInContext::ESRTPStreamInUninitialized),      
        iCurrentRTCPState(MSRTPStreamInContext::ESRTPStreamInUninitialized)
            
    {
    
    }
		
// ---------------------------------------------------------------------------
// CSRTPStreamIn::CSRTPStreamIn
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamIn::CSRTPStreamIn(  CSRTPSession& aSession)
    :   CSRTPStream(aSession,  ETrue),
        iStates(MSRTPStreamInContext::EMaxStates),
        iCurrentRTPState(MSRTPStreamInContext::ESRTPStreamInLateBinding),     
        iCurrentRTCPState(MSRTPStreamInContext::ESRTPStreamInLateBinding)
       
    {
    
    }

// ---------------------------------------------------------------------------
// CSRTPStreamIn::CSRTPStreamIn
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamIn::CSRTPStreamIn( CSRTPSession& aSession,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs )
    :   CSRTPStream(aSession, aCon, aObs,  ETrue),
        iStates(MSRTPStreamInContext::EMaxStates),
        iCurrentRTPState(MSRTPStreamInContext::ESRTPStreamInLateBinding),     
        iCurrentRTCPState(MSRTPStreamInContext::ESRTPStreamInLateBinding)
       
    {
    
    }

// ---------------------------------------------------------------------------
// CSRTPStreamIn::CSRTPStreamIn
// 
// ---------------------------------------------------------------------------
//
CSRTPStreamIn::CSRTPStreamIn( CSRTPSession& aSession,
                                      MSRTPReKeyingObserver& aObs )
    :   CSRTPStream(aSession, aObs,  ETrue),
        iStates(MSRTPStreamInContext::EMaxStates),
        iCurrentRTPState(MSRTPStreamInContext::ESRTPStreamInLateBinding),     
        iCurrentRTCPState(MSRTPStreamInContext::ESRTPStreamInLateBinding)
       
    {
    
    }
		

// ---------------------------------------------------------------------------
// HBufC8* CSRTPStreamIn::UnprotectSrtpL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSRTPStreamIn::UnprotectSrtpL( const TDesC8& aPacket )
    {
 	SRTP_DEBUG_DETAIL( "CSRTPStreamIn::UnprotectSrtpL Entry" );
     
    if ( IsContextSet() )
    	{
    	SRTP_DEBUG_DETAIL( "CSRTPStreamIn::UnprotectSrtpL Exit" );
    	return (CurrentRTPState().DoUnprotectSrtpL(aPacket) );
    	}
    SRTP_DEBUG_DETAIL( "CSRTPStreamIn::UnprotectSrtpL Exit" );
    
    User::Leave (KErrArgument);
    return NULL;
    }

// ---------------------------------------------------------------------------
// HBufC8* CSRTPStreamIn::UnprotectSrtcpL()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSRTPStreamIn::UnprotectSrtcpL( const TDesC8& aPacket )
    {
    if (IsContextSet())
    	return (CurrentRTCPState().DoUnprotectSrtcpL(aPacket));
    User::Leave (KErrArgument);
    return NULL;
    }


// -----------------------------------------------------------------------------
// CSRTPStreamIn::InitializeStatesL
// -----------------------------------------------------------------------------
//	
void CSRTPStreamIn::InitializeStatesL()
    {
	iStates.AppendL(TSRTPStreamInStateUninit(*this, *iHandlerRTP, *iHandlerRTCP),
					sizeof(TSRTPStreamInStateUninit));
	iStates.AppendL(TSRTPStreamInStateLateBind(*this, *iHandlerRTP, *iHandlerRTCP),
					sizeof(TSRTPStreamInStateLateBind));
	iStates.AppendL(TSRTPStreamInStateNormal(*this, *iHandlerRTP, *iHandlerRTCP),
					sizeof(TSRTPStreamInStateNormal));
    }

// -----------------------------------------------------------------------------
// CSRTPStreamIn::ChangeRTPState
// -----------------------------------------------------------------------------
//      
void CSRTPStreamIn::ChangeRTPState(MSRTPStreamInContext::TContextInState aNewState)
    {
    SRTP_DEBUG_TINT_VALUE( "CSRTPStreamIn::ChangeRTPState", aNewState);
    
    iCurrentRTPState = aNewState;
    }

// -----------------------------------------------------------------------------
// CSRTPStreamIn::ChangeRTCPState
// -----------------------------------------------------------------------------
//      
void CSRTPStreamIn::ChangeRTCPState(MSRTPStreamInContext::TContextInState aNewState)
    {
    iCurrentRTCPState = aNewState;
    }

// ----------------------------------------------------------------------------
// CSRTPStreamIn::CurrentRTPState
// ----------------------------------------------------------------------------
//
TSRTPStreamInStateBase& CSRTPStreamIn::CurrentRTPState()
	{
	SRTP_DEBUG_TINT_VALUE( "CSRTPStreamIN::CurrentRTPState()", iCurrentRTPState);
    SRTP_DEBUG_TINT_VALUE( "CSRTPStreamIN::RTP State Count", iStates.Count() );    
    
	return iStates.At(iCurrentRTPState);
	}

// ----------------------------------------------------------------------------
// CSRTPStreamIn::CurrentRTCPState
// ----------------------------------------------------------------------------
//
TSRTPStreamInStateBase& CSRTPStreamIn::CurrentRTCPState()
	{
	SRTP_DEBUG_TINT_VALUE( "CSRTPStreamIN::CurrentRTCPState()", iCurrentRTPState );
    SRTP_DEBUG_TINT_VALUE( "CSRTPStreamIN::RTCP State Count", iStates.Count() );    
     	
	return iStates.At(iCurrentRTCPState);
	}

