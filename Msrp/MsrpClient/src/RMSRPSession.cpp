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

#ifndef RMSRPSESSION_CPP_
#define RMSRPSESSION_CPP_


//  Include Files
#include "MsrpCommon.h"
#include "RMSRPSession.h"
#include "RMSRP.h"


//  Member Functions

RMSRPSession::RMSRPSession() :
    RSubSessionBase()
    {
    MSRPLOG("RMSRP Session.. Ctor");
    }


TInt RMSRPSession::CreateServerSubSession( RMSRP& aRMSRP, const TUint32 aIapId )
    {
    MSRPLOG("RMSRP Session.. Create client-server SubSession");
    
    return CreateSubSession( aRMSRP, EMSRPCreateSubSession, TIpcArgs( aIapId ) );
    }


void RMSRPSession::CloseServerSubSession( )
    {
    MSRPLOG("RMSRP Session.. Close client-server SubSession");
    CloseSubSession(EMSRPCloseSubSession);
    }


void RMSRPSession::GetLocalPathL( TDes8 &aLocalHost, TDes8 &aSessionID )
    {
    MSRPLOG("RMSRP Session.. GetLocalPath");
    
    User::LeaveIfError( SendReceive(EMSRPLocalPath, TIpcArgs( &iLocalPathMSRPDataPckg )) );
    
    aLocalHost = iLocalPathMSRPDataPckg().iLocalHost;
    aSessionID = iLocalPathMSRPDataPckg().iSessionID;    
    }


void RMSRPSession::Connect( const TDesC8& aRemoteHost,
                            const TUint aRemotePort,
                            const TDesC8& aRemoteSessionID,
                            TRequestStatus& aStatus )
    {
    MSRPLOG("RMSRP Session.. Connect");
    
    iConnectMSRPDataPckg().iRemoteHost = aRemoteHost;
    iConnectMSRPDataPckg().iRemotePort = aRemotePort;
    iConnectMSRPDataPckg().iRemoteSessionID = aRemoteSessionID;
    
    SendReceive( EMSRPConnect, TIpcArgs( &iConnectMSRPDataPckg ), aStatus );    
    }


void RMSRPSession::ListenConnections( const TDesC8& aRemoteHost,
                                      const TUint aRemotePort,
                                      const TDesC8& aRemoteSessionID,
                                      TBool aIsMessage,
                                      TRequestStatus& aStatus )
    {
    MSRPLOG("RMSRP Session.. Listen for incoming connections");
    
    iListenMSRPDataPckg().iRemoteHost = aRemoteHost;
    iListenMSRPDataPckg().iRemotePort = aRemotePort;
    iListenMSRPDataPckg().iRemoteSessionID = aRemoteSessionID;
    iListenMSRPDataPckg().iIsMessage = aIsMessage;
        
    SendReceive( EMSRPListenConnections, TIpcArgs( &iListenMSRPDataPckg ), aStatus );    
    }


void RMSRPSession::ListenMessages( TBool aIsMessage,
                                   TRequestStatus& aStatus )
    {
    MSRPLOG("RMSRP Session.. Listen for incoming messages");
    
    iListenMSRPDataPckg().iIsMessage = aIsMessage;
    
    SendReceive( EMSRPListenMessages, TIpcArgs( &iListenMSRPDataPckg ), aStatus );    
    }


void RMSRPSession::ListenSendResult( const TDesC8& aSessionID, TRequestStatus& aStatus )
    {
    MSRPLOG("RMSRP Session.. ListenSendResult");
    
    iSendResultListenMSRPDataPckg().iSessionID = aSessionID;
    
    SendReceive( EMSRPListenSendResult, TIpcArgs( &iSendResultListenMSRPDataPckg ), aStatus );    
    }


TInt RMSRPSession::SendMessage( TDesC8& aMessageBuffer )
    {
    MSRPLOG("RMSRP Session.. SendMessage");
    
    iSendMSRPDataPckg().iExtMessageBuffer = aMessageBuffer;

    return SendReceive( EMSRPSendMessage, TIpcArgs( &iSendMSRPDataPckg ) );
    }


TInt RMSRPSession::CancelSending( TDesC8& aMessageId )
    {
    MSRPLOG("RMSRP Session.. CancelSending");
    return SendReceive( EMSRPCancelSending, TIpcArgs( &aMessageId ) );
    }


void RMSRPSession::CancelReceiving( )
    {
    MSRPLOG("RMSRP Session.. CancelReceiving Entered");
    SendReceive( EMSRPCancelReceiving );
    MSRPLOG("RMSRP Session.. CancelReceiving Done ");
    }
    

void RMSRPSession::CancelSendResultListening( )
    {
    MSRPLOG("RMSRP Session.. CancelSendRespListening");
    SendReceive( EMSRPCancelSendRespListening );
    }


const TDesC8& RMSRPSession::GetMessageIdOfSentMessage() const
    {
    MSRPLOG("RMSRP Session.. GetMessageIdOfSentMessage");
    return iSendResultListenMSRPDataPckg().iMessageId;
    }
    

TInt RMSRPSession::GetStatusOfSentMessage() const
    {
    MSRPLOG("RMSRP Session.. GetStatusOfSentMessage");
    return iSendResultListenMSRPDataPckg().iStatus;
    }
    

TInt RMSRPSession::GetStatusOfReceivedMessage() const
    {
    MSRPLOG("RMSRP Session.. GetStatusOfReceivedMessage");
    return iListenMSRPDataPckg().iStatus;
    }


const TDesC8& RMSRPSession::GetReceivedMessage() const
    {
    MSRPLOG("RMSRP Session.. GetReceivedMessage");
    return iListenMSRPDataPckg().iExtMessageBuffer;
    }

TBool RMSRPSession::GetListenProgress() const
    {
    MSRPLOG("RMSRP Session.. GetListenProgress");
    return iListenMSRPDataPckg().iIsProgress;
    }

TInt RMSRPSession::GetBytesReceived() const
    {
    MSRPLOG("RMSRP Session.. GetBytesReceived");
    return iListenMSRPDataPckg().iBytesRecvd;
    }

TInt RMSRPSession::GetTotalBytesReceived() const
    {
    MSRPLOG("RMSRP Session.. GetTotalBytesReceived");
    return iListenMSRPDataPckg().iTotalBytes;
    }

TBool RMSRPSession::GetSendProgress() const
    {
    MSRPLOG("RMSRP Session.. GetSendProgress");
    return iSendResultListenMSRPDataPckg().iIsProgress;
    }

TInt RMSRPSession::GetBytesSent() const
    {
    MSRPLOG("RMSRP Session.. GetBytesSent");
    return iSendResultListenMSRPDataPckg().iBytesSent;
    }

TInt RMSRPSession::GetTotalBytes() const
    {
    MSRPLOG("RMSRP Session.. GetTotalBytes");
    return iSendResultListenMSRPDataPckg().iTotalBytes;
    }


TInt RMSRPSession::SendFileL(TDesC8& aFileParamBuffer)
    {
    /* Send File Params to the sub-session */
    MSRPLOG("RMSRP Session.. SendFileParams");
    iSendMSRPDataPckg().iExtMessageBuffer = aFileParamBuffer;
    return SendReceive( EMSRPSendFile, TIpcArgs( &iSendMSRPDataPckg ) );
    }

TInt RMSRPSession::ReceiveFileL(TDesC8& aFileParamBuffer)
    {
    /* Send File Params to the sub-session */
    MSRPLOG("RMSRP Session.. SendFileParams");
    iSendMSRPDataPckg().iExtMessageBuffer = aFileParamBuffer;
    return SendReceive( EMSRPReceiveFile, TIpcArgs( &iSendMSRPDataPckg ) );
    }
#endif /* RMSRPSESSION_CPP_ */
