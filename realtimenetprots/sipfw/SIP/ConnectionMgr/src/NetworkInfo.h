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
* Name          : NetworkInfo.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef __NETWORKINFO_H__
#define __NETWORKINFO_H__

// INCLUDES
#include <etelmm.h>
#include <stringpool.h>
#include "sipnetworkinfoobserver.h"
#include "sipaccessnetworkinfo.h"

// FORWARD DECLARATIONS
class CNetworkManager;
class CSIPRequest;


// CLASS DEFINITION
/**
 * Observes the network mode and 
 */
class CNetworkInfo :
	public CBase,
	public MSIPNetworkInfoObserver
	{
public: // Constructors and destructor
	static CNetworkInfo* NewL(CNetworkManager& aNetworkManager);

	~CNetworkInfo();

public: // From MSIPNetworkInfoObserver
	void InfoChanged( const TSIPAccessNetworkInfo& aInfo );

public: // New functions
	void HandleRequestL( CSIPRequest& aRequest,
						 TBool aIntegrityProtected );

private:
	CNetworkInfo(CNetworkManager& aNetworkManager);

	void ConstructL();

	void TranslateInfoL();

	void DetermineAccessType();

	void DetermineAccessInfoL();

	void AddHeaderL( CSIPRequest& aRequest );

private: // Data
	//Source where the network info is retrieved
	CNetworkManager& iNetworkManager;

	//Copy of the access network info, obtained from a plugin
	TSIPAccessNetworkInfo iInfo;

	//Network model. Updated on reception of 
	//MSIPNetworkInfoObserver::InfoChanged
	RStringF iAccessType;
	
	//Computed on reception of MSIPNetworkInfoObserver::InfoChanged
	RStringF iAccessInfoValue;

	RStringF i3gppGeran;
	RStringF i3gppUtranFdd;
	RStringF i3gppUtranTdd;
	RStringF i3gppCdma2000;	

private: // For unit tests	
#ifdef CPPUNIT_TEST
	friend class CNetworkInfoStub;
	friend class CNetworkInfoTest;
#endif
	};


#endif // end of __NETWORKINFO_H__

// End of File
