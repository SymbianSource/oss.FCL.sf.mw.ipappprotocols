// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPQuerySrv.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include <stringpool.h>
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPQuerySrv.h"
#include "MSIPHostResolver.h"

_LIT8(KDot8, ".");
// ----------------------------------------------------------------------------
// CSIPQuerySrv::CSIPQuerySrv
// ----------------------------------------------------------------------------
//
CSIPQuerySrv::CSIPQuerySrv(TBool aSipsUri)
:CSIPQueryBase(), iSipsUri(aSipsUri)
	{
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPQuerySrv::ConstructL( const TDesC8& aTarget )
	{
 	SetTargetL( aTarget );
	}
	
// ----------------------------------------------------------------------------
// CSIPQuerySrv::NewL
// ----------------------------------------------------------------------------
//
CSIPQuerySrv* CSIPQuerySrv::NewL( const TDesC8& aTarget, TBool aSipsUri )
	{
	CSIPQuerySrv* self = new ( ELeave ) CSIPQuerySrv(aSipsUri);
	CleanupStack::PushL( self );
	self->ConstructL( aTarget );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::~CSIPQuerySrv
// ----------------------------------------------------------------------------
//
CSIPQuerySrv::~CSIPQuerySrv()
	{
	delete iTarget;
	iRecordArray.Reset();
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::SetTargetProtocolL
// ----------------------------------------------------------------------------
//
HBufC8* CSIPQuerySrv::SetTargetProtocolL( const TDesC8& aTarget,
										  RStringF aProtocol,
										  TBool aIsSipsUri )
	{
	_LIT8(Ksipudp, "_sip._udp.");
	_LIT8(Ksiptcp, "_sip._tcp.");
	_LIT8(Ksipstcp, "_sips._tcp.");
	TUint32 len = aTarget.Length() + Ksipudp().Length();
	if (aIsSipsUri)
		{
		len++;
		}
	HBufC8* temp =  HBufC8::NewLC( len );
	TPtr8 tempPtr = temp->Des();
	if (aIsSipsUri)
		{
		// If SIPS URI, ignore the value of aProtocol. and act always as
		// transport is TLS (Ksipstcp)
		tempPtr.Append( Ksipstcp );
		}

	else if ( aProtocol == SIPStrings::StringF( SipStrConsts::ETCP ) )
		{
		tempPtr.Append( Ksiptcp );
		}
		else if ( aProtocol == SIPStrings::StringF( SipStrConsts::EUDP ) )
			{
			tempPtr.Append( Ksipudp );
			}
			else
				{
				User::Leave( KErrNotSupported );
				}
	tempPtr.Append( aTarget );
	CleanupStack::Pop( temp );
	return temp;
	}
	
// ----------------------------------------------------------------------------
// CSIPQuerySrv::SetTargetTLSL
// ----------------------------------------------------------------------------
//
HBufC8* CSIPQuerySrv::SetTargetTLSL( const TDesC8& aTarget )
	{
	_LIT8(Ksiptls, "_sip._tls.");
	TUint32 len = aTarget.Length() + Ksiptls().Length();
	HBufC8* temp =  HBufC8::NewLC( len );
	TPtr8 tempPtr = temp->Des();
	tempPtr.Append( Ksiptls );
	tempPtr.Append( aTarget );
	CleanupStack::Pop( temp );
	return temp;
	}
	
// ----------------------------------------------------------------------------
// CSIPQuerySrv::AddL
// ----------------------------------------------------------------------------
//
void CSIPQuerySrv::AddL( TDnsRespSRV aSrv )
	{
	TLinearOrder<TDnsRespSRV> order( CSIPQuerySrv::Compare );
	User::LeaveIfError( iRecordArray.InsertInOrderAllowRepeats( aSrv, order ) );
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::SetTargetL
// ----------------------------------------------------------------------------
//
void CSIPQuerySrv::SetTargetL( const TDesC8& aTarget )
	{
	__ASSERT_ALWAYS ( aTarget.Length() > 0, User::Leave( KErrArgument ) );

	// KQueryDataMaxLength is same as TDnsQryData's max length in dns_qry.h
	const TInt KQueryDataMaxLength = 255; 	
	__ASSERT_ALWAYS ( aTarget.Length() < KQueryDataMaxLength, 
					User::Leave( KErrArgument ) );

	HBufC8* temp = aTarget.AllocL();
	delete iTarget;
	iTarget = temp;
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::RecordArray
// ----------------------------------------------------------------------------
//
RArray<TDnsRespSRV>& CSIPQuerySrv::RecordArray()
	{
	return iRecordArray;
	} 

// ----------------------------------------------------------------------------
// CSIPQuerySrv::SRVRecordL
// ----------------------------------------------------------------------------
//
TDnsRespSRV& CSIPQuerySrv::SRVRecordL( TInt aIndex )
	{
	__ASSERT_ALWAYS ( aIndex < iRecordArray.Count(), 
					  User::Leave( KErrArgument ) );
					  
	return iRecordArray[aIndex];
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::Compare
// ----------------------------------------------------------------------------
//
TInt CSIPQuerySrv::Compare( const TDnsRespSRV& aFirst,
							const TDnsRespSRV& aSecond )
	{
	if ( aFirst.Priority() < aSecond.Priority() )
		{
		return -1;
		}
	if ( aFirst.Priority() > aSecond.Priority() )
		{
		return 1;
		}
	if ( aFirst.Priority() == aSecond.Priority() )
		{
		if ( aSecond.Weight() == 0 )
			{
			return -1;
			}
		else
			{
			return 1;
			}
		}
	return 0;
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::QueryBuf
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPQuerySrv::QueryBuf()
	{
	TDnsQuery query( *iTarget, KDnsRRTypeSRV );
	TDnsQueryBuf querybuf( query );
	return iQueryBuf = querybuf;
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::QueryResultBuf
// ----------------------------------------------------------------------------
//
TDes8& CSIPQuerySrv::QueryResultBuf()
	{
	return iResult;
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::QueryResult
// ----------------------------------------------------------------------------
//
TDnsRespSRV CSIPQuerySrv::QueryResult()
	{
	return iResult();
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::Query
// ----------------------------------------------------------------------------
//
void CSIPQuerySrv::Query( MSIPHostResolver& aResolver )
	{
	aResolver.Resolver().Query( QueryBuf(), 
								QueryResultBuf(), 
								aResolver.RequestStatus() );	
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::HandleQueryResultL
// ----------------------------------------------------------------------------
//
void CSIPQuerySrv::HandleQueryResultL( MSIPHostResolver& aResolver )
	{
	if ( QueryResult().Target().CompareF( KDot8 ) != KErrNone )
		{
		AddL( QueryResult() );
		}
	
	while ( aResolver.Resolver().QueryGetNext( QueryResultBuf() ) 
												== KErrNone )
		{
		if ( QueryResult().Target().CompareF( KDot8 ) != KErrNone )
			{
			//Records are sorted in right order in inserting phase
			AddL( QueryResult() );
			}
		}
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::ResultTargetL
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPQuerySrv::ResultTargetL()
	{
	return SRVRecordL( 0 ).Target();
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::ResultPortL
// ----------------------------------------------------------------------------
//
TUint CSIPQuerySrv::ResultPortL() 
	{
	return SRVRecordL( 0 ).Port();
	}
	
// ----------------------------------------------------------------------------
// CSIPQuerySrv::RemoveElementL
// ----------------------------------------------------------------------------
//
void CSIPQuerySrv::RemoveElementL( TInt aIndex )
	{
	if ( aIndex < iRecordArray.Count() && aIndex >= 0 )
		{
		iRecordArray.Remove( aIndex );
		}
	}

// ----------------------------------------------------------------------------
// CSIPQuerySrv::ArrayCount
// ----------------------------------------------------------------------------
//
TInt CSIPQuerySrv::ArrayCount() 
	{
	return iRecordArray.Count();
	}
		
