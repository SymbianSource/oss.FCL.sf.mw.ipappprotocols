
// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



#include <e32std.h>                

// -----------------------------------------------------------------------------
// E32Dll() is the entry point to the application DLL
// DLL entry point. In EKA2 entry point is disabled.
// Returns: TInt:   Always KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
	{
    return KErrNone;
	}
#endif

// End of file
