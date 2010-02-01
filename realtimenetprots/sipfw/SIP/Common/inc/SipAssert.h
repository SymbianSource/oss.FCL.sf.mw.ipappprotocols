/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SipAssert.h
* Part of       : SIPCommon
* Version       : SIP/3.0 
*
*/




/**
 @internalComponent
*/


#ifndef __SIP_ASSERT__
#define __SIP_ASSERT__

#include <e32std.h>


#if defined(_DEBUG)

//Panic is used in debug builds, so the exact location where the assertion
//failed is caught early in testing.
#define __SIP_ASSERT_LEAVE(c, i) __ASSERT_ALWAYS(c, User::Panic(KNullDesC, i))

#define __SIP_ASSERT_RETURN(c, i) __ASSERT_ALWAYS(c, User::Panic(KNullDesC, i))

#define __SIP_ASSERT_RETURN_VALUE(c, i) __ASSERT_ALWAYS(c, User::Panic(KNullDesC, i))

#else

//For release builds, leave is used instead of panicking.
//Panic is avoided to keep the SIP stack running.
#define __SIP_ASSERT_LEAVE(c, i) __ASSERT_ALWAYS(c, User::Leave(i))

//Where leaving isn't possible, __SIP_ASSERT_RETURN can be used in place of
//__SIP_ASSERT_LEAVE
#define __SIP_ASSERT_RETURN(c, i) if (!(c)) return

#define __SIP_ASSERT_RETURN_VALUE(c, i) if (!(c)) return(i)

#endif


#endif
