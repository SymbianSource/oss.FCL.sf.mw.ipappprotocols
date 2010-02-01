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
// class for handling rtp flow, initialization
// 
//

/**
 @file
*/

#include <e32base.h>
#include "rtp.h"
#include "rtputils.h"

_LIT(KRTP, "RTP");

#if !defined(EKA2)
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
	return KErrNone;
	}
#endif

GLDEF_C void Panic(TRtpPanicCode aPanicCode)
	{
	__RTP_LOG1(_L("User::Panic(KRTP, %d"), aPanicCode);
	User::Panic(KRTP, aPanicCode);
	}
