// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name        : ProfileUtil.cpp
// Part of     : SIP Profile Client / Settings User Interface
// Utility class for manipulating profiles and their attributes
// Version     : 1.0
//




// INCLUDE FILES
#include    <e32std.h>
#include    <e32base.h>
#include    <eikenv.h>
#include 	<badesca.h>
#include    "ProfileUtil.h"
#include    "TSIPServerData.h"
#include    "TSIPProfileData.h"
#include    "sipmanagedprofile.h"
#include    "siphostport.h"
#include    "sipuri.h"
#include 	"sipstrings.h"
#include    "sipstrconsts.h"
#include    <sipex.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ProfileUtil::ProfileUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ProfileUtil::ProfileUtil()
    {
    }

// -----------------------------------------------------------------------------
// ProfileUtil::CopyProfileAttributesL
// Copies profile attributes from temp data structure to profile
// -----------------------------------------------------------------------------
//
void ProfileUtil::CopyProfileAttributesL( 
    CSIPManagedProfile* aProfile, 
    const TSIPProfileData& aProfileData )
    {
    // Service provider name
    HBufC8* providerName = HBufC8::NewLC( KMaxProviderNameLength );
    providerName->Des().Copy( aProfileData.iProviderName );
    TInt err = aProfile->SetParameter(KSIPProviderName ,providerName->Des());
    User::LeaveIfError(err);    
    CleanupStack::PopAndDestroy();  // providerName

    // AP index
    err=aProfile->SetParameter(KSIPAccessPointId,(TUint32)aProfileData.iAPIndex);
    User::LeaveIfError(err);    

    // Compression setting
    TBool compression = ( aProfileData.iCompression == EComprYes );
    err=aProfile->SetParameter(KSIPSigComp,compression);
    User::LeaveIfError(err);
    
    // Registration mode
    TBool autoReg = ( aProfileData.iRegistrationMode == EAlwaysOn );
    err=aProfile->SetParameter(KSIPAutoRegistration,autoReg);
    User::LeaveIfError(err);    

    // Security negotiation
    TBool security = ( aProfileData.iSecurity == ESecurityOn );
    err=aProfile->SetParameter(KSIPSecurityNegotiation,security);
    User::LeaveIfError(err);
    
    // Username
    if ( aProfileData.iUsername != KEmptyString )
        {
      
        HBufC8* uName = HBufC8::NewLC( KMaxUsernameLength + 
                                       KSIPProtocolHeader().Length() );       
        uName->Des().Copy( KSIPProtocolHeader );
        uName->Des().Append( aProfileData.iUsername );
     
        err=aProfile->SetParameter(KSIPUserAor,*uName );
        User::LeaveIfError(err);
        
        CleanupStack::PopAndDestroy( 1 );  // uName
        }
    
    }

// -----------------------------------------------------------------------------
// ProfileUtil::CopyServerAttributesL
// Copies server attributes from temp data structure to profile
// -----------------------------------------------------------------------------
//
void ProfileUtil::CopyServerAttributesL(
    CSIPManagedProfile* aProfile,
    const TUint32 aServer,
    const TSIPServerData& aProfileData )
    {
    // Digest realm
    TInt err(KErrNone);
    if ( aProfileData.iRealm != KEmptyString )
        {
        HBufC8* realm = HBufC8::NewLC( aProfileData.iRealm.Length() );
        realm->Des().Copy( aProfileData.iRealm );        
		err=aProfile->SetParameter( 
		         aServer,KSIPDigestRealm, *realm );
		User::LeaveIfError(err);
        CleanupStack::PopAndDestroy();  // realm
        }

    // Username        
    if ( aProfileData.iUsername != KEmptyString )
        {
        HBufC8* uName = HBufC8::NewLC( aProfileData.iUsername.Length() );
        uName->Des().Copy( aProfileData.iUsername );
        err=aProfile->SetParameter( 
		         aServer,KSIPDigestUserName,*uName );
		User::LeaveIfError(err);
        CleanupStack::PopAndDestroy();  // uName    
        }

    // Password
    if ( aProfileData.iPassword != KEmptyString )
        {
        HBufC8* passw = HBufC8::NewLC( aProfileData.iPassword.Length() );
        passw->Des().Copy( aProfileData.iPassword );        
        err=aProfile->SetParameter( 
		         aServer,KSIPDigestPassword,*passw );
		User::LeaveIfError(err);
        CleanupStack::PopAndDestroy();  // passw
        }
    
    HBufC* defBuf = CEikonEnv::Static()->AllocReadResourceLC(
        R_SIP_EX_DEF_SERVER_ADDRESS );
    
    // If there are default values set, then the URI string
    // will be empty. Otherwise the changed values are copied
    if ( aProfileData.iTransportProtocol != ETranspProtocolUDP ||
         ( aServer == KSIPOutboundProxy &&
           aProfileData.iLooseRouting != ELooseRoutingOn ) ||
         aProfileData.iPortNumber != KDefaultPortNumber ||
         (aProfileData.iAddress != defBuf->Des() &&
         aProfileData.iAddress.Length() > 0
         ))
        {
        // Don't need defBuf no more
        CleanupStack::PopAndDestroy();  // defBuf
        
        // Creates, formats and saves URI to profile
        ProfileUtil::CopyServerURIL( aProfile, aServer, aProfileData );
        }
    else
        {
        // Don't need defBuf no more
        CleanupStack::PopAndDestroy();  // defBuf

        // Empty string  . New API will leave if empty sting has set 
        // So there is no point of setting this .
        /*
        err=aProfile->SetParameter(KSIPOutboundProxy,KEmptyString8 );
        User::LeaveIfError(err);
        */
        }
    }

// -----------------------------------------------------------------------------
// ProfileUtil::CopyToTempStruct
// Copies profile attributes from profile to temp data struct
// -----------------------------------------------------------------------------
//
void ProfileUtil::CopyToTempStruct(
    CSIPManagedProfile* aProfile,
    TSIPProfileData& aProfileData )
    {
    // Profile ID
    TInt err=  aProfile->GetParameter(KSIPProfileId,(TUint32 &)aProfileData.iID);
    if(err)	
    	{
    	// don't know what to do     	
    	}    
    // Access Point index
    err=  aProfile->GetParameter(KSIPAccessPointId ,(TUint32 &)aProfileData.iAPIndex);
    if(err)	
    	{
    	// don't know what to do     	
    	}    
    
    // Service provider name
    
    const TDesC8 * tmpProviderName = 0;
    err=  aProfile->GetParameter(KSIPProviderName,tmpProviderName);
    if(err)	
    	{
    	// don't know what to do     	
    	}    
    else
    	{
    	aProfileData.iProviderName.Copy( *tmpProviderName );	
    //	delete tmpProviderName;
    	}    

    // Registration mode
    TBool bResult(EFalse);
    err=  aProfile->GetParameter(KSIPAutoRegistration,bResult);
    if(err)
    	{
    		// don't know what to do 
    	}    
    if (bResult)
        {
        aProfileData.iRegistrationMode = EAlwaysOn;
        }
    else
        {
        aProfileData.iRegistrationMode = EWhenNeeded;
        }
	//reset the value to EFalse since we are not caring err .
	bResult = EFalse;
	err=  aProfile->GetParameter(KSIPSigComp,bResult);
    if(err)
    	{
    		// don't know what to do 
    	}    
    if ( bResult)
        {
        aProfileData.iCompression = EComprYes;
        }
    else
        {
        aProfileData.iCompression = EComprNo;
        }
    
    //reset the value to EFalse since we are not caring err .
    // Security negotiation
    bResult = EFalse;
	err=  aProfile->GetParameter(KSIPSecurityNegotiation ,bResult);
    if(err)
    	{
    		// don't know what to do 
    	}
    	   
    if (bResult)
        {
        aProfileData.iSecurity = ESecurityOn;
        }
    else
        {   
        aProfileData.iSecurity = ESecurityOff;
        }

    // Profile type    
    aProfileData.iServiceProfile = aProfile->Type();

    // Username
    const TDesC8 * userName = 0;
    err=  aProfile->GetParameter(KSIPUserAor ,userName);
    if(err)
    	{
    		// don't know what to do 
    	}
    if (userName && 
         (userName->Length() > 
           KSIPProtocolHeader().Length() ) )
        {
        TPtrC8 ptr( *userName);
        aProfileData.iUsername.Copy( 
            ptr.Right( ptr.Length() - KSIPProtocolHeader().Length() ) );
        }    
    else
        {
        aProfileData.iUsername.Zero();
        }   
    }

// -----------------------------------------------------------------------------
// ProfileUtil::CopyServerSettingsToTempStructL
// Copies server attributes from profile to temp data struct
// -----------------------------------------------------------------------------
//
void ProfileUtil::CopyServerSettingsToTempStructL(
    CSIPManagedProfile* aProfile,
    const TUint32 aServer,
    TSIPServerData& aProfileData )
    {
    CEikonEnv* env = CEikonEnv::Static();
	TInt err(KErrNone);    
    const TDesC8 * uri = 0;    
    err= aProfile->GetParameter(aServer,KSIPServerAddress,uri);
    User::LeaveIfError(err);    	     
	const TDesC8 * realm =0;	
	err=aProfile->GetParameter(aServer,KSIPDigestRealm,realm);
	User::LeaveIfError(err);	    
	const TDesC8 * uName = 0;	
	err=aProfile->GetParameter(aServer,KSIPDigestUserName,uName);
	User::LeaveIfError(err);	
	// Can't be retrived according to new api
    //const TDesC8& passw = aProfile->ServerExtensionParameter( 
    //    aServer, KLabelPassword );
    
    aProfileData.iRealm.Copy( *realm );
    aProfileData.iUsername.Copy( *uName );
    aProfileData.iPassword.Copy( KNullDesC);

    if ( *uri == KEmptyString8 )
        {
        // Set default settings if URI is empty
        aProfileData.iAddress =         
            env->AllocReadResourceLC( R_SIP_EX_DEF_SERVER_ADDRESS )->Des();
        CleanupStack::PopAndDestroy();  // addr                           
        aProfileData.iPortNumber = KDefaultPortNumber;
        aProfileData.iTransportProtocol = ETranspProtocolUDP;
        if ( aServer == KSIPOutboundProxy)
            {
            aProfileData.iLooseRouting = ELooseRoutingOn;
            }
        else
            {
            aProfileData.iLooseRouting = ELooseRoutingOff;
            }
        }
    else
        {
        CSIPURI* sipURI = CSIPURI::DecodeL( *uri );
        CleanupStack::PushL( sipURI );
        // Get proxy/registrar address
        if ( sipURI->HostPort().Host().Length() )        
            {
            aProfileData.iAddress.Copy( sipURI->HostPort().Host() );            
            }
        else
            {       
            aProfileData.iAddress =         
                env->AllocReadResourceLC( R_SIP_EX_DEF_SERVER_ADDRESS )->Des();
            CleanupStack::PopAndDestroy();  // addr                           
            }

        // Get lr parameter
        RStringF lr = SIPStrings::Pool().StringF(SipStrConsts::ELr,SIPStrings::Table());
        aProfileData.iLooseRouting = sipURI->HasParam(lr);       
        // Get the port 
        if (! sipURI->HostPort().HasPort())
            {
            aProfileData.iPortNumber = KDefaultPortNumber;
            }
        else
            {
            aProfileData.iPortNumber = sipURI->HostPort().Port();
            }
    
        // Read transport protocol parameter
/*        HBufC* transpParamTCP;
        HBufC8* transpParamTCP8;
        transpParamTCP = env->AllocReadResourceLC( R_SIP_EX_TCP_PARAM );
        transpParamTCP8 = HBufC8::NewLC( transpParamTCP->Length() );
        transpParamTCP8->Des().Copy( transpParamTCP->Des() );        
		RStringF transportParam = SIPStrings::Pool().StringF(SipStrConsts::ETransport,SIPStrings::Table());
        if ( (*transpParamTCP8).Compare( sipURI->ParamValue(transportParam).DesC() ) == 0 )
            {
            aProfileData.iTransportProtocol = ETranspProtocolTCP;
            }
        else
            {
            // UDP is default transport parameter
  */          aProfileData.iTransportProtocol = ETranspProtocolUDP;
   /*         }
     */   
        CleanupStack::PopAndDestroy( 2 );      // transpParamTCP, transpParamTCP8

        CleanupStack::PopAndDestroy();   // sipURI
        }    
    }

// -----------------------------------------------------------------------------
// ProfileUtil::ProfileModificationsL
// Compares profile attributes with the ones on the temporary structure
// -----------------------------------------------------------------------------
//
TBool ProfileUtil::ProfileModificationsL(
    CSIPManagedProfile* aProfile,
    const TSIPProfileData& aProfileData )
    {
    TBool modifs = EFalse;
    
    // Service provider name
    HBufC8* providerName = HBufC8::NewLC( KMaxProviderNameLength );
    providerName->Des().Copy( aProfileData.iProviderName );

    HBufC8* userName = HBufC8::NewLC( KMaxUsernameLength );
    userName->Des().Copy( aProfileData.iUsername );   

	const TDesC8 * tmpProviderName = 0;
	TInt err(KErrNone);
    err=  aProfile->GetParameter(KSIPProviderName,tmpProviderName);
    User::LeaveIfError(err);
       
    TBool nameComp = providerName->Des().Compare( *tmpProviderName ) != 0;
    
    TUint32 iapId(0);
    err=  aProfile->GetParameter(KSIPAccessPointId,iapId);
    User::LeaveIfError(err);    
    TBool iapComp = TUint( aProfileData.iAPIndex ) != iapId;
    
    // error 514: (Warning -- Unusual use of a Boolean expression)
    // PC-Lint refuses to understand XOR operation
    TBool bSigComp(EFalse);
    err=  aProfile->GetParameter(KSIPSigComp,bSigComp);
    User::LeaveIfError(err)	;
    
    TBool comprComp = ( aProfileData.iCompression == EComprYes ) ^ 
                      bSigComp; //lint !e514
                      
    // error 514: (Warning -- Unusual use of a Boolean expression)
    // PC-Lint refuses to understand XOR operation
    
    TBool bAutoReg(EFalse);
    err=  aProfile->GetParameter(KSIPAutoRegistration,bAutoReg);
    User::LeaveIfError(err);
         
    TBool registrationComp = ( aProfileData.iRegistrationMode == EAlwaysOn ) ^
                    			bAutoReg; //lint !e514
    
    // error 514: (Warning -- Unusual use of a Boolean expression)
    // PC-Lint refuses to understand XOR operation
    
    TBool bSecurityNeg(EFalse);
    err=  aProfile->GetParameter(KSIPSecurityNegotiation,bSecurityNeg);
    User::LeaveIfError(err);
         
    TBool securityComp = ( aProfileData.iSecurity == ESecurityOn ) ^
            				bSecurityNeg; //lint !e514
	const TDesC8 * aor = 0;
    err=  aProfile->GetParameter(KSIPUserAor,aor);    
    User::LeaveIfError(err);
    
    TPtrC8 ptr(*aor);
    // Username comparison
    TBool uNameComp = 
        userName->Des().Compare(
            ptr.Right( ptr.Length() - KSIPProtocolHeader().Length() )  ) != 0;

    TBool serviceComp = ( aProfileData.iServiceProfile.iSIPProfileName !=
                          aProfile->Type().iSIPProfileName ) ||
                        ( aProfileData.iServiceProfile.iSIPProfileClass !=
                          aProfile->Type().iSIPProfileClass );

    if ( nameComp || iapComp || comprComp || registrationComp ||
         securityComp || uNameComp || serviceComp )
        {        
        modifs = ETrue;
        }   
        
    CleanupStack::PopAndDestroy( 2 );  // providerName, userName

    return modifs;
    }

// -----------------------------------------------------------------------------
// ProfileUtil::ServerModifciationsL
// Compares server attributes with the ones on the temporary structure
// -----------------------------------------------------------------------------
//
TBool ProfileUtil::ServerModificationsL(
    CSIPManagedProfile* aProfile,
    const TUint32 aServer,
    const TSIPServerData& aData )
    {
    TSIPServerData serverData;

    ProfileUtil::CopyServerSettingsToTempStructL( 
        aProfile, aServer, serverData );

    TBool addrComp = serverData.iAddress != aData.iAddress;
    TBool lrComp = serverData.iLooseRouting != aData.iLooseRouting;
    TBool realmComp = serverData.iRealm != aData.iRealm;
    TBool uNameComp = serverData.iUsername != aData.iUsername;
    TBool passwComp = serverData.iPassword != aData.iPassword;
    TBool transpComp = 
        serverData.iTransportProtocol != aData.iTransportProtocol;
    TBool portComp = serverData.iPortNumber != aData.iPortNumber;

    return ( addrComp || lrComp || realmComp || uNameComp || passwComp ||
             transpComp || portComp );
    }

// -----------------------------------------------------------------------------
// ProfileUtil::CopyProfileL
// Copies profile attributes from one profile to another
// -----------------------------------------------------------------------------
//
void ProfileUtil::CopyProfileL(
    CSIPManagedProfile* aOrigProfile,
    CSIPManagedProfile* aCopiedProfile )
    {
    // Copy profile settings    
	TInt err(KErrNone);    
    aCopiedProfile->SetType( aOrigProfile->Type() );
    TUint32 iapId(0);
    err=  aOrigProfile->GetParameter(KSIPAccessPointId,iapId);    
    User::LeaveIfError(err);
    
	err=aCopiedProfile->SetParameter(KSIPAccessPointId,iapId);	
	User::LeaveIfError(err);	
	
	const MDesC8Array * aors = 0;
    err=  aOrigProfile->GetParameter(KSIPRegisteredAors,aors);    
    User::LeaveIfError(err);
    
    
	err=aCopiedProfile->SetParameter(KSIPRegisteredAors,*aors);	
	User::LeaveIfError(err);
	
    const TDesC8 * pIdentiy = 0;
    err=  aOrigProfile->GetParameter(KSIPPrivateIdentity,pIdentiy);    
    User::LeaveIfError(err);    
	err=aCopiedProfile->SetParameter(KSIPPrivateIdentity,*pIdentiy);	
	User::LeaveIfError(err);
    
    
	const TDesC8 * providerName = 0;
    err=  aOrigProfile->GetParameter(KSIPProviderName,providerName);    
    User::LeaveIfError(err);    
	err=aCopiedProfile->SetParameter(KSIPProviderName,*providerName);	
	User::LeaveIfError(err);
	
	TBool bSigcomp(EFalse);
    err=  aOrigProfile->GetParameter(KSIPSigComp,bSigcomp);    
    User::LeaveIfError(err);    
	err=aCopiedProfile->SetParameter(KSIPSigComp,bSigcomp);	
	User::LeaveIfError(err);
	
	TBool bAutoReg(EFalse);
    err=  aOrigProfile->GetParameter(KSIPAutoRegistration,bAutoReg);    
    User::LeaveIfError(err);
    err=aCopiedProfile->SetParameter(KSIPAutoRegistration,bAutoReg);	
	User::LeaveIfError(err);
	

	TBool bSecurityNeg(EFalse);
    err=  aOrigProfile->GetParameter(KSIPNegotiatedSecurityMechanism ,bSecurityNeg);    
    User::LeaveIfError(err);
    err=aCopiedProfile->SetParameter(KSIPNegotiatedSecurityMechanism ,bSecurityNeg);	
	User::LeaveIfError(err);
	
	TBool bDefault(EFalse);
    err=  aOrigProfile->GetParameter(KSIPDefaultProfile ,bDefault);    
    User::LeaveIfError(err);
        
    if(bDefault)
        {
        err=aCopiedProfile->SetParameter(KSIPDefaultProfile,bDefault);
		User::LeaveIfError(err);
        }

	const TDesC8 * outboundProxy = 0;
    err=  aOrigProfile->GetParameter(KSIPOutboundProxy,KSIPServerAddress,outboundProxy );    
    User::LeaveIfError(err);
    err=aCopiedProfile->SetParameter(KSIPOutboundProxy ,KSIPServerAddress,*outboundProxy);	
	User::LeaveIfError(err);
		
	const TDesC8 * realm = 0;
    err=  aOrigProfile->GetParameter(KSIPOutboundProxy,KSIPDigestRealm,realm );    
    User::LeaveIfError(err);
    err=aCopiedProfile->SetParameter(KSIPOutboundProxy ,KSIPDigestRealm ,*realm);
	User::LeaveIfError(err);
    
    const TDesC8 * uName = 0;
    err=  aOrigProfile->GetParameter(KSIPOutboundProxy,KSIPDigestUserName,uName );
    User::LeaveIfError(err);
    err=aCopiedProfile->SetParameter(KSIPOutboundProxy,KSIPDigestUserName,*uName);
	User::LeaveIfError(err);    
 
    err=  aOrigProfile->GetParameter(KSIPRegistrar,KSIPDigestRealm,realm );    
    User::LeaveIfError(err);
    err=aCopiedProfile->SetParameter(KSIPRegistrar ,KSIPDigestRealm,*realm);	
	User::LeaveIfError(err);
       
    err=  aOrigProfile->GetParameter(KSIPRegistrar,KSIPDigestUserName,uName );    
    User::LeaveIfError(err);
    err=aCopiedProfile->SetParameter(KSIPRegistrar,KSIPDigestUserName,*uName);	
	User::LeaveIfError(err);
	
// TODO : This can't be done according to new API .            
/*	aCopiedProfile->SetServerExtensionParameterL( 
        CSIPProfile::ERegistrar, 
        KLabelPassword,
		aOrigProfile->ServerExtensionParameter( 
            CSIPProfile::ERegistrar, 
            KLabelPassword ) );
*/            
    }

// -----------------------------------------------------------------------------
// ProfileUtil::CopyServerURIL
// Copies server URI to the profile
// -----------------------------------------------------------------------------
//
void ProfileUtil::CopyServerURIL(
    CSIPManagedProfile* aProfile,
    const TUint32 aServer,
    const TSIPServerData& aProfileData )
    {    
    CEikonEnv* env = CEikonEnv::Static();

    // Check if the default values are used in port 
    // and transport protocol.
    TBool defaultValues = EFalse;
    if( aProfileData.iPortNumber == KDefaultPortNumber &&
        aProfileData.iTransportProtocol == KDefaultTranspProtocol )
        {
        defaultValues = ETrue;
        }

    // Set URI (contains proxy address, port and 
    // loose routing & transport parameters)     
    HBufC8* host = HBufC8::NewLC( aProfileData.iAddress.Length() );
    host->Des().Copy( aProfileData.iAddress );

    // If default values are used, the port is not added to URI.
    CSIPHostPort* port = NULL;    
    if( defaultValues )
        {
        port = CSIPHostPort::DecodeL( host->Des() );            
        }
    else
        {
        port = CSIPHostPort::DecodeL(host->Des());
        port->SetPort(aProfileData.iPortNumber );            
        }
    CleanupStack::PushL(port);    
    HBufC8* uriString;    
    // String is of format "sip:[public username]"
    uriString = HBufC8::NewLC( host->Length() + 
                               KSIPProtocolHeader().Length() );
    uriString->Des().Copy( KSIPProtocolHeader );
    uriString->Des().Append( host->Des() );
    CSIPURI* uri = CSIPURI::DecodeL(*uriString);    
    CleanupStack::PopAndDestroy(1);    // uriString      
    uri->SetHostPortL(port);
    CleanupStack::Pop(port);
    CleanupStack::PopAndDestroy(1);    // host
    CleanupStack::PushL( uri ); // Ownership of port is transferred
    // Loose routing param
    if ( aProfileData.iLooseRouting == ELooseRoutingOn &&
         aServer != KSIPRegistrar )
        {
        RStringF lr = SIPStrings::Pool().StringF(SipStrConsts::ELr,SIPStrings::Table());
        uri->SetParamL(lr);
        }

    // If default values are used, the transport protocol is not added to URI.
    if( !defaultValues )
        {
       	RStringF transportParam = SIPStrings::Pool().StringF(SipStrConsts::ETransport,SIPStrings::Table());
       	if ( aProfileData.iTransportProtocol == ETranspProtocolUDP )
       		{
       		RStringF udp = SIPStrings::Pool().StringF(SipStrConsts::EUdp,SIPStrings::Table());
       		uri->SetParamL(transportParam,udp );	
       		}
       	else if (aProfileData.iTransportProtocol == ETranspProtocolTCP)
       		{
       		RStringF tcp = SIPStrings::Pool().StringF(SipStrConsts::ETcp,SIPStrings::Table());
       		uri->SetParamL(transportParam,tcp );	       	
       		}
       	else
       		{
       		User::Leave( KErrNotFound );
       		} 
        }
    
    HBufC8* txt = uri->ToTextL();
    CleanupStack::PushL( txt );    
    TInt err= aProfile->SetParameter( aServer,KSIPServerAddress,*txt );
    User::LeaveIfError(err);
    CleanupStack::PopAndDestroy( 2 );   // uri, txt               
    }
