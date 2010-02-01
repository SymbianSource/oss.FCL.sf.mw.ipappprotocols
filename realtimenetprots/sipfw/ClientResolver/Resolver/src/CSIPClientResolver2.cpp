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
// Name          : CSIPClientResolver2.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPClientResolver2.h"
#include "sipresolvedclient2.h"
#include "TSipClient.h"
#include "SIPCRLogs.h"
#include "sipclientresolverconfig.h"
#include "siprequest.h"
#include "sipuri.h"
#include "sipcodecutils.h"
#include "uricontainer.h"
#include "CleanupResetAndDestroy.h"
#include "CSipHeaderStrategyBase.h"

// ----------------------------------------------------------------------------
// CSIPClientResolver2::NewL
// ----------------------------------------------------------------------------
//
CSIPClientResolver2* CSIPClientResolver2::NewL ( CSIPRequest& aRequest )
    {
    SIP_CR_LOG("CSIPClientResolver2::NewL")
    CSIPClientResolver2* self = CSIPClientResolver2::NewLC ( aRequest );
    CleanupStack::Pop ( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSIPClientResolver2::NewLC
// ----------------------------------------------------------------------------
//
CSIPClientResolver2* CSIPClientResolver2::NewLC ( CSIPRequest& aRequest )
    {
    SIP_CR_LOG("CSIPClientResolver2::NewLC")
    CSIPClientResolver2* self = new( ELeave ) CSIPClientResolver2;
    CleanupStack::PushL( self );
    self->ConstructL( aRequest );
    return self;
    }
   
// ----------------------------------------------------------------------------
// CSIPClientResolver2::CSIPClientResolver2
// ----------------------------------------------------------------------------
//
CSIPClientResolver2::CSIPClientResolver2()
    {
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver2::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver2::ConstructL( CSIPRequest& aRequest )
    {
    iClientResolverConfig = CSipClientResolverConfig::NewL();
    UIDsForUserNameL( aRequest );
	LoadImplementationsL();
    }
    
// ----------------------------------------------------------------------------
// CSIPClientResolver2::~CSIPClientResolver2
// ----------------------------------------------------------------------------
//
CSIPClientResolver2::~CSIPClientResolver2 ()
    {
    SIP_CR_LOG("CSIPClientResolver2::~CSIPClientResolver2")
    iResolvedClients.ResetAndDestroy();
    iUids.Reset();
    iClientsData.Reset();
    delete iClientResolverConfig;
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver2::ClientsData
// ----------------------------------------------------------------------------
//
RArray< TSipClient >& CSIPClientResolver2::ClientsData()
    {
    SIP_CR_LOG("CSIPClientResolver2::ClientsData")
    return iClientsData;
    }    
    
// ----------------------------------------------------------------------------
// CSIPClientResolver2::GetSipClientDataL
// ----------------------------------------------------------------------------
//
TBool CSIPClientResolver2::GetSipClientDataL( RArray< TSipClient >& aUids )
    {
    SIP_CR_LOG("CSIPClientResolver2::GetSipClientDataL")
    TBool found(EFalse);

    if ( iClientsData.Count() > 0 )
    	{
    	ListImplementationsL();
    	for( TInt i = 0; i < iClientsData.Count(); ++i )
    		{
        	TSipClient tmp = iClientsData[i];
        	if ( tmp.PluginType() == TSipClient::EFirstGeneration )
        		{
        		found = EFalse;
        		return found;
        		}
        	found = ETrue;
        	aUids.AppendL( tmp );
        	}
    	}
    SIP_CR_INT_LOG("CSIPClientResolver2::GetSipClientDataL count:",iUids.Count())
    return found;
    }   
        
 // ----------------------------------------------------------------------------
// CSIPClientResolver2::FetchMatchingUidsL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver2::FetchMatchingUidsL( CSIPRequest& aRequest, 
								 			 CSipHeaderStrategyBase& aStrategy)
    {
    SIP_CR_LOG("CSIPClientResolver2::FetchMatchingUidsL")
    iClientsData.Reset();
    TBool match = EFalse;
    TUid clientUid;
    RPointerArray< CSIPResolvedClient2 > tempClients;
    CSIPClientResolver2::PushLC( &tempClients );
    for ( TInt i = iResolvedClients.Count()-1; i >= 0; --i )
        {
        CSIPResolvedClient2* client = iResolvedClients[i];
        match = aStrategy.MatchL( *client,aRequest,clientUid );
        if ( match )
            {
            SIP_CR_LOG("CSIPClientResolver2::FetchMatchingUidsL match")
            TSipClient temp( clientUid, client->ConnectSupported(), EFalse );
            temp.SetImplementationUid( client->iImplementationUid );
            iClientsData.AppendL( temp );
            match = EFalse;
            tempClients.AppendL( client );
            iResolvedClients.Remove( i );
            iResolvedClients.Compress();
            }
        }
    	 
    if ( tempClients.Count() )
    	{	
    	iResolvedClients.ResetAndDestroy();
    	for ( TInt i = tempClients.Count()-1; i >= 0; --i )
    		{
    		iResolvedClients.AppendL( tempClients[i] );
    		tempClients.Remove( i );
    		tempClients.Compress();
    		}
    	}	
    CleanupStack::Pop();//tempClients
    tempClients.Reset(); 
    }
    
// -----------------------------------------------------------------------------
// CSIPClientResolver2::PushLC
// -----------------------------------------------------------------------------
//
void CSIPClientResolver2::PushLC(RPointerArray<CSIPResolvedClient2>* aArray)
    {
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArray,aArray));
    }
  
// -----------------------------------------------------------------------------
// CSIPClientResolver2::ResetAndDestroyArray
// -----------------------------------------------------------------------------
//
void CSIPClientResolver2::ResetAndDestroyArray(TAny* anArray)
	{
    RPointerArray<CSIPResolvedClient2>* array =
        reinterpret_cast<RPointerArray<CSIPResolvedClient2>*>(anArray);
    if (array)
        {
        array->ResetAndDestroy();
        }
	}    
  
// ----------------------------------------------------------------------------
// CSIPClientResolver2::Clients
// ----------------------------------------------------------------------------
//
RPointerArray< CSIPResolvedClient2 >& CSIPClientResolver2::Clients()
    {
    SIP_CR_LOG("CSIPClientResolver2::Clients")
    return iResolvedClients;
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver2::UIDsForUserNameL
// ----------------------------------------------------------------------------
//
RArray <TUid> CSIPClientResolver2::UIDsForUserNameL( CSIPRequest& aRequest )
    {
    SIP_CR_LOG("CSIPClientResolver2::UIDsForUserNameL")
    iUids.Reset();
    if ( aRequest.RequestURI() )
    	{
    	if ( !aRequest.RequestURI()->IsSIPURI() )
    		{
    		User::Leave(KErrCorrupt);
    		}
    	iClientResolverConfig->GetUIDsForUserNameL(
    					aRequest.RequestURI()->SIPURI()->User(),iUids );
    	}
    SIP_CR_INT_LOG("CSIPClientResolver2::UIDsForUserNameL count:",iUids.Count())
    return iUids;
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver2::ListImplementationsL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver2::ListImplementationsL()
	{
	SIP_CR_LOG("CSIPClientResolver2::ListImplementationsL")
	RImplInfoPtrArray allClients;
    REComSession::ListImplementationsL( KSIPResolvedClient2IFUid, allClients );
    CleanupResetAndDestroyPushL( allClients );
    SIP_CR_INT_LOG("All new plug-ins count", allClients.Count())
    TInt err = KErrNone;

    for ( TInt i = 0; i < allClients.Count(); ++i )
	    {
	    CImplementationInformation* info = allClients[i];
	    TRAP(err,  AddClientDataL( *info ) );

	    if( err == KErrNoMemory )
            {
            User::Leave( err );
            }	    
	    }

    CleanupStack::PopAndDestroy( 1 ); // allClients
	}

// ----------------------------------------------------------------------------
// CSIPClientResolver2::AddClientDataL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver2::AddClientDataL( CImplementationInformation& aInfo )
	{
	SIP_CR_LOG("CSIPClientResolver2::AddClientDataL")       
    for ( TInt i = 0; i < iClientsData.Count(); ++i ) 
    	{
    	if ( iClientsData[i].ImplementationUid() == aInfo.ImplementationUid() )
    		{
    		SIP_CR_LOG("CSIPClientResolver2::AddClientDataL check rombased")
    		iClientsData[i].SetRomBased( aInfo.RomBased() );
    		iClientsData[i].SetPluginType( TSipClient::ESecondGeneration );
    		return; 
    		}
    	}
	}
	
// ----------------------------------------------------------------------------
// CSIPClientResolver2::LoadImplementationsL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver2::LoadImplementationsL()
	{
	SIP_CR_LOG("CSIPClientResolver2::LoadImplementationsL")
	CSIPResolvedClient2* resolvedClient2 = NULL;

	for ( TInt i = 0; i < iUids.Count(); ++i )
	    {
	    TRAPD( err, resolvedClient2 = reinterpret_cast< CSIPResolvedClient2* >( 
	        REComSession::CreateImplementationL( iUids[i],
	        			 _FOFF( CSIPResolvedClient2, iInstanceKey ) ) ) );
        if ( !err )
        	{
        	CleanupStack::PushL( resolvedClient2 );
        	resolvedClient2->iImplementationUid = iUids[i];
        	iResolvedClients.AppendL( resolvedClient2 );
        	CleanupStack::Pop( resolvedClient2 );
        	}
        err = 0;
	    }

	iUids.Reset();
	SIP_CR_INT_LOG("CSIPClientResolver2::LoadImplementationsL count:",
													iResolvedClients.Count())
	}


// End of File

