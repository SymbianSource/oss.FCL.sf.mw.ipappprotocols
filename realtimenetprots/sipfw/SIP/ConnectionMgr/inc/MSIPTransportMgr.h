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
* Name          : MSIPTransportMgr.h
* Part of       : SIPSec
* Version       : SIP/5.0 
*
*/




/**
 @internalComponent
*/

#ifndef MSIPTRANSPORTMGR_H
#define MSIPTRANSPORTMGR_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TSIPTransportParams;
class TInetAddr;
class MSIPTransportRemovalObserver;


// CLASS DECLARATION
/**
* An interface for allocationg and deallocating transport resources
* for IPSec SAs and TLS. 
*
*  @lib N/A
*/
class MSIPTransportMgr
    {
	public:
	
		/** 
		* Reserves UPD and TCP transports with the given parameters.
		* @param aTransportParams on return contains the id of 
		*        the reserved transport
		* @param aLocalSendPort local port used for sending
		* @param aLocalReceivePort local port listened on UDP and TCP
		* @param aRemoteAddr remote address
		* @param aRemoteSendPort remote port where messages are expected from.
		*/	
		virtual void ReserveTransportL( TSIPTransportParams& aTransportParams,
                                        TUint aLocalSendPort,
									    TUint aLocalReceivePort,
			                            const TInetAddr& aRemoteAddr,
			                            TUint aRemoteSendPort ) = 0;
			                           
		/** 
		* Pre-allocates a TLS transport. 
		* The actual destination address is known when the first 
		* SIP request is sent using this transport and 
		* the TLS connection establishment is delayed until that.
		* @param aTransportParams on return contains the id of 
		*        the pre-allocated transport resources
		*/	
		virtual void CreateTLSTransportL( 
		                    TSIPTransportParams& aTransportParams ) = 0;   
 
        /** 
		* Deallocates resources reserved for the transport.
		* @param aTransportId identifies the transport 
		*/
		virtual void RemoveTransport( TUint32 aTransportId ) = 0;
		
		/** 
		* Deallocates resources reserved for the transport. If return value
		* is more than zero and observer is provided, the caller must wait
		* asynchronous completion of removal. Completion callback of removal
		* observer interface is called as many times as there is asynchronous
		* removal operations.
		* @param aTransportId identifies the transport 
		* @param aRemovalObserver interface for asynchronous removal completion
		*        callbacks; may be NULL if caller is not interested about
		*        removal completion 
		* @return number of asynchronous remove operations
		*/
		virtual TInt RemoveTransport( 
		                    TUint32 aTransportId,
		                    MSIPTransportRemovalObserver* aRemovalObserver ) = 0;
		
		/** 
		* Retrieves local address of spesified IAP
		* @param aIapId id of accesspoint which local address is retrieved
		* @param aAddr on return will contain retrieved local address
		* @returns KErrNone if success, otherwise system wide error code
		*/
		virtual TInt GetLocalAddress( TUint32 aIapId, TInetAddr& aAddr ) = 0;
	};

#endif // MSIPTRANSPORTMGR_H

// End of File
