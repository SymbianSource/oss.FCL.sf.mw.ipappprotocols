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
* Name          : ResolvingResults.h
* Part of       : TransactionUser
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef RESOLVINGRESULTS_H
#define RESOLVINGRESULTS_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RStringF;
class MSIPResolvingResult;

// CLASS DECLARATION
/*
 * This class is used for storing and accessing the addresses returned from a
 * URI resolving.
 */
class CResolvingResults : public CBase
	{
public: // Constructor and destructor

	/**	 
	 * @return New CResolvingResults object, ownership is transferred
	 */
	static CResolvingResults* NewL();

	~CResolvingResults();

public: // New functions
	
	/**
	 * Return the resolve results.
	 *	 
	 * @return Array containing all the resolving results obtained so far.
	 *	Ownership is not transferred.
	 */
	RPointerArray<MSIPResolvingResult>& Addresses();	

    /**
	 * Obtain the next address, that hasn't been tried yet.
	 *
	 * @param aAddress OUT: Found address.
	 *				   Set only if this function returns ETrue.
	 * @param aTransport OUT: Transport protocol of aAddress.
	 *				     Set only if this function returns ETrue.
	 * @param aForceUDP OUT: ETrue if transport of aAddress must remain UDP.
	 *				    Set only if this function returns ETrue.
	 * @return ETrue If an address that hasn't been already tried was found.
	 *	EFalse when no more addresses are left, that haven't been already tried.
	 */
	TBool GetNext(TInetAddr& aAddress,
				  RStringF& aTransport,
				  TBool& aForceUDP);

	/**
	 * Determine if there can be more resolving results for the URI.
	 *
	 * @return ETrue  If the URI should be resolved again to get more results
	 *		   EFalse If current URI has been fully resolved
	 */
	TBool ContinueResolvingCurrentUri() const;

	/**
	 * Clear all resolving results.
	 */
	void ClearAll();

private: // For internal use

	CResolvingResults();

private: // Data

    RPointerArray<MSIPResolvingResult> iAddresses;

#ifdef CPPUNIT_TEST	
	friend class CUserAgent_Test;
	friend class CUserAgentClient_Test;
	friend class CTransactionUser_Test;
	friend class CNormalUAC_ResolveAddress_Test;
	friend class CNormalUAC_WaitResponse_Test;
	friend class CInviteUAC_Start_Test;
	friend class CInviteUAC_WaitResponse_Test;
	friend class CInviteUAC_ResolveAckAddress_Test;
#endif
	};

#endif // end of RESOLVINGRESULTS_H

// End of File
