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
* Name          : MSigComp.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/
#ifndef MSIGCOMP_H
#define MSIGCOMP_H

#include <e32std.h>


class MSigComp
	{
public:

    /**
     * Destructor.
	 */
	virtual ~MSigComp () {}

    /**
     * Checks if signalling compression is installed.
     *
     * @returns ETrue if installed, EFalse otherwise.
	 */
    virtual TBool IsSupported () const = 0;

    /**
     * After calling this function ConnectionMgr always uses
     * signalling compression for requests sent with 
     * the returned compartment ID.
     * Multiple users can share a compartment to a 
     * next hop identified by TInetAddr.
     * The compartment ID and the TInetAddr are bound together 
     * when the first request towards the TInetAddr is sent.
     * Multiple comparment IDs can point to the same TInetAddr.
     *
     * @param aIapId the IAP used for the compartment
     *
     * @returns a generated ID for the compartment
	 */
    virtual TUint32 CreateCompartmentL(TUint32 aIapId) = 0;

    /**
     * Removes compartment identified by the compartment ID
     *
     * @param aCompartmentId the compartment ID
	 */
    virtual void RemoveCompartment(TUint32 aCompartmentId) = 0;

	/**
     * The function returns count of compartments
     *
     * @returns	count of compartments
     */
	virtual TInt CompartmentCount() = 0;

	/**
     * If the compartment has been previously used for decompressing,
     * resets the compartment. If not, deletes the compartment. 
     *
     * @param aCompartmentId the compartment ID
     */
	virtual void SendFailedL(TUint32 aCompartmentId) = 0;
	};

#endif // MSIGCOMP_H

// End of File
