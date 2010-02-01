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
// Name          : TSipCSReceiveItem.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//




#include "TSipCSReceiveItem.h"
#include "sipmessage.h"

// -----------------------------------------------------------------------------
// TSipCSReceiveItem::TSipCSReceiveItem
// -----------------------------------------------------------------------------
//
TSipCSReceiveItem::TSipCSReceiveItem (const TSIPIds& aIds,
                                      CBufFlat* aSipMessageHeaders,
                                      HBufC8* aSipMessageContent,
                                      TBool aContainsSIPRequest,
                                      TInt aError)
 : iIds (aIds),
   iHeaders (aSipMessageHeaders),
   iContent (aSipMessageContent),
   iContainsSIPRequest (aContainsSIPRequest),
   iError (aError)
	{
	}

// -----------------------------------------------------------------------------
// TSipCSReceiveItem::TSipCSReceiveItem
// -----------------------------------------------------------------------------
//
TSipCSReceiveItem::TSipCSReceiveItem (const TSIPIds& aIds, TInt aError)
 : iIds (aIds),
   iHeaders (0),
   iContent (0),
   iContainsSIPRequest (EFalse),
   iError (aError)
	{
	}

// -----------------------------------------------------------------------------
// TSipCSReceiveItem::Headers
// -----------------------------------------------------------------------------
//
CBufFlat* TSipCSReceiveItem::Headers ()
    {
    return iHeaders;
    }
    
// -----------------------------------------------------------------------------
// TSipCSReceiveItem::Content
// -----------------------------------------------------------------------------
//
HBufC8* TSipCSReceiveItem::Content ()
    {
    return iContent;
    }

// -----------------------------------------------------------------------------
// TSipCSReceiveItem::BufSizes
// -----------------------------------------------------------------------------
//
TSIPMessageBufSizes TSipCSReceiveItem::BufSizes () const
    {
    TSIPMessageBufSizes sizes;
    if (iHeaders) 
        {
        sizes.iHeaderBufSize = iHeaders->Ptr(0).Length();
        }
    if (iContent)
        {
        sizes.iContentBufSize = iContent->Length();
        }
    return sizes;
    }

// -----------------------------------------------------------------------------
// TSipCSReceiveItem::Ids
// -----------------------------------------------------------------------------
//
const TSIPIds& TSipCSReceiveItem::Ids () const
	{
    return iIds;
	}
	
// -----------------------------------------------------------------------------
// TSipCSReceiveItem::ContainsSIPRequest
// -----------------------------------------------------------------------------
//	
TBool TSipCSReceiveItem::ContainsSIPRequest () const
	{
	return iContainsSIPRequest;	
	}

// -----------------------------------------------------------------------------
// TSipCSReceiveItem::Error
// -----------------------------------------------------------------------------
//
TInt TSipCSReceiveItem::Error () const
	{
    return iError;
	}
