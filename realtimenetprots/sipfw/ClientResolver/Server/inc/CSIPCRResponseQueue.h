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
* Name          : CSIPCRResponseQueue.h
* Part of       : SIP Client Resolver
* Version       : 1.0 
*
*/




/**
 @internalComponent
*/
#ifndef CSIPCRRESPONSEQUEUE_H
#define CSIPCRRESPONSEQUEUE_H

// INCLUDES
#include <e32base.h>
#include "CSIPCRResponseItem.h"
#include "_sipcodecdefs.h"


class CSIPCRResponseQueue : public CBase
    {
public:
	
	CSIPCRResponseQueue ();
	
	~CSIPCRResponseQueue ();
	
	TBool IsEmpty () const;
	
	CSIPCRResponseItem& First();

	TInt AddLast (const CSIPCRResponseItem* aItem);

	void RemoveFirst ();

	void RemoveByRequestId (TUint32 aRequestId);
	
	void RemoveAll ();

private: // Data

    RPointerArray<CSIPCRResponseItem> iResponseItems;

private: // For testing purposes

	UNIT_TEST(CSIPCRResponseQueueTest)      
    };

#endif // CSIPCRRESPONSEQUEUE_H

// End of File
