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
* Description:    Represents receiver (incoming) SRTP stream.
*
*/




#ifndef __SRTP_STREAM_IN_H__
#define __SRTP_STREAM_IN_H__

// INCLUDES
#include <e32def.h>
#include "srtpstream.h"
#include "msrtpstreamincontext.h"

// FORWARD DECLARATIONS
class CSRTPSession;
class CSRTPCryptoContext;
class MSRTPReKeyingObserver;
class TSRTPStreamInStateBase;

/**
* Represents an SRTP stream.
*/
class CSRTPStreamIn : public CSRTPStream, 
					  public MSRTPStreamInContext
    {
    public:
        /**
        * Two-phased constructor.
        * Use this function if the stream will use the SRTP session's default
        * cryptographic context.
        * @param aSession The session containing the cryptographic
        *   context for this object.
        * @param aSSRC The synchronization source ID of the RTP stream
        *   that corresponds to this object.
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamIn* NewL( CSRTPSession& aSession,
                                      TUint aSSRC);

        /**
        * Two-phased constructor.
        * Use this function if the stream will have its own cryptographic
        * context.
        * @param aSession The session containing the cryptographic
        *   context for this object.
        * @param aSSRC The synchronization source ID of the RTP stream
        *   that corresponds to this object.
        * @param aCon The cryptographic context, ownership is transfered.
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamIn* NewL( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs );	

        /**
        * Two-phased constructor.
        * Use this function if the stream will use the SRTP session's default
        * cryptographic context and "late binding" is to be used, that is, 
        * no SSRC is provided.
        * @param aSession The session containing the cryptographic
        *   context for this object.
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamIn* NewL( CSRTPSession& aSession);

        /**
        * Two-phased constructor.
        * Use this function if the stream will have its own cryptographic
        * context and "late binding" is to be used, that is, 
        * no SSRC is provided.
        * @param aSession The session containing the cryptographic
        *   context for this object.
        * @param aCon The cryptographic context, ownership is transfered. 
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamIn* NewL( CSRTPSession& aSession,
                                      CSRTPCryptoContext* aCon,
                                      MSRTPReKeyingObserver& aObs );	
        /**
        * Two-phased constructor.
        * Use this function if the stream will have its own cryptographic
        * context and this cryptographic context will be set later.
        * Note that if the crypto context is not been set, the encoded packets
        * will not be handled.
        * @param aSession The session which includes the streams.
        * @param aSSRC The synchronization source ID of the RTP stream
        *   that corresponds to this object.
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamIn* NewL( CSRTPSession& aSession,
                                      TUint aSSRC,
                                      MSRTPReKeyingObserver& aObs );
        
        /**
        * Two-phased constructor.
        * Use this function if the stream will have its own cryptographic
        * context and "late binding" is to be used, that is, 
        * no SSRC is provided.Stream own cryptocontext will be set later.
        * Note that if the crypto context is not been set, the encoded packets
        * will not be handled.
        * @param aSession The session which includes the streams.
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamIn* NewL( CSRTPSession& aSession,
                                      MSRTPReKeyingObserver& aObs );                              	                              
    
        /**
        * Unprotect an SRTP packet.
        * The settings for the protection are fetched from the appropriate
        * cryptographic context. If the operation fails, the function leaves
        * with a system error code.
        * @param aPacket   The SRTP packet to process.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return An RTP packet.
        */
        IMPORT_C HBufC8* UnprotectSrtpL( const TDesC8& aPacket );
        
        /**
        * Unprotect an SRTCP packet.
        * The settings for the protection are fetched from the appropriate
        * cryptographic context. If the operation fails, the function leaves
        * with a system error code.
        * @param aPacket   The SRTCP packet to process.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return An RTCP packet.
        */
        IMPORT_C HBufC8* UnprotectSrtcpL( const TDesC8& aPacket );
        
        /**
        * Set/Update CryptoContext for InStream
        * Setting cryptographic context must be done to enable encode/decode packets.
        * During offer/Answer, offerer do not know which crypto context is 
        * chosen by answerer. CryptoContext then can be set after getting response
        * from answerer. If crypto context has not been set, no packets in secure 
        * session will be handled. 
        * Updating stream specific crypto Context 
        * Note that to update crypto context, ROC will not be re-intialed to zero but 
        * the states and number of receiving packets will be reset. This can be 
        * use only when keys lifetime has not expired.
        * Crypto context ownershíp is transfered.
        * @param aCon aCryyptoContext which wants be used, ownership is transfered.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        IMPORT_C void SetCryptoInL(CSRTPCryptoContext* aCon);
        
        /**
        * Destructor
        * Note that crypto context will be deleted if stream has it's own cryptocontext
        */        
        ~CSRTPStreamIn( );   
        
        /**
        * Delete the existed crypto context and re-initial the states
        * @param None
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
		void UpdateCryptoAndStatesL();
	

    private:  //methods
        CSRTPStreamIn (CSRTPSession& aSession,
                       TUint aSSRC);

        CSRTPStreamIn (CSRTPSession& aSession,
                       TUint aSSRC,
                       CSRTPCryptoContext* aCon,
                       MSRTPReKeyingObserver& aObs );
                       
        CSRTPStreamIn (CSRTPSession& aSession,
                       TUint aSSRC,
                       MSRTPReKeyingObserver& aObs );               
    
        CSRTPStreamIn (CSRTPSession& aSession);

        CSRTPStreamIn (CSRTPSession& aSession,
                       CSRTPCryptoContext* aCon,
                       MSRTPReKeyingObserver& aObs );
        CSRTPStreamIn (CSRTPSession& aSession,
                       MSRTPReKeyingObserver& aObs );               

        void ConstructL();
        
        /**
        * Create states defined in TSRTPStreamInStateBase
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void.
        */
        void InitializeStatesL();

    private:  
        TSRTPStreamInStateBase& CurrentRTPState();

        TSRTPStreamInStateBase& CurrentRTCPState();
        
    private:  //from MSRTPStreamInContext
        
        /*
        * Change state of the current incoming stream (for RTP)
        * @return void.
        */
        void ChangeRTPState(MSRTPStreamInContext::TContextInState aNewState);

        /*
        * Change state of the current incoming stream (for RTCP)
        * @return void.
        */
        void ChangeRTCPState(MSRTPStreamInContext::TContextInState aNewState);
                        
    protected:
        /*
        states for CSRTPStreamIn
        */
	    CArrayVarFlat<TSRTPStreamInStateBase> iStates;
        /*
        the same stream can handle RTP and RTCP packets,
        these can have different states
        */        
	    MSRTPStreamInContext::TContextInState iCurrentRTPState;
	    MSRTPStreamInContext::TContextInState iCurrentRTCPState;
	
	private:
		friend class CSRTPSession;	
	#ifdef EUNIT_TESTING
        friend class UT_CSRTPStreamIn; 
        friend class UT_CSRTPSession;       
    #endif     
    };

#endif // __SRTP_STREAM_IN_H__

