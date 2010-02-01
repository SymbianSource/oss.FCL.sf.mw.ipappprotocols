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
* Name          : timerrequest.h
* Part of       : LightWeightTimer
* Private header file for the Lightweight timer subsystem.
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef C_TIMERREQUEST_H
#define C_TIMERREQUEST_H

// INCLUDES
#include <e32base.h>
#include "Lwtimer.h"

// FORWARD DECLARATIONS

// CLASS DECLARATION
/*
 * This class stores information of a single timer request.
 */
class CTimerRequest : public CBase
	{
public: // Constructors and destructor

	/**
	 * Creates a new CTimerRequest object with empty values.
	 * This version is used for searching through the stored timer requests.
	 *
	 * @return New CTimerRequest object, ownership is transferred.
	 */
	static CTimerRequest* NewL();

	/**
	 * Creates new CTimerRequest object.
	 *
	 * @pre	aObserver != NULL
	 *
	 * @param aTimerId TimerId of the timer request
	 * @param aExpirationTime Time when the timer will expire
	 * @param aObserver When the timer expires, this object is used to inform
	 *	about it. Ownership isn't transferred.
	 * @param aTimerParam User specified value which is passed to expiration
	 *	handler when the timer expires. Ownership isn't transferred.
	 * @return New CTimerRequest object, ownership is transferred.
	 */
	static CTimerRequest* NewL(TTimerId aTimerId,
							   TTime aExpirationTime,
							   MExpirationHandler* aObserver,
							   TAny* aTimerParam);

	~CTimerRequest();
	
public: // New functions

	/**
	 * Compares the expiration times of two timer requests.
	 *
	 * @param aItem Timer request whose expiration time is compared with aItem2
	 * @param aItem2 Another timer request
	 * @return value
	 *	0  = expiration times are equal
	 *  1  = expiration time of aItem2 occurs before expiration time of aItem
	 *  -1 = expiration time of aItem occurs before expiration time of aItem2
	 */
	static TInt Compare(const CTimerRequest& aItem,
						const CTimerRequest& aItem2);

	/**
	 * Compares the TimerIds of two timer requests.
	 *
	 * @param aItem Timer request whose TimerId is compared with aItem2
	 * @param aItem2 Another timer request
	 * @return value ETrue if TimerIds are same, EFalse otherwise	 
	 */
	static TBool CompareId(const CTimerRequest& aItem,
						   const CTimerRequest& aItem2);

	/**
	 * Retrieves the TimerId of the timer request.
	 *	 
	 * @return value TimerId
	 */
	TTimerId TimerId() const;

	/**
	 * Set the TimerId value of the timer request.
	 *
	 * @param aTimerId Value for TimerId
	 */
	void SetTimerId(TTimerId aTimerId);

	/**
	 * Retrieves the expiration time of the timer request.	 
	 *	 
	 * @return value Time when the timer will expire
	 */
	TTime ExpirationTime() const;

	/**
	 * Retrieve the observer callback.
	 *
	 * @return value Expiration observer. Ownership is not transferred.
	 */
	MExpirationHandler* Observer();

	/**
	 * Retrieves the user specified value, given when the timer was created.
	 *
	 * @return value User specified timer parameter. Ownership isn't
     *  transferred.
	 */
	TAny* TimerParam();

private: // Constructors

	CTimerRequest();

	CTimerRequest(TTimerId aTimerId,
				  TTime aExpirationTime,
				  MExpirationHandler* aObserver,
				  TAny* aTimerParam);

private: // Data

	//Default key offset is zero. iTimerId is the search key.
	//TimerId is used for identifying the logical timer.
	TTimerId iTimerId;

	//Expiration time of the timer
	TTime iExpirationTime;

	//Callback to use when the timer expires. Not owned.
	MExpirationHandler* iObserver;

	//Optional parameter specified by the user when the timer was set.
	//If not specified, the value is NULL.
	//The interpretation of this pointer is left to the application.
	//CTimerRequest doesn't own the resource pointed by iTimerParam.
	TAny* iTimerParam;

private: // For testing purposes

#ifdef CPPUNIT_TEST	
	friend class CTimerManager_test;
	friend class CTimerStore_test;
	friend class CTimerRequest_test;
#endif
	};

#endif // C_TIMERREQUEST_H

// End of File
