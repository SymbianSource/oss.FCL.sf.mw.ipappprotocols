/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    
*
*/




#ifndef __RTPMANAGER_H
#define __RTPMANAGER_H

#include <commdbconnpref.h>
#include <es_enum.h>
#include <rtpdef.h>
#include <rtpheader.h>
#include "rtpsession.h"
#include "rtpsdes.h"
#include "localaddrresolver.h"
#include "rtpmanagerdef.h"
#include "rtpasignuniqueid.h"
#include "srtpsession.h"
#include "rtpsessionsrtp.h"

/**
*  Container of all rtp sessions. 
*
*  @lib RtpService.dll
*/
class CRtpManager : public CBase,
    				public MRtpAsignUniqueID
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
    // Friend class used for testing
        friend class CRtpUtRtpManager;
    #endif
    public: // Data types
        enum TCreateStreamType
            {
            ECreateRecvStream = 0,
            ECreateTranStream,
            ECreateTranStreamExt
            };

    public:
        static CRtpManager* NewL( MRtpErrNotify& aErrNotify );
        ~CRtpManager();

        TInt OpenL( const TRtpSdesParams& aSdesInfo, 
                    const TDesC* aRtpPacketDll, 
                    RSocketServ* aSocketServPtr,
                    RConnection* aRConnPtr );

        /**
        * Open and initialize the CRtpAPI object synchronously.
        * @param TInt aIapId - [input] IAP ID. If -1, no IAP selection dialog
        *   will pop up. Instead, the default IAP will be used.
        * @return TInt - KErrNone if successful; system wide error code
        *   otherwise
        */
        TInt StartConnection( TInt aIapId );

        /**
        * Open and initialize the CRtpAPI object in asynchronous mode
        * @param TRequestStatus& aStatus - [output] On completion, will contain
        *   a status code: KErrNone if successful; system wide error code
        *   otherwise.
        * @param TInt aIapId - [input] IAP ID. If -1, no IAP selection dialog
        *   will pop up. Instead, the default IAP will be used.
        * @return TInt - KErrNone if successful; system wide error code
        *   otherwise
        */
        TInt StartConnection( TRequestStatus& aStatus, TInt aIapId );

        /**
        * Cancels asynchoronous start of connection
        * Any open requests will be completed with KErrCancel.
        * @return None
        */
        void CancelStart(); 

        void Close( void );

        void SetLocalSdes( const TRtpSdesParams& aSdesInfo );

        TInetAddr& GetLocalIPAddressL();

        TRtpId CreateSessionL( const TCreateSessionParams& aSessionParams,
                               TUint& aPort,
                               TBool aEnableRtcp,
                               const TRtcpParams* aRtcpParams );
                               
		TRtpId CreateSessionL( const TCreateSessionParams& aSessionParams,
                               TUint& aPort,
                               TBool aEnableRtcp,
                               const TRtcpParams* aRtcpParams,
                               CSRTPSession& aSRTPSession );
        TInt StartSession( TRtpId aSessionId );

        void CloseSession( TRtpId aSessionId );

        TInt SetRemoteAddress( TRtpId aSessionId, TInetAddr& aRemoteAddr );
        TInt SetRemoteRtcpAddress( TRtpId aSessionId, TInetAddr& aRemoteAddr );

        /**
        * Does the actual work of creating a stream. Used by the other
        * stream creation methods.
        * @return ID of the created stream, or KNullId if not successful
        */
        TRtpId CreateStreamL( TRtpId aSessionId,
                              const TCreateStreamType aStreamType,
                              TRtpPayloadType aPayloadType,
                              TRtpSSRC& aSSRC );

        TRtpId CreateReceiveStreamL( TRtpId aSessionId, const TRcvStreamParams& aParams );
        TRtpId CreateTransmitStreamL( TRtpId aSessionId, const TTranStreamParams& aParams, 
                                      TRtpSSRC& aSSRC );

        TRtpId CreateTransmitStreamExtL( TRtpId aSessionId, const TTranStreamParams& aParams, 
                                         const TRtpSSRC aSSRC );

        void CloseStream( TRtpId aStreamId );

        TInt RegisterRtpObserver( TRtpId aSessionId, MRtpObserver& aObserver );

        void UnregisterRtpObserver( TRtpId aSessionId );
        
        TInt RegisterRtpPostProcessingObserver( TRtpId aSessionId, MRtpPostProcessingObserver& aRtpObserver );

        void UnregisterRtpPostProcessingObserver( TRtpId aSessionId );

        TInt SetNonRTPDataObserver( TRtpId aSessionId, 
                                    MNonRTPDataObserver* aNonRTPDataObserver );
        
        TInt SendRtpPacket( TRtpId aTranStreamId, 
                            const TRtpSendHeader& aHeaderInfo, 
                            const TDesC8& aPayloadData,
                            const TArray<TRtpCSRC> *aCsrcList = NULL );

        TInt SendRtpPacket( TRtpId aTranStreamId,
                            const TRtpSendHeader& aHeaderInfo,
                            const TDesC8& aPayloadData,
                            TRequestStatus& aStatus,
                            const TArray<TRtpCSRC> *aCsrcList = NULL );

        TInt SendRtpPacket( TRtpId aTranStreamId,
                            TRtpSequence aSequenceNum,
                            const TRtpSendHeader& aHeaderInfo,
                            const TDesC8& aPayloadData,
                            TRequestStatus& aStatus,
                            const TArray<TRtpCSRC> *aCsrcList = NULL );

        void SendDataL( TRtpId aSessionId,
                       TBool aUseRTPSocket,
                       const TDesC8& aData,
                       TRequestStatus& aStatus );

        void CancelSend( TRtpId aSessionId );

        TInt RegisterRtcpObserver( TRtpId aSessionId, MRtcpObserver& aObserver );

        void UnregisterRtcpObserver( TRtpId aSessionId );

        TInt SendRtcpByePacketL( TRtpId aTranStreamId, const TDesC8& aReason );

        TInt SendRtcpAppPacketL( TRtpId aTranStreamId, const TRtcpApp& aApp );

        /**
        * Send an RTCP RR packet for a Reception stream.
        * @param TRtpId aRcvStreamId - [input] Reception stream ID
        * @return TInt - KErrNone if successful; system wide error code otherwise
        */
        TInt SendRtcpRrPacketL( TRtpId aRcvStreamId );
        
        /**
        * Send an RTCP SR packet for a Transmit stream.
        * @param TRtpId aTranStreamId - [input] Transmit stream ID
        * @return TInt - KErrNone if successful; system wide error code otherwise
        */
        TInt SendRtcpSrPacketL( TRtpId aTranStreamId );

        /**
        * Suspend RTCP sending on/off, calculations will continue. 
        * @param TRtpId aSessionId - [input] RTP Session ID
        * @param TBool aAutoSending - [input] Auto sending flag. 
        *							  ETrue:  RTCP sending will be scheduled normally
        *							  EFalse: RTCP sending will be suspended
        * @return TInt - KErrNone if successful; KErrNotFound if invalid session id;
        *				 KErrNotSupported if RTCP disabled;  system wide error code otherwise				
        */
        TInt SuspendRtcpSending( TRtpId aSessionId, TBool aAutoSending );
        
		/**
        * Gets the status of automatic RTCP sending.
        * @param TRtpId aSessionId - [input] RTP Session ID
        * @param TBool aAutoSending - [output] ETrue: RTCP atuo sending is on 
        *				  					   EFalse: RTCP auto sending is off
 		* @return TInt - KErrNone if successful; KErrNotFound if invalid session id;
        *				 KErrNotSupported if RTCP disabled;  system wide error code otherwise
        */
        TInt IsRtcpSendingSuspended( TRtpId aSessionId, TBool& aAutoSending );

        TRtpId GetSessionId( TRtpId aStreamId );

        RSocket* GetRtpSocket( TRtpId aSessionId );

        RSocket* GetRtcpSocket( TRtpId aSessionId );

        TInt GetStreamStatistics( TRtpId aStreamId, TRtpPeerStat& aStat );

        TUint32 GetSamplingRate( TUint8 aPayloadType );

        TInt SetSamplingRate( TUint8 aPayloadType, TUint32 aSampleRate );

        TInt SetRtcpParameters( TRtpId aSessionId, const TRtcpParams& aRtcpParams );

    private:
        CRtpManager( MRtpErrNotify& aErrNotify );
        void ConstructL( void );
        CRtpSession* GetSession( TRtpId aSessionId );

        TRtpId AssignUniqueID();
        TInt AddRtpObject( TArrayStore aArrayID );
        TInt FindRtpObject( const TRtpId aMagicKey, TUint& aSessionAddress );
        TInt RemoveRtpObject( const TRtpId aMagicKey, const TObjectType aObjectType );
        void RemoveRtpAllObjects();
    
        /*
        * Opens the socket server and the connection
        * @return KErrNone if successful, system error code otherwise
        */
        TInt PrepareConnection( TCommDbConnPref& aPrefs, TInt aIapId );

        /*
        * @return Internet Access Point ID of iConn
        */
        TInt GetIapId( TUint32& aIapId );
        
        TInt AddStreamToSession(TRtpId aSessionId, TRtpId aStreamId);

        void CheckSdesCName();
        
    private:   // data
        TBool iStandardRtp;
        TBool iEnableRtcp;
        RLibrary iLibrary;

        RSocketServ iSocketServ;
        RConnection iConn;
        RSocketServ* iSocketServPtr;  // Don't use until future
        RConnection* iConnPtr;    // Don't use until future
        TBool iConnected;
		TInt iIapId;

        TInetAddr iLocalAddr;

        CRtpSDES* iDefaultSdes;

        TRtpId iNumOfObjects;   // count of session and participants IDs assigned
        CArrayFixFlat<TArrayStore>* iSessionArray;   //keep track of all RTP session;

        TUint32 iProfileRTPTimeRates[KRtpMaxPayloadTypes];

        MRtpErrNotify& iErrNotify;

        TCommDbConnPref iPrefs;
    private:    
    #ifdef EUNIT_TESTING
	  friend class UT_CRtpAPI;
	  friend class UT_CRtpManager;
	#endif     	    
        
    };

#endif    // __RTPMANAGER_H

// End of File
