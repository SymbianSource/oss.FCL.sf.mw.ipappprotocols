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
* Name          : CSipCSReceiveQueue.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPCSRECEIVEQUEUE_H
#define CSIPCSRECEIVEQUEUE_H

#include <e32base.h>
#include "TSipCSReceiveItem.h"
#include "SipStackServerDefs.h"
#include "_sipcodecdefs.h"

class CSIPMessage;


class CSipCSReceiveQueue : public CBase
	{
public:
	
    CSipCSReceiveQueue ();
    ~CSipCSReceiveQueue ();

    TBool IsEmpty() const;
    
    TSipCSReceiveItem& First ();

    TInt AddLast (const TSipCSReceiveItem& aItem);
    
    void RemoveFirst ();
    
    void RemoveSIPRequestsWith (TTransactionId aTransactionId);

private: // Data

    RArray<TSipCSReceiveItem> iReceiveItems;

private: // For testing purposes

	UNIT_TEST(CSipCSReceiveQueueTest)
	UNIT_TEST(CSipCSReceiverBaseTest)
	};

#endif // CSIPCSRECEIVEQUEUE_H

// End of File
