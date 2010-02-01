/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class for manipulating profiles and their attributes
*
*/


// INCLUDE FILES
#include    <escapeutils.h>
#include    <delimitedpathsegment8.h> 
#include    <sipmanagedprofile.h>
#include    <StringLoader.h>
#include 	<uriutils.h>
#include    <sipstrings.h>
#include    <sipaddress.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include	<e32const.h>
#include    "gssipprofileutil.h"
#include    "tgssipserverdata.h"
#include    "tsipsettingsdata.h"
#include    "gssippluginlogger.h"

const TUint32 KDefaultSNAPIdentifier = KMaxTUint32;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::GSSIPProfileUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
GSSIPProfileUtil::GSSIPProfileUtil()
    {
    __GSLOGSTRING("GSSIPProfileUtil::GSSIPProfileUtil" )
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::CopyProfileAttributesL
// Copies profile attributes from temp data structure to profile
// -----------------------------------------------------------------------------
//
void GSSIPProfileUtil::CopyProfileAttributesL( 
    CSIPManagedProfile* aProfile, 
    const TSIPSettingsData& aProfileData )
    {
    __GSLOGSTRING("GSSIPProfileUtil::CopyProfileAttributesL Start" )
    // Service provider name
    HBufC8* providerName = 
        EscapeUtils::ConvertFromUnicodeToUtf8L( aProfileData.iProviderName );
    CleanupStack::PushL( providerName );
    User::LeaveIfError( 
    	aProfile->SetParameter( KSIPProviderName, providerName->Des() ) );
    CleanupStack::PopAndDestroy( providerName ) ;
    
    // Service profile
    aProfile->SetType( aProfileData.iServiceProfile );      
    
    if ( aProfileData.iSNAPIndex != KDefaultSNAP )
        {
        // SNAP index
        User::LeaveIfError( aProfile->SetParameter( 
        KSIPSnapId, (TUint32)aProfileData.iSNAPIndex ) );
        }
    else
        {
        // AP index
        User::LeaveIfError( aProfile->SetParameter( 
            KSIPAccessPointId, (TUint32)aProfileData.iAPIndex ) );
        }
    
    // Publib username / address of record
    if ( aProfileData.iUsername.Length() )
        {
        TBool protocolFound( EFalse );
        
        TBuf<KSIPProtocolHeaderLength + 1> sipHeader( KSIPProtocolHeader );
        sipHeader.Append( KSIPColonSeparator );
        TBuf<KSIPSProtocolHeaderLength + 1> sipsHeader( KSIPSProtocolHeader );
        sipsHeader.Append( KSIPColonSeparator );
        
        EscapeUtils::TEscapeMode mode = EscapeUtils::EEscapeNormal;
        HBufC8* conv = 
            EscapeUtils::ConvertFromUnicodeToUtf8L( aProfileData.iUsername );
        CleanupStack::PushL( conv );
        HBufC8* enc = EscapeUtils::EscapeEncodeL( conv->Des(), mode );
        CleanupStack::PushL( enc );
        
        // Set public username if "sip:" was found.
        if ( aProfileData.iUsername.Length() > KSIPProtocolHeaderLength )
            {
            TBuf<KSIPProtocolHeaderLength + 1> lowerCase;
            lowerCase.Copy( aProfileData.iUsername.Left( 
                KSIPProtocolHeaderLength + 1 ) );
            lowerCase.LowerCase();
            
            if ( !sipHeader.Compare( lowerCase ) )
                {
                User::LeaveIfError( 
                	aProfile->SetParameter( KSIPUserAor, enc->Des() ) );
                protocolFound = ETrue;
                }
            }
        
        // Set public username if "sips:" was found.
        if ( aProfileData.iUsername.Length() > KSIPSProtocolHeaderLength )
            {
            TBuf<KSIPSProtocolHeaderLength + 1> lowerCase;
            lowerCase.Copy( aProfileData.iUsername.Left( 
                KSIPSProtocolHeaderLength + 1 ) );
            lowerCase.LowerCase();
                
            if ( !sipsHeader.Compare( lowerCase ) )
                {
                User::LeaveIfError( 
                	aProfile->SetParameter( KSIPUserAor, enc->Des() ) );
                protocolFound = ETrue;
                }
            }
        
        // Set scheme and public username
        if ( !protocolFound )
            {
            TInt length = enc->Length() + sipHeader.Length();
            HBufC8* username = HBufC8::NewLC( length );
            username->Des().Append( sipHeader );
            username->Des().Append( enc->Des() );
                
            // Cut some characters if length exceeds max. username length.
			if ( length > KMaxUsernameLength )
            	{
                username->Des().Delete( 
                    KMaxUsernameLength, length - KMaxUsernameLength );
                }

            User::LeaveIfError(
            	aProfile->SetParameter( KSIPUserAor, username->Des() ) );
            CleanupStack::PopAndDestroy( username );
            }
        
        CleanupStack::PopAndDestroy( enc );
        CleanupStack::PopAndDestroy( conv );    
        }
    else
       	{
       	User::LeaveIfError( 
               	aProfile->SetParameter( KSIPUserAor, KNullDesC8 ) );
       	}
        
    // Compression setting
    TBool compression = ( aProfileData.iCompression == EGSComprYes );
    User::LeaveIfError( aProfile->SetParameter( KSIPSigComp, compression ) );
    
    // Registration mode
    if ( aProfileData.iRegistrationMode == EGSWhenNeeded )
    	{
    	User::LeaveIfError( 
    		aProfile->SetParameter( KSIPAutoRegistration, EFalse ) );
    	}
    else
    	{
    	User::LeaveIfError( 
    		aProfile->SetParameter( KSIPAutoRegistration, ETrue ) );
    	}
    
    // Security negotiation
    TBool security = ( aProfileData.iSecurity == EGSSecurityOn );
    User::LeaveIfError( 
    	aProfile->SetParameter( KSIPSecurityNegotiation, security ) );
    __GSLOGSTRING("GSSIPProfileUtil::CopyProfileAttributesL End" )
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::CopyServerAttributesL
// Copies server attributes from temp data structure to profile
// -----------------------------------------------------------------------------
//
void GSSIPProfileUtil::CopyServerAttributesL(
    CSIPManagedProfile* aProfile,
    TUint32 aServer,
    const TGSSIPServerData& aProfileData )
    {
    __GSLOGSTRING("GSSIPProfileUtil::CopyServerAttributesL Start" )
    TInt err( KErrNone );
    EscapeUtils::TEscapeMode mode = EscapeUtils::EEscapeNormal;
    
    // Digest realm
    HBufC8* realm =
        EscapeUtils::ConvertFromUnicodeToUtf8L( aProfileData.iRealm );
    CleanupStack::PushL( realm );
    HBufC8* encRealm = EscapeUtils::EscapeEncodeL( realm->Des(), mode ); 
    ReplaceEscapedSpace( encRealm );
    err = aProfile->SetParameter( 
        aServer, KSIPDigestRealm, encRealm->Des() );
    delete encRealm;
    CleanupStack::PopAndDestroy( realm );
    User::LeaveIfError( err );

    // Username        
    HBufC8* uname =
        EscapeUtils::ConvertFromUnicodeToUtf8L( aProfileData.iUsername ); 
    CleanupStack::PushL( uname );
    HBufC8* encUname = EscapeUtils::EscapeEncodeL( uname->Des(), mode );    
    err = aProfile->SetParameter( 
        aServer, KSIPDigestUserName, encUname->Des() );
    delete encUname;
    CleanupStack::PopAndDestroy( uname );
    User::LeaveIfError( err );
    
    // Password
    if ( aProfileData.iPasswordChanged )
	    {
	    HBufC8* passw =
	        EscapeUtils::ConvertFromUnicodeToUtf8L( aProfileData.iPassword );
	    CleanupStack::PushL( passw );    
	    err = aProfile->SetParameter( 
	        aServer, KSIPDigestPassword, passw->Des() );    
	    CleanupStack::PopAndDestroy( passw );
	    User::LeaveIfError( err );	    	
	    }
    
    // Address
    GSSIPProfileUtil::CopyServerURIL( aProfile, aServer, aProfileData );
    __GSLOGSTRING("GSSIPProfileUtil::CopyServerAttributesL End" )
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::CopyToTempStructL
// Copies profile attributes from profile to temp data struct
// -----------------------------------------------------------------------------
//
void GSSIPProfileUtil::CopyToTempStructL(
    CSIPManagedProfile* aProfile,
    TSIPSettingsData& aProfileData )
    {
    __GSLOGSTRING("GSSIPProfileUtil::CopyToTempStructL Start" )
    // Profile ID
    TUint32 id( KErrNone );
    aProfile->GetParameter( KSIPProfileId, id );
    aProfileData.iID = id;
    
    // Service provider name
    const TDesC8* providerName;
    aProfile->GetParameter( KSIPProviderName, providerName );
    HBufC* providerName16 = 
        EscapeUtils::ConvertToUnicodeFromUtf8L( *providerName );    
    aProfileData.iProviderName.Copy( 
    	providerName16->Des().LeftTPtr( KMaxSIPProviderNameLength ) );
    delete providerName16;
    
    // Profile type    
    aProfileData.iServiceProfile = aProfile->Type();
    
    // Access Point ID
    aProfile->GetParameter( KSIPAccessPointId, id );
    aProfileData.iAPIndex = id;
    
    // Destination ID
    TInt err( KErrNone );
    err = aProfile->GetParameter( KSIPSnapId, id );
    if ( !err && ( static_cast<TInt>(id) != KErrNotFound) )
        {
        aProfileData.iSNAPIndex = id;
        }
    else
    	{
        aProfileData.iSNAPIndex = KDefaultSNAP;
    	}
    
    // Public username / address of record
    const TDesC8* userName;
    aProfile->GetParameter( KSIPUserAor, userName );
    HBufC8* decodedUsername = EscapeUtils::EscapeDecodeL( *userName );
    CleanupStack::PushL( decodedUsername );
    HBufC* userName16 =
        EscapeUtils::ConvertToUnicodeFromUtf8L( decodedUsername->Des() );
    aProfileData.iUsername.Copy( 
    	userName16->Des().LeftTPtr( KMaxUsernameLength ) );    
    delete userName16;
    CleanupStack::PopAndDestroy( decodedUsername );
    
     // Registration mode
    TBool mode;
    aProfile->GetParameter( KSIPAutoRegistration, mode );
    if ( mode )
        {
        aProfileData.iRegistrationMode = EGSAlwaysOn;
        }
    else
        {
        aProfileData.iRegistrationMode = EGSWhenNeeded;
        }

    // Signal compression
    TBool compression;
    aProfile->GetParameter( KSIPSigComp, compression );
    if( compression )
        {
        aProfileData.iCompression = EGSComprYes;
        }
    else
        {
        aProfileData.iCompression = EGSComprNo;
        }
    
    // Security negotiation
    TBool negotiation;
    aProfile->GetParameter( KSIPSecurityNegotiation,negotiation );
    if( negotiation )
        {
        aProfileData.iSecurity = EGSSecurityOn;
        }
    else
        {   
        aProfileData.iSecurity = EGSSecurityOff;
        }
    
    // Private user identity
    const TDesC8* privateIdentity;
    aProfile->GetParameter( KSIPPrivateIdentity, privateIdentity );
    HBufC8* decodedIdentity = EscapeUtils::EscapeDecodeL( *privateIdentity );
    CleanupStack::PushL( decodedIdentity );
    HBufC* privateIdentity16 =
        EscapeUtils::ConvertToUnicodeFromUtf8L( decodedIdentity->Des() );
    aProfileData.iRegistrar.iPrivateIdentity.Copy( 
    	privateIdentity16->Des().LeftTPtr( KMaxServerPrivateIdentityLength ) );
    delete privateIdentity16;
    CleanupStack::PopAndDestroy( decodedIdentity );
    __GSLOGSTRING("GSSIPProfileUtil::CopyToTempStructL End" )
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::CopyServerSettingsToTempStructL
// Copies server attributes from profile to temp data struct
// -----------------------------------------------------------------------------
//
void GSSIPProfileUtil::CopyServerSettingsToTempStructL(
    CSIPManagedProfile* aProfile,
    TUint32 aServer,
    TGSSIPServerData& aProfileData )
    {
    __GSLOGSTRING("GSSIPProfileUtil::CopyServerSettingsToTempStructL Start" )
    SIPStrings::OpenL();

    // Digest realm
    const TDesC8* realm;
    TInt err = aProfile->GetParameter( aServer, KSIPDigestRealm, realm );
    User::LeaveIfError( err );
    
    HBufC8* decodedRealm = EscapeUtils::EscapeDecodeL( *realm );
    CleanupStack::PushL( decodedRealm );
    HBufC* realm16 =
        EscapeUtils::ConvertToUnicodeFromUtf8L( decodedRealm->Des() );
    aProfileData.iRealm.Copy( realm16->Des().LeftTPtr( KMaxRealmLength ) );
    delete realm16;
    CleanupStack::PopAndDestroy( decodedRealm );
    
    // Username
    const TDesC8* uName;
    err = aProfile->GetParameter( aServer, KSIPDigestUserName, uName );
    User::LeaveIfError( err );
    
    HBufC8* decodedUname = EscapeUtils::EscapeDecodeL( *uName );
    CleanupStack::PushL( decodedUname );
    HBufC* uName16 =
            EscapeUtils::ConvertToUnicodeFromUtf8L( decodedUname->Des() );
    aProfileData.iUsername.Copy( 
    	uName16->Des().LeftTPtr( KMaxServerUsernameLength ) );
    delete uName16;
    CleanupStack::PopAndDestroy( decodedUname );
    
    // Server address and other params
    const TDesC8* uri;
    err = aProfile->GetParameter( aServer, KSIPServerAddress, uri );
    User::LeaveIfError( err );
    
    if ( !uri->Length() )
        {
        // Set default settings if URI is empty.
        aProfileData.iAddress.Zero();
        aProfileData.iPortNumber = KDefaultPortNumber;
        aProfileData.iTransportProtocol = EGSTranspProtocolAuto;
        if ( aServer == KSIPOutboundProxy )
            {
            aProfileData.iLooseRouting = EGSLooseRoutingOn;
            }
        }
    else
        {
        // Parse uri
        TUriParser8 parser;
        User::LeaveIfError( parser.Parse( *uri ) );
        aProfileData.iAddress.Zero();
        
        // Scheme
        if ( parser.IsPresent( EUriScheme ) )
            {
            const TDesC8& scheme = parser.Extract( EUriScheme );
            HBufC* scheme16 = EscapeUtils::ConvertToUnicodeFromUtf8L(scheme);
            TPtr trimmedScheme ( scheme16->Des().LeftTPtr(
            	KMaxProxyLength-aProfileData.iAddress.Length() ) );
            aProfileData.iAddress.Append( trimmedScheme );
            aProfileData.iAddress.Append( KSIPColonSeparator );
            delete scheme16;
            }
        else
            {
            aProfileData.iAddress.Append( KSIPProtocolHeader );
            aProfileData.iAddress.Append( KSIPColonSeparator );
            }
        
        // Host
        if ( parser.IsPresent( EUriHost ) )
            {
            const TDesC8& host = parser.Extract( EUriHost );
            UriUtils::TUriHostType type = UriUtils::HostType( host );
            if ( type == UriUtils::EIPv6Host )
                {
                HBufC8* ipv6Host8 = HBufC8::NewLC( host.Length() + 2 );
                ipv6Host8->Des().Copy( host );
                GSSIPProfileUtil::AddBrackets( ipv6Host8->Des() );
                HBufC* ipv6Host16 = 
                    EscapeUtils::ConvertToUnicodeFromUtf8L( *ipv6Host8 );
	            TPtr trimmedHost ( ipv6Host16->Des().LeftTPtr(
	            	KMaxProxyLength-aProfileData.iAddress.Length() ) );
                aProfileData.iAddress.Append( trimmedHost );
                delete ipv6Host16;
                CleanupStack::PopAndDestroy( ipv6Host8 );
                }
            else
                {
                HBufC* ipv4Host16 = 
                    EscapeUtils::ConvertToUnicodeFromUtf8L( host );
	            TPtr trimmedHost ( ipv4Host16->Des().LeftTPtr(
	            	KMaxProxyLength-aProfileData.iAddress.Length() ) );
                aProfileData.iAddress.Append( trimmedHost );
                delete ipv4Host16;
                }    
            }
        
        // Port
        if ( parser.IsPresent( EUriPort ) )
            {
            const TDesC8& port = parser.Extract( EUriPort );
            TLex8 lex( port );
            lex.Val( aProfileData.iPortNumber );
            
            if ( aProfileData.iPortNumber < 0 )
	            {
				aProfileData.iPortNumber = KDefaultPortNumber;	            	
	            }
            }
        else
            {
            aProfileData.iPortNumber = KDefaultPortNumber;
            }
        
        // Loose routing and transport
        if ( parser.IsPresent( EUriPath ) )
            {
            const TDesC8& path = parser.Extract( EUriPath );
        
            TDelimitedPathSegmentParser8 pathSegmentParser; 
            pathSegmentParser.Parse( path ); 
            TPtrC8 segment; 
        
            // Set temporary values for loose routing and transport.
            aProfileData.iLooseRouting = EGSLooseRoutingOff;
            aProfileData.iTransportProtocol = EGSTranspProtocolAuto;
        
            while( pathSegmentParser.GetNext( segment ) == KErrNone ) 
                {   
                TBuf<KMaxSegmentLength> seg;
                seg.Copy( segment );
                seg.LowerCase();
                // Loose routing
                if ( !seg.Compare( KLooseRoutingParam ) )
                    {
                    aProfileData.iLooseRouting = EGSLooseRoutingOn;
                    }
                // Transport
                if ( !seg.Find( KTransportParamPrefix ) )
                    {
                    seg.UpperCase();
                    if ( !seg.Right( KTCP().Length() ).Compare( KTCP ) )
                        {
                        aProfileData.iTransportProtocol = EGSTranspProtocolTCP;
                        }
                    if ( !seg.Right( KUDP().Length() ).Compare( KUDP ) )
                        {
                        aProfileData.iTransportProtocol = EGSTranspProtocolUDP;
                        }
                    }
                }    
            }
        }
        
    SIPStrings::Close();    
    __GSLOGSTRING("GSSIPProfileUtil::CopyServerSettingsToTempStructL End" )
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::ProfileModificationsL
// Compares profile attributes with the ones on the temporary structure
// -----------------------------------------------------------------------------
//
TBool GSSIPProfileUtil::ProfileModificationsL(
    CSIPManagedProfile* aProfile,
    const TSIPSettingsData& aProfileData )
    {
    __GSLOGSTRING("GSSIPProfileUtil::ProfileModificationsL Start" )
    TInt err( KErrNone );
    TBool modifs = EFalse;
    
    // Service provider name
    const TDesC8* pName;
    HBufC8* providerName = 
        EscapeUtils::ConvertFromUnicodeToUtf8L( aProfileData.iProviderName );
    CleanupStack::PushL( providerName );    
    aProfile->GetParameter( KSIPProviderName, pName );
    User::LeaveIfError( err );
    TBool nameComp = providerName->Des().Compare( *pName ) != 0;
    CleanupStack::PopAndDestroy( providerName );

    // Public username / address of record
    const TDesC8* aor;
    HBufC8* userName = 
        EscapeUtils::ConvertFromUnicodeToUtf8L( aProfileData.iUsername );
    CleanupStack::PushL( userName );
    err = aProfile->GetParameter( KSIPUserAor, aor );
    User::LeaveIfError( err );
    HBufC8* decodedAor = EscapeUtils::EscapeDecodeL( *aor );
    TBool uNameComp = userName->Des().Compare( decodedAor->Des() ) != 0;
    delete decodedAor;
    CleanupStack::PopAndDestroy( userName );
    
    // Destination 
    TUint32 snapid;    
    TBool snapComp = EFalse;
    TBool snapUsed = EFalse;
    
    err = aProfile->GetParameter( KSIPSnapId, snapid );
    if ( err == KErrNotFound )
        {
        if ( aProfileData.iSNAPIndex != KDefaultSNAP )
	        {
	        snapComp = ETrue;	        	
	        }
        }
    else 
        {
		if(snapid == KDefaultSNAPIdentifier && aProfileData.iSNAPIndex == KDefaultSNAP)
			{
			snapUsed=EFalse;
			snapComp=EFalse;
			}
		else
			{
	        User::LeaveIfError( err );  
    	    snapUsed = ETrue;
        	snapComp = TUint( aProfileData.iSNAPIndex ) != snapid;
			}
        }

    // Access Point 
    TUint32 apid;    
    err = aProfile->GetParameter( KSIPAccessPointId, apid );
    User::LeaveIfError( err );  

	TBool iapComp (EFalse);
    if ( !snapUsed )
    	{
	    iapComp = TUint( aProfileData.iAPIndex ) != apid;	
    	}
        
    // Signaling compression
    TBool sigcomp;
    err = aProfile->GetParameter( KSIPSigComp, sigcomp );
    User::LeaveIfError( err );
    TBool comprComp = 
        aProfileData.iCompression == (TInt)sigcomp ? EFalse : ETrue;
    
    // Registration method
    TBool registration;
    TBool registrationComp = EFalse;
    err = aProfile->GetParameter( KSIPAutoRegistration, registration );
    User::LeaveIfError( err );
    if ( aProfileData.iRegistrationMode == EGSWhenNeeded )
    	{
    	if ( registration )
    		{
    		registrationComp = ETrue;
    		}
    	}
    else if ( aProfileData.iRegistrationMode == EGSAlwaysOn )
    	{
    	if ( !registration )
    		{ // was WhenNeeded or 'only on home network'
    		registrationComp = ETrue;
    		}
    	}
   
    // Security negotiation        
    TBool negation;
    err = aProfile->GetParameter( KSIPSecurityNegotiation, negation );
    User::LeaveIfError( err );
    TBool securityComp = 
        aProfileData.iSecurity == (TInt)negation ? EFalse : ETrue;
 
    TBool serviceComp = ( aProfileData.iServiceProfile.iSIPProfileName !=
                          aProfile->Type().iSIPProfileName ) ||
                        ( aProfileData.iServiceProfile.iSIPProfileClass !=
                          aProfile->Type().iSIPProfileClass );

    if ( nameComp || iapComp || snapComp || comprComp || registrationComp ||
         securityComp || uNameComp || serviceComp )
        {        
        modifs = ETrue;
        }   
    __GSLOGSTRING("GSSIPProfileUtil::ProfileModificationsL End" ) 
    return modifs;
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::ServerModifciationsL
// Compares server attributes with the ones on the temporary structure
// -----------------------------------------------------------------------------
//
TBool GSSIPProfileUtil::ServerModificationsL(
    CSIPManagedProfile* aProfile,
    TUint32 aServer,
    const TGSSIPServerData& aData )
    {
    __GSLOGSTRING("GSSIPProfileUtil::ServerModificationsL" )
    TGSSIPServerData serverData;

    GSSIPProfileUtil::CopyServerSettingsToTempStructL( 
        aProfile, aServer, serverData );

    TBool addrComp  = serverData.iAddress != aData.iAddress;
    TBool lrComp    = ( serverData.iLooseRouting != aData.iLooseRouting && 
                        aServer != KSIPRegistrar );
    TBool realmComp = serverData.iRealm != aData.iRealm;
    TBool uNameComp = serverData.iUsername != aData.iUsername;

    TBool tpComp    = serverData.iTransportProtocol != aData.iTransportProtocol;
    TBool portComp  = serverData.iPortNumber != aData.iPortNumber;

    return ( addrComp || lrComp || realmComp || uNameComp || aData.iPasswordChanged ||
             tpComp || portComp );
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::CopyProfileL
// Copies profile attributes from one profile to another
// -----------------------------------------------------------------------------
//
void GSSIPProfileUtil::CopyProfileL(
    CSIPManagedProfile* aOrigProfile,
    CSIPManagedProfile* aCopiedProfile )
    {
    __GSLOGSTRING("GSSIPProfileUtil::CopyProfileL Start" )
    TInt err( KErrNone );
    // Copy profile settings
    aCopiedProfile->SetType( aOrigProfile->Type() );
    
    TUint32 apid;
    err = aOrigProfile->GetParameter( KSIPAccessPointId, apid );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPAccessPointId, apid );    
    User::LeaveIfError( err );
    	
    const TDesC8* aor; 
    err = aOrigProfile->GetParameter( KSIPUserAor, aor );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPUserAor, *aor ); 
	User::LeaveIfError( err );
	
	const TDesC8* identity; 
	err = aOrigProfile->GetParameter(KSIPPrivateIdentity, identity );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPPrivateIdentity, *identity );
	User::LeaveIfError( err );
	
	const TDesC8* provider;
	err = aOrigProfile->GetParameter( KSIPProviderName, provider );
	User::LeaveIfError( err );
	err = aCopiedProfile->SetParameter(KSIPProviderName, *provider );
	User::LeaveIfError( err );
	
	TBool sigcomp;
	err = aOrigProfile->GetParameter( KSIPSigComp,sigcomp );
	User::LeaveIfError( err );
	err = aCopiedProfile->SetParameter( KSIPSigComp,sigcomp );
	User::LeaveIfError( err );
	
	TBool autoregistration;
	err = aOrigProfile->GetParameter( KSIPAutoRegistration,autoregistration );
	User::LeaveIfError( err );
	err = aCopiedProfile->SetParameter( KSIPAutoRegistration,autoregistration );
	User::LeaveIfError( err );
	
	TBool negation;
	err = aOrigProfile->GetParameter( KSIPSecurityNegotiation, negation );
	User::LeaveIfError( err );
	err = aCopiedProfile->SetParameter( KSIPSecurityNegotiation, negation );
	User::LeaveIfError( err );
		
    TBool defaultValue;
    err = aOrigProfile->GetParameter( KSIPDefaultProfile, defaultValue );
    User::LeaveIfError( err );
    if ( defaultValue )
        {
        err = aCopiedProfile->SetParameter( KSIPDefaultProfile, defaultValue );
        User::LeaveIfError( err );
        }

    // Copy Proxy settings
    const TDesC8* address;
    err = aOrigProfile->GetParameter( KSIPOutboundProxy, KSIPServerAddress, address );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPOutboundProxy, KSIPServerAddress, *address );
    User::LeaveIfError( err );
    
    const TDesC8* realm;
    err = aOrigProfile->GetParameter( KSIPOutboundProxy, KSIPDigestRealm, realm );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPOutboundProxy, KSIPDigestRealm, *realm );    
    User::LeaveIfError( err );
    
    const TDesC8* user;
    err = aOrigProfile->GetParameter( KSIPOutboundProxy, KSIPDigestUserName, user );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPOutboundProxy, KSIPDigestUserName, *user ); 
    User::LeaveIfError( err );
 
    // Copy Registrar server settings
    err = aOrigProfile->GetParameter( KSIPRegistrar, KSIPServerAddress, address );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPRegistrar, KSIPServerAddress, *address );
    User::LeaveIfError( err );
    
    err = aOrigProfile->GetParameter( KSIPRegistrar, KSIPDigestRealm, realm );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPRegistrar, KSIPDigestRealm, *realm );    
    User::LeaveIfError( err );

    err = aOrigProfile->GetParameter( KSIPRegistrar, KSIPDigestUserName, user );
    User::LeaveIfError( err );
    err = aCopiedProfile->SetParameter( KSIPRegistrar, KSIPDigestUserName, *user ); 
    User::LeaveIfError( err );
    __GSLOGSTRING("GSSIPProfileUtil::CopyProfileL End" )
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::CopyServerURIL
// Copies server URI to the profile
// -----------------------------------------------------------------------------
//
void GSSIPProfileUtil::CopyServerURIL(
    CSIPManagedProfile* aProfile,
    TUint32 aServer,
    const TGSSIPServerData& aProfileData )
    {
    __GSLOGSTRING("GSSIPProfileUtil::CopyServerURIL Start" )
    SIPStrings::OpenL();
    
    // Set settings to KSIPServerAddress param.
    if ( aProfileData.iAddress.Length() > 0 )
        {
        HBufC8* finalAddress = HBufC8::NewLC( KMaxProxyLength * 2 );
        
        // Convert address to 8-bit descriptor format.
        TPtrC addr = aProfileData.iAddress;
        HBufC8* conv = EscapeUtils::ConvertFromUnicodeToUtf8L( addr );
        CleanupStack::PushL( conv );    
        
        // Parse address.
        TUriParser8 parser;
        User::LeaveIfError( parser.Parse( conv->Des() ) );
        // Append scheme to final address if needed.
        if ( !parser.IsPresent( EUriScheme ) )
            {
            finalAddress->Des().Append( KSIPProtocolHeader );
            finalAddress->Des().Append( KSIPColonSeparator );
            }
        // Append proxy/registrar address to final address.
        finalAddress->Des().Append( conv->Des() );
        // Check host type. Add brackets for ipv6 type of address.
        TUriParser8 parser2;
        User::LeaveIfError( parser2.Parse( finalAddress->Des() ) );
        if ( parser2.IsPresent( EUriHost ) )
            {
            const TDesC8& scheme = parser2.Extract( EUriScheme );
            const TDesC8& host = parser2.Extract( EUriHost );
            UriUtils::TUriHostType type = UriUtils::HostType( host );
            if ( type == UriUtils::EIPv6Host )
                {
                HBufC8* ipv6Address = HBufC8::NewL( KMaxProxyLength );
                ipv6Address->Des().Append( host );
                GSSIPProfileUtil::AddBrackets( ipv6Address->Des() );
                TBuf8<1> colon ( KNullDesC8 );
                colon.Copy( KSIPColonSeparator );
                ipv6Address->Des().Insert( 0, colon );
                ipv6Address->Des().Insert( 0, scheme );
                finalAddress->Des().Copy( ipv6Address->Des() ); 
                delete ipv6Address;
                }
            }
        CleanupStack::PopAndDestroy( conv );

        // Set used port if it's not default one.
        if ( aProfileData.iPortNumber != KDefaultPortNumber )
            {     
            TBuf8<KMaxLengthForPort> port;
            port.Num( aProfileData.iPortNumber );
            finalAddress->Des().Append( KSIPColonSeparator );
            finalAddress->Des().Append( port );
            }

        // Set uri path part if there is something to be added.
        HBufC8* finalPath = HBufC8::NewLC( KMaxLengthForPath ); 
        ParseUriPathL( aProfile, aServer, aProfileData, finalPath );
        finalAddress->Des().Append( finalPath->Des() );
        CleanupStack::PopAndDestroy( finalPath );

        // Set final address to profile.   
        TInt err = aProfile->SetParameter( 
            aServer, KSIPServerAddress, finalAddress->Des() );
        User::LeaveIfError( err );

        CleanupStack::PopAndDestroy( finalAddress );
        }
	// If host is set to empty, empty KSIPServerAddress param.
    else
        {
        TInt err = aProfile->SetParameter( 
            aServer, KSIPServerAddress, KNullDesC8 );
        User::LeaveIfError( err );
        }

    SIPStrings::Close();
    __GSLOGSTRING("GSSIPProfileUtil::CopyServerURIL End" )
    }

// -----------------------------------------------------------------------------
// GSSIPProfileUtil::ParseUriPathL
// Parses uri path.
// -----------------------------------------------------------------------------
//    
void GSSIPProfileUtil::ParseUriPathL(
    CSIPManagedProfile* aProfile,
    TUint32 aServer,
    const TGSSIPServerData& aProfileData,
    HBufC8* aFinalPath )
    {
    __GSLOGSTRING("GSSIPProfileUtil::ParseUriPathL Start" )
    TInt err( KErrNone );
   
    // Add Loose routing param. (if needed)    
    if ( aServer != KSIPRegistrar && 
        aProfileData.iLooseRouting == EGSLooseRoutingOn ) 
        {
        aFinalPath->Des().Append( KSegmentSeparator );
        aFinalPath->Des().Append( KLooseRoutingParam );
        }
        
    // Add Transport param TCP. (if needed)
    if ( aProfileData.iTransportProtocol == EGSTranspProtocolTCP )
        {             
        aFinalPath->Des().Append( KSegmentSeparator );
        aFinalPath->Des().Append( KTransportParamPrefix );
        aFinalPath->Des().Append( KTCP );
        }
    
    // Add Transport param UDP. (if needed)
    if ( aProfileData.iTransportProtocol == EGSTranspProtocolUDP )
        {            
        aFinalPath->Des().Append( KSegmentSeparator );
        aFinalPath->Des().Append( KTransportParamPrefix );
        aFinalPath->Des().Append( KUDP );
        }
    
    // Get path data from saved profile.
    const TDesC8* serverTxt;
    err = aProfile->GetParameter( aServer, KSIPServerAddress, serverTxt );
    User::LeaveIfError( err );
    
    if ( serverTxt->Length() )
        {
        // Parse address.
        TUriParser8 parser;
        User::LeaveIfError( parser.Parse( *serverTxt ) );
        if ( parser.IsPresent( EUriPath ) )
            {
            const TDesC8& path = parser.Extract( EUriPath );
        
            // Put any other params than lr and transport back to string.            
            TDelimitedPathSegmentParser8 pathSegmentParser; 
            pathSegmentParser.Parse( path ); 
            TPtrC8 segment; 
            while( pathSegmentParser.GetNext( segment ) == KErrNone ) 
                {   
                TBuf<KMaxSegmentLength> seg;
                seg.Copy( segment );
                seg.LowerCase();
            
                if ( seg.Compare( KLooseRoutingParam ) &&
                    seg.Find( KTransportParamPrefix ) )
                    {
                    aFinalPath->Des().Append( KSegmentSeparator );
                    aFinalPath->Des().Append( seg );
                    }
                }
            }
        }
    __GSLOGSTRING("GSSIPProfileUtil::ParseUriPathL End" )
    }
    
// -----------------------------------------------------------------------------
// GSSIPProfileUtil::SchemeIncluded
// Checks if sip or sips scheme included.
// -----------------------------------------------------------------------------
//    
TInt GSSIPProfileUtil::SchemeIncluded( const TDesC8& aAddr, 
										TBool aHostBrackets )
	{
	__GSLOGSTRING("GSSIPProfileUtil::SchemeIncluded" )
	TLex8 lex( aAddr );
    lex.Mark();
    TUint chrCount = 0;
    TChar chr = lex.Get();
    while ( chr != 0 && chr != ':' )
    	{
        chrCount++;
        chr = lex.Get();
        }

    TBool sipSchemeIncluded( EFalse );
    if ( chr != 0 )
    	{
        chrCount++; //include ':' too
        TPtrC8 remainder = lex.RemainderFromMark();
        _LIT8(KSIPScheme, "sip:");
		_LIT8(KSIPSScheme, "sips:");
		
		// set scheme to lovercase because of comparing
		HBufC8* scheme = HBufC8::New( chrCount );
        scheme->Des().Copy( remainder.Left( chrCount ) );
        scheme->Des().LowerCase();
        
        if ( !scheme->Compare( KSIPScheme ) ||
        	!scheme->Compare( KSIPSScheme ) )
        	{
        	sipSchemeIncluded = ETrue;
        	}
        
        delete scheme;
        
        if ( aHostBrackets )
        	{ // check host brackets 
        	if ( sipSchemeIncluded )
        		{ // scheme separated from uri, check host
        		if( GSSIPProfileUtil::BracketsIncluded( 
        			remainder.Right( aAddr.Length() - chrCount ) ) == -1 )
        			{
        			sipSchemeIncluded = -1;
        			}
        		}
        	else
        		{ // scheme not included, check host == aAddr
        		if( GSSIPProfileUtil::BracketsIncluded( aAddr ) == -1)
        			{
        			sipSchemeIncluded = -1;
        			}
        		}
        	}
        }
    
	return sipSchemeIncluded;
	}
	
// -----------------------------------------------------------------------------
// GSSIPProfileUtil::BracketsIncluded
// Check if brackets included correctly in IPv6 reference.
// -----------------------------------------------------------------------------
//    
TInt GSSIPProfileUtil::BracketsIncluded( TPtrC8 aHost )
	{
	__GSLOGSTRING("GSSIPProfileUtil::BracketsIncluded" )
	TBool ret ( ETrue );
	UriUtils::TUriHostType type = UriUtils::HostType( aHost );
	if ( type == UriUtils::EIPv6Host )
		{
		TLex8 lex( aHost );
	    lex.Mark();
	    TUint chrCount = 0;
	    TChar chr = lex.Get();
	    TInt lBracket( KErrNotFound );
	    TInt rBracket( KErrNotFound );
	    TInt separator( KErrNotFound );
    
    	while ( chr != 0 )
	    	{
	        if ( chr == '[' )
	        	{
	        	lBracket = chrCount;
	        	}
	        else if ( chr == ']' )
	        	{
	        	rBracket = chrCount;
	        	}
	        else if ( chr == '@' )
	        	{
	        	separator = chrCount;
	        	}
	        else
	        	{
	        	}
	        chrCount++;
	        chr = lex.Get();
	        }
	    chrCount--; // the last character 
	        
	    if ( lBracket == KErrNotFound && rBracket == KErrNotFound )
    		{ //bracets not included, add them
			ret = EFalse;
	    	}
	    else if ( ( lBracket == KErrNotFound && rBracket != KErrNotFound) ||
	    	( lBracket != KErrNotFound && rBracket == KErrNotFound ) )
	    	{ // both brackets not found
	    	ret = KErrNotFound;
	    	}
	    else if ( lBracket != KErrNotFound && rBracket != KErrNotFound )
	    	{ //brackets are found
	    	if ( !( lBracket == 0 && rBracket == chrCount ) &&
	    		!( separator > 0 && lBracket ==  separator + 1 
	    			&& rBracket == chrCount ) )
	    		{ // brackets are placed incorrectly
	    		ret = KErrNotFound;
	    		}
	    	}
	    else
	    	{
	    	}
		}
    return ret;
	
	}
	
// -----------------------------------------------------------------------------
// GSSIPProfileUtil::AddBrackets
// Adds brackets into IPv6 reference.
// -----------------------------------------------------------------------------
//    
void GSSIPProfileUtil::AddBrackets( TPtr8 aHost )
	{
	__GSLOGSTRING("GSSIPProfileUtil::AddBrackets" )
	_LIT8( KLBracket, "[" );
    _LIT8( KRBracket, "]" );
    
    aHost.Insert( 0, KLBracket );
    aHost.Append( KRBracket );
    }
    
// -----------------------------------------------------------------------------
// GSSIPProfileUtil::RemoveBrackets
// Remove brackets from IPv6 reference.
// -----------------------------------------------------------------------------
//    
void GSSIPProfileUtil::RemoveBrackets( TPtr8 aHost )
	{
	__GSLOGSTRING("GSSIPProfileUtil::RemoveBrackets" )
	TInt index;
	
	index = aHost.Locate( '[' );
	if ( index != KErrNotFound )
		{
		aHost.Delete( index, 1 );
		}
		
	index = aHost.Locate( ']' );
	if ( index != KErrNotFound )
		{
		aHost.Delete( index, 1 );
		}
	}
	
// -----------------------------------------------------------------------------
// GSSIPProfileUtil::URIValid
// Checks if reference is valid URI.
// -----------------------------------------------------------------------------
//    
TInt GSSIPProfileUtil::URIValid( const TDesC8& aValue )
	{
	__GSLOGSTRING("GSSIPProfileUtil::URIValid" )
	TUriParser8 parser;
	return parser.Parse(aValue);
	}
	
// -----------------------------------------------------------------------------
// GSSIPProfileUtil::ReplaceEscapedSpace
// Replace %20 to ' '.
// -----------------------------------------------------------------------------
//    
void GSSIPProfileUtil::ReplaceEscapedSpace( HBufC8* aValue )
	{
	__GSLOGSTRING("GSSIPProfileUtil::ReplaceEscapedSpace" )
	_LIT8( KEscapedSpace, "%20" );
	_LIT8( KSpace, " " );
	TInt pos = KErrNone;
	while ( pos != KErrNotFound )
		{
		pos = aValue->Des().Find( KEscapedSpace );
		if ( pos != KErrNotFound )
			{
			aValue->Des().Replace( pos, 3, KSpace );
			}
		}
	}

// End of File
