/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include "srtpstream.h"
#include "srtpsession.h"
#include "srtpcryptohandler.h"
#include "srtpcryptohandlersrtp.h"
#include "srtpcryptohandlersrtcp.h"
#include "msrtprekeyingobserver.h"
#include "srtpcryptocontext.h"

// CONSTANTS
const TInt CSRTPStream::iStreamOffset = _FOFF( CSRTPStream, iStreamLink );

// ---------------------------------------------------------------------------
// CSRTPStream::SetROC()
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CSRTPStream::SetROC(  TUint32 aROC )
    {
    iROC = aROC;
    }

// ---------------------------------------------------------------------------
// CSRTPStream::CSRTPStream
// 
// ---------------------------------------------------------------------------
//
CSRTPStream::CSRTPStream( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      TBool aIsStreamInType)
    :   iSession(aSession),
        iContext(NULL),
        iObserver(NULL),
        iStrmSpecificCrypto(EFalse), 
        iHandlerRTP(NULL),       
        iHandlerRTCP(NULL),       
        iROC(0),
        iSSRC(aSSRC),
        iIsStreamInType(aIsStreamInType),
        iRekey(EFalse)
    {
    iSession.AddStreamToList(this);    
    }

// ---------------------------------------------------------------------------
// CSRTPStream::CSRTPStream
// 
// ---------------------------------------------------------------------------
//
CSRTPStream::CSRTPStream( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs ,
                                      TBool aIsStreamInType)
      : iSession(aSession),
        iContext(aCon),
        iObserver(&aObs),
        iStrmSpecificCrypto(ETrue),
        iHandlerRTP(NULL),
        iHandlerRTCP(NULL),
        iROC(0),
        iSSRC(aSSRC),
        iIsStreamInType(aIsStreamInType),
        iRekey(EFalse)    
    {
    iSession.AddStreamToList(this);
    }
	
// ---------------------------------------------------------------------------
// CSRTPStream::CSRTPStream
// 
// ---------------------------------------------------------------------------
//
CSRTPStream::CSRTPStream( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      MSRTPReKeyingObserver& aObs ,
                                      TBool aIsStreamInType)
      : iSession(aSession),
        iContext(NULL),
        iObserver(&aObs),
        iStrmSpecificCrypto(EFalse),
        iHandlerRTP(NULL),
        iHandlerRTCP(NULL),
        iROC(0),
        iSSRC(aSSRC),
        iIsStreamInType(aIsStreamInType),
        iRekey(EFalse)      
    {
    iSession.AddStreamToList(this);
    }
// ---------------------------------------------------------------------------
// CSRTPStream::CSRTPStream
// 
// ---------------------------------------------------------------------------
//
CSRTPStream::CSRTPStream( CSRTPSession& aSession,
                          TBool aIsStreamInType)
    :   iSession(aSession),
        iContext(NULL),
        iObserver(NULL),
        iStrmSpecificCrypto(EFalse), 
        iHandlerRTP(NULL),       
        iHandlerRTCP(NULL),       
        iROC(0),
        iSSRC(0),                // set dummy 0 for SSRC in "late binding" mode
        iIsStreamInType(aIsStreamInType),
        iRekey(EFalse)
    {
    iSession.AddStreamToList(this);    
    }


// ---------------------------------------------------------------------------
// CSRTPStream::CSRTPStream
// 
// ---------------------------------------------------------------------------
//
CSRTPStream::CSRTPStream( CSRTPSession& aSession,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs,
                                      TBool aIsStreamInType)
      : iSession(aSession),
        iContext(aCon),
        iObserver(&aObs),
        iStrmSpecificCrypto(ETrue),
        iHandlerRTP(NULL),       
        iHandlerRTCP(NULL),       
        iROC(0),
        iSSRC(0),          // set dummy 0 for SSRC in "late binding" mode
        iIsStreamInType(aIsStreamInType),
        iRekey(EFalse)
    {
    iSession.AddStreamToList(this);
    }
	
// ---------------------------------------------------------------------------
// CSRTPStream::CSRTPStream
// 
// ---------------------------------------------------------------------------
//
CSRTPStream::CSRTPStream( CSRTPSession& aSession,
                                      MSRTPReKeyingObserver& aObs,
                                      TBool aIsStreamInType)
      : iSession(aSession),
        iContext(NULL),
        iObserver(&aObs),
        iStrmSpecificCrypto(EFalse),
        iHandlerRTP(NULL),       
        iHandlerRTCP(NULL),       
        iROC(0),
        iSSRC(0),          // set dummy 0 for SSRC in "late binding" mode
        iIsStreamInType(aIsStreamInType),
        iRekey(EFalse)
    {
    iSession.AddStreamToList(this);
    }
// ---------------------------------------------------------------------------
// CSRTPStream::ROC
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TUint32 CSRTPStream::ROC() const
    {
    return iROC;
    }

// ---------------------------------------------------------------------------
// CSRTPStream::SSRC
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CSRTPStream::SSRC() const
    {
    return iSSRC;
    }

// ---------------------------------------------------------------------------
// CSRTPStream::SSRC
// 
// ---------------------------------------------------------------------------
//
void CSRTPStream::SetSSRC( const TUint aSSRC )
	{
	//should only be set when it is in latebinding state with In-Stream
	iSSRC=aSSRC;	
	}
// ---------------------------------------------------------------------------
// CSRTPStream::GetCryptoContext()
// 
// ---------------------------------------------------------------------------
//
CSRTPCryptoContext& CSRTPStream::GetCryptoContext()
    {
    if (iStrmSpecificCrypto || iContext) 
    	{
    	return *iContext;		
    	}
    else
    	{
    	return iSession.GetCryptoContext();    
    	}	
    }


// ---------------------------------------------------------------------------
// CSRTPStream::GetCryptoContext()
// 
// ---------------------------------------------------------------------------
//
TBool CSRTPStream::IsContextSet()
    {
    if (iStrmSpecificCrypto && iHandlerRTP && iHandlerRTCP) 
    	{
    	return ETrue;		
    	}
    if (iSession.iSesssionCrypto && iHandlerRTP && iHandlerRTCP )
    	{
    	return ETrue;    
    	}	
    return EFalse;	
    }

// ---------------------------------------------------------------------------
// CSRTPStream::~CSRTPStream
// 
// ---------------------------------------------------------------------------
//
CSRTPStream::~CSRTPStream()
    {
	iSession.RemoveStreamFromList(this);   
    }

// ---------------------------------------------------------------------------
// CSRTPStream::CreateCryptoHandlerSRTPL
// 
// ---------------------------------------------------------------------------
//
void CSRTPStream::CreateCryptoHandlerSRTPL()
    {
    if (!iHandlerRTP)
        {
        iHandlerRTP = CSRTPCryptoHandlerSRTP::NewL(*this); 
        }    
    }

// ---------------------------------------------------------------------------
// CSRTPStream::CreateCryptoHandlerSRTCPL
// 
// ---------------------------------------------------------------------------
//
void CSRTPStream::CreateCryptoHandlerSRTCPL()
    {
    if (!iHandlerRTCP)
        {
        iHandlerRTCP = CSRTPCryptoHandlerSRTCP::NewL(*this); 	
        }    
    }

// ---------------------------------------------------------------------------
// CSRTPStream::ReKeyNeeded
// 
// ---------------------------------------------------------------------------
//
void CSRTPStream::ReKeyNeeded()
	{
	if (iObserver)
		{
		iObserver->SRTPMasterKeyStaleEvent(*this);
		}
	else
		{
		iSession.ReKeyNeeded(*this,iStrmSpecificCrypto );
		}	
	}

// ---------------------------------------------------------------------------
// CSRTPStream::KeyExpired
// 
// ---------------------------------------------------------------------------
//
void CSRTPStream::KeyExpired()
	{
	iRekey = ETrue;
	iSession.iRekey = ETrue;
	}
// -----------------------------------------------------------------------------
// CSRTPStream::UpdateCryptoAndStatesL()
// -----------------------------------------------------------------------------
//
void CSRTPStream::UpdateCryptoHandlerL()
	{
	//Note ROC is not updating
	
    delete iHandlerRTP; iHandlerRTP=NULL;    
	delete iHandlerRTCP;iHandlerRTCP=NULL;

  	//should create handler and initial state
	CreateCryptoHandlerSRTPL();
	CreateCryptoHandlerSRTCPL();
	}	
	
// ---------------------------------------------------------------------------
// CSRTPStream::CreateCryptoHandlerSRTCPL
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSRTPStream::StreamType()
	{
	return iIsStreamInType;
	}
