/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipsigcomphandler.h
* Part of       : SIP SigComp Controller
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef __SIPSIGCOMPHANDLER_H__
#define __SIPSIGCOMPHANDLER_H__

#include <e32base.h>
#include <ecom/ecom.h>
#include "MSigCompController.h"


const TUid KSIPSigCompInterfaceUid = { 0x101FED8E };


class CSigCompHandler : public CBase, public MSigCompController
	{
    public:

	    virtual inline ~CSigCompHandler();

	public: // Data

		/// Unique key for implementations of this interface.
		TUid iInstanceKey;

    protected: // Constructors:

        inline CSigCompHandler();
	};

#include "sipsigcomphandler.inl"

#endif // end of __SIPSIGCOMPHANDLER_H__

// End of File
