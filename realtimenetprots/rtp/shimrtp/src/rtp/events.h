// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Header for class handling RTP callback  events
// 
//

/**
 @file
 @internalComponent
*/


#ifndef EVENTS_H
#define EVENTS_H

class MEventSource
	{
 public:
	virtual void HandleErrorL(TInt aError) = 0;
	virtual void CloseNow() = 0;
	virtual void CloseSession() = 0;
	virtual TInt WhatShouldClose() = 0;
	virtual ~MEventSource(){};
	};


/** A callback. */
class TRtpCallback
	{
 public:
	/** @param aFn The function to callback
		@param aPtr The value to pass to the aPtr argument of that function
		*/
	TRtpCallback(TRtpCallbackFunction aFn, TAny* aPtr);

	/** The callback function */
	TRtpCallbackFunction iFn;
	/** The parameter to pass to the aPtr argument of the function in iFn */
	TAny* iPtr;
	};

class TCallbackRegistration
	{
 public:
	TCallbackRegistration(TUint aType, TRtpCallback aCallback,
						  TUint32 aParameter);
	TUint iType;
	TRtpCallback iCallback;
	TUint32 iParameter;
	TUint32 iState;

	TBool ShouldEventFire(TRtpEvent& aEvent);

	inline TRtpEventType Type() const;

	inline TBool OneShot() const;
	};

class REventModel : public RArray<TCallbackRegistration>
	{
 public:
	void RegisterEventCallbackL(TUint aType, 
								TRtpCallbackFunction aCallback, 
								TAny* aPtr,
								TInt aParameter);
	void HandleEventL(TRtpEventType aEvent, TInt aStatus, TAny* aPtr);
	
	TInt RemoveCallback(TRtpEventType aType);

	TBool CanCloseNow();
	void CloseSession();
	TInt WhatShouldClose();
 private:
	enum TCloseStatus
		{
		ENormal,
		ECallingBack,
		EClosing,
		EClosingSession
		};
	TCloseStatus iCloseStatus;
	};

inline TRtpEventType TCallbackRegistration::Type() const 
	{
	return static_cast<TRtpEventType>(iType & ~ERtpOneShot);
	};

inline TBool TCallbackRegistration::OneShot() const 
	{ 
	return (iType & ERtpOneShot) != 0;
	};

const TInt KErrCloseSession = -17400;
const TInt KErrCloseCurrent = -17401;

#endif // EVENTS_H

