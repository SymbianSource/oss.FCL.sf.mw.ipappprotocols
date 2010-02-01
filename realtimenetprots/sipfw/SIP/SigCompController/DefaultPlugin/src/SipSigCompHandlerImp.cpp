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
// Name          : SipSigCompHandlerImp.cpp
// Part of       : SIPSigComp
// Version       : SIP/3.0
//



#include <centralrepository.h>
#include <f32file.h>
#include <utf.h>
#include "sipprivatecrkeys.h"
#include "sigcomp.h"
#include "SipSigCompHandlerImp.h"
#include "sigcompstateitem.h"
#include "sipsigcompcompartmentctx.h"
#include "sipsigcompcompartmentuser.h"
#include "sigcompcompartment.h"
#include "sigcompvalueconverter.h"
#include "compressinglogic.h"
#include "sipresponse.h"
#include "siprequest.h"
#include "sipsigcomphandlerinitparams.h"
#include "SipAssert.h"
#include "sip.h"
#include "siperr.h"
#include "sipstrings.h"
#include "sipstrconsts.h"
#include "TSIPTransportParams.h"
#include "MLocalName.h"

const TInt CSipSigCompCompartmentCtx::iOffset = 
    _FOFF( CSipSigCompCompartmentCtx, iLink );
    
const TInt CSipSigCompCompartmentUser::iOffset = 
    _FOFF( CSipSigCompCompartmentUser, iLink );

_LIT( KSigCompStaticDictionaryFile, "\\private\\101f5d36\\sipsdp.dic" );

const TInt KMaxAlgorithmLength = 256; // A value that should be large enough


// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::NewL
// -----------------------------------------------------------------------------
//
CSipSigCompHandlerImpl* CSipSigCompHandlerImpl::NewL( TAny* aInitParams )
	{
	CSipSigCompHandlerImpl* self = CSipSigCompHandlerImpl::NewLC( aInitParams );
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::NewLC
// -----------------------------------------------------------------------------
//
CSipSigCompHandlerImpl* CSipSigCompHandlerImpl::NewLC( TAny* aInitParams )
	{
	CSipSigCompHandlerImpl* self = new ( ELeave ) CSipSigCompHandlerImpl(
		static_cast<TSIPSigCompHandlerInitParams*>( aInitParams ) );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::CSipSigCompHandlerImpl
// -----------------------------------------------------------------------------
//
CSipSigCompHandlerImpl::CSipSigCompHandlerImpl(
    TSIPSigCompHandlerInitParams* aInitParams ) : 
    iCompartmentList( CSipSigCompCompartmentCtx::iOffset ),
    iCompartmentUserList( CSipSigCompCompartmentUser::iOffset ),
	iLocalName( aInitParams->iLocalName )
	{
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::ConstructL
// -----------------------------------------------------------------------------
//
void CSipSigCompHandlerImpl::ConstructL()
	{
	iCompressingLogic = CSigCompCompressingLogic::NewL( *this );
	
	HBufC* algorithmBuf = HBufC::NewLC( KMaxAlgorithmLength );
	TPtr algorithm( algorithmBuf->Des() );
	TInt cyclesPerBit = 0;
	TInt stateMemSize = 0;
	TInt decompressionMemSize = 0;

    CRepository* repository = CRepository::NewLC( KCRUidSIP );
    User::LeaveIfError( repository->Get( KSIPSigCompCPB, cyclesPerBit ) );
    User::LeaveIfError( repository->Get( KSIPSigCompSMS, stateMemSize ) );
    User::LeaveIfError( repository->Get( KSIPSigCompDMS, decompressionMemSize ) );
    User::LeaveIfError( repository->Get( KSIPSigCompAlgorithm, algorithm ) );
    User::LeaveIfError( repository->Get( KSIPSigCompMaxCompartments,
                        iMaxCompartmentCount ) );    
    CleanupStack::PopAndDestroy( repository );
    
    HBufC8* algorithmBuf8 = HBufC8::NewLC( KMaxAlgorithmLength*2 );
    TPtr8 algorithm8( algorithmBuf8->Des() );
    TInt err = CnvUtfConverter::ConvertFromUnicodeToUtf8( algorithm8, algorithm );
    if ( err )
        {
        User::Leave( err );
        }
    CleanupStack::Pop( algorithmBuf8 );
    CleanupStack::PopAndDestroy( algorithmBuf );
    CleanupStack::PushL( algorithmBuf8 );
	
	TUint8 dictSHA1[] =
		{
		0xfb, 0xe5, 0x07, 0xdf, 0xe5, 0xe6, 0xaa, 0x5a, 0xf2, 0xab,
		0xb9, 0x14, 0xce, 0xaa, 0x05, 0xf9, 0x9c, 0xe6, 0x1b, 0xa5
		};

	TSigCompStateItem dict;
	dict.iStateIdentifier.Set( dictSHA1, sizeof( dictSHA1 ) );
	dict.iStateLength = 0x12e4;
	dict.iStateAddress = 0;
	dict.iStateInstruction = 0;
	dict.iMinimumAccessLength = 6;
	dict.iStateValue.Set( KSigCompStaticDictionaryFile );

	CSigComp::TMemorySize dmsIndex =
		TSigcompValueConverter::ConvertDecompressionMemorySize(
		    decompressionMemSize );
	CSigComp::TCyclesPerBit cpbIndex =
		TSigcompValueConverter::ConvertCyclesPerBit( cyclesPerBit );
	CSigComp::TMemorySize smsIndex =
		TSigcompValueConverter::ConvertStateMemorySize( stateMemSize );
		
	iSigComp = CSigComp::NewL( dmsIndex,
	                           cpbIndex,
	                           smsIndex,
	                           algorithm8,
	                           dict,
	                           EFalse );
	                          
	CleanupStack::PopAndDestroy( algorithmBuf8 );
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::~CSipSigCompHandlerImpl
// -----------------------------------------------------------------------------
//
CSipSigCompHandlerImpl::~CSipSigCompHandlerImpl()
	{
	delete iSigComp;
	RemoveAllCompartments();
	delete iCompressingLogic;
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::CompartmentCount
// -----------------------------------------------------------------------------
//
TInt CSipSigCompHandlerImpl::CompartmentCount()
	{
	TInt compartmentcount = 0;
	TSglQueIter<CSipSigCompCompartmentCtx> listIter( iCompartmentList );
	listIter.SetToFirst();
	while ( ( listIter++ ) != NULL )
		{
		compartmentcount++;
		}
	return compartmentcount;
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::IsSupported
// -----------------------------------------------------------------------------
//
TBool CSipSigCompHandlerImpl::IsSupported() const
	{
	return ETrue;
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::CreateCompartmentL
// -----------------------------------------------------------------------------
//
TUint32 CSipSigCompHandlerImpl::CreateCompartmentL( TUint32 aIapId )
    {
	return CreateCompartmentUserL( aIapId );
    }
   
    
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::RemoveCompartment
// -----------------------------------------------------------------------------
// 
void CSipSigCompHandlerImpl::RemoveCompartment( TUint32 aCompartmentId )
    {
    DeleteCompartmentUser( aCompartmentId );
    }

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::DecompressL
// -----------------------------------------------------------------------------
//
CBufBase* CSipSigCompHandlerImpl::DecompressL(
    const TDesC8& aMessage, 
    TUint& aBytesConsumed,
    TBool aIsStreamBased )
	{
	return iSigComp->DecompressL( aMessage, aBytesConsumed, aIsStreamBased );
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::EncodeL
// -----------------------------------------------------------------------------
//
CBufBase* CSipSigCompHandlerImpl::EncodeL(
    const TSIPTransportParams& aTransportParams,
    CSIPRequest& aRequest,
    const TInetAddr& aAddress,
    TBool aStreambasedProtocol,
    TBool& aCompressed )
	{
	return iCompressingLogic->EncodeL( aTransportParams, 
	                                   aRequest, 
	                                   aAddress,
						               aStreambasedProtocol,
						               aCompressed );
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::EncodeL
// -----------------------------------------------------------------------------
//
CBufBase* CSipSigCompHandlerImpl::EncodeL(
    const TSIPTransportParams& aTransportParams,
    CSIPResponse& aResponse,
    const TInetAddr& aAddress,
    TBool aStreambasedProtocol,
    TBool& aCompressed )
	{
	return iCompressingLogic->EncodeL( aTransportParams,
	                                   aResponse, 
	                                   aAddress,
					                   aStreambasedProtocol,
					                   aCompressed );
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::IsCompleteSigCompMessageL
// -----------------------------------------------------------------------------
//
TBool CSipSigCompHandlerImpl::IsCompleteSigCompMessageL(
    const TDesC8& aMessage ) const
	{
	return iSigComp->IsCompleteL( aMessage );
	}
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::IsSigCompMsg
// -----------------------------------------------------------------------------
//
TBool CSipSigCompHandlerImpl::IsSigCompMsg( const TDesC8& aMessage ) const
	{
	return iSigComp->IsSigCompMsg( aMessage );
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::AllowL
// -----------------------------------------------------------------------------
//
void CSipSigCompHandlerImpl::AllowL( const TInetAddr& aAddress, TUint32 aIapId )
	{
	CSipSigCompCompartmentCtx* compartment =
				SearchCompartmentByInetAddr( aAddress, aIapId );
	if ( !compartment )
		{
		iSigComp->Deny();
		}
	else
		{
		iSigComp->AllowL( *compartment->Compartment() );
		compartment->SetCurrentState( CSipSigCompCompartmentCtx::EAlive );
		}
	}
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::Deny
// -----------------------------------------------------------------------------
//
void CSipSigCompHandlerImpl::Deny()
	{
	iSigComp->Deny();
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::SendFailedL
// -----------------------------------------------------------------------------
//
void CSipSigCompHandlerImpl::SendFailedL( TUint32 aCompartmentId )
	{
	CSipSigCompCompartmentUser* compUser =
	    SearchCompartmentUserByCompId( aCompartmentId );
	                                   
	if ( compUser)
	    {
	    compUser->SendFailedL();
	    }
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::HasCompartmentUser
// -----------------------------------------------------------------------------
//
TBool CSipSigCompHandlerImpl::HasCompartmentUser( 
    const TSIPTransportParams& aTransportParams )
	{
	return ( !aTransportParams.IgnoreCompartmentId() && 
	         SearchCompartmentUserByCompId( aTransportParams.CompartmentId() ) );
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::CreateCompartmentId
// -----------------------------------------------------------------------------
//
TUint32 CSipSigCompHandlerImpl::CreateCompartmentId()
	{
	if ( iCompartmentIdCounter < KMaxTUint32 )
		{
		iCompartmentIdCounter++;
		}
	else
		{
		iCompartmentIdCounter = 1;
		}	
	return iCompartmentIdCounter;
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::CreateCompartmentUserL
// -----------------------------------------------------------------------------
//
TUint32 CSipSigCompHandlerImpl::CreateCompartmentUserL( TUint32 aIapId )
	{
	TUint32 compId = CreateCompartmentId();
	CSipSigCompCompartmentUser* compUser =
			CSipSigCompCompartmentUser::NewL( *this,
			                                  compId, 
			                                  aIapId );

	iCompartmentUserList.AddLast( *compUser );
	return compId;
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::DeleteCompartmentUser
// -----------------------------------------------------------------------------
//
TInt CSipSigCompHandlerImpl::DeleteCompartmentUser( TUint32 aCompartmentId )
	{
	CSipSigCompCompartmentUser* compUser = 
	    SearchCompartmentUserByCompId( aCompartmentId );

    if ( compUser )
        {
        iCompartmentUserList.Remove( *compUser );
        delete compUser;
        return KErrNone;
        }
        
	return KErrNotFound;
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::SearchCompartmentUserL
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentUser* CSipSigCompHandlerImpl::SearchCompartmentUserL(
    TUint32 aCompartmentId, 
    const TInetAddr& aAddress )
	{
	CSipSigCompCompartmentUser* compUser =
	    SearchCompartmentUserByCompId( aCompartmentId );

    if ( compUser && !compUser->Match( aAddress ) )
	    {
		// SetAddressL maps compartmentUser with existing compartment
		// having same remote address. if no matching compartment
		// can be found, totally new compartment is created and
		// compartmentUser is mapped to that
		compUser->SetAddressL( aAddress );
		}
	return compUser;
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::SearchCompartmentUserByCompId
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentUser* CSipSigCompHandlerImpl::SearchCompartmentUserByCompId(
    TUint32 aCompartmentId )
	{
    CSipSigCompCompartmentUser* listItem;
	TSglQueIter<CSipSigCompCompartmentUser> listIter( iCompartmentUserList );
	listIter.SetToFirst();

	while ( ( listItem = listIter++ ) != NULL )
		{
		if ( listItem->Match( aCompartmentId ) )
		    {
		    return listItem;
		    }
		}
	return 0;
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::RemoveAllCompartments
// -----------------------------------------------------------------------------
//
void CSipSigCompHandlerImpl::RemoveAllCompartments()
	{
	// Removing compartment users will lead to removal of compartments as well
	
	CSipSigCompCompartmentUser* listItem;
	TSglQueIter<CSipSigCompCompartmentUser> listIter( iCompartmentUserList );
	listIter.SetToFirst();

	while ( ( listItem = listIter++ ) != NULL )
		{
		iCompartmentUserList.Remove( *listItem );
		delete listItem;
		}
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::EncodeSipL
// -----------------------------------------------------------------------------
//
CBufBase* CSipSigCompHandlerImpl::EncodeSipL(
    CSIPResponse& aResponse,
	TBool aStreambasedProtocol )
	{
	if ( aStreambasedProtocol )
		{
		return aResponse.ToTextHeaderPartL();
		}
	else
		{
		return aResponse.ToTextL();
		}
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::EncodeSipL
// -----------------------------------------------------------------------------
//
CBufBase* CSipSigCompHandlerImpl::EncodeSipL(
    CSIPRequest& aRequest,
	TBool aStreambasedProtocol )
	{
	if ( aStreambasedProtocol )
		{
		return aRequest.ToTextHeaderPartL();
		}
	else
		{
		return aRequest.ToTextL();
		}
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::EncodeSipAndCompressL
// -----------------------------------------------------------------------------
//
CBufBase* CSipSigCompHandlerImpl::EncodeSipAndCompressL(
    const TSIPTransportParams& aTransportParams,
    CSIPResponse& aResponse,
    const TInetAddr& aAddress,
    TBool aStreambasedProtocol )
	{
	TUint32 tempCompUserId( 0 );
	
	CSipSigCompCompartmentUser* compUser = 
	    SearchCompartmentUserL( aTransportParams.CompartmentId(), aAddress );
	    
	if ( !compUser )
	    {
	    // Create temporary compartment
	    tempCompUserId = 
	        CreateCompartmentUserL( aTransportParams.IapId() );
	        
	    compUser = SearchCompartmentUserL( tempCompUserId, aAddress );
	    
	    __SIP_ASSERT_LEAVE( compUser, KErrNotFound );
	    }
	    
	CBufBase* data = aResponse.ToTextL();
	CleanupStack::PushL(data);
	
	__ASSERT_ALWAYS( compUser->Compartment(), User::Leave( KErrNotReady ) );
	
	CBufBase* compressed =
		compUser->Compartment()->CompressL( data->Ptr(0), 
		                                    aStreambasedProtocol );
		
	CleanupStack::PopAndDestroy(data);
	
	if ( tempCompUserId > 0 )
	    {
	    // Remove temporary compartment
    	RemoveCompartment( tempCompUserId );
	    }
	
	return compressed;
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::EncodeSipAndCompressL
// -----------------------------------------------------------------------------
//
CBufBase* CSipSigCompHandlerImpl::EncodeSipAndCompressL(
    const TSIPTransportParams& aTransportParams,
    CSIPRequest& aRequest,
    const TInetAddr& aAddress,
    TBool aStreambasedProtocol )
	{    
	CSipSigCompCompartmentUser* compUser =
						SearchCompartmentUserL( aTransportParams.CompartmentId(), 
						                        aAddress );
						                       
	__SIP_ASSERT_LEAVE( compUser, KErrNotFound );
	CBufBase* data = aRequest.ToTextL();
	CleanupStack::PushL( data );
	
	__ASSERT_ALWAYS( compUser->Compartment(), User::Leave( KErrNotReady ) );
	
	CBufBase* compressed =
		compUser->Compartment()->CompressL( data->Ptr(0), 
		                                    aStreambasedProtocol );
	CleanupStack::PopAndDestroy( data );
	return compressed;
	}

// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::KillMe
// -----------------------------------------------------------------------------
//
void CSipSigCompHandlerImpl::KillMe( CSipSigCompCompartmentCtx* aCompartmentCtx )
	{
	iCompartmentList.Remove( *aCompartmentCtx );
	delete aCompartmentCtx;
	}


// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::CreateCompartmentL
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentCtx* CSipSigCompHandlerImpl::CreateCompartmentL( 
    const TInetAddr& aAddress,
    TUint32 aIapId,
    TBool aDynamicCompression )
    {
    __ASSERT_ALWAYS(CompartmentCount() < iMaxCompartmentCount,
		User::Leave(KErrSIPMaxCompartmentsInUse));
		
    CSipSigCompCompartmentCtx* compCtx = 
        CSipSigCompCompartmentCtx::NewL( *this,
                                         *iSigComp,
                                         aAddress,
                                         aIapId,
                                         aDynamicCompression );
                                         
    iCompartmentList.AddLast( *compCtx );
    return compCtx;
    }
    
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::SearchCompartmentByInetAddr
// -----------------------------------------------------------------------------
//
CSipSigCompCompartmentCtx* CSipSigCompHandlerImpl::SearchCompartmentByInetAddr(
    const TInetAddr& aAddress,
    TUint32 aIapId )
	{
	CSipSigCompCompartmentCtx* listItem;
	TSglQueIter<CSipSigCompCompartmentCtx> listIter( iCompartmentList );
	listIter.SetToFirst();

	while ( ( listItem = listIter++ ) != NULL )
		{
		if ( listItem->Match( aAddress, aIapId ) )
			{
			return listItem;
			}
		}
	return 0;
	}
	
// -----------------------------------------------------------------------------
// CSipSigCompHandlerImpl::Match
// -----------------------------------------------------------------------------
//
TBool CSipSigCompHandlerImpl::Match( const TDesC8& aAddress )
	{
	return iLocalName.Match( aAddress );
	}

// End of File

