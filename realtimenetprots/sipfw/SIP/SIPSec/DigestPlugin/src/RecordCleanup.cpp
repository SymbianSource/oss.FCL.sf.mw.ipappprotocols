// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : RecordCleanup.cpp
// Part of       : SIPSec
// Version       : SIP/4.0 
//



#include "RecordCleanup.h"
#include "CSIPSecUserRecord.h"

// -----------------------------------------------------------------------------
// TRecordCleanup::TRecordCleanup
// -----------------------------------------------------------------------------
//
TRecordCleanup::TRecordCleanup( CSIPSecUserRecord& aRecord,
			    			    CSIPSecDigestObserver& aObserver ) :
    iRecord( aRecord ),
    iObserver( aObserver )
    {
    }
