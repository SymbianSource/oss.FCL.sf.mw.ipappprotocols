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
// Name          : CSIPQueryNaptr.cpp
// Part of       : ServerResolver
// Version       : SIP/4.0 
//



#include <stringpool.h>
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "CSIPQueryNaptr.h"
#include "MSIPHostResolver.h"

_LIT8(Ks, "s");
_LIT8(KServiceSIP, "SIP+*");
_LIT8(KServiceSIPS, "SIPS+*");
_LIT8(KStarPlusD2T, "*+D2T");
_LIT8(KStarPlusD2TPlusStar, "*+D2T+*");
_LIT8(KStarPlusD2U, "*+D2U");
_LIT8(KStarPlusD2UPlusStar, "*+D2U+*");

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::NewL
// ----------------------------------------------------------------------------
//
CSIPQueryNaptr* CSIPQueryNaptr::NewL( const TDesC8& aTarget, TBool aSipsUri )
	{
	CSIPQueryNaptr* self = new ( ELeave ) CSIPQueryNaptr(aSipsUri);
	CleanupStack::PushL( self );
	self->ConstructL( aTarget );
	CleanupStack::Pop( self );
	return self;
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::QueryBuf
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPQueryNaptr::QueryBuf()
	{
	TDnsQuery query( *iTarget, KDnsRRTypeNAPTR );
	TDnsQueryBuf querybuf( query );
	return iQueryBuf = querybuf;
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::QueryResultBuf
// ----------------------------------------------------------------------------
//
TDes8& CSIPQueryNaptr::QueryResultBuf()
	{
	return iResult; 
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::QueryResult
// ----------------------------------------------------------------------------
//
TDnsRespNAPTR CSIPQueryNaptr::QueryResult()
	{
	return iResult();
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::~CSIPQueryNaptr
// ----------------------------------------------------------------------------
//
CSIPQueryNaptr::~CSIPQueryNaptr()
	{
	delete iTarget;
	if ( iRecordArray )
		{
		iRecordArray->ResetAndDestroy();
		delete iRecordArray;
		}
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::AddL
// ----------------------------------------------------------------------------
//
void CSIPQueryNaptr::AddL( TDnsRespNAPTR* aNaptr )
	{
	TLinearOrder<TDnsRespNAPTR> order( CSIPQueryNaptr::Compare );
	User::LeaveIfError( iRecordArray->InsertInOrderAllowRepeats( aNaptr,
																 order ) );
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::Compare
// ----------------------------------------------------------------------------
//
TInt CSIPQueryNaptr::Compare( const TDnsRespNAPTR& aFirst, 
							  const TDnsRespNAPTR& aSecond )
	{
	if ( aFirst.Order() < aSecond.Order() )
		{
		return -1;
		}
	if ( aFirst.Order() > aSecond.Order() )
		{
		return 1;
		}
	if ( aFirst.Order() == aSecond.Order() )
		{
		if ( aFirst.Pref() == aSecond.Pref() )
			{
			if ( NaptrProtocol( aFirst ) ==
				 SIPStrings::StringF( SipStrConsts::EUDP ) )
				{
				return -1;
				}
			else if ( NaptrProtocol( aSecond ) ==
					  SIPStrings::StringF( SipStrConsts::EUDP ) )
				{
				return 1;
				}
				else
					{
					return 0;
					}
			}
		if ( aFirst.Pref() < aSecond.Pref() )
			{
			return -1;
			}
		if ( aFirst.Pref() > aSecond.Pref() )
			{
			return 1;
			}
		}
	return 0;
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::NAPTRRecordL
// ----------------------------------------------------------------------------
//
TDnsRespNAPTR& CSIPQueryNaptr::NAPTRRecordL( TInt aIndex )
	{
	__ASSERT_ALWAYS ( aIndex < iRecordArray->Count(), User::Leave( KErrArgument ) );
	return ( *( *iRecordArray )[aIndex] );
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::RemoveElementL
// ----------------------------------------------------------------------------
//
void CSIPQueryNaptr::RemoveElementL( TInt aIndex )
	{
	if ( aIndex < iRecordArray->Count() && aIndex >= 0 )
		{
		TDnsRespNAPTR* naptr = ( *iRecordArray )[aIndex];
		delete naptr;
		naptr = NULL;
		iRecordArray->Remove( aIndex );
		}
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::MatchToRulesL
// ----------------------------------------------------------------------------
//
TBool CSIPQueryNaptr::MatchToRulesL( const TDnsRespNAPTR& aNaptr )
	{	
	if ( aNaptr.Flags().CompareF( Ks ) !=0 )
		{
		return EFalse;
		}
		
	if ( aNaptr.Regexp().Length() != 0 )
		{
		return EFalse;
		}
		
	HBufC8* temp = aNaptr.Replacement().AllocL();
	TPtr8 ptr = temp->Des();
	ptr.Trim();
	if ( temp->Length() == 0 )
		{
		delete temp;
		return EFalse;
		}
	delete temp;
	
	if ( aNaptr.Service().Match( KServiceSIPS ) != KErrNotFound )
		{
		if ( NaptrProtocol( aNaptr ) ==
			 SIPStrings::StringF( SipStrConsts::ETLS ) )
			{
			return ETrue;
			}
		}
	
	if (!iSipsUri)
		{
		if ( aNaptr.Service().Match( KServiceSIP ) != KErrNotFound )
			{
			if ( NaptrProtocol( aNaptr ) !=
				 SIPStrings::StringF( SipStrConsts::EEmpty ) )
				{
				return ETrue;
				}
			}
		}
	
	return EFalse;
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::RecordArray
// ----------------------------------------------------------------------------
//
RPointerArray<TDnsRespNAPTR>& CSIPQueryNaptr::RecordArray()
	{
	return *iRecordArray;
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::NaptrProtocol
// ----------------------------------------------------------------------------
//
RStringF CSIPQueryNaptr::NaptrProtocol( const TDnsRespNAPTR& aNaptr )
	{	
	if ( aNaptr.Service().Match( KStarPlusD2T ) != KErrNotFound ||
		aNaptr.Service().Match( KStarPlusD2TPlusStar ) != KErrNotFound )
		{
		if (aNaptr.Service().Match( KServiceSIPS ) != KErrNotFound)
			{
			return SIPStrings::StringF( SipStrConsts::ETLS );
			}
		return SIPStrings::StringF( SipStrConsts::ETCP );
		}
	
	if( aNaptr.Service().Match( KStarPlusD2U ) != KErrNotFound ||
		aNaptr.Service().Match( KStarPlusD2UPlusStar ) != KErrNotFound )
		{
		return SIPStrings::StringF( SipStrConsts::EUDP );
		}
	
	return SIPStrings::StringF( SipStrConsts::EEmpty );
	}


// ----------------------------------------------------------------------------
// CSIPQueryNaptr::Query
// ----------------------------------------------------------------------------
//
void CSIPQueryNaptr::Query( MSIPHostResolver& aResolver )
	{
	aResolver.Resolver().Query( QueryBuf(),
							    QueryResultBuf(),
								aResolver.RequestStatus() );	
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPQueryNaptr::ConstructL( const TDesC8& aTarget )
	{
	// KQueryDataMaxLength is same as TDnsQryData's max length in dns_qry.h
	const TInt KQueryDataMaxLength = 255; 	
 
	__ASSERT_ALWAYS ( aTarget.Length() < KQueryDataMaxLength, 
					  User::Leave( KErrArgument ) );
					  
	__ASSERT_ALWAYS ( aTarget.Length() > 0, User::Leave( KErrArgument ) );


	iTarget = aTarget.AllocL();
	iRecordArray = new ( ELeave ) RPointerArray<TDnsRespNAPTR>; 
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::CSIPQueryNaptr
// ----------------------------------------------------------------------------
//
CSIPQueryNaptr::CSIPQueryNaptr(TBool aSipsUri)
:CSIPQueryBase(), iSipsUri(aSipsUri)
	{
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::HandleQueryResultL
// ----------------------------------------------------------------------------
//
void CSIPQueryNaptr::HandleQueryResultL( MSIPHostResolver& aResolver )
	{
	TDnsRespNAPTR* naptr = NULL;
	if ( MatchToRulesL( QueryResult() ) )
		{
		naptr = new ( ELeave ) TDnsRespNAPTR();
		CleanupStack::PushL( naptr );
		naptr->SetFlags( iResult().Flags() );
		naptr->SetOrder( iResult().Order() );
		naptr->SetPref( iResult().Pref() );
		naptr->SetRegexp( iResult().Regexp() );
		naptr->SetReplacement( iResult().Replacement() );
		naptr->SetService( iResult().Service() );
		AddL( naptr );
		CleanupStack::Pop( naptr );
		}
	while ( aResolver.Resolver().QueryGetNext( QueryResultBuf() ) 
												== KErrNone )
		{
		if ( MatchToRulesL( QueryResult() ) )
			{
			naptr = new ( ELeave ) TDnsRespNAPTR();
			CleanupStack::PushL( naptr );
			naptr->SetFlags( iResult().Flags() );
			naptr->SetOrder( iResult().Order() );
			naptr->SetPref( iResult().Pref() );
			naptr->SetRegexp( iResult().Regexp() );
			naptr->SetReplacement( iResult().Replacement() );
			naptr->SetService( iResult().Service() );
			AddL( naptr );
			CleanupStack::Pop( naptr );
			}
		}
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::ResultTargetL
// ----------------------------------------------------------------------------
//
const TDesC8& CSIPQueryNaptr::ResultTargetL()
	{
	return NAPTRRecordL( 0 ).Replacement();
	}

// ----------------------------------------------------------------------------
// CSIPQueryNaptr::ResultProtocolL
// ----------------------------------------------------------------------------
//
RStringF CSIPQueryNaptr::ResultProtocolL() 
	{
	return NaptrProtocol( NAPTRRecordL( 0 ) );
	}
	
// ----------------------------------------------------------------------------
// CSIPQueryNaptr::ArrayCount
// ----------------------------------------------------------------------------
//
TInt CSIPQueryNaptr::ArrayCount() 
	{
	return iRecordArray->Count();
	}
