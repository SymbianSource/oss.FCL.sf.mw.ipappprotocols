/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSipConnectionMgr.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/
#ifndef MSIPCONNECTIONMGR_H
#define MSIPCONNECTIONMGR_H

#include <e32std.h>
#include "sipconnection.h" 

class TSIPTransportParams;
class CSIPRequest;
class CSIPResponse;
class MSIPNATBindingObserver;
class CUri8;

class MSipConnectionMgr
    {
public:

    /**
    * Destructor.
    */
    virtual ~MSipConnectionMgr () {}

    /**
    * Opens an IAP for use and creates a new sending socket. 
    * Multiple users may share the same IAP but each has an own sending socket.
    * So the IAP may already be open.
    *
    * @param aParams the transport params including the IAP ID.
    *
    * @return The current connection state 
    */
    virtual CSIPConnection::TState OpenL(
        const TSIPTransportParams& aParams) = 0;

    /**
    * Checks whether a connection and the related sockets have been opened.
    *
    * @param aParams the transport params including the IAP ID.
    *
    * @returns ETrue if opened, otherwise EFalse
    */
    virtual TBool IsOpen(const TSIPTransportParams& aParams) = 0;

    /**
    * Closes a connection and the related sockets. 
    * Closes the actual IAP based on a reference count.
    * When the last users calls this function the actual IAP is closed.
    * Sending sockets for the user are closed whether or not there
    * are other users for the IAP. 
    *
    * @param aParams the transport params including the IAP ID.
    */
    virtual void Close(const TSIPTransportParams& aParams) = 0;
    
    /**
    * Sets a RSocket option used for all the sending sockets
    * related to the given transport owner. 
    * @pre IsOpen() == ETrue
    * @param aParams the transport params including the IAP ID.
    * @param aOptionName An integer constant which identifies an option.
    * @param aOptionLevel An integer constant which identifies 
    *        level of an option.
    * @param aOption Option value packaged in a descriptor.
    */        
    virtual void SetOptL(const TSIPTransportParams& aParams,
                         TUint aOptionName,
                         TUint aOptionLevel,
                         const TDesC8& aOption) = 0;
             
    /**
    * Sets a RSocket option used for all the sending sockets
    * related to the given transport owner. 
    * @pre IsOpen() == ETrue
    * @param aParams the transport params including the IAP ID.
    * @param aOptionName An integer constant which identifies an option.
    * @param aOptionLevel An integer constant which identifies 
    *        level of an option.
    * @param aOption Option value as an integer.
    */                              
    virtual void SetOptL(const TSIPTransportParams& aParams,
                         TUint aOptionName,
                         TUint aOptionLevel,
                         TInt aOption) = 0;     

    /**
    * Function sends a SIP Request to a remote host.
    *
    * @pre IsOpen() == ETrue
    * @param aParams the transport params including the IAP ID.
    * @param aRequest A reference to a CSIPRequest object
    * @param aForceUDP ETrue if transport protocol can't be changed from UDP.
    * @param aOutboundProxy outbound proxy, NULL pointer if not used.
    *        The ownership is not transferred.     
    * @param aAddress A remote destination address
    * @param aStatus On completion, will contain an error code
    *        The aStatus is a reference to the calling active
    *        object´s iStatus.     
    */
    virtual void SendL(const TSIPTransportParams& aParams,
                       CSIPRequest& aRequest,
                       TBool aForceUDP,
                       const CUri8* aOutboundProxy,
                       const TInetAddr& aRemoteAddr,
                       TRequestStatus& aStatus) = 0;

    /**
    * Function sends a SIP Response to a remote host.
    * 
    * @pre IsOpen() == ETrue
    * @param aParams the transport params including the IAP ID.
    * @param aResponse    A reference to a CSIPResponse object
    * @param aId The Id of transaction that is sending RESPONSE
    * @param aStatus On completion, will contain an error code.
    *        The aStatus is a reference to the calling active
    *          object´s iStatus.
    */
    virtual void SendL(const TSIPTransportParams& aParams,
                       CSIPResponse& aResponse,
                       TTransactionId aId,
                       TRequestStatus& aStatus) = 0;

    /**
    * Function cancels an outstanding operation.
    *
    * @param aStatus The aStatus is a reference to the calling active
    *          object´s iStatus.
    * @return A system wide error code 
    */
    virtual TInt CancelSend(TRequestStatus &aStatus)=0;

    /**
    * Function cancels all outstanding operations for a transaction.
    *
    * @param aId TransactionId
    * @param aCancelAlso2xxResponses If EFalse, outstanding 2xx responses
    *           of the transaction are not canceled. If ETrue, all operations
    *           related to the transaction are canceled.
    * @return A system wide error code 
    */
    virtual TInt CancelResponseSend(TTransactionId aId, 
                                    TBool aCancelAlso2xxResponses) = 0;

    /**
    * Disconnect the TCP connection to the remote host
    *
    * @param aParams the transport params including the IAP ID.
    * @param aRemoteAddress Address of remote host
    *
    */
    virtual void TcpDisconnect(const TSIPTransportParams& aParams,
                               const TInetAddr& aRemoteAddress) = 0;
                               
    /**
    * Frees all the resources reserved for the observer.
    * @param aSIPNATBindingObserver a observer
    */
    virtual void FreeResources(
        MSIPNATBindingObserver& aSIPNATBindingObserver ) = 0;
    };

#endif // MSIPCONNECTIONMGR_H

// End of File