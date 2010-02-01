/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : SIPProfileStoragePrivateCRKeys.h
* Part of     : SIPServer
* Header file for SIPServer Private Cenrep Keys
* Version     : 1.0
* The definition of provate Central Repository keys to facilitate 
* the storgae and retrieval of profile settings from cenrep
*
*/




/**
 @internalComponent
 @released
*/

#ifndef SIPPROFILESTORAGEPRIVATECRKEYS_H
#define SIPPROFILESTORAGEPRIVATECRKEYS_H

const TUid      KCrUidSipProfileSrv = {0x101F413C};
const TUint32   KProfileIdMask      = 0xFF000000;
const TInt      KProfileIdShift     = 24;
const TUint32   KVariableIdMask     = 0x00FFF000;
const TInt      KVariableIdShift    = 12;
const TUint32   KArrayIndexMask     = 0x00000FFF;
const TInt      KMaxBufLength       = NCentralRepositoryConstants::KMaxUnicodeStringLength;
const TUint32	KProfileAndVarMask	= 0xFFFFF000;
const TUint32	KAllKeys			= 0;
const TUint32	KNoMask				= 0;
const TInt      KMaxProfileId       = 255;

const TUint32 KSIPMaxKeyValue 		= 4095;
const TUint32 KSIPExtProxyParam = KSIPMaxKeyValue - 1 ;
const TUint32 KSIPProfileName = KSIPMaxKeyValue - 2;
const TUint32 KSIPRegistrarExtParam = KSIPMaxKeyValue - 3;
const TUint32 KSIPExtensionParam = KSIPMaxKeyValue - 4;
const TUint32 KSIPProfileClass = KSIPMaxKeyValue - 5;
const TUint32 KSIPDynamicProxy = KSIPMaxKeyValue - 6;
const TUint32 KSIPExtensionIntParam = KSIPMaxKeyValue - 7;
const TUint32 KSipExtensionBoolParam = KSIPMaxKeyValue - 8;
const TUint32 KSIPExtensionDescParam = KSIPMaxKeyValue - 9;
const TUint32 KSIPExtensionDescArrayParam = KSIPMaxKeyValue - 10;
const TUint32 KSIPProxyDigestUserName = KSIPMaxKeyValue - 11;
const TUint32 KSIPProxyDigestRealm = KSIPMaxKeyValue - 12;
const TUint32 KSIPProxyDigestPassword = KSIPMaxKeyValue - 13;
const TUint32 KSIPRegistrarDigestUserName = KSIPMaxKeyValue - 14;
const TUint32 KSIPRegistrarDigestRealm = KSIPMaxKeyValue - 15; 
const TUint32 KSIPRegistrarDigestPassword = KSIPMaxKeyValue - 16;
const TUint32 KSIPProfileStorageId = KSIPMaxKeyValue - 17;  
const TUint32 KSIPContextId = KSIPMaxKeyValue - 18;
const TUint32 KSIPIsEnabled = KSIPMaxKeyValue - 19;

const TUint32 KSIPDefaultProfileID = 1;

#endif // SIPPROFILESTORAGEPRIVATECRKEYS_H
