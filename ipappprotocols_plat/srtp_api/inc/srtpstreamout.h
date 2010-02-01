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
* Description:    Represents a sender(outgoing) SRTP stream.
*
*/




#ifndef __SRTP_STREAM_OUT_H__
#define __SRTP_STREAM_OUT_H__

// INCLUDES
#include <e32def.h>
#include "srtpstream.h"
#include "msrtpstreamoutcontext.h"


// FORWARD DECLARATIONS
class CSRTPSession;
class CSRTPCryptoContext;
class MSRTPReKeyingObserver;
class TSRTPStreamOutStateBase;

/**
* Represents an SRTP stream.
*/
class CSRTPStreamOut : public CSRTPStream, 
					   public MSRTPStreamOutContext
    {
    public:
        /**
        * Two-phased constructor.
        * Use this function if the stream will use the SRTP session's default
        * cryptographic context.
        * @param aSession  The session containing the cryptographic
        *   context for this object.
        * @param aSSRC  The synchronization source ID of the RTP stream
        *   that corresponds to this object.
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamOut* NewL( CSRTPSession& aSession, TUint aSSRC );

        /**
        * Two-phased constructor.
        * Use this function if the stream will have its own cryptographic
        * context.
        * @param aSession  The session containing the cryptographic
        *   context for this object.
        * @param aSSRC  The synchronization source ID of the RTP stream
        *   that corresponds to this object.
        * @param aCon  The cryptographic context, ownership is transfered. 
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamOut* NewL( CSRTPSession& aSession,
                                       TUint aSSRC,
                                       CSRTPCryptoContext* aCon,
                                       MSRTPReKeyingObserver& aObs );
        
        /**
        * Two-phased constructor.
        * Use this function if the stream will have its own cryptographic
        * context. Note that if crypto context is not set later, all data
        * packet will not be processed.
        * @param aSession  The session containing the cryptographic
        *   context for this object.
        * @param aSSRC  The synchronization source ID of the RTP stream
        *   that corresponds to this object.
        * @param aCon  The cryptographic context 
        * @param aObs  The callback object
        * @leave KErrNone if success, system-wide error code otherwise
        */
    	IMPORT_C static CSRTPStreamOut* NewL( CSRTPSession& aSession,
                                       TUint aSSRC,
                                       MSRTPReKeyingObserver& aObs );                                       
	
    public:
        /**
        * Protect an RTP packet.
        * The settings for the protection are fetched from the appropriate
        * cryptographic context. If the protection fails, the function leaves
        * with a system error code and the provided RTP packet is unaltered.
        * @param aPacket  The RTP packet to protect. The length of descriptor
        *  should be the same with the actual data, otherwise encoding will not be 
        *  correct.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return An SRTP packet.
        */
        IMPORT_C HBufC8* ProtectRtpL( const TDesC8& aPacket );

        /**
        * Protect an RTCP packet.
        * The settings for the protection are fetched from the appropriate
        * cryptographic context. If the protection fails, the function leaves
        * with a system error code and the provided RTCP packet is unaltered.
        * @param aPacket  The RTCP packet to protect.
        * @leave KErrNone if success, system-wide error code otherwise
        * @return An SRTCP packet.
        */
        IMPORT_C HBufC8* ProtectRtcpL( const TDesC8& aPacket );

        /**
        * Set CryptoContext for OutStream
        * Setting cryptographic context must be done to enable encode/decode packets.
        * During offer/Answer, offerer do not know which crypto context is 
        * chosen by answerer. CryptoContext then can be set after getting response
        * from answerer.
        * Updating stream specific crypto Context. 
        * Note that to update crypto context, ROC will not be re-intialed to zero but 
        * the states and number of receiving packets will be reset. This can be 
        * use only when keys lifetime has not expired.
        * Crypto context ownershíp is transfered.
        * @param aCon aCryyptoContext which wants be used
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
        IMPORT_C void SetCryptoOutL(CSRTPCryptoContext* aCon);
        
         ~CSRTPStreamOut( );    
         
       	/**
        * Delete the existed crypto context and re-initial the states
        * @param None
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void
        */        
		void UpdateCryptoAndStatesL();
		   
    private:  //from MSRTPStreamOutContext

        /*
        * Change state of the current outgoing stream (for RTP)
        * @return void.
        */
        void ChangeRTPState(MSRTPStreamOutContext::TContextOutState aNewState);

        /*
        * Change state of the current outgoing stream (for RTCP)
        * @return void.
        */
        void ChangeRTCPState(MSRTPStreamOutContext::TContextOutState aNewState/*TState aNewState*/);        

    private:  //methods
        CSRTPStreamOut (CSRTPSession& aSession,
                       TUint aSSRC);

        CSRTPStreamOut (CSRTPSession& aSession,
                       TUint aSSRC,
                       CSRTPCryptoContext* aCon,
                       MSRTPReKeyingObserver& aObs );
                       
        CSRTPStreamOut (CSRTPSession& aSession,
                       TUint aSSRC,
                       MSRTPReKeyingObserver& aObs );
               

        void ConstructL();
        
        /**
        * Create states defined in TSRTPStreamOutStateBase
        * @leave KErrNone if success, system-wide error code otherwise
        * @return void.
        */
        void InitializeStatesL();

        TSRTPStreamOutStateBase& CurrentRTPState();

        TSRTPStreamOutStateBase& CurrentRTCPState();
                       
    protected:
        /*
        states for CSRTPStreamOut
        */
	    CArrayVarFlat<TSRTPStreamOutStateBase> iStates;
        /*
        the same stream can handle RTP and RTCP packets,
        these can have different states
        */
	    MSRTPStreamOutContext::TContextOutState iCurrentRTPState;
	    MSRTPStreamOutContext::TContextOutState iCurrentRTCPState;   
	
	private:    
		//friend class CSRTPSession;    
	#ifdef EUNIT_TESTING
        friend class UT_CSRTPStreamOut;        
    #endif                                  
    };

#endif // __SRTP_STREAM_OUT_H__
