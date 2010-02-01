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
//



#include "SIPExResolverPlugin.h"

#include <apacmdln.h>

// SIPEx capabilities - could be also defined in resource of this plugin
// but then length of capablities text is in resource file limited to 
// 255 characters. However, defining capabilities in resource file's opaque_data 
// field is more efficient way.
_LIT8(KCapabilities,
"<SIP_CLIENT ALLOW_STARTING=\"YES\">\
<SIP_HEADERS>\
<ACCEPT value=\"application/sdp\"/>\
<ACCEPT value=\"SIPEx/InstantMessage\"/>\
</SIP_HEADERS>\
<SDP_LINES>\
<LINE name=\"m\" value=\"application 0 TCP SIPEx\"/>\
</SDP_LINES>\
</SIP_CLIENT>");

// -----------------------------------------------------------------------------
// CTTCNResolverPlugin1::NewL
// -----------------------------------------------------------------------------
//
CSIPExResolverPlugin* CSIPExResolverPlugin::NewL()
	{
	CSIPExResolverPlugin* self = new ( ELeave ) CSIPExResolverPlugin;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// -----------------------------------------------------------------------------
// CSIPExResolverPlugin::ConstructL
// -----------------------------------------------------------------------------
//
void CSIPExResolverPlugin::ConstructL()
	{
	const TUid KMyImplementationUid = { KSIPExResolverPluginUID };

	RImplInfoPtrArray infoArray;
	REComSession::ListImplementationsL( KSIPResolvedClientIFUid, infoArray );
	CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &infoArray ) );

	TBool found = EFalse;
	for ( TInt i=0; !found && i < infoArray.Count(); i++ )
		{
		CImplementationInformation* info = infoArray[ i ];
		if ( info->ImplementationUid() == KMyImplementationUid )
			{
			TLex8 lex( info->DataType() );
			TUint value( 0 );
			User::LeaveIfError( lex.Val( value, EHex ) );
		    iApplicationUID.iUid = value;
			found = ETrue;
			}
		}

	CleanupStack::PopAndDestroy( 1 ); // infoArray

	if ( !found )
		{
		User::Leave( KErrNotFound );
		}
		
	User::LeaveIfError( iApaSession.Connect() );
	User::LeaveIfError( iApaSession.GetAppInfo( iAppInfo, iApplicationUID ) );		
	}

// -----------------------------------------------------------------------------
// CSIPExResolverPlugin::~CSIPExResolverPlugin
// -----------------------------------------------------------------------------
//
CSIPExResolverPlugin::~CSIPExResolverPlugin()
	{
	iApaSession.Close();
	}
	
// -----------------------------------------------------------------------------
// CSIPExResolverPlugin::ChannelL
// -----------------------------------------------------------------------------
//	
TUid CSIPExResolverPlugin::ChannelL( RStringF /*aMethod*/,
    const TDesC8& /*aRequestUri*/,
    const RPointerArray<CSIPHeaderBase>& /*aHeaders*/,
    const TDesC8& /*aContent*/,
    const CSIPContentTypeHeader* /*aContentType*/)
    {
    return iApplicationUID;
    }


// -----------------------------------------------------------------------------
// CSIPExResolverPlugin::ConnectL
// -----------------------------------------------------------------------------
//
void CSIPExResolverPlugin::ConnectL( TUid aUid )
	{
	// Launch app based on uid passed from SIP stack
	//
	TApaAppInfo appInfo;
	User::LeaveIfError( iApaSession.GetAppInfo( appInfo, aUid ) );
	CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
#ifdef EKA2
	cmdLine->SetExecutableNameL( appInfo.iFullName );
#else
	cmdLine->SetLibraryNameL( appInfo.iFullName );
#endif
	User::LeaveIfError( iApaSession.StartApp( *cmdLine ) );
	CleanupStack::PopAndDestroy( cmdLine ); 
	}
	
// -----------------------------------------------------------------------------
// CTTCNResolverPlugin1::Capabilities
// -----------------------------------------------------------------------------
//
const TDesC8& CSIPExResolverPlugin::Capabilities()
    {
    return KCapabilities;
    }

// -----------------------------------------------------------------------------
// CSIPExResolverPlugin::ResetAndDestroy
// -----------------------------------------------------------------------------
//
void CSIPExResolverPlugin::ResetAndDestroy( TAny* anArray )
	{
    RImplInfoPtrArray* array = reinterpret_cast<RImplInfoPtrArray*>( anArray );
    if (array)
        {
        array->ResetAndDestroy();
        }
	}

