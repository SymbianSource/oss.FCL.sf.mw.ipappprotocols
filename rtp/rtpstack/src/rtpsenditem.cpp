/*
* Copyright (c) 2002-2003 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




#include "rtpsenditem.h"

const TInt CRtpSendItem::iOffset = _FOFF(CRtpSendItem,iLink);

CRtpSendItem* CRtpSendItem::NewL(const TDesC8& aPacket, TRequestStatus& aStatus)
	{
	CRtpSendItem* self = CRtpSendItem::NewLC( aPacket, aStatus );
	CleanupStack::Pop( self );
	return self;
	}

CRtpSendItem* CRtpSendItem::NewLC(const TDesC8& aPacket, TRequestStatus& aStatus)
	{
	CRtpSendItem* self=new(ELeave) CRtpSendItem( aStatus );
	CleanupStack::PushL(self);
	self->ConstructL(aPacket);
	return self;
	}

CRtpSendItem* CRtpSendItem::NewL( const TDesC8& aPacket, 
    TRequestStatus& aStatus, TRequestStatus& aClientStatus )
    {
    CRtpSendItem* self = CRtpSendItem::NewLC( aPacket, aStatus, aClientStatus );
    CleanupStack::Pop( self );
    return self;
    }

CRtpSendItem* CRtpSendItem::NewLC( const TDesC8& aPacket, 
    TRequestStatus& aStatus, TRequestStatus& aClientStatus )
    {
    CRtpSendItem* self=new(ELeave) CRtpSendItem( aStatus, aClientStatus );
    CleanupStack::PushL(self);
    self->ConstructL( aPacket );
    return self;
    }

void CRtpSendItem::ConstructL( const TDesC8& aPacket )
	{
	iPacket = aPacket.AllocL();
	}

CRtpSendItem::CRtpSendItem(TRequestStatus& aStatus)
: iStatus( aStatus )
	{}

CRtpSendItem::CRtpSendItem( TRequestStatus& aStatus, 
    TRequestStatus& aClientStatus )
: iStatus( aStatus ), iClientStatus( &aClientStatus )
    {}

CRtpSendItem::~CRtpSendItem()
	{
	delete iPacket;
	iPacket=NULL;
	}

const TDesC8& CRtpSendItem::GetData()
	{
	return *iPacket;
	}

TRequestStatus* CRtpSendItem::ClientStatus()
    {
    return iClientStatus;
    }


