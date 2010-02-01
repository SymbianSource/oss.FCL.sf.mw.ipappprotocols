/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipaccessnetworkinfo.h
* Part of     : SIP NetworkMonitor
* See class definition below.
* Version     : SIP/6.0 
*
*/



#ifndef TSIPACCESSNETWORKINFO_H
#define TSIPACCESSNETWORKINFO_H

// INCLUDES
#include <e32base.h>
#include <etelmm.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Container for access network information
*/
class TSIPAccessNetworkInfo 
	{
public: // Constructors and destructor
	inline TSIPAccessNetworkInfo();
	
	inline TSIPAccessNetworkInfo(
		TPckgBuf<RMobilePhone::TMobilePhoneNetworkInfoV2>& aNetworkInfoPckg,
		RMobilePhone::TMobilePhoneLocationAreaV1& aLocationArea );

	inline ~TSIPAccessNetworkInfo();

public: // New functions
	inline TBool operator==(const TSIPAccessNetworkInfo& aNetInfo) const;

public: // Data
	RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;

	// MCC (Mobile Country Code)
	RMobilePhone::TMobilePhoneNetworkCountryCode iMobileCountryCode;
	// MNC (Mobile Network Code)
	RMobilePhone::TMobilePhoneNetworkIdentity iMobileNetworkCode;


	// NokiaTSY includes valid LAC and CellId also when iAreaKnown is EFalse.
	TBool iAreaKnown;
	// LAC (Location Area Code) (GSM/WCDMA)
	TUint iLocationAreaCode;
	// CI (Cell identity code) (GSM/WCDMA)
	TUint iCellId;
	};

#include "sipaccessnetworkinfo.inl"

#endif // TSIPACCESSNETWORKINFO_H
