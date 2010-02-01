/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSigCompController.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef __MSIGCOMPCONTROLLER_H__
#define __MSIGCOMPCONTROLLER_H__

#include <in_sock.h>
class CSIPRequest;
class CSIPResponse;
class TSIPTransportParams;

/**
 * Interface class for sigcomp controller
 */

class MSigCompController 
	{
	public:

    /**
     * Destructor.
	 */

    virtual ~MSigCompController () { };

    /**
     * Checks if signalling compression is installed.
     *
     * @returns ETrue if installed, EFalse otherwise.
	 */
    virtual TBool IsSupported () const = 0;

    /**
     * After calling this function Connection Mgr always uses
     * signalling compression for requests sent with compartmentId
     * returned by this function.      
     * Multiple users can share a compression to one host:
     * In sending phase remote inetaddr and compartmentid are mapped together.
     * If some other compartmentid has already same remote destination address
     * mapped with it, ids share the same compartment.
     *
     * @returns compartmentId
	 */
    virtual TUint32 CreateCompartmentL (TUint32 aIapId) = 0;

    /**
     * Removes compression for passed compartmentId.
     * If no compression for the host was found, the function silently returns.
	 */
    virtual void RemoveCompartment (TUint32 aCompartmentId) = 0;

	/**
	 * The function encodes the Request and compresses 
	 * it if there is compartment matching compartmentId. After calling
	 * this function, next-hop address is mapped with compartmentId.
	 * Leaves with a system wide error if encoding or compressing fails.
	 * 
	 * @param aTransportParams contains compartmentId and iapId
	 *
	 * @param aRequest a request to encode and compress	
	 *			
	 * @param aAddress	A remote destination address
	 *					
	 * @returns encoded data, the ownership is transferred
	 */
	virtual CBufBase* EncodeL (const TSIPTransportParams& aTransportParams,
	                           CSIPRequest& aRequest,
                               const TInetAddr& aAddress,
							   TBool aStreambasedProtocol,
							   TBool& aCompressed) = 0;

	/**
	 * The function encodes the Response and compresses 
     * it if there is compartment matching compartmentId. After calling
	 * this function, next-hop address is mapped with compartmentId.
	 * Leaves with a system wide error if encoding or compressing fails.
	 * 
	 * @param aTransportParams contains compartmentId and iapId
	 *
	 * @param aResponse a response to encode and compress	
	 *				
	 * @param aAddress	A remote destination address					
	 * 
	 * @returns encoded data, the ownership is transferred
	 */
	virtual CBufBase* EncodeL (const TSIPTransportParams& aTransportParams,
	                           CSIPResponse& aResponse,
                               const TInetAddr& aAddress,
							   TBool aStreambasedProtocol,
							   TBool& aCompressed) = 0;

	/**
	 * Checks if the message contains the delimeter indicating SigComp
	 * message end. This function must be used only with stream-based
	 * transports, otherwise it will always return EFalse.
	 *
	 * @pre IsSigCompMsg()==ETrue
	 *
	 * @returns ETrue is the SigComp message is complete, EFalse otherwise
	 */
	virtual TBool IsCompleteSigCompMessageL (const TDesC8& aMessage) const = 0;

	
	/**
	 * Checks if the first byte contains bit pattern indicating the start
	 * of the SigComp message. Function examines only the first byte.
	 *
	 * @returns ETrue if the first byte indicates the start of the SigComp message,
	 *          EFalse otherwise
	 */
	virtual TBool IsSigCompMsg (const TDesC8& aMessage) const = 0;


  	/**
	 * Decompress the message; leaves on failure. 
	 * Leaves with a system wide error decompressing fails.
	 * The function expects the client to provide complete SigComp message.
	 *
	 * @pre IsSigCompMsg()==ETrue
	 * 
	 * @param aMessage a message to decompress. The message must start with SigComp specific
	 *                 delimeter and end with SigComp specific delimeter
	 *
	 * @param aBytesConsumed on return will contain the number of bytes consumed by the decompression
	 *                       from the original message 
	 * @returns decompressed message, the ownership is transferred
	 */
	virtual CBufBase* DecompressL (const TDesC8& aMessage,
                                   TUint& aBytesConsumed,
                                   TBool aIsStreamBased) = 0;

	/**
	 * The function returns count of compartments
	 *
	 * @returns count of compartments
	 */
	virtual TInt CompartmentCount () = 0;

	/**
     * If the compartment has been previously used for decompressing,
     * resets the compartment. If not, deletes the compartment. 
     *
     * @param aCompartmentId the IAP for which the sending failed
     *
     */
	virtual void SendFailedL (TUint32 aCompartmentId) = 0;

	/**
	 * Allows the state of the decompressed message to be saved; leaves on failure.
	 * 
	 * @param aAddress	A remote destination address					
	 * @param aIapId	A iap id
	 */
	virtual void AllowL(const TInetAddr& aAddress, TUint32 aIapId) = 0;

	/**
	 * Denies the state of the message to be saved.
	 */
	virtual void Deny() = 0;

    };

#endif // end of __MSIGCOMPCONTROLLER_H__

