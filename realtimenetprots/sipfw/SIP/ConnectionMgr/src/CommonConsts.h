/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : CommonConsts.h
* Part of       : ConnectionMgr
* Version       : SIP/5.0
*
*/




/**
 @internalComponent
*/


#ifndef __COMMONCONSTS_H__
#define __COMMONCONSTS_H__


const TUint KDefaultSipPort = 5060;

const TUint KDefaultSipsPort = 5061;

const TInt KMaxUdpMessageSize = 1300;

// Maximum UDP message length is 64 kB (RFC 768)
const TInt KMaxMessageLength = 64000;

const TUint32 KDefaultOwnerId = 0;

const TUint KProtocolTls = 31;

const TInt KMaxPortValue = 65535;
const TInt KMinPortValue = 1;

_LIT8( KParamrport, "rport" );


#endif // end of __COMMONCONSTS_H__

// End of File
