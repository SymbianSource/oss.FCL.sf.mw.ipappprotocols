// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : NetworkInfo.cpp
// Part of     : ConnectionMgr
// Implementation
// Version     : SIP/6.0
//



#include "NetworkInfo.h"
#include "CNetworkManager.h"
#include "siprequest.h"
#include "sippaccessnetworkinfoheader.h"
#include "sipstrings.h"
#include "sipstrconsts.h"

// ----------------------------------------------------------------------------
// CNetworkInfo::NewL
// ----------------------------------------------------------------------------
//
CNetworkInfo* CNetworkInfo::NewL( CNetworkManager& aNetworkManager )
	{
	CNetworkInfo* self = new ( ELeave ) CNetworkInfo( aNetworkManager );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::CNetworkInfo
// ----------------------------------------------------------------------------
//
CNetworkInfo::CNetworkInfo( CNetworkManager& aNetworkManager ) :
	iNetworkManager( aNetworkManager )
	{
	const RStringF empty = SIPStrings::StringF( SipStrConsts::EEmpty );
	iAccessType = empty;
	iAccessInfoValue = empty;
	
	//If the plugin has not yet been loaded, the returned iInfo will be empty.
	//It doesn't matter, since the plugin will be loaded before any SIP request
	//can be sent, and at that point plugin will synchronously obtain current
	//info, and pass it to here.
	iInfo = iNetworkManager.RegisterAccessNetworkObserver( *this );	
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::ConstructL
// ----------------------------------------------------------------------------
//
void CNetworkInfo::ConstructL()
	{
	_LIT8( K3gppGeran, "3GPP-GERAN" );
	_LIT8( K3gppUtranFdd, "3GPP-UTRAN-FDD" );
	_LIT8( K3gppUtranTdd, "3GPP-UTRAN-TDD" );
	_LIT8( K3gppCdma2000, "3GPP-CDMA2000" );
	
	i3gppGeran = SIPStrings::Pool().OpenFStringL( K3gppGeran );
	i3gppUtranFdd = SIPStrings::Pool().OpenFStringL( K3gppUtranFdd );
	i3gppUtranTdd = SIPStrings::Pool().OpenFStringL( K3gppUtranTdd );
	i3gppCdma2000 = SIPStrings::Pool().OpenFStringL( K3gppCdma2000 );

	//This can't be done before i3gppGeran etc have been initialized
	TranslateInfoL();
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::~CNetworkInfo
// ----------------------------------------------------------------------------
//
CNetworkInfo::~CNetworkInfo()
	{
	iNetworkManager.DeregisterAccessNetworkObserver();
	iAccessType.Close();
	iAccessInfoValue.Close();
	i3gppGeran.Close();
	i3gppUtranFdd.Close();
	i3gppUtranTdd.Close();
	i3gppCdma2000.Close();
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::InfoChanged
// ----------------------------------------------------------------------------
//
void CNetworkInfo::InfoChanged( const TSIPAccessNetworkInfo& aInfo )
	{	
	iInfo = aInfo;	
	TRAP_IGNORE( TranslateInfoL() )
	}

// ---------------------------------------------------------------------------
// CNetworkInfo::HandleRequestL
// Add P-Access-Network-Info header when needed
// ----------------------------------------------------------------------------
//
void CNetworkInfo::HandleRequestL( CSIPRequest& aRequest,
								   TBool aIntegrityProtected )
	{
	//Delete possibly existing old header, since e.g. cell may have changed,
	//or a refreshed request may no longer be sent integrity protected etc.
	aRequest.DeleteHeaders(
		SIPStrings::StringF( SipStrConsts::EPAccessNetworkInfoHeader ) );

	if ( aIntegrityProtected )
		{		
		RStringF method = aRequest.Method();			
		if ( method != SIPStrings::StringF( SipStrConsts::EAck ) &&
			 method != SIPStrings::StringF( SipStrConsts::ECancel ) )
			{
			AddHeaderL( aRequest );
			}
		}
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::AddHeaderL
// ----------------------------------------------------------------------------
//
void CNetworkInfo::AddHeaderL( CSIPRequest& aRequest )
	{
	// Valid LAC and CellId are included also when iInfo.iAreaKnown is EFalse
	if ( iAccessType != SIPStrings::StringF( SipStrConsts::EEmpty ) )
		{
		CSIPPAccessNetworkInfoHeader* header =
			CSIPPAccessNetworkInfoHeader::NewLC( iAccessType );

		if ( iAccessType == i3gppGeran )
			{
			header->SetParamL( SIPStrings::StringF( SipStrConsts::ECgi3gpp ),
			 				   iAccessInfoValue );
			}
		if ( iAccessType == i3gppUtranFdd ||
			 iAccessType == i3gppUtranTdd ||
			 iAccessType ==	i3gppCdma2000 )
			{
			header->SetParamL(
				SIPStrings::StringF( SipStrConsts::EUtranCellId3gpp ),
			 	iAccessInfoValue );
			}

		aRequest.AddHeaderL( header );
		CleanupStack::Pop( header );
		}
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::TranslateInfoL
// ----------------------------------------------------------------------------
//
void CNetworkInfo::TranslateInfoL()
	{
	DetermineAccessType();
	DetermineAccessInfoL();
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::DetermineAccessType
// ----------------------------------------------------------------------------
//
void CNetworkInfo::DetermineAccessType()
	{
	iAccessType.Close();

	switch ( iInfo.iNetworkMode )
		{
		//GSM/GPRS or DCS1800 network
		case RMobilePhone::ENetworkModeGsm:			
			iAccessType = i3gppGeran.Copy();
			break;

		//CDMA (cdma2000) network.
		case RMobilePhone::ENetworkModeCdma2000:
			iAccessType = i3gppCdma2000.Copy();
			break;

		//WCDMA (UTRA Frequency Division Duplex (FDD)) network
		case RMobilePhone::ENetworkModeWcdma:
			iAccessType = i3gppUtranFdd.Copy();
			break;
	
		//TD-CDMA (UTRA Time Division Duplex (TDD)) network	
		case RMobilePhone::ENetworkModeTdcdma:
			iAccessType = i3gppUtranTdd.Copy();
			break;

		//AMPS network
		case RMobilePhone::ENetworkModeAmps:
		//CDMA (IS-95) network
		case RMobilePhone::ENetworkModeCdma95:
		//Other network modes
		case RMobilePhone::ENetworkModeUnknown:
		case RMobilePhone::ENetworkModeUnregistered:
		default:			
			iAccessType = SIPStrings::StringF( SipStrConsts::EEmpty );
		}
	}

// ----------------------------------------------------------------------------
// CNetworkInfo::DetermineAccessInfoL
// Valid LAC and CellId are included also when iInfo.iAreaKnown is EFalse.
// ----------------------------------------------------------------------------
//
void CNetworkInfo::DetermineAccessInfoL()
	{
	iAccessInfoValue.Close();

	if ( iAccessType == i3gppGeran ||
	     iAccessType == i3gppUtranFdd ||
		 iAccessType == i3gppUtranTdd ||
		 iAccessType == i3gppCdma2000 )
		{
		//Long enough to hold:
		//	MCC: 3 digits
		//  MNC: 2 or 3 digits (depending on MCC value)
		//  LAC: 4 hex digits (16 bits)
		//  Cell Identity: 4 or 7 hex digits (16 or 28 bits)
		const TInt KAccessInfoStringLength = 18;
		HBufC8* accessInfo = HBufC8::NewLC( KAccessInfoStringLength );		
		TPtr8 ptr = accessInfo->Des();		

		//MCC = TBuf<4> RMobilePhone::TMobilePhoneNetworkCountryCode;
		//MNC = TBuf<8> RMobilePhone::TMobilePhoneNetworkIdentity;

		ptr.Append( iInfo.iMobileCountryCode.Left( 3 ) );		
		ptr.Append( iInfo.iMobileNetworkCode.Left( 3 ) );

		//LAC is a 16 bit value
		const TUint KLacMask = 0xffff;
		//Include leading zeros
		_LIT8( K16BitFormat, "%04x" );
		ptr.AppendFormat( K16BitFormat, iInfo.iLocationAreaCode & KLacMask );

		if ( iAccessType == i3gppGeran )
			{
			//CI (Cell Identity) is a 16 bit value
			const TUint KCellIdMask = 0xffff;
			ptr.AppendFormat( K16BitFormat, iInfo.iCellId & KCellIdMask );
			}
		else
			{
			//UMTS Cell Identity is a 28 bit value
			const TUint KUmtsCellIdMask = 0xfffffff;
			_LIT8( KUmtsCellIdFormat, "%07x" );
			ptr.AppendFormat( KUmtsCellIdFormat,
							  iInfo.iCellId & KUmtsCellIdMask );
			}

		iAccessInfoValue = SIPStrings::Pool().OpenFStringL( *accessInfo );		
		CleanupStack::PopAndDestroy( accessInfo );
		}
	else
		{		
		iAccessInfoValue = SIPStrings::StringF( SipStrConsts::EEmpty );
		}
	}
