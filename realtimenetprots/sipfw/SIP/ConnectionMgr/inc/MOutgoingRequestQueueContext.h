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
* Name          : MOutgoingRequestQueueContext.h
* Part of       : ConnectionMgr
* See class definition below
* Version       : SIP/5.0
*
*/



#ifndef MOUTGOINGREQUESTQUEUECONTEXT_H

/**
* @internalComponent
*/
#define MOUTGOINGREQUESTQUEUECONTEXT_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPRequest;
class TInetAddr;
class TSIPTransportParams;
class MSocketUsagePermissionObserver;
class RSocket;

// CLASS DEFINITION
/**
 *
 */
class MOutgoingRequestQueueContext
	{
	public: // Abstract methods

		virtual void ContinueSendToTransportL( const TSIPTransportParams& aParams,
                		                       CSIPRequest& aRequest,
                		                       TBool aForceUDP,
                							   const TInetAddr& aRemoteAddr,
                							   TUint aLocalPort,
                							   TUint aOrigTransport,
                							   TRequestStatus& aStatus,
                							   TBool aPublicAddrResolved ) = 0;
        							   
        virtual void RequestFailed( TRequestStatus& aStatus, TInt aError ) = 0; 
        
        virtual RSocket& GetUdpSocketL( const TSIPTransportParams& aParams, 
                                        const TInetAddr& aRemoteAddr,
                                        TUint& aLocalPort,
                                        MSocketUsagePermissionObserver* aObserver,
                                        TBool& aPermissionToUse ) = 0;
        
        virtual void UdpSocketFree( const TSIPTransportParams& aParams,
                                    const TInetAddr& aRemoteAddr,
                                    TUint aLocalPort,
                                    MSocketUsagePermissionObserver* aObserver,
                                    RSocket& aSocket ) = 0;

	};
	
#endif // MOUTGOINGREQUESTQUEUECONTEXT_H

