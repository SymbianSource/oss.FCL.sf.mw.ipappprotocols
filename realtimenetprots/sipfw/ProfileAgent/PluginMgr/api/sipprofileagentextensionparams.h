/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Name        : sipprofileagentextensionparams.h
* Part of     : SIP / SIP Profile Agent
*
*/



#ifndef SIPPROFILEAGENTEXTENSIONPARAMS_H_
#define SIPPROFILEAGENTEXTENSIONPARAMS_H_

//  INCLUDES
#include <e32base.h>
#include <sipprofiletypeinfo.h>

//FORWARD DECLARATIONS
class CSipProfileAgentConfigRetriever;

/**
 * @publishedPartner
 * @deprecated
 */
#define KSipProfileAgentConfigurationExtParam  1

/**
 * @publishedPartner
 * @released
 */
#define KSipProfileAgentConfigExtension  100

// CLASS DECLARATION
/**
* @publishedPartner
* @deprecated
*
* Contains extension parameters needed for SIP Profile Agent plug-ins.
* @lib N/A
*/
class MSipProfileAgentConfigExtension
    {
    
public:
    /**
     * Defines the profiletype specifc configration properties
     */
    enum TProfileProperty
        {
        EProfileRegistrationValue,
        EMaxProfileProperty
        };
public:
    /**
    * Returns the default registration expiry value based on the profile type.
    */
    virtual TUint ExpiryValue(TSIPProfileTypeInfo::TSIPProfileClass aClass) = 0;
    };

// CLASS DECLARATION
/**
* @publishedPartner
* @released
*
* Contains extension parameters needed for SIP Profile Agent plug-ins.
* @lib N/A
*/
class CSipProfileAgentConfigExtension : public CBase
	{
	
public:
	/**
	 * Defines the profiletype specifc configration properties
	 */
	enum TProfileProperty
		{
		/*REGISTER expiry value*/
		EProfileRegistrationValue,
		/*SUBSCRIBE expiry value*/
		EProfileSubscriptionValue,
		/*Marks the end of enum, its value may change*/
		EMaxProfileProperty
		};
	
public:
    static CSipProfileAgentConfigExtension* NewL();
    
    static CSipProfileAgentConfigExtension* NewLC();
    
    ~CSipProfileAgentConfigExtension();
    
    /**
    * Returns the default expiry value based on the profile type and property.
    */
    virtual TUint ExpiryValueL(TSIPProfileTypeInfo::TSIPProfileClass aClass,
            TProfileProperty aProperty);
    
protected:
    CSipProfileAgentConfigExtension();

private:
    void ConstructL();
    
    CSipProfileAgentConfigRetriever* iConfigRetriever;
	
	};
#endif /*SIPPROFILEAGENTEXTENSIONPARAMS_H_*/
