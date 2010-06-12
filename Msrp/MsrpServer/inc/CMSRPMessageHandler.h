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
#include "CMSRPMessageChunkState.h"

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
const TInt KMaxBufferSize = 2500;
const TInt KSmallBuffer = 1024;
const TInt KMaxChunkReadSize =  65000;
// Class declaration

/* MSRP Message handler
 * 
 */

class CMSRPMessageHandler : public CBase, public MMSRPMessageHandler, 
                            public MMSRPIncomingMessage, public MMSRPOutgoingMessage, 
                            public MMSRPWriterObserver
    {
public:
    
    enum TMSRPMessageState
        {
        EIdle,
        EInProgress,
        EChunkSent,
        EMessageSent,
        EWaitingForResponse,
        EWaitingForReport,        
        EMessageDone
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
    
public: // From base classes
    
    // From MMSRPWriterObserver
	TWriteStatus GetSendBufferL( TPtrC8& aData, TBool aInterrupt ); 
    
    MMSRPWriterObserver::TMsgStatus WriteDoneL( TInt aStatus );
    
    // From MMSRPMessageHandler
    void AddHeaderL( TMsrpHeaderType aHeaderType, TPtrC8& aHeaderValue, 
            TPtrC8& aHeaderTypeAndValue );
    
    void AddContentL( TPtrC8& aContent );
    
    void SetTransactionId( TPtrC8& aTransactionId );
    
    void SetStatusOfResponseL( TPtrC8& aStatusCode, TPtrC8& aStatusMessage );
    
    void EndOfMessageL( TMsrpMsgEndStatus aStatus );
    
    // From MMSRPIncomingMessage
    TMSRPMessageType MessageType( );
       
    CMSRPMessage* GetIncomingMessage( );
    
    TBool SendResponseL( MMSRPMessageObserver* aMessageObserver,
								MMSRPConnection& aConnection, TUint aResponseCode );
    
    TDesC8& TransactionId( );
        
    CMSRPResponse* GetIncomingResponse( );
        
    // From MMSRPOutgoingMessage
    void SendMessageL( MMSRPConnection& aConnection );
        
    TBool IsOwnerOfResponse( MMSRPIncomingMessage& aIncomingMessage );
    
    TBool ConsumeResponseL( MMSRPIncomingMessage& aIncomingMessage );

    TBool IsMessageComplete();
    
    TBool GetSendResultL( TUint &aErrorCode, HBufC8* &aMessageId );
    
    void SendFileL( MMSRPConnection& aConnection );
        
    void ConsumeFileResponseL( MMSRPIncomingMessage& aIncomingMessage );
       


    TBool IsInFile();
       
    void ReceiveFileL( );
       
    TBool FileTransferComplete( );
       
    void WritetoFileL(CMSRPMessageHandler *aIncomingMsgHandler );
       
    void UpdateResponseStateL(CMSRPMessageHandler *incomingMsgHandler);
       
    TBool IfResponseReqL();
private:
    // Constructor
    CMSRPMessageHandler( MMSRPMessageObserver* aMessageObserver );
    
    CMSRPMessageHandler( );
    
    void ConstructL( const TDesC8& aMessage );
    
    void ConstructL( TMSRPMessageType aMessageType );
    
    // Member Functions
    void CreateByteRangeHeaderL();
    
    void CreateTransactionId();
    
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
         
    void SendFileChunkL( MMSRPConnection& aConnection );
       
        

    void SendFileStateL( TInt aBytes );
        
    void ReceiveFileStateL( TInt aBytes );
        
    void WriteChunkToFileL(const TDesC8& aFileChunk ,TDesC8& aTransactionId);  
private:
    
    // Server interface
    MMSRPMessageObserver* iMSRPMessageObserver;
    
    // Transaction Id
    TBuf8< KMaxLengthOfTransactionIdString > iTransactionId;
            
    // MSRP Message
    CMSRPMessage* iMessage;

    // MSRP response;
    CMSRPResponse* iResponse;
    
    TBool iResponseNeeded;
    
    RBuf8 iBuffer;
    
    TPtrC8 iContentPtr;
    
    TBool iWriteDone;    
    
    TMSRPMessageType iActiveMsgType;
    
    TMSRPMessageState iState;
    
    TMsrpMsgEndStatus iMsgEndStatus;        
    
    /* For File Use Case */
    TBool isFile; 
    RFile iFile;
    RFs iFs;
    TInt iFileSize;
    
    TBool iInterrupt;
    HBufC8* iFileBuffer;
    TBool iFileTransferComplete;
    RPointerArray <CMSRPMessageChunkState> iChunkList;       
    TInt iBufPosInFile; //file buffer start pos in file
    TInt iEndPosInBuffer;    
    TInt iStartPosInBuffer;
    TInt iFileBytesSent;

    TBool iProgress;
    TInt iNotifiedBytes;
    TInt iPendingBytes;
    };

#endif /* CMSRPMESSAGEHANDLER_H_ */
