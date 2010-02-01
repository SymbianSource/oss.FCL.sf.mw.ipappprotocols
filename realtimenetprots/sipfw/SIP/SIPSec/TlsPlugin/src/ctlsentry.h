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
* Name        : ctlsentry.h
* Part of     : SIPSec TLS Plugin
* Interface   : 
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef C_TLSENTRY_H
#define C_TLSENTRY_H

// INCLUDES
#include <e32base.h>
#include <in_sock.h>

// FORWARD DECLARATIONS
class MSIPSecUser;

// CLASS DECLARATION

/**
 * @brief This class represents the information of a one TLS connection.
 */
class CTLSEntry : public CBase 
    {
public: // Constructors and destructor

    /**
    * Constructs the object and pushes it to cleanupstack.
    * @param aNextHop Next hop address
    * @param aTransportId Transport identifier
    * @param aSIPSecUser SIPSec user
    * @return Return New object, ownership is transferred.    
    */
    static CTLSEntry* NewLC( const TInetAddr& aNextHop,
    						 TUint32 aTransportId,
    						 const MSIPSecUser& aSIPSecUser );

    /**
    * Destructor
    */
    ~CTLSEntry();

public: // New functions

	/**
     * Checks if the next hop address matches.
     *
     * @param aNextHop Address to compare with
     * @return ETrue If aNextHop is same as the stored one.
     *		   EFalse Otherwise
     */
	TBool CompareNextHop( const TInetAddr& aNextHop ) const;

	/**
     * Checks if the SIPSec user matches.
     *
     * @param aSIPSecUser SIPSec user to compare with.
     * @return ETrue If SIPSec user is same as the stored one.
     *		   EFalse Otherwise
     */
	TBool CompareSIPSecUser( const MSIPSecUser& aSIPSecUser ) const;

	/**
     * Get the transport identifier.
     *
     * @return Transport identifier.
     */
	TUint32 TransportId() const;

private: // Constructor, for internal use

	/*
     * Constructor
     * @param aNextHop Next hop address
     * @param aTransportId Transport identifier
     * @param aSIPSecUser SIPSec user
     */
    CTLSEntry( const TInetAddr& aNextHop,
    		   TUint32 aTransportId,
    		   const MSIPSecUser& aSIPSecUser );

private: // Data

	const TInetAddr iNextHop;
	TUint32 iTransportId;

	//It is enough to know the SIPSec user which created this entry. No need
	//to keep track of other SIPSec users sending to the same next hop.
	const MSIPSecUser& iSIPSecUser;

#ifdef CPPUNIT_TEST
	friend class CTLSEntryTest;
#endif
    };

#endif //end of C_TLSENTRY_H

// End of File
