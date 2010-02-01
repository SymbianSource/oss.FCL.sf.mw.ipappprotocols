// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : CSIPClientResolver.cpp
// Part of       : SIP Client Resolver
// Version       : 1.0
//



// INCLUDES
#include "CSIPClientResolver.h"
#include "sdpcodecstringpool.h"
#include "siprequest.h"
#include "sipresponse.h"
#include "CSIPClientData.h"
#include "CSIPClientDataParser.h"
#include "CSipLaunchingStrategies.h"
#include "CleanupResetAndDestroy.h"
#include "SipResolvedClient.h"
#include "TSipClient.h"
#include "SIPCRLogs.h"
#include <e32math.h>
#include "CSIPClientResolver2.h"

_LIT(KWorkerThreadName, "SIPClientResolverWorker");


// ----------------------------------------------------------------------------
// CSIPClientResolver::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPClientResolver* CSIPClientResolver::NewL ()
    {
    CSIPClientResolver* self = new( ELeave ) CSIPClientResolver;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop (self);
    return self;
    }
    
// ----------------------------------------------------------------------------
// CSIPClientResolver::CSIPClientResolver
// ----------------------------------------------------------------------------
//
CSIPClientResolver::CSIPClientResolver()
    : CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::ConstructL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::ConstructL()
    {
    SdpCodecStringPool::OpenL();
    iECom = &(REComSession::OpenL());

    // Create XML parser for SIP client data
    iClientDataParser = CSIPClientDataParser::NewL();

    // Create arrays
    iRegistry = new( ELeave )RPointerArray< CSIPClientData >;

    // Fill implementations registry
    ListImplementationsL();

    // Create strategies
    iLaunchingStrategies = CSipLaunchingStrategies::NewL( *this );

    // Request notification from ECom when plugin registry changes
    NotifyOnChange();
    }
    
// ----------------------------------------------------------------------------
// CSIPClientResolver::~CSIPClientResolver
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPClientResolver::~CSIPClientResolver ()
    {
    delete iTmpPluginCaps;
    Cancel();
    delete iLaunchingStrategies;
    RemoveRegistry();
    delete iClientDataParser;
    if (iECom)
        {
        iECom->Close();
        }
    REComSession::FinalClose();
    SdpCodecStringPool::Close();
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::FindUidL
// ----------------------------------------------------------------------------
//
EXPORT_C CSIPResponse* CSIPClientResolver::FindUidL(
    const CSIPRequest& aRequest,
    RArray< TSipClient >& aUids )
    {
    SIP_CR_STR_LOG("CSIPClientResolver::FindUidL", aRequest.Method().DesC())

    CSIPRequest& request = const_cast<CSIPRequest&>( aRequest );
 	CSIPClientResolver2* clientresolver2 = NULL;
 	clientresolver2 = CSIPClientResolver2::NewLC(request);
    RefreshClientDataL();
    RArray< TUid > uids;
    CleanupClosePushL( uids );
    CopyAllUidsL ( uids );
    CSIPResponse* response = 
        iLaunchingStrategies->ApplyL( request, uids, *clientresolver2 );
    if( !response )
        {
        if ( !( clientresolver2 && 
        	    clientresolver2->GetSipClientDataL( aUids ) ) )
        	{
        	TInt uidCount = uids.Count();
        	for( TInt i = 0; i < uidCount; i++ )
            	{
            	MSipClient* client = GetByUID( uids[i] );
            	if( client )
                	{
                	TSipClient tmp( client->ClientUid(), 
                					client->AllowStarting(), 
                                	client->RomBased() );
                	aUids.AppendL( tmp );
                	}
            	}
        	}
        }
    else
        {
        SIP_CR_INT_LOG("CSIPClientResolver::FindUidL returns response",
                       response->ResponseCode())
        }    
	CleanupStack::PopAndDestroy(1); // uids
	CleanupStack::PopAndDestroy( clientresolver2 );
	
	for (TInt i=0; i < aUids.Count(); i++)
	    {
	    SIP_CR_INT_LOG("CSIPClientResolver::FindUidL matching client", 
	                   aUids[i].Uid().iUid)
	    }
	
	return response;
	}
	
// ----------------------------------------------------------------------------
// CSIPClientResolver::RefreshClientDataL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::RefreshClientDataL()
    {
    TInt err = KErrNone;
    TInt count = iRegistry->Count();
    for( TInt i = count-1; i >= 0; i-- )
        {
        CSIPClientData* client = (*iRegistry)[ i ];
        if( client->HasDynamicCapabilities() )
            {
            CSIPClientData* clone = client->CloneWithoutCapabilitiesLC();
            TRAP( err, ReloadClientDataL( *clone ) );
            if( err == KErrNone )
                {
                // Replace with the new client data
                (*iRegistry)[ i ] = clone;
                CleanupStack::Pop( clone );                
                }
            else if( err == KErrNoMemory )
                {
                User::Leave( err );
                }
            else
                {
                // Remove the invalid client
                iRegistry->Remove( i );
                CleanupStack::PopAndDestroy( clone );
                }
            delete client;
            }
        }    
    }	

// ----------------------------------------------------------------------------
// CSIPClientResolver::GetByUID
// ----------------------------------------------------------------------------
//
MSipClient* CSIPClientResolver::GetByUID( const TUid& aUid ) const
    {
    TInt count = iRegistry->Count();
    for( TInt i = 0; i < count; i++ )
        {
        MSipClient* client = (*iRegistry)[ i ];
        if( client->ClientUid() == aUid )
            {
            return client;
            }
        }
	return NULL;
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::DoCancel
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::DoCancel()
    {
    iECom->CancelNotifyOnChange( iStatus );	
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::RunL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::RunL()
    {
    // Update implementations registry, fail silently
    TRAP_IGNORE( ListImplementationsL() )

    // Request further notification from ECom when plugin registry changes
    NotifyOnChange();
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::ListImplementationsL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::ListImplementationsL()
    {
    // Create a temporary array to avoid destroying 
    // an existing registry if we run out of memory
    RPointerArray< CSIPClientData >* tmpRegistry =
            new( ELeave ) RPointerArray< CSIPClientData >;
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, tmpRegistry ) );

    // ROM clients
    TEComResolverParams romResolverParams;
	RImplInfoPtrArray romClients;
	REComSession::ListImplementationsL( KSIPResolvedClientIFUid,
	                                    romResolverParams,
	                                    KRomOnlyResolverUid,
	                                    romClients );
    CleanupResetAndDestroyPushL( romClients );  
    ConvertClientDataL( romClients, *tmpRegistry, ETrue );
    SIP_CR_INT_LOG("ROM-based plug-ins count", romClients.Count())
   
    // RAM clients     
	RImplInfoPtrArray allClients;
	REComSession::ListImplementationsL( KSIPResolvedClientIFUid, allClients );
    CleanupResetAndDestroyPushL( allClients );
    SIP_CR_INT_LOG("All plug-ins count", allClients.Count())
    RemoveDuplicates( romClients, allClients );
    ConvertClientDataL( allClients, *tmpRegistry, EFalse );
    
    CleanupStack::PopAndDestroy( 1 ); // allClients
    CleanupStack::PopAndDestroy( 1 ); // romClients

    RemoveRegistry();
    delete iRegistry;
    iRegistry = tmpRegistry;
    CleanupStack::Pop( 1 ); // tmpRegistry
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::RemoveRegistry
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::RemoveRegistry()
    {
    if( iRegistry )
        {
        iRegistry->ResetAndDestroy();
        delete iRegistry;
        iRegistry = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::NotifyOnChange
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::NotifyOnChange()
    {
    iECom->NotifyOnChange( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::CopyAllUidsL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::CopyAllUidsL ( RArray< TUid >& aUids )
    {
    TInt count = iRegistry->Count();
    for( TInt i = 0; i < count; i++ )
        {
        MSipClient* client = (*iRegistry)[ i ];
        aUids.AppendL( client->ClientUid() );	
        }
    }
  
// ----------------------------------------------------------------------------
// CSIPClientResolver::ConvertClientDataL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::ConvertClientDataL(
    const RImplInfoPtrArray& aImplInfo,
    RPointerArray< CSIPClientData >& aRegistry,
    TBool aRomClient )
    {
    TInt err = KErrNone;
    TInt clientCount = aImplInfo.Count();
    for( TInt i = 0; i < clientCount; i++ )
        {
        // Ignore clients with invalid data
        CImplementationInformation* info = aImplInfo[ i ];
        TRAP( err, AddClientDataL( aRegistry, *info, aRomClient ) );

        SIP_CR_STR_LOG("Plug-in with 'default_data'", info->DataType())
        SIP_CR_INT_LOG("Plug-in status", err)
        
        if( err == KErrNoMemory )
            {
            User::Leave( err );
            }
        }      
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::AddClientDataL
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::AddClientDataL( 
    RPointerArray< CSIPClientData >& aRegistry,
    CImplementationInformation& aInfo, 
    TBool aRomClient )
    {
	TLex8 lex( aInfo.DataType() );
    TUint32 uidValue( 0 );
    User::LeaveIfError( lex.Val( uidValue, EHex ) );
    TUid clientUid;
    clientUid.iUid = uidValue;     
    const TBool romBased = ( aRomClient && aInfo.RomBased() );
    TPtrC8 xml( aInfo.OpaqueData() );
    const TBool dynamicCaps = ( xml.Length() == 0 );
    
    CSIPClientData* clientData = 
        CSIPClientData::NewLC( aInfo.ImplementationUid(), clientUid,
                               romBased, dynamicCaps );    
    if ( !dynamicCaps )
        {
        // XML specified in resource-file.
        iClientDataParser->ParseL( clientData, xml );
        }
    aRegistry.AppendL( clientData );
    CleanupStack::Pop( clientData );
    }
    
// ----------------------------------------------------------------------------
// CSIPClientResolver::ReloadClientDataL
// ----------------------------------------------------------------------------
// 
void CSIPClientResolver::ReloadClientDataL( CSIPClientData& aClientData )
    {
    iTmpImplementationUid = aClientData.ImplementationUid();
    CreateWorkerThreadL();
    iClientDataParser->ParseL( &aClientData, *iTmpPluginCaps );
    delete iTmpPluginCaps;
    iTmpPluginCaps = NULL;
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::RemoveDuplicates
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::RemoveDuplicates( 
    const RImplInfoPtrArray& aRomInfo,
    RImplInfoPtrArray& aAllInfo )
    {
    TInt romInfoCount = aRomInfo.Count();
    for( TInt i=0; i<romInfoCount; i++ )
        {
        for( TInt j=aAllInfo.Count()-1; j>=0; j-- )
            {
            CImplementationInformation* info = aAllInfo[j];
            if( info->ImplementationUid() == aRomInfo[i]->ImplementationUid() )
                {
                aAllInfo.Remove( j );
                delete info;
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::ResetAndDestroy
// ----------------------------------------------------------------------------
//
void CSIPClientResolver::ResetAndDestroy( TAny* anArray )
    {
    RPointerArray< CSIPClientData >* array =
        reinterpret_cast<RPointerArray< CSIPClientData >*>( anArray );
    if (array)
        {
        array->ResetAndDestroy();
        delete array;
        }
    }
    
// ----------------------------------------------------------------------------
// CSIPClientResolver::CreateWorkerThreadL
// ----------------------------------------------------------------------------
//   
void CSIPClientResolver::CreateWorkerThreadL()
    {
    TName threadName(KWorkerThreadName);
    // Append a random number to make the name unique
    const TInt KThreadIdWidth = 10;
    threadName.AppendNumFixedWidthUC(Math::Random(), EHex, KThreadIdWidth);
    RThread thread;
    TInt err = thread.Create(threadName,
                             WorkerThreadFunction,
                             KDefaultStackSize,
                             NULL, // Use the same heap as the main thread
                             this);
    User::LeaveIfError(err);
    TRequestStatus status;
    thread.Logon(status);
    thread.Resume();
    User::WaitForRequest(status);
    TExitType exitType = thread.ExitType();
    thread.Close();
    if (exitType == EExitPanic)
        {
        User::Leave(KErrGeneral);
        }
    User::LeaveIfError(status.Int());
    }

// ----------------------------------------------------------------------------
// CSIPClientResolver::WorkerThreadFunction
// ----------------------------------------------------------------------------
//     
TInt CSIPClientResolver::WorkerThreadFunction(TAny* aPtr)
    {
    CSIPClientResolver* self = 
        reinterpret_cast<CSIPClientResolver*>(aPtr);
    TInt err = KErrNoMemory;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (cleanupStack)
        {
        TRAP(err, self->ReadPluginCapsL());
        REComSession::FinalClose(); // Needed for each thread separately
        }
    delete cleanupStack;
    return err;
    }
    
// ----------------------------------------------------------------------------
// CSIPClientResolver::ReadPluginCapsL
// ----------------------------------------------------------------------------
//  
void CSIPClientResolver::ReadPluginCapsL()
    {
    CSIPResolvedClient* plugin = 
        reinterpret_cast< CSIPResolvedClient* >( 
            REComSession::CreateImplementationL( 
                iTmpImplementationUid, 
                _FOFF( CSIPResolvedClient, iInstanceKey ) ) );
    CleanupStack::PushL( plugin );
    HBufC8* capsBuf = plugin->Capabilities().AllocL();
    CleanupStack::PopAndDestroy( plugin );
    delete iTmpPluginCaps;
    iTmpPluginCaps = capsBuf;
    }

// End of File

