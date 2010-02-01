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
* Name          : ClientMechanisms.h
* Part of       : SIPSec
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef CSIPSECCLIENTMECHANISMS_H
#define CSIPSECCLIENTMECHANISMS_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* This class contains the mechanism names that application inserted in
* Security-Client headers, indicating it is willing to use these mechanisms.
*/
class CSIPSecClientMechanisms : public CBase
	{
public:	
	/**
	 * Create a new instance of CSIPSecClientMechanisms
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return New CSIPSecClientMechanisms object, ownership is transferred.
	 */
	static CSIPSecClientMechanisms* NewL();

	~CSIPSecClientMechanisms();

	/**
	 * Add a mechanism name
	 *
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aMechanism Mechanism name
	 * @return
	 */
	void AddL(const TDesC8& aMechanism);

	/**
	 * Removes all stored mechanisms.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 */
	void RemoveAll();

	/**
	 * Checks if there are no mechanisms.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *	 
	 * @return ETrue if there are no mechanisms present,
	 *		   EFalse if there is at least one mechanism
	 */
	TBool IsEmpty() const;

	/**
	 * Searches the stored mechanisms, and looks if aMechanism is among them.
	 *	 
	 * @pre
	 * @post
	 *
	 * @see
	 *
	 * @param aMechanism Mechanism name
	 * @return ETrue if aMechanism is present in the stored mechanisms
	 */
	TBool IsPresent(const TDesC8& aMechanism) const;

private:
	CSIPSecClientMechanisms();

	RPointerArray<HBufC8> iMechanismNames;
	};

#endif // end of CSIPSECCLIENTMECHANISMS_H

// End of File
