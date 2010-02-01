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
* Name          : MCompartmentCtxUser.h
* Part of       : SIPSigComp
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MCOMPARTMENTCTXUSER_H__
#define __MCOMPARTMENTCTXUSER_H__

#include <e32std.h>


class MCompartmentCtxUser
	{
public:

    /**
     * Destructor.
	 */
	virtual ~MCompartmentCtxUser() {}

	/**
     * The compartment was deleted.
     *
     */
	virtual void CompartmentDeleted() = 0;
	};

#endif // __MCOMPARTMENTCTXUSER_H__

// End of File
