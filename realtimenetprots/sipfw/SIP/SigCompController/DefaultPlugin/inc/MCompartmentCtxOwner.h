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
* Name          : MCompartmentCtxOwner.h
* Part of       : SIP SigComp Controller
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef __MCOMPARTMENTCTXOWNER_H__
#define __MCOMPARTMENTCTXOWNER_H__

#include <e32base.h>

class CSipSigCompCompartmentCtx;

class MCompartmentCtxOwner
	{
public:
	virtual ~MCompartmentCtxOwner() {}

	virtual void KillMe(CSipSigCompCompartmentCtx* aCompartmentCtx) = 0;
	};

#endif // end of __MCOMPARTMENTCTXOWNER_H__

// End of File
