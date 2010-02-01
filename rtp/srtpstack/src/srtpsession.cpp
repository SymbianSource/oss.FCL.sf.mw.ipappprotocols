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
* Description:    Contains a default cryptographic context for SRTP streams.
*
*/




// INCLUDES
#include "srtpsession.h"
#include "srtpstream.h"
#include "srtpcryptocontext.h"
#include "srtpstreamin.h"
#include "srtpstreamout.h"
#include "msrtprekeyingobserver.h"
#include "srtputils.h"

// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses default cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPSession* CSRTPSession::NewL( const TInetAddr& aDestination )
    {
    CSRTPSession* self = CSRTPSession::NewLC( aDestination );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPSession* CSRTPSession::NewL( const TInetAddr& aDestination,
                                     CSRTPCryptoContext* aCon,
                                     MSRTPReKeyingObserver& aObs ) 
    {
    CSRTPSession* self = CSRTPSession::NewLC( aDestination, aCon, aObs );
    CleanupStack::Pop( self );
    return self;    
    }


// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPSession* CSRTPSession::NewLC( const TInetAddr& aDestination )
    {                                     
    CSRTPSession* self = new( ELeave ) CSRTPSession ( aDestination );                                      
    CleanupStack::PushL( self );    
    return self;
    }	

// ---------------------------------------------------------------------------
// Two-phased constructor. Used when stream uses its own cryptographic context
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CSRTPSession* CSRTPSession::NewLC( const TInetAddr& aDestination,
                                     CSRTPCryptoContext* aCon,
                                     MSRTPReKeyingObserver& aObs )
    {                                     
    CSRTPSession* self = new( ELeave ) CSRTPSession ( aDestination, aObs  );
    CleanupStack::PushL( self ); 
    self->ConstructL( aCon );                                      
    return self;
    }	
        

// -----------------------------------------------------------------------------
// CSRTPSession::CSRTPSession
// -----------------------------------------------------------------------------
//
 CSRTPSession::CSRTPSession( const TInetAddr& aDestination,
                             MSRTPReKeyingObserver& aObs )
    :   iStreamList(CSRTPStream::iStreamOffset),
        iStreamIter( iStreamList ),
        iDestination(aDestination),
        iObserver(&aObs),
        iStreamCount(NULL),    
        iRekey(EFalse)    
    {
    }

// -----------------------------------------------------------------------------
// CSRTPSession::CSRTPSession
// -----------------------------------------------------------------------------
//
 CSRTPSession::CSRTPSession(const TInetAddr& aDestination)
    :   iStreamList(CSRTPStream::iStreamOffset),
        iStreamIter( iStreamList ),
        iDestination(aDestination),
        iContext(NULL),
        iObserver(NULL),
        iStreamCount(NULL),    
        iSesssionCrypto(EFalse),
        iRekey(EFalse)             
    {
    }

// -----------------------------------------------------------------------------
// CSRTPSession::ConstructL
// -----------------------------------------------------------------------------
//
void CSRTPSession::ConstructL( CSRTPCryptoContext* aCon )
	{
	if( !aCon )
	    {
	    //delete this;// because of NewLC so we need to delete it
	    User::Leave( KErrArgument );
	    }
	else 
	    {
	    if (!aCon->Valid())
		    {
		    User::Leave( KErrArgument );
		    }
		iSesssionCrypto=ETrue;
		iContext = aCon;
		    
	    }
	}
	
	
// ---------------------------------------------------------------------------
// CSRTPSession::~CSRTPSession
// ---------------------------------------------------------------------------
//
CSRTPSession::~CSRTPSession()
    {
    if (iContext)
    	{
    	delete iContext;iContext=NULL;	
    	}
    if (iStreamCount)
    	{
    	RemoveAllStream( );
    	}
    iStreamList.Reset();	
    }

// -----------------------------------------------------------------------------
// CSRTPSession::UpdateCryptoContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CSRTPSession::SetCryptoContextL(CSRTPCryptoContext* aCon)
	{
	//Note that ROC is not re-set.
	if( !aCon)
	    {
	    User::Leave( KErrArgument );
	    }
	if (!iRekey )
		{
		if (iContext && iSesssionCrypto )
			{
			delete iContext;iContext=NULL;
			}
		iSesssionCrypto = ETrue;
		iContext = aCon;
		//should implement a way that session knows how to updated the stream
		if (!iStreamList.IsEmpty())
			{
			CSRTPStream* item=NULL;
	
			iStreamIter.SetToFirst();
			while ((item=iStreamIter++) != NULL)
				{
				TRAPD( err, item->UpdateCryptoAndStatesL() );
				if ( err )
				    {
				    iContext = NULL; // Cannot take ownership if leave occurs
				    User::Leave( err );
				    }
				}
			}
		
		}
	}

// -----------------------------------------------------------------------------
// CSRTPSession::RemoteAddr()
// -----------------------------------------------------------------------------
//
EXPORT_C const TInetAddr& CSRTPSession::RemoteAddr() 
	{
	return iDestination;
	}

// ---------------------------------------------------------------------------
// CSRTPSession::StreamL
// ---------------------------------------------------------------------------
//       

EXPORT_C CSRTPStream& CSRTPSession::StreamL(TUint aSSRC, TBool aIsInStream)
	{
	CSRTPStream* item=NULL;
	
	iStreamIter.SetToFirst();
	while ((item=iStreamIter++) != NULL)
			{
			if (item->SSRC()==aSSRC && aIsInStream== item->StreamType())
				return *item;
			}
	User::Leave(KErrNotFound);
	return *item;
	}

// ---------------------------------------------------------------------------
// CSRTPSession::ProtectRTPL
// ---------------------------------------------------------------------------
// 
EXPORT_C HBufC8* CSRTPSession::ProtectRTPL(TUint aSSRC, const TDesC8& aPacket)
	{
	SRTP_DEBUG_DETAIL( "CSRTPSession::ProtectRTPL ENTRY" );
  	
	//find the matching ssrc stream
	CSRTPStreamOut* stream = static_cast<CSRTPStreamOut*> (&StreamL(aSSRC, EFalse));
	SRTP_DEBUG_TUINT_VALUE( "aSSRC is", aSSRC);
     
	//If it is streamIn
	SRTP_DEBUG_DETAIL( "CSRTPSession::ProtectRTPL EXIT" );

	return stream->ProtectRtpL( aPacket );
	}

// ---------------------------------------------------------------------------
// CSRTPSession::UnprotectRTPL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSRTPSession::UnprotectRTPL(TUint aSSRC, const TDesC8& aPacket)
	{
	SRTP_DEBUG_DETAIL( "CSRTPSession::UnprotectRTPL ENTRY" );
  	   
	//Find Stream by SSRC
	CSRTPStreamIn* stream;
	if(FindStream(aSSRC, ETrue))
		{
		SRTP_DEBUG_TUINT_VALUE( "not latebinding and stream found and aSSRC is", aSSRC);
    
		 stream= static_cast<CSRTPStreamIn*> (&StreamL(aSSRC, ETrue));
		//If found
		return stream->UnprotectSrtpL( aPacket );
		}
	//If not found, try to find the stream SSRC=0;	
	SRTP_DEBUG_DETAIL( "UnprotectRTPL is latebinding and If not found, try to find the stream SSRC=0");
    
    SRTP_DEBUG_DETAIL( "CSRTPSession::UnprotectRTPL EXIT" );		
	return FindLateBindingStreamAndUnprotectRTPL(aSSRC, aPacket);
		
	}
	
// ---------------------------------------------------------------------------
// CSRTPSession::ProtectRTCPL
// ---------------------------------------------------------------------------
// 
EXPORT_C HBufC8* CSRTPSession::ProtectRTCPL(TUint aSSRC, const TDesC8& aPacket)
	{
	//find the matching ssrc stream
	CSRTPStreamOut* stream = static_cast<CSRTPStreamOut*> (&StreamL(aSSRC, EFalse));
	//If it is streamIn
	return stream->ProtectRtcpL( aPacket );
	}

// ---------------------------------------------------------------------------
// CSRTPSession::UnprotectRTPL
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC8* CSRTPSession::UnprotectRTCPL(TUint aSSRC, const TDesC8& aPacket)
	{
	//Find Stream by SSRC
	CSRTPStreamIn* stream;
	if(FindStream(aSSRC, ETrue) )
		{
		 stream= static_cast<CSRTPStreamIn*> (&StreamL(aSSRC, ETrue));
		//If found
		return stream->UnprotectSrtcpL( aPacket );
		}
	//If not found, return NULL since there must already some RTP packet received	
	// But for outgoing stream to receive RR and SDES so it should be able to 
	// decod this RTCP packet as well
	return FindLateBindingStreamAndUnprotectRTCPL(aSSRC, aPacket);
		
	}	
// ---------------------------------------------------------------------------
// CSRTPSession::FindStream
// ---------------------------------------------------------------------------
//       

TBool CSRTPSession::FindStream(TUint aSSRC, TBool aIsInStream )
	{
	TBool found=EFalse;
	TInt count = iStreamCount;
	CSRTPStream* item=NULL;
	
	iStreamIter.SetToFirst();
	item=iStreamIter;
	while ((item=iStreamIter++) != NULL && count !=0)
			{
			if (item->SSRC()==aSSRC &&  (aIsInStream== item->StreamType()))
				{
				found = ETrue;
				return found;
				}
			count --;
			}
	return found;
	}

// ---------------------------------------------------------------------------
// CSRTPSession::FindLateBindingStreamAndUnprotect
// ---------------------------------------------------------------------------
//       

HBufC8* CSRTPSession::FindLateBindingStreamAndUnprotectRTPL(TUint aSSRC, 
													const TDesC8& aPacket)
	{
	SRTP_DEBUG_DETAIL( "CSRTPSession::FindLateBindingStreamAndUnprotectRTPL ENTRY" );
  	
	CSRTPStream* item=NULL;
	
	iStreamIter.SetToFirst();
	while ((item=iStreamIter++) != NULL)
			{
			//In the case of late binding state unprotect failed but SSRC 
    		// has set; we should set it again to the corrected one in the 
    		// case SSRC is different from previous one 											
			if ((item->StreamType()))
				{
				if ((item->SSRC()==0) || 
					(item->SSRC()!=0 && 
						static_cast<CSRTPStreamIn*> (item)->iCurrentRTPState== 
						MSRTPStreamInContext::ESRTPStreamInLateBinding) 
						)
					{
					SRTP_DEBUG_TUINT_VALUE( "FindLateBindingStreamAndUnprotectRTPL and ssrc is", aSSRC );
		  								
					item->SetSSRC(aSSRC);
					HBufC8* packet=	static_cast<CSRTPStreamIn*> 
									(item)->UnprotectSrtpL( aPacket );
					return packet;				
					}
	 			}
			}
	
	SRTP_DEBUG_TUINT_VALUE( "NOT Find any match SSRC in FindLBStrmUnprotectRTPL and ssrc is", aSSRC);
				 		
	//If not found stream ssrc =0it leaves
	User::Leave(KErrCorrupt);
	//keep away warnings
	SRTP_DEBUG_DETAIL( "CSRTPSession::FindLateBindingStreamAndUnprotectRTPL EXIT" );
  	
	return NULL;
	}

// ---------------------------------------------------------------------------
// CSRTPSession::FindLateBindingStreamAndUnprotectRTCP
// ---------------------------------------------------------------------------
//       

HBufC8* CSRTPSession::FindLateBindingStreamAndUnprotectRTCPL(TUint aSSRC, 
													const TDesC8& aPacket)
	{
	SRTP_DEBUG_DETAIL( "CSRTPSession::FindLateBindingStreamAndUnprotectRTCPL ENTRY" );
  	
	CSRTPStream* item=NULL;
	
	iStreamIter.SetToFirst();
	while ((item=iStreamIter++) != NULL)
			{
			//Only InStreams are handled for received RTCP Packets.
			if (item->StreamType())
				{
			//In the case of late binding state unprotect failed but SSRC 
    		// has set; we should set it again to the corrected one in the 
    		// case SSRC is different from previous one 											
		
			if ((item->SSRC()==0) || 
				(item->SSRC()!=0 && 
					static_cast<CSRTPStreamIn*> (item)->iCurrentRTCPState== 
					MSRTPStreamInContext::ESRTPStreamInLateBinding))
				{
					CSRTPStreamIn *pStrmIn = static_cast<CSRTPStreamIn*> (item);

					if(pStrmIn->SSRC()!=0 && pStrmIn->iCurrentRTPState == MSRTPStreamInContext::ESRTPStreamInNormal)
						{
						if(pStrmIn->SSRC() != aSSRC )
							{
							User::Leave(KErrCorrupt);
							}
						}
				item->SetSSRC(aSSRC);
				HBufC8* packet=	static_cast<CSRTPStreamIn*> 
								(item)->UnprotectSrtcpL( aPacket );
				return packet;				
				}
	 		SRTP_DEBUG_TUINT_VALUE( "FindLateBindingStreamAndUnprotectRTCPL and ssrc is", aSSRC);
				}
			}
	//If not found stream ssrc =0it leaves
	User::Leave(KErrCorrupt);
	//keep away warnings
	SRTP_DEBUG_DETAIL( "CSRTPSession::FindLateBindingStreamAndUnprotectRTCPL EXIT" );
  	
	return NULL;
	}

// ---------------------------------------------------------------------------
// CSRTPSession::AddStreamToList()
// ---------------------------------------------------------------------------
//
void CSRTPSession::AddStreamToList(CSRTPStream *aStream)
    {   
    iStreamList.AddLast(*aStream);
    ++iStreamCount;
    }

// ---------------------------------------------------------------------------
// CSRTPSession::RemoveStreamFromListL()
// ---------------------------------------------------------------------------
//
void CSRTPSession::RemoveStreamFromList(CSRTPStream *aStream)
    {   
    //when there are 2 same stream ...it could not remove
    TBool found = FindStream(aStream->SSRC(), aStream->StreamType() );
    if (found)
    	{
        iStreamList.Remove(*aStream);
	    --iStreamCount;
    	}
    }

// ---------------------------------------------------------------------------
// CSRTPSession::RemoveAllStream
// ---------------------------------------------------------------------------
//       

void CSRTPSession::RemoveAllStream( )
	{
	CSRTPStream* item=NULL;
	iStreamIter.SetToFirst();
	item=iStreamIter;
	while ((item=iStreamIter++) != NULL)
		{
		delete item; item=NULL;
		}
	}
// ---------------------------------------------------------------------------
// void CSRTPSession::GetCryptoContext()
// 
// ---------------------------------------------------------------------------
//
CSRTPCryptoContext& CSRTPSession::GetCryptoContext()
    {
     return *iContext;
    }


// ---------------------------------------------------------------------------
// void CSRTPSession::ReKeyNeeded()
// 
// ---------------------------------------------------------------------------
//
void CSRTPSession::ReKeyNeeded(const CSRTPStream& aStream, TBool aIsStrmCrypto)
    {
    if (iObserver)
		{
		if (aIsStrmCrypto)
			{
			iObserver->SRTPMasterKeyStaleEvent(aStream);
			}
		else
			{
			iObserver->SRTPMasterKeyStaleEvent(*this);
			}	
		}
	}
