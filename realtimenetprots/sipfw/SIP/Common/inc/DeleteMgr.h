/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : DeleteMgr.h
* Part of       : SIPCommon
* Version       : SIP/2.0 
*
*/




/**
 @internalComponent
*/


#ifndef DELETEMGR_H
#define DELETEMGR_H

#include <e32base.h>
#include <e32std.h>


//Panic reasons
enum TDeleteMgrPanic
	{
	EDeleteMgrFncPrecondNotMet
	};


/*
 * CDeleteMgr is an active object handling the deletion of CBase-derived
 * objects asyncronously, avoiding the need to use constructs like
 * "delete this".
 * Using CDeleteMgr for freeing the memory guarantees that the call stack
 * won't include any code from the object that is to be deleted.
 */
class CDeleteMgr : public CActive
	{
public:
	/**	 
	 * Creates a new object.
	 *
	 * @pre 
	 * @post
	 *
	 * @see
	 *	 	 
	 * @return value New CDeleteMgr object. Ownership is transferred.
	 */
	static CDeleteMgr* NewL();


	/**	 
	 * Frees the resources of CDeleteMgr. Any delete requests which have been
	 * issued, but not yet executed, are also done.
	 *
	 * @pre 
	 * @post
	 *
	 * @see
	 *	 	 
	 * @return value -
	 */
	~CDeleteMgr();

	
	/**
	 * Request an asynchronous deletion of memory. The memory will be freed
	 * when the CDeleteMgr::RunL() will be called by the Active Scheduler.	 
	 *
	 * @pre aItemToDelete != NULL, aItemToDelete != this CDeleteMgr instance
     *  itself, pointer to an object can't be added more than once
	 *
	 * @param aItemToDelete CBase derived object to be deleted. Ownership is
	 *	transferred.
	 * @return value KErrNone if successful, otherwise a system wide error code
	 */
	TInt AddDeleteRequest(const CBase* aItemToDelete);


	//from CActive:
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

private:
	CDeleteMgr();


	/**
	 * Starts waiting for AddDeleteRequest() events.
	 *
	 * @pre 
	 *	 
	 * @return value -
	 */
	void WaitForRequests();
	

	//Pointers which have not yet been deleted are stored here
	RPointerArray<CBase> iDeleteRequests;


#ifdef CPPUNIT_TEST
	friend class CDeleteMgrTest;
	friend class CUserAgentClient_Test;
	friend class CNormalUAC_ResolveAddress_Test;
	friend class CNormalUAC_WaitResponse_Test;
	friend class CNormalUAS_Start_Test;
#endif
	};

#endif // end of DELETEMGR_H

// End of File
