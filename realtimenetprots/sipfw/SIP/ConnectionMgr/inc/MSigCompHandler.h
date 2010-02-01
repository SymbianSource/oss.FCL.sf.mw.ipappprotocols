/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSigCompHandler.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/



#ifndef __MSIGCOMPHANDLER_H__

/**
* @internalComponent
*/
#define __MSIGCOMPHANDLER_H__

#include <in_sock.h>
#include "SipHostPort.h"


/**
 * Interface class for sigcomp handler.
 */
class MSigCompHandler 
	{
	public:

	/**
	 * Destructor.
	 */
	virtual ~MSigCompHandler () {}

    /**
     * After calling this function Connection Mgr always uses
     * signalling compression for requests sent to this host.
     * Multiple users can share a compression to a given host.
     * This is implemented as a reference count.
     * If a compression already exists, the referrence count is
     * incremented by one.
     *
     * @param aHostPort for which all the outgoing requests should be sent
     *        compressed.
	 */
    virtual void UseL (const CSIPHostPort& aHostPort) = 0;

    /**
     * Decrements the reference count to a initiated compression.
     * When reference count reaches zero,
     * the compression for this host is stopped. 
     * If no compression for the host was found, the function silently returns.
     *
     * @param aHostPort for which to unuse compression.
	 */
    virtual void UnUse (const CSIPHostPort& aHostPort) = 0;    

	/**
	 * Decompress the message; leaves on failure. 
	 *
	 * SigComp specífic leave codes:
	 * EDecompressionFailure - the decompressor failed to decompress the
	 *						   message.
	 * EIncompleteMessage - the SigComp message was incomplete
	 * The function expects the client to provide complete SigComp message.
	 *
	 * @pre IsSigCompMsg()==ETrue
	 * 
	 * @param aMessage a message to decompress. The message must start with
	 *				   SigComp specific delimeter and end with SigComp specific
	 *				   delimeter
	 *
	 * @param aBytesConsumed on return will contain the number of bytes
	 *						 consumed by the decompression from the original
	 						 message 
	 * @returns decompressed message, the ownership is transferred
	 */
	virtual HBufC8*
		DecompressL(const TDesC8& aMessage, TUint& aBytesConsumed) = 0;
	
	/**
	 * Compress the message using the algorith provided while
	 * creating the instance of the CSigComp class.
	 *
	 * Leaves with CSigComp::TError::ECompressionError error code if the
	 *	compression fails.
	 * Leaves with KErrNotFound error code if compartment is not found.
	 * 
	 * @param aMessage a message to compress.
	 *					
	 * @param aAddress	A remote destination address
	 *					
	 * @param aSIPHostPort The host where message will be sent after
	 *					   compression.
	 *	The hostport identifies the compartment that will be used for
	 *					   compression.
	 * 
	 * @returns Compressed message, ownership is transfered to the client.
	 */
	virtual HBufC8* CompressL(const TDesC8& aMessage, TInetAddr aAddress, 
							  const CSIPHostPort& aHostPort,
							  TBool aStreamBasedProtocol) = 0;

	/**
	 * Checks if the message contains the delimeter indicating SigComp
	 * message end. This function must be used only with stream-based
	 * transports, otherwise it will always return EFalse.
	 *
	 * @pre IsSigCompMsg()==ETrue
	 *
	 * @returns ETrue is the SigComp message is complete, EFalse otherwise
	 */
	virtual TBool IsCompleteSigCompMessageL(const TDesC8& aMessage) const = 0;

	
	/**
	 * Checks if the first byte contains bit pattern indicating the start
	 * of the SigComp message. Function examines only the first byte.
	 *
	 * @returns ETrue if the first byte indicates the start of the SigComp
	 *			message, EFalse otherwise
	 */
	virtual TBool IsSigCompMsg(const TDesC8& aMessage) const = 0;

	/**
    * Checks if signalling compression is installed.
    *
    * @returns ETrue if installed, EFalse otherwise.
	*/
	virtual TBool Available() const = 0; 

	/**
	 * Allows the state of the decompressed message to be saved; leaves on
	 * failure.
	 * 
	 * @param aAddress	A remote destination address
	 *					
	 * @param aHostPort A host port
	 */
	virtual void AllowL(TInetAddr aAddr, const CSIPHostPort& aHostPort) = 0;

	/**
	 * Denies the state of the message to be saved.
	 */
	virtual void Deny() = 0;

	/**
	 * Checks if hostport has compartment already
	 *
	 * @param aHostPort A host port
	 *
	 * @returns ETrue if compartment exists
	 */
	virtual TBool HasCompartment(const CSIPHostPort& aHostPort) = 0;
};

#endif // end of __MSIGCOMPHANDLER_H__

// End of File
