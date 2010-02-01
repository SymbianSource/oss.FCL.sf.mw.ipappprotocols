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
* Name          : SIPMessageUtility.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/

#ifndef SIPMESSAGEUTILITY_H
#define SIPMESSAGEUTILITY_H

// INCLUDES
#include <e32base.h>
#include "SipStackServerDefs.h"
#include "SipLogs.h"    //USE_SIP_LOGS is defined here
#include "TransactionBase.h"

// FORWARD DECLARATIONS
class RStringF;
class TInetAddr;
class CURIContainer;
class CSIPMessage;
class CSIPResponse;
class CSIPHeaderBase;
class CSIPFromToHeaderBase;
class CSIPViaHeader;

class CUserAgent;

// CONSTANTS

// CLASS DECLARATION
/*
 * Miscellaneous SIP message related functions
 */
class CSIPMessageUtility : public CBase
	{
public: // Constructor and destructor

	CSIPMessageUtility();
	
	~CSIPMessageUtility();

public: // New functions

	/**
	 * Compares two TInetAddr objects.
	 *
	 * @pre aAddr2 has port filled
	 * @param aAddr TInetAddr to compare
	 * @param aTransportProtocol Transport protocol of aAddr
	 * @param aAddr2 Another TInetAddr to compare	 
	 * @return value ETrue if ip-addresses and ports match, EFalse otherwise
	 */
	static TBool CompareTInetAddr(const TInetAddr& aAddr,
								  RStringF aTransportProtocol,
                                  const TInetAddr& aAddr2);

	/**
	 * Adds random characters to the given descriptor.
	 *	 
	 * @pre aBuf must have space for at least aLength additional characters
	 *
	 * @param aBuf Descriptor into which the random characters are added
	 * @param aLength Amount of characters added to aBuf
	 * @param aCaseSensitive ETrue if both upper and lowercase characters are
	 *	allowed to be added into aBuf. EFalse if only lowercase characters are
	 *	to be used.
	 * @param aMsg SIP message, contains useful input for generating random
	 *	data. Can be NULL. Ownership isn't transferred.
	 * @param aTransactionId Id of the associated transaction
	 * @param aUserAgent Address of the associated UserAgent object, can be
	 *	NULL. Ownership isn't transferred.	 
	 */
	void AddRandomStringL(TDes8& aBuf,
						  TInt aLength,
						  TBool aCaseSensitive,
						  CSIPMessage* aMsg,
						  TTransactionId aTransactionId,
						  const CUserAgent* aUserAgent);

    /**
	 * Computes a checksum over the data pointed by aPtr, and adds the result
     * into aBuf.
	 *
     * @param aBuf Buffer to which the result is appended
     * @param aPtr Pointer to data for which the checksum is calculated
     * @param aLength Length of the data to be checksummed	 
	 */
    static void ComputeChecksum(TDes8& aBuf, const TAny* aPtr, TInt aLength);

	/**
	 * Returns a reference to the method of the SIP message. In case of request
	 * this is the request method, and in case of response, it is the method in
	 * CSeq header.
	 *
     * @param aMsg SIP message
	 * @return value Reference to method
	 */
	static RStringF MessageMethod(CSIPMessage& aMsg);

    /**
	 * Returns the transaction type.
	 *
	 * @param aMsg SIP message
	 * @param aIncomingMsg
	 *		ETrue if the message was received from remote endpoint
	 *		EFalse if the message is sent by the local endpoint
	 * @return Transaction type
	 */
	static CTransactionBase::TTransactionType
        TransactionType(CSIPMessage& aMsg, TBool aIncomingMsg);

	/**
	 * Checks whether the top Via header has a branch beginning with the magic
	 * cookie.
	 *
	 * @param aMsg SIP message
	 * @return value ETrue if top via has branch beginning with the magic
	 *	cookie
	 */
	static TBool HasViaMagicCookie(CSIPMessage& aMsg);

    /**
	 * Checks whether aTransport has an allowed value.	 
	 *
	 * @param aTransport Transport protocol
	 * @return value ETrue if aTransport is valid, EFalse otherwise
	 */
    static TBool CheckTransport(RStringF aTransport);

	/**
	 * Reads transport protocol from the top Via header of aMsg, and fills the
	 * value into aTransport.
	 *
	 * @param aMsg IN: SIP message
	 * @param aTransport OUT: transport protocol from aMsg
	 * @return ETrue: Transport was found and set, EFalse: Transport not found,
	 *	or has unknown value
	 */
	static TBool TransportProtocol(CSIPMessage& aMsg, RStringF& aTransport);

	/**
	 * Updates the top via header's transport parameter to match the value of
	 * aTransport.
	 *
	 * @pre CheckTransport(aTransport) == ETrue
	 *		aMsg has Via header	  
	 *
	 * @param aMsg SIP message
	 * @param aTransport Transport protocol
	 */
	static void UpdateViaTransportL(CSIPMessage& aMsg, RStringF aTransport);

	/**
	 * Compares tags of two To (or From) headers.
	 *
	 * @param aHeader To (or From) header
	 * @param aHeader2 To (or From) header
	 * @return value ETrue if the tags are equal, EFalse otherwise
	 */
	static TBool CompareTags(const CSIPFromToHeaderBase& aHeader,
							 const CSIPFromToHeaderBase& aHeader2);

	/**
	 * Copies the specified headers from aSrc into aDest, maintaining the
	 * order.
	 *
	 * @param aSrc SIP message from where the headers are copied
	 * @param aDest SIP message to which the headers are copied. If aDest
	 *	contains same headers before copying, the old headers are removed.
	 * @param aHeaderName Identifies the headers	 
	 */
	static void CopyHeadersL(CSIPMessage& aSrc,
							 CSIPMessage& aDest,
							 RStringF aHeaderName);

	/**
	 * Copies the Authorization and Proxy-Authorization headers from aSrc into
	 * aDest. If aSrc is NULL, nothing is done.
	 *	 
	 * @param aSrc SIP message from where the headers are copied, can be NULL.
	 *	Ownership is not transferred.
	 * @param aDest SIP message to which the headers are copied. If aDest
	 *	contains same headers before copying, the old headers are removed.	 
	 */
	static void CopyAuthorizationHeadersL(CSIPMessage* aSrc,
							 			  CSIPMessage& aDest);							 
							 
	/**
	 * Makes a copy of the specified header, and returns a pointer to it.
	 * This function only copies one header, and if there are multiple headers
	 * in aMsg, only the first is copied.
	 *
	 * @param aMsg SIP message from where the header is copied	 
	 * @param aHeaderName Identifies the header
	 * @return Copy of the header, or NULL if header wasn't present in aMsg.
	 *	Ownership is transferred.
	 */
	static CSIPHeaderBase* CopyHeaderFromMsgL(CSIPMessage& aMsg,											  
											  RStringF aHeaderName);

	/**
	 * Fills the CSeq header into aMsg. If aMsg already contains a CSeq header,
	 * it isn't changed.
	 *
	 * @param aMsg IN/OUT: SIP message
	 * @param aSeq CSeq sequence number
	 * @param aMethod CSeq method	 
	 */
	static void FillCSeqL(CSIPMessage& aMsg,
						  TUint aSeq,
						  RStringF aMethod);

    /**
	 * Checks whether the SIP message is an ACK request
	 *
     * @param aMsg SIP message
     * @return ETrue aMsg is ACK, EFalse otherwise
	 */
	static TBool IsAck(const CSIPMessage& aMsg);

	/**
	 * Checks whether the SIP response is a final response.
	 *
     * @param aResp SIP response
     * @return ETrue  aResp is a final response (2xx-6xx),
     *         EFalse Otherwise
	 */
	static TBool IsFinalResponse(const CSIPResponse& aResp);
	
    /**
	 * Checks whether the URI has a parameter comp=sigcomp
	 *
     * @param aUri URI
     * @return ETrue If aUri has a parameter comp=sigcomp, EFalse otherwise
	 */
    static TBool HasSigCompParam(const CURIContainer& aUri);

    /**
	 * Checks whether the SIP message is a 2xx class response
	 *
     * @param aMsg SIP message
     * @return ETrue aMsg is a 2xx response, EFalse otherwise
	 */
	static TBool Is2xxResponse(const CSIPMessage& aMsg);

    /**
	 * Returns the topmost Via header of SIP message.
	 *
     * @param aMsg SIP message
     * @return Via header, or NULL if no Via header exists in aMsg. Ownership
     *  is not transferred.
	 */
    static CSIPViaHeader* TopVia(CSIPMessage& aMsg);

    static const TDesC8& BranchMagicCookie();    

	static const TDesC8& UriDescriptor(const CURIContainer& aUri);

	/**
	 * Determines if the local address is in the IPv4 private address space.
	 *
     * @param aTransportMgr TransportMgr
     * @param aIapId IAP-id
     * @return ETrue Local address is a private address
     *		   EFalse Otherwise
	 */
	static TBool IsPrivateAddressL(MSIPTransportMgr& aTransportMgr,
								   TUint32 aIapId);

private:

	static HBufC* ConvertUtf8LC(const TDesC8& aUtf8);

	/**
	 * Returns the SIP port of aAddr.
	 *
	 * @param aAddr Address
	 * @param aTransportProtocol Transport protocol
	 * @return value Port number
	 */
	static TUint SIPPort(const TInetAddr& aAddr, RStringF aTransportProtocol);

	/**
	 * Builds a buffer containing random data.
	 *
	 * @param aLength Amount of bytes the buffer will contain
	 * @param aMsg SIP message, contains useful input for generating random
	 *	data. Can be NULL. Ownership isn't transferred.
	 * @param aTransactionId Id of the associated transaction
	 * @param aUserAgent Address of the associated UserAgent object, can be
	 *	NULL. Ownership isn't transferred.
	 * @return value Buffer containing random data. Ownership is transferred.
	 */
	HBufC8* BuildInputDataL(TUint aLength,
						    CSIPMessage* aMsg,
						    TTransactionId aTransactionId,
						    const CUserAgent* aUserAgent);

	/**
	 * Checks if there is space left for aSize amount of bytes in the aBuf.
	 *
	 * @param aBuf Descriptor
	 * @param aSize Amount of bytes
	 * @return value ETrue if aBuf has free space for at least aSize bytes,
	 *	EFalse otherwise.
	 */	
	static TBool FitsInBuf(const TDes8& aBuf, TInt aSize);

	/**
	 * Returns aBits long sequence of bits from the aBuf.
	 *
	 * @param aBuf Descriptor from where the bits are read
	 * @param aBits How many bits are read
	 * @param aCounter Offset into the descriptor aBuf, indicating the position
	 *	from where the bits are read. Zero means the bits are read from the
	 *  beginning of aBuf.
	 * @return value Bit sequence containing aBits bits
	 */
	static TUint8 GetNextBits(const TDesC8& aBuf,
							  TInt aBits,
							  TUint& aCounter);

    /**
	 * Adds random data into the aBuf. The random data is generated using the
	 * SIP message (aMsg) as input.
	 *
	 * @param aMsg SIP message, can be NULL. Ownership isn't transferred
	 * @param aBuf Random data is appended to this buffer	 
	 */
    void AddCheckSumOfSipMessageL(CSIPMessage* aMsg, TDes8& aBuf);

	/**
	 * Adds random data into the aBuf. The random data is generated using
     * aHeader and iCounter as inputs and by combining the user, host and tag
     * into a single descriptor, and calculating a checksum over the data.
	 *
	 * @param aHeader To or From header	 
	 * @param aBuf Random data is appended to this buffer	 
	 */
	void AddCheckSumOfFromToHeaderL(CSIPFromToHeaderBase& aHeader,        
                                    TDes8& aBuf);

	/**
	 * Adds random data into the aBuf. The random data is generated using the
	 * CSeq sequence number of the aMsg as input.
	 *
	 * @param aMsg SIP message
	 * @param aBuf Random data is appended to this buffer	 
	 */	
	static void AddCheckSumOfCSeq(CSIPMessage& aMsg, TDes8& aBuf);

	/**
	 * Adds random data into the aBuf. The random data is generated using the
	 * iCounter and current time as inputs.	 
	 *	 
	 * @param aBuf Random data is appended to this buffer	 
	 */	
	void AddCheckSumOfClock(TDes8& aBuf) const;

	/**
	 * Adds random data into the aBuf. The random data is generated using the
	 * iCounter and aTransactionId as inputs.
	 *	 
	 * @param aTransactionId Id of the associated transaction
	 * @param aBuf Random data is appended to this buffer	 
	 */	
	void AddCheckSumOfTaIdL(TTransactionId aTransactionId, TDes8& aBuf);

	/**
	 * Adds random data into the aBuf. The random data is generated using the
	 * request method and Request URI of the aMsg as inputs. If aMsg is a
     * response, nothing is added to aBuf.
	 *
	 * @param aMsg SIP message
	 * @param aBuf Random data is appended to this buffer	 
	 */	
	static void AddCheckSumOfRequestLineL(CSIPMessage& aMsg, TDes8& aBuf);

	/**
	 * Adds random data into the aBuf. The random data is generated using the
	 * clock, amount of free and used memory and the inactivity time.
	 *	 
	 * @param aBuf Random data is appended to this buffer	 
	 */	
	void AddSystemInfo(TDes8& aBuf) const;

private: // Data

	//Seed value for Math::Rand()
	TInt64 iSeed;

	//Counter value used for generating random values
	TInt32 iCounter;

private: // For testing purposes

#ifdef CPPUNIT_TEST
	friend class CSIPMessageUtility_Test;
#endif
	};

#endif // end of SIPMESSAGEUTILITY_H

// End of File
