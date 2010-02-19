/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Validation methods for checking username etc.
*
*/


// INCLUDE FILES
#include    <StringLoader.h>
#include    <escapeutils.h>
#include    <AknUtils.h>
#include    <sipstrings.h>
#include    <sipaddress.h>
#include    <gssipsettingspluginrsc.rsg> //GUI Resource
#include    "sipserversettingvalidator.h"
#include    "tsipsettingsdata.h"
#include    "gssipprofileutil.h"
#include    "gssippluginlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SIPServerSettingValidator::SIPServerSettingValidator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
SIPServerSettingValidator::SIPServerSettingValidator()
    {
    __GSLOGSTRING("SIPServerSettingValidator::SIPServerSettingValidator" )
    }

// -----------------------------------------------------------------------------
// SIPServerSettingValidator::ValidateServerUsernameL
// Validates proxy/registrar server username
// -----------------------------------------------------------------------------
//
TBool SIPServerSettingValidator::ValidateServerUsernameL( 
    TAny* aDes )
    {  
    __GSLOGSTRING("SIPServerSettingValidator::ValidateServerUsernameL" )
    __ASSERT_ALWAYS( aDes != NULL, User::Leave( KErrArgument ) );
    TBool result( EFalse );
    TDes* desPtr = static_cast<TDes*>( aDes );
    TDes& des = *desPtr;
    
	if ( des.Length() == 0 )    
		{
		return ETrue;
		}
		
    SIPStrings::OpenL();    // Needed for CSIPAddress.
     
    AknTextUtils::StripCharacters( des, KAknStripListControlChars );
    
    // Check that scheme is found from setting data.
    TBool protocolFound( EFalse );
    TBuf<KSIPProtocolHeaderLength + 1> sipHeader( KSIPProtocolHeader );
    sipHeader.Append( KSIPColonSeparator );
    TBuf<KSIPSProtocolHeaderLength + 1> sipsHeader( KSIPSProtocolHeader );
    sipsHeader.Append( KSIPColonSeparator );
    
    if ( des.Length() > KSIPProtocolHeaderLength )
        {
        if ( !sipHeader.Compare( 
            des.Left( KSIPProtocolHeaderLength + 1 ) ) )
            {
            protocolFound = ETrue;
            }
        }
    
    if ( des.Length() > KSIPSProtocolHeaderLength )
        {
        if ( !sipsHeader.Compare( 
            des.Left( KSIPSProtocolHeaderLength + 1 ) ) )
            {
            protocolFound = ETrue;
            }
        }   
    
    // Create address to be validated.
    HBufC8* tmp = EscapeUtils::ConvertFromUnicodeToUtf8L( des );
    CleanupStack::PushL( tmp );
    HBufC8* conv = NULL; 
    
    if ( !protocolFound )
        {
        conv = HBufC8::NewLC( sipHeader.Length() + tmp->Length() );
        conv->Des().Copy( sipHeader );    
        conv->Des().Append( tmp->Des() );
        }
    else
        {
        conv = EscapeUtils::ConvertFromUnicodeToUtf8L( des );
        CleanupStack::PushL( conv );
        }    
        
    EscapeUtils::TEscapeMode mode = EscapeUtils::EEscapeNormal;
    HBufC8* enc = EscapeUtils::EscapeEncodeL( conv->Des(), mode );
    CleanupStack::PushL( enc );
    
    CSIPAddress* addr = CSIPAddress::DecodeL( enc->Des() );
    CleanupStack::PushL( addr );
   
    // Finally, validate entered host.
    if ( addr->Uri8().Uri().Validate() == KErrNone )
        {
        result = ETrue;
        }
   
    // PopAndDestroy all items in CleanupStack.
    CleanupStack::PopAndDestroy( addr ); 
    CleanupStack::PopAndDestroy( enc );
    CleanupStack::PopAndDestroy( conv );
    CleanupStack::PopAndDestroy( tmp );
    
    SIPStrings::Close();
    return result;
    }

// -----------------------------------------------------------------------------
// SIPServerSettingValidator::ValidateServerAddressL
// Validates proxy/registrar server address
// -----------------------------------------------------------------------------
//
TBool SIPServerSettingValidator::ValidateServerAddressL( TAny* aDes )
    {   
    __GSLOGSTRING("SIPServerSettingValidator::ValidateServerAddressL" )
    __ASSERT_ALWAYS( aDes != NULL, User::Leave( KErrArgument ) );
    TBool result( ETrue );
    TBool onlySpaces( ETrue );
    
    SIPStrings::OpenL();    // Needed for CSIPAddress.
    
    TDes* desPtr = static_cast<TDes*>( aDes );
    TDes& des = *desPtr;

    AknTextUtils::StripCharacters( des, KAknStripListControlChars );

    // Check that is there other characters than spaces.
    for ( TInt i = 0; i < des.Length() && onlySpaces; i++ )
        {
        const TChar KSpaceCharacter = ' ';
        if ( des[ i ] != KSpaceCharacter )
            {
            onlySpaces = EFalse;
            }
        }
    
    // Allow user set spaces, spaces are removed elsewhere.
    if ( !onlySpaces )
        {
        HBufC8* finalAddress = HBufC8::NewLC( KMaxProxyLength * 2 );
        
        // Convert address to 8-bit descriptor format.
        HBufC8* conv = EscapeUtils::ConvertFromUnicodeToUtf8L( des );
        CleanupStack::PushL( conv );
        
        // Parse address.
        TUriParser8 parser;
        User::LeaveIfError( parser.Parse( conv->Des() ) );
        
        // Check if scheme included and also if brackets in host correctly.
        TInt sipSchemeIncluded = 
        	GSSIPProfileUtil::SchemeIncluded( conv->Des(), ETrue );
        if ( sipSchemeIncluded == KErrNotFound )
        	{ 
        	// Brackets in host incorrectly.
        	result = EFalse;
        	}
        else if ( !sipSchemeIncluded && parser.IsPresent( EUriScheme ) )
			{
			// Scheme exists but it's not "sip" or "sips" scheme.
			result = EFalse;
			}
		else if ( !sipSchemeIncluded )
		    {
		    // Create address for validation. Scheme is added.
		    finalAddress->Des().Append( KSIPProtocolHeader );
		    finalAddress->Des().Append( KSIPColonSeparator );
		    finalAddress->Des().Append( conv->Des() );
		    if ( GSSIPProfileUtil::URIValid( *finalAddress ) )
		        {
		        result = EFalse;
		        }
		    }
		else
			{
			// Create address for validation.
			finalAddress->Des().Append( conv->Des() );
			if ( GSSIPProfileUtil::URIValid( *finalAddress ) )
				{
				result = EFalse;
				}
			}
		CleanupStack::PopAndDestroy( conv );
		
		// Validate entered host.
    	if ( result )    	
    		{
    		User::LeaveIfError( parser.Parse( *finalAddress ) );
            if ( parser.Validate() != KErrNone )
                {
                result = EFalse;
                }
    		}
        
        CleanupStack::PopAndDestroy( finalAddress );
        }
    
    SIPStrings::Close();
    return result;
    }

//  End of File  
