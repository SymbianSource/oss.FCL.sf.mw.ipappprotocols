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
* Name          : CSigCompMgr.h
* Part of       : Dialogs
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef CSIGCOMPMGR_H
#define CSIGCOMPMGR_H

#include <e32base.h>
#include <stringpool.h>
#include "_sipcodecdefs.h"

class MSigComp;
class CURIContainer;
class CSIPURI;
class CSIPRouteHeader;


/**
 * @brief Class for using and unusing signalling compression
 *
 */
class CSigCompMgr : public CBase
	{
public:

	/**
	 * Creates a new CSigCompMgr instance
	 * @returns a new instance of CSigCompMgr
	 */
    static CSigCompMgr* NewL (TUint32 aIapId,
                              MSigComp& aSigComp);

	/**
	 * Creates a new CSigCompMgr instance and pushes it to CleanupStack
	 * @returns a new instance of CSigCompMgr
	 */
    static CSigCompMgr* NewLC (TUint32 aIapId,
                               MSigComp& aSigComp);

	/**
	 * Destructor
	 */
    ~CSigCompMgr();

	/**
	 * Updates the compression information. This might lead into
     * deleting a used compartment or/and creation of a new one. 
	 * @param aRemoteTarget the URI in the remote Contact
     * @param aRouteSet the dialog's current Route-set.
	 * @param aLocalTarget the local target of the dialog.
	 *        If not zero and compression is used contains 
	 *        on return comp=sigcomp parameter.
	 *        If not zero and compression is not used
	 *        comp=sigcomp parameter is removed.
     */
    void UpdateL (const CURIContainer& aRemoteTarget,
                  const RPointerArray<CSIPRouteHeader>& aRouteSet,
				  CSIPURI& aLocalTarget,
				  TBool aUpdateLocalTarget=ETrue);
	
	/**
	 * Gets the current compartment id
	 * @returns zero if no compartment is used. Otherwise the compartment id.
	 */	
	TUint32 CompartmentId() const;

private:

	CSigCompMgr (TUint32 aIapId, MSigComp& aSigComp);

    TBool HasSigCompParameter (const CSIPURI& aURI) const;
    void UpdateL (const CSIPURI& aURI);
	
private: // Data
	
    TUint32 iIapId;
    MSigComp& iSigComp;
	RStringF iCompParamName;
	RStringF iSigCompValue;
    TUint32 iCompartmentId;

private: // For testing purposes

	UNIT_TEST(CSigCompMgrTest)
	};

#endif // CSIGCOMPMGR_H

// End of File
