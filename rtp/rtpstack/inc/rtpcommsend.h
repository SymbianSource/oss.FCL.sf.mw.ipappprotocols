/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef RtpCommSend_H
#define RtpCommSend_H

#include <es_sock.h>
#include <rtpdef.h>

#include "rtpsenditem.h"
#include "rtpcommdef.h"
#include "rtputil.h"

/**
*  Class definition for sending
*
*  @lib RtpService.dll
*/
class CRtpCommSend : public CActive
    {
    #if defined( _DEBUG ) && defined( RTP_UNIT_TEST )
    // Friend class used for testing
        friend class CRtpUtRtpCommSend;
    #endif 
    public:
        static CRtpCommSend* NewL( RSocket& aSocket, MRtpErrNotify& aErrNotify, 
                                   /*TInt aBufSize,*/ TInetAddr& aAddr ); 

        ~CRtpCommSend();

        TInt Send( const TDesC8& aPacket );
        void Send( const TDesC8& aPacket, TRequestStatus& aStatus );
        void CancelAsyncSend();
        void SetToAddress(TInetAddr& aAddr);

    protected: // from CActive

        /**
        * Cancel any outstanding operation
        */
        void DoCancel();

        /**
        * Called when operation complete
        */
        void RunL();

    private:
        CRtpCommSend( RSocket& aSocket, MRtpErrNotify& aErrNotify, TInetAddr& aAddr );
        void ConstructL( /*TInt aBufSize*/ );

        /**
        * Handle a 'write buffer empty' situation.
        */
        void SendNextPacket();
        
        /**
        * Remove Item From Queue
        */
        void RemoveFromQueue();
        
        /**
         * Complete and remove queue items.
         * @since S60 3.2.3
         * @param aError completion code
         */
        void CompleteAndRemoveRequests( TInt aError );
        
        /**
         * Complete client's request.
         * @since S60 3.2.3
         * @param aItem send item
         * @param aReason completion code
         */
        void CompleteClientRequest( CRtpSendItem& aItem, TInt aReason ) const;

    private: // Member data
        
        enum TWriteState 
            {
            ESending, EWaiting, ECommsFailed
            };

   
        RSocket&        iSocket;
        TInetAddr       iToAddr;
        TWriteState     iWriteStatus;
        TPtrC8          iBufPtr;  // As a data memeber to hold the packet point for
                                  // Asyn sending in case the sending packet is out of scope.
        MRtpErrNotify&  iRtpErrNotify; 
	    TSglQue <CRtpSendItem> iSendQue;
        /** List iterator - needed to delete all the items */
		TSglQueIter<CRtpSendItem> iQueueIter;
		#ifdef EUNIT_TESTING
	  	friend class UT_CRtpAPI;
	  	friend class UT_CRtpSession;
	  	friend class UT_CRtpSessionSrtp;
	  	friend class UT_CRtpComm;
		#endif     	    
           };

#endif	// RtpCommSend_H
