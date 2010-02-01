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
// Name          : CSIPCRResponseQueue.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPCRResponseQueue.h"

// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::CSIPCRResponseQueue
// -----------------------------------------------------------------------------
//
CSIPCRResponseQueue::CSIPCRResponseQueue ()
	{
    iResponseItems.Reset ();
	}	

// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::~CSIPCRResponseQueue
// -----------------------------------------------------------------------------
//
CSIPCRResponseQueue::~CSIPCRResponseQueue ()
    {
   	iResponseItems.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::IsEmpty
// -----------------------------------------------------------------------------
//
TBool CSIPCRResponseQueue::IsEmpty () const
    {
	return (iResponseItems.Count () == 0); 
    }

// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::First
// -----------------------------------------------------------------------------
//
CSIPCRResponseItem& CSIPCRResponseQueue::First ()
    {
    __ASSERT_ALWAYS (iResponseItems.Count () > 0, 
                     User::Panic(KNullDesC, KErrGeneral));
    return *(iResponseItems[0]);
    }

// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::AddLast()
// -----------------------------------------------------------------------------
//
TInt CSIPCRResponseQueue::AddLast  (const CSIPCRResponseItem* aItem)
    {
 	return iResponseItems.Append (aItem);
    }

// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::RemoveFirst
// -----------------------------------------------------------------------------
//
void CSIPCRResponseQueue::RemoveFirst  ()
    {
    if (iResponseItems.Count () > 0) 
        {
        CSIPCRResponseItem* item = iResponseItems[0];
        iResponseItems.Remove (0);
        delete item;
        }
    }

// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::RemoveByRequestId
// -----------------------------------------------------------------------------
//
void CSIPCRResponseQueue::RemoveByRequestId (TUint32 aRequestId)
    {
    for (TInt i=iResponseItems.Count()-1; i >= 0; i--)
        {
        CSIPCRResponseItem* item = iResponseItems[i];
        if (aRequestId == item->RequestId())
            {
            iResponseItems.Remove(i);
            delete item;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPCRResponseQueue::RemoveAll
// -----------------------------------------------------------------------------
//    
void CSIPCRResponseQueue::RemoveAll ()
    {
    iResponseItems.ResetAndDestroy();
    }

// End of File
