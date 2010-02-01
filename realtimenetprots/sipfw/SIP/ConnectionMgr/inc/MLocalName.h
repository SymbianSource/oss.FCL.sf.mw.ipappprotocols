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
* Name          : MLocalName.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MLOCALNAME_H__
#define __MLOCALNAME_H__

#include <e32base.h>

class MLocalName
	{
public:
	virtual ~MLocalName() {}

	virtual TBool Match( const TDesC8& aAddress ) = 0;
	};

#endif // end of __MLOCALNAME_H__

// End of File
