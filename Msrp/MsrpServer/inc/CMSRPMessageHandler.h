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

#ifndef CMSRPMESSAGEHANDLER_H_
#define CMSRPMESSAGEHANDLER_H_

// INCLUDES
#include <e32base.h>
#include <f32file.h> 

#include "MSRPCommon.h"
#include "MMSRPMessageHandler.h"
#include "MMSRPIncomingMessage.h"
#include "MMSRPOutgoingMessage.h"
#include "MMSRPWriterObserver.h"

// Forward declarations
class MMSRPMessageHandler;
class MMSRPIncomingMessage;
class MMSRPOutgoingMessage;
class MMSRPWriterObserver;
class MMSRPMessageObserver;
class CMSRPMessage;
class CMSRPResponse;
class CMSRPReport;
class CMSRPMessageBase;
class RStringF;

// Constants
//const TInt KMaxBufferSize = 2500;
const TInt KMaxBufferSize = 10000;
const TInt KSmallBuffer = 1024;
const TInt KMaxChunkReadSize =  65000;
// Class declaration

/*
 * MSRP message handler class. Each MSRP message creates an instance of this class
 * THis class handles both incoming and outgoing messages
 */
class CMSRPMessageHandler : public CBase, public MMSRPMessageHandler, 
                            public MMSRPIncomingMessage, public MMSRPOutgoingMessage, 
                            public MMSRPWriterObserver
    {
    public:
        
        enum TMSRPMessageState
            {
            EIdle,                // 0
            EInProgress,          // 1
            EChunkSent,           // 2
            EMessageSent,         // 3
            ETerminated ,         // 4
            EWaitingForResponse,  // 5
            EWaitingForReport,    // 6
            EMessageDone          // 7
            };
            
        /* Two-Phased constructor
         * 
         */
        static CMSRPMessageHandler* NewL( MMSRPMessageObserver* aMessageObserver, 
                const TDesC8& aMessage );
        
        static CMSRPMessageHandler* NewLC( MMSRPMessageObserver* aMessageObserver, 
                const TDesC8& aMessage );
        
        static CMSRPMessageHandler* NewL( TMSRPMessageType aMessageType );
        
        static CMSRPMessageHandler* NewLC( TMSRPMessageType aMessageType );
        
        /**
         * Destructor.
         */
        ~CMSRPMessageHandler();
    
        static TInt LinkOffset();		
        TSglQueLink iLink;
        
        /**
         * Get message id of the current message
         * @return message id of the message, ownership is transferred
         */
        HBufC8* MessageIdLC( ); 
        
        /**
         * Get currently received bytes in this message handler
         * @param aBytesTransferred number of transferred bytes
         * @param aTotalBytes total size of data
         */
        void CurrentReceiveProgress( TInt& aBytesTransferred, TInt& aTotalBytes ); 
    
        /**
         * Get currently transferred bytes in this message handler
         * @param aBytesTransferred number of transferred bytes
         * @param aTotalBytes total size of data
         */
        void CurrentSendProgress( TInt& aBytesTransferred, TInt& aTotalBytes ); 
    
        /**
         * Sets the message observer entoty
         * @param aMessageObserver MMSRPMessageObserver
         */
        void SetMessageObserver( MMSRPMessageObserver* aMessageObserver ); 
    
        /**
         * Checks whether we are sending or receiving a message with this handler
         * @return true if we are sending a message
         */
        TBool IsSending( ); 
    
        /**
         * returns the temporary file name
         * @return temporary filename which contains incoming data
         */
        TFileName& TemporaryFileName( ); 
    
        /**
         * Set the temporary file name. Used when message is divided to multiple chunks
         * @param afilename temporary File name
         */
        void SetTemporaryFileName( TFileName aFileName ); 
    
        /**
         * Response is no longer needed, it can be deleted
         */
        void ResponseHandled( ); 

        /**
         * Terminates the receiving of message
         * @param aConnection connection instance
         * @param aMessageObserver observer for completed send response
         */
        void TerminateReceiving( MMSRPMessageObserver* aMessageObserver, 
                MMSRPConnection& aConnection );

        /**
         * Terminates the sending of message
         */
        void TerminateSending( );

        /**
         * Returns true if message sending or receiving is terminated
         * @return true if FS is terminated
         */
        TBool IsTransmissionTerminated( );

        /**
         * Returns true if REPORT response was requested
         * @return true if REPORT must be sent
         */
        TBool IsReportNeeded( );

    public: // From base classes
        
        // From MMSRPWriterObserver
        const TDesC8& GetSendBufferL( TWriteStatus& aStatus, TBool aInterrupt ); 
        
        MMSRPWriterObserver::TMsgStatus WriteDoneL( TInt aStatus );
        
        // From MMSRPMessageHandler
        void AddHeaderL( TMsrpHeaderType aHeaderType, TPtrC8& aHeaderValue, 
                TPtrC8& aHeaderTypeAndValue );
        
        void AddContentL( TPtrC8& aContent, TBool aByteRangeFound = EFalse );
        
        void SetTransactionId( TDesC8& aTransactionId );
        
        void SetStatusOfResponseL( TPtrC8& aStatusCode, TPtrC8& aStatusMessage );
        
        void EndOfMessageL( TMsrpMsgEndStatus aStatus );
        TBool CheckMessageChunkL( CMSRPMessageHandler& aOtherMessage );
        
    public: // From MMSRPIncomingMessage
        TMSRPMessageType MessageType( );
        CMSRPMessage* GetIncomingMessage( ) const;
        void SendResponseL( MMSRPMessageObserver* aMessageObserver,
                                    MMSRPConnection& aConnection, TUint aResponseCode );
        TBool SendReportL( MMSRPMessageObserver* aMessageObserver, 
                                        MMSRPConnection& aConnection, TUint aStatusCode );
        TDesC8& TransactionId( );
        CMSRPResponse* GetIncomingResponse( ) const;
        CMSRPReport* GetIncomingReport( ) const;
            
    public: // From MMSRPOutgoingMessage
    
        void SendMessageL( MMSRPConnection& aConnection );
        TBool IsOwnerOfResponse( MMSRPIncomingMessage& aIncomingMessage );
        TUint ConsumeResponseL( MMSRPIncomingMessage& aIncomingMessage );
        TBool IsMessageComplete();
        TUint GetSendResultL( HBufC8* &aMessageId );
        TBool IsFailureHeaderPartial(); 
        
    private:
        // Constructor
        CMSRPMessageHandler( MMSRPMessageObserver* aMessageObserver );
        
        CMSRPMessageHandler( );
        
        void ConstructL( const TDesC8& aMessage );
        
        void ConstructL( TMSRPMessageType aMessageType );
        
        // Member Functions
        void CreateByteRangeHeaderL();
        
        void CreateTransactionIdL();
        
        void WriteInitialLineToBufferL();
        
        void WriteHeadersToBufferL();
        
        void WriteContentToBuffer();
        
        void WriteFileContentToBufferL();
        
        void WriteEndLineToBuffer(TMsrpMsgEndStatus = EMessageEnd );
        
        TInt ExtractByteRangeParams( TDesC8& aHeaderValue, TInt& aStartPos, 
                TInt& aEndPos, TInt& aTotal );
        
        RStringF GetStatusStringL( TUint aStatusCode );
        
        TUint CheckValidityOfMessage( TMSRPMessageType aMessageType, 
                CMSRPMessageBase* aMessage );
        
        TInt FillFileBufferL();
             
        /*
        * Opens temporary file. Name of the file is save to iTempFileName
        * and Rfile is in iTempFile
        */
        void CreateTemporaryFileL( );
    
        /*
        * Opens temporary file. Name of the file is save to iTempFileName
        * and Rfile is in iTempFile
        * @param aFilename filename to open
        */
        void OpenTemporaryFileL( const TDesC& aFilename );
    
        /*
        * Writes the given buffer to a file
        * @param aBuffer buffer to be written into the file
        */
        void WriteMessageToFileL( TDesC8& aBuffer );
    
        /*
        * The message continues and a new segment of the message is being
        * written to the end of file. 
        * @param aBuffer buffer to be written into the file
        */
        void AppendMessageToFileL( TDesC8& aBuffer );
        
    private:
        
        // Server interface
        MMSRPMessageObserver* iMSRPMessageObserver;
        
        // array of sent chunks, transaction id is saved from
        // sent chunks
        RPointerArray< HBufC8 > iSentChunks;
                
        // MSRP Report
        CMSRPReport* iReport;
    
        // MSRP response;
        CMSRPResponse* iResponse;

        // MSRP message
        CMSRPMessage* iMessage;

        TBool iResponseNeeded;
        
        RBuf8 iBuffer;
        
        TPtrC8 iContentPtr;
        
        TBool iWriteDone;    
        
        TMSRPMessageType iActiveMsgType;
        
        TMSRPMessageState iState;
        
        
        /* For File Use Case */
        RFile iFile;
        RFs iFs;
        TInt iFileSize;
        TBool isSending;
        
        TBool iInterrupt;
        HBufC8* iFileBuffer;
        TBool iFileTransferComplete;
        TInt iBufPosInFile; //file buffer start pos in file
        TInt iEndPosInBuffer;    
        TInt iStartPosInBuffer;
        TInt iFileBytesSent;    
        TBool iTerminateReceiving;
        TBool iTerminateSending;
    
        TInt iNotifiedBytes;
        TInt iPendingBytes;
        
        /// temporary file name which is used
        TFileName iTempFileName;
    
        /// file for incoming data, owned
        RFile* iTempFile;
        
        // number of bytes currently transmitted or received
        TInt iCurrentNumberOfBytes;
        
        // current message chunk status
        TMsrpMsgEndStatus iMessageEnding;
    };

#endif /* CMSRPMESSAGEHANDLER_H_ */
