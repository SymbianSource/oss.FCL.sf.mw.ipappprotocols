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
// Name          : CSipCSReceiveQueue.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//




#include "CSipCSReceiveQueue.h"
#include "sipmessage.h"

// -----------------------------------------------------------------------------
// CSipCSReceiveQueue::CSipCSReceiveQueue
// -----------------------------------------------------------------------------
//
CSipCSReceiveQueue::CSipCSReceiveQueue ()
	{
    iReceiveItems.Reset();
	}	

// -----------------------------------------------------------------------------
// CSipCSReceiveQueue::~CSipCSReceiveQueue
// -----------------------------------------------------------------------------
//
CSipCSReceiveQueue::~CSipCSReceiveQueue ()
	{
	while (iReceiveItems.Count() > 0) 
        {
        RemoveFirst();
        }
	iReceiveItems.Close();
	}

// -----------------------------------------------------------------------------
// CSipCSReceiveQueue::IsEmpty
// -----------------------------------------------------------------------------
//
TBool CSipCSReceiveQueue::IsEmpty () const
	{ 
	return (iReceiveItems.Count() == 0); 
	}

// -----------------------------------------------------------------------------
// CSipCSReceiveQueue::First
// -----------------------------------------------------------------------------
//
TSipCSReceiveItem& CSipCSReceiveQueue::First ()
	{
    __ASSERT_ALWAYS (iReceiveItems.Count() > 0, 
                     User::Panic(KNullDesC, KErrGeneral));
    return iReceiveItems[0];
	}

// -----------------------------------------------------------------------------
// CSipCSReceiveQueue::AddLast
// -----------------------------------------------------------------------------
//
TInt CSipCSReceiveQueue::AddLast (const TSipCSReceiveItem& aItem)
    {
	return iReceiveItems.Append(aItem);
    }
    
// -----------------------------------------------------------------------------
// CSipCSReceiveQueue::RemoveFirst
// -----------------------------------------------------------------------------
//
void CSipCSReceiveQueue::RemoveFirst ()
	{
	if (iReceiveItems.Count() > 0) 
        {
        TSipCSReceiveItem& item = iReceiveItems[0];
        delete item.Headers();
        delete item.Content();
	    iReceiveItems.Remove(0);
        }
	}
	
// -----------------------------------------------------------------------------
// CSipCSReceiveQueue::RemoveSIPRequestsWith
// -----------------------------------------------------------------------------
//
void CSipCSReceiveQueue::RemoveSIPRequestsWith (TTransactionId aTransactionId)
	{
	for (TInt i=iReceiveItems.Count()-1; i>=0; i--)
		{
		TSipCSReceiveItem& item = iReceiveItems[i];
		if (item.Ids().iRequestId ==  aTransactionId && 
		    item.ContainsSIPRequest())
			{
        	delete item.Headers();
        	delete item.Content();
	    	iReceiveItems.Remove(i);
			}
		}
	}
