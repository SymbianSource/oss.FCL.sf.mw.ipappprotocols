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
* Description:    .
*
*/




// INCLUDES
#include <e32std.h>
#include "srtpmasterkey.h"
#include "srtpmastersalt.h"
#include "srtpcryptohandler.h"
#include "srtpdef.h"
#include "srtpcryptocontext.h"
#include "srtpcryptoparams.h"


// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPCryptoContext* CSRTPCryptoContext::NewL( const CSRTPMasterKey* aKey,
                                            const CSRTPMasterSalt* aSalt, 
                                            const TSrtpCryptoParams& aCryptoParams)
    {
    
    
    CSRTPCryptoContext* self = CSRTPCryptoContext::NewLC( aKey,
                                                           aSalt,
                                                           aCryptoParams );
    CleanupStack::Pop( self );    
    
    return self;
    }	

// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPCryptoContext* CSRTPCryptoContext::NewLC( const CSRTPMasterKey* aKey,
                                            const CSRTPMasterSalt* aSalt,
                                            const TSrtpCryptoParams& aCryptoParams)
    {
                                          
    CSRTPCryptoContext* self = new( ELeave )CSRTPCryptoContext( aCryptoParams );
                                                                
                                                      
    CleanupStack::PushL( self );    
    self->ConstructL( aKey, aSalt );
    
    return self;
    }	

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::CSRTPCryptoContext
// ---------------------------------------------------------------------------
//
CSRTPCryptoContext::CSRTPCryptoContext(  const TSrtpCryptoParams& aCryptoParams) :
            iHandlerList(CSRTPCryptoHandler::iHandlerOffset), 
            iHandlerIter( iHandlerList ),                           
            iCryptoParams (aCryptoParams)
            
            {
            }


// -----------------------------------------------------------------------------
// CSRTPCryptoContext::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPCryptoContext::ConstructL( const CSRTPMasterKey* aKey,
                                     const CSRTPMasterSalt* aSalt)
	{
    if( !aKey || !aSalt )
        {
        User::Leave( KErrNoMemory );
        }
    else
        {
        iKey = aKey;
        iSalt = aSalt;
        }
    
    if (!Valid())
    	{
    	User::Leave(KErrArgument);
    	}
    }
	
// ---------------------------------------------------------------------------
// CSRTPCryptoContext::CSRTPCryptoContext
// ---------------------------------------------------------------------------
//
CSRTPCryptoContext::~CSRTPCryptoContext( )
           
    {
    delete iKey; iKey=NULL;
    delete iSalt; iSalt=NULL;
    iHandlerList.Reset();
    }


// ---------------------------------------------------------------------------
// CSRTPCryptoContext::SetMasterKey()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSRTPCryptoContext::SetMasterKey(const CSRTPMasterKey* aKey) 
    {
    if (iKey)
    	{
    	delete iKey; 
    	iKey=NULL;	
    	}
    iKey = aKey;
    
    CSRTPCryptoHandler* handler;
    iHandlerIter.SetToFirst();
    while( ( handler = iHandlerIter++ ) != NULL )
        {
        handler->SRTPMasterKeyChanged();        
        }
    }


// ---------------------------------------------------------------------------
// CSRTPCryptoContext::SetMasterSalt()
// ---------------------------------------------------------------------------
//
EXPORT_C  void CSRTPCryptoContext::SetMasterSalt(const CSRTPMasterSalt* aSalt) 
    {
    if(iSalt)
    	{
    	delete iSalt;
    	iSalt = NULL;	
    	}
    iSalt = aSalt;

    CSRTPCryptoHandler* handler;
    iHandlerIter.SetToFirst();
    while( ( handler = iHandlerIter++ ) != NULL )
        {
        handler->SRTPMasterSaltChanged();        
        }
    }

    


// ---------------------------------------------------------------------------
// CSRTPCryptoContext::AddCryptoChangeObserver()
// ---------------------------------------------------------------------------
//
 void CSRTPCryptoContext::AddCryptoChangeObserver(CSRTPCryptoHandler *aObserver)
    {
    iHandlerList.AddLast(*aObserver);
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::RemoveCryptoChangeObserver()
// ---------------------------------------------------------------------------
//
 void CSRTPCryptoContext::RemoveCryptoChangeObserver(CSRTPCryptoHandler *aObserver)
    {   
    if (!iHandlerList.IsEmpty() && FindHandler(aObserver))
        {
        iHandlerList.Remove(*aObserver);
        }
    }
// ---------------------------------------------------------------------------
// CSRTPCryptoContext::MasterKey()
// ---------------------------------------------------------------------------
//
EXPORT_C  const CSRTPMasterKey& CSRTPCryptoContext::MasterKey() const 
    {
    return *iKey;
    }
// ---------------------------------------------------------------------------
// CSRTPCryptoContext::MasterSalt()
// ---------------------------------------------------------------------------
//
EXPORT_C  const CSRTPMasterSalt& CSRTPCryptoContext::MasterSalt() const
    {
    return *iSalt;
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::CryptoParams()
// ---------------------------------------------------------------------------
//
EXPORT_C const TSrtpCryptoParams& CSRTPCryptoContext::CryptoParams() 
    {
    return iCryptoParams;
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::SetRCCm3Sync()
// ---------------------------------------------------------------------------
//
EXPORT_C void CSRTPCryptoContext::SetRCCm3Sync(TBool aSync)
	{
	iCryptoParams.iIsRCCm3Sync = aSync;
	}

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::IsEqual()
// ---------------------------------------------------------------------------
//
TBool CSRTPCryptoContext::IsEqual(const CSRTPCryptoContext& aContext)
	{
	TBool isEqual= EFalse;
	if (!aContext.iKey->iKey->Compare(iKey->iKey->Des()) && 
		!aContext.iSalt->iKey->Compare(iSalt->iKey->Des()) &&
		!aContext.iKey->iMKI->Compare(iKey->iMKI->Des()))
		{
		if (aContext.iCryptoParams.iSrtpEncAlg==iCryptoParams.iSrtpEncAlg &&
			aContext.iCryptoParams.iSrtpEncAlg==iCryptoParams.iSrtcpEncAlg &&
			aContext.iCryptoParams.iSrtpAuthAlg==iCryptoParams.iSrtpAuthAlg  &&
			aContext.iCryptoParams.iSrtcpAuthAlg==iCryptoParams.iSrtcpAuthAlg &&
			aContext.iCryptoParams.iSrtpAuthTagLen==iCryptoParams.iSrtpAuthTagLen &&
			aContext.iCryptoParams.iSrtcpAuthTagLen==iCryptoParams.iSrtcpAuthTagLen &&
			aContext.iCryptoParams.iSrtpKeyDervRate==iCryptoParams.iSrtpKeyDervRate &&
			aContext.iCryptoParams.iSrtcpKeyDervRate==iCryptoParams.iSrtcpKeyDervRate &&
			aContext.iCryptoParams.iSrtpReplayProtection==iCryptoParams.iSrtpReplayProtection &&
			aContext.iCryptoParams.iSrtcpReplayProtection==iCryptoParams.iSrtcpReplayProtection &&
			aContext.iCryptoParams.iROCTransRate==iCryptoParams.iROCTransRate &&
			aContext.iCryptoParams.iMasterKeysLifeTime==iCryptoParams.iMasterKeysLifeTime &&
			aContext.iCryptoParams.iReplayWindowSizeHint==iCryptoParams.iReplayWindowSizeHint &&
			aContext.iCryptoParams.iIsRCCm3Sync==iCryptoParams.iIsRCCm3Sync)
		   		return !isEqual;
		}
	return isEqual;
	}

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::operator=
// ---------------------------------------------------------------------------
//
const CSRTPCryptoContext& CSRTPCryptoContext::operator=(const CSRTPCryptoContext& aContext)
    {
    return aContext;
    }

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::UpdateCryptoParams
// ---------------------------------------------------------------------------
//
void CSRTPCryptoContext::UpdateCryptoParams(const  TSrtpCryptoParams& aCryptoParams )
	{
	iCryptoParams = aCryptoParams;
	}
	
// ---------------------------------------------------------------------------
// CSRTPCryptoContext::FindCryptoHandler
// ---------------------------------------------------------------------------
//       

TBool CSRTPCryptoContext::FindHandler( CSRTPCryptoHandler *aObserver )
	{
	TBool found=EFalse;
	CSRTPCryptoHandler* handler=NULL;
	
	iHandlerIter.SetToFirst();
	handler =iHandlerIter;
	while ((handler=iHandlerIter++)!=NULL)
			{
			if (handler == aObserver)
				{
				found = ETrue;
				return found;
				}
			}
	return found;
	}

// ---------------------------------------------------------------------------
// CSRTPCryptoContext::Valid()
// ---------------------------------------------------------------------------
//        
EXPORT_C  TBool CSRTPCryptoContext::Valid() const
    {
    if (!(iKey->Valid()))
        {
        return EFalse;
        }    
        
    if (!(iSalt->Valid()))
        {
        return EFalse;
        }    
        
    if ((iCryptoParams.iSrtpEncAlg!=EEncAES_CM &&
          iCryptoParams.iSrtpEncAlg!=ENullAlg) ||( 
         iCryptoParams.iSrtcpEncAlg!=EEncAES_CM &&
        	iCryptoParams.iSrtcpEncAlg!=ENullAlg))
        {
        return EFalse;
        }  

    if ((iCryptoParams.iSrtpAuthAlg!=EAuthHMAC_SHA1 &&
          iCryptoParams.iSrtpAuthAlg!=EAuthNull  &&
          iCryptoParams.iSrtpAuthAlg!=EAuthRCCm1 &&
          iCryptoParams.iSrtpAuthAlg!=EAuthRCCm2 &&
          iCryptoParams.iSrtpAuthAlg!=EAuthRCCm3 ) || 
          (iCryptoParams.iSrtcpAuthAlg!=EAuthHMAC_SHA1 &&
          iCryptoParams.iSrtcpAuthAlg!=EAuthNull ))
        {
        return EFalse;
        }  
	if (iCryptoParams.iSrtpAuthAlg==EAuthHMAC_SHA1 &&
		(iCryptoParams.iSrtpAuthTagLen!=KSRTPAuthTagLength80 &&
		iCryptoParams.iSrtpAuthTagLen!=KSRTPAuthTagLength32
		))
		{
		return EFalse;
		}
	
	if (iCryptoParams.iSrtcpAuthAlg==EAuthHMAC_SHA1 &&
		(iCryptoParams.iSrtcpAuthTagLen!=KSRTPAuthTagLength80 &&
		iCryptoParams.iSrtcpAuthTagLen!=KSRTPAuthTagLength32
		))
		{
		return EFalse;
		}	
    if ((iCryptoParams.iSrtpAuthAlg==EAuthRCCm1 ||
          iCryptoParams.iSrtpAuthAlg==EAuthRCCm2 ) && 
          iCryptoParams.iSrtpAuthTagLen != KSRTPAuthTagLength112)
        {
        return EFalse;
        }  
	if (iCryptoParams.iSrtpAuthAlg==EAuthRCCm3 && 
          iCryptoParams.iSrtpAuthTagLen != KSRTPAuthTagLength32)
        {
        return EFalse;
        }  
	if ((iCryptoParams.iSrtpAuthAlg==EAuthNull  && 
          iCryptoParams.iSrtpAuthTagLen != 0)||(
          iCryptoParams.iSrtcpAuthAlg==EAuthNull  && 
          iCryptoParams.iSrtcpAuthTagLen != 0))
        {
        return EFalse;
        }  

    if (iCryptoParams.iPrefixLen!=KSRTPPrefixLength)
        {
        return EFalse;
        }  
        
    if (!iCryptoParams.iROCTransRate)
        {
        return EFalse;
        }      
        
    return ETrue;
    }

