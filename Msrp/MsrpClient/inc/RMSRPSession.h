/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef RMSRPSESSION_H_
#define RMSRPSESSION_H_


//  Include Files
#include <e32base.h>
#include <e32std.h>

// INCLUDES
#include "MMSRPServerInterface.h"


// Forward Declarations
class RMSRP;


// Class declaration

class RMSRPSession : public RSubSessionBase, public MMSRPServerInterface
    {
    public:
        /**
          * constructor.
          */
        RMSRPSession();
        
    public: // New functions
        /**
          * Opens a Sub-Session to MSRP Server
          * @param aRMSRP reference to main server session
          * @param aIapId internet accesss point identifier
          * @param aSession session id from invite
          * @return Status of the operation
          */
        TInt CreateServerSubSession( RMSRP& aRMSRP, 
                                     const TUint32 aIapId,
                                     const TDesC8& aSessionId );
        
        void CloseServerSubSession( );
        
        /**
          * Send a request to server whether to receive 
          * or not to receive progress reports
          * @param aProgress True if user wants to receive progress
          *                  reports
          * @return system-wide error code
          */
        TInt SetProgressReports( TBool aProgress );        
        
    public: // From MMSRPServerInterface
        
        void GetLocalPathL( TDes8 &aLocalHost );
        void Connect( const TDesC8& aRemoteHost,
                      const TUint aRemotePort,
                      const TDesC8& aRemoteSessionID,
                      TRequestStatus& aStatus );
        void ListenConnections( const TDesC8& aRemoteHost,
                                const TUint aRemotePort,
                                const TDesC8& aRemoteSessionID,
                                TBool aIsMessage,
                                TRequestStatus& aStatus );
        void ListenMessages( TBool aIsMessage,
                             TRequestStatus& aStatus );
        void ListenSendResult( const TDesC8& aSessionID,
                               TRequestStatus& aStatus );
        TInt SendMessage( TDesC8& aMessageBuffer );
        TInt CancelSending( const TDesC8& aMessageId );
        TInt CancelReceiving( const TDesC8& aMessageId );
        void CancelSendResultListening( );
        const TDesC8& GetMessageIdOfSentMessage( ) const;
        TInt GetStatusOfSentMessage( ) const;
        TInt GetStatusOfReceivedMessage( ) const;
        const TDesC8& GetReceivedMessage( ) const;
        TBool GetListenProgress() const;
        TInt GetBytesReceived() const;
        TInt GetTotalBytesReceived() const;
        TBool GetSendProgress() const;
        TInt GetBytesSent() const;
        TInt GetTotalBytes() const;
        TDesC8& ListenMessageId();
        TDesC8& SendMessageId();
    
    private: //variables
        
        // Local path data to be recived from the server
        TPckgBuf< TLocalPathMSRPData > iLocalPathMSRPDataPckg;
            
        // Connection data to be sent to server
        TPckgBuf< TConnectMSRPData > iConnectMSRPDataPckg;
        
        // Message sending data to be sent to server
        TPckgBuf< TSendMSRPData > iSendMSRPDataPckg;
    
        // Listen for incoming messages data package
        TPckgBuf< TListenMSRPData > iListenMSRPDataPckg;
    
        // Listen for response to sent messages data package
        TPckgBuf< TListenSendResultMSRPData > iSendResultListenMSRPDataPckg;
    };

#endif /* RMSRPSESSION_H_ */
