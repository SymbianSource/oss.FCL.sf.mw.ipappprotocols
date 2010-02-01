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
* Name          : MIcmpReceiver.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __MICMPRECEIVER_H__
#define __MICMPRECEIVER_H__

#include <e32base.h>

class TInetAddr;


class MIcmpErrorObserver
	{
public:
	virtual ~MIcmpErrorObserver() {}
		
	/**
	 * An ICMP occurred using the specified remote address.
	 *
	 * @pre
	 * @post
	 *
	 * @param aAddress Remote address on which the error occurred
	 * @return value -
	 */
	virtual void IcmpError(const TInetAddr& aAddress) = 0;
	};

#endif // end of __MICMPRECEIVER_H__

// End of File
