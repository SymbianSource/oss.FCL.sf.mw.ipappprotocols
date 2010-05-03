/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : sipprofileimsauthorization.h
* Part of       : ProfileAgent
* Version       : SIP/4.1 
*
*/




/**
 @internalComponent
*/

#ifndef SIPPROFILEIMSAUTHORIZATION_H
#define SIPPROFILEIMSAUTHORIZATION_H

// INCLUDES
#include <etelmm.h>
#include <mmretrieve.h>

// FORWARD DECLARATIONS
class CSIPProfileSIMRecord;
class MSIPProfileIMSAuthorizationDataObserver;

//---------------------------------------------------------

/**
 * @brief Class represent USIM/ISIM authorization
 * 
 */
class CSIPProfileIMSAuthorization : public CActive
    {
public: //Constructors and destructor
    
    /**
    * Constructs an object
    * @param aObserver
    * @return new instance, ownership is tranferred
    */
    static CSIPProfileIMSAuthorization* NewL( 
        MSIPProfileIMSAuthorizationDataObserver& aObserver,
        RMobilePhone& aPhone );
        
    /**
    * Destructor
    */
    ~CSIPProfileIMSAuthorization();

public://new methods

    /**
    * Starts
    */
    void RequestL();

    /**
    * Get 
    */
    RMobilePhone::CImsAuthorizationInfoV5& Response();
    
    /**
     * Checks whether IMSAuthorization is supported  
     */
    TBool IsImsAuthorizationAllowed();
    
    /*
     * Resets IMS Authorization value
     */
    void ResetImsAuthorizationAllowed(TBool aStatus);
    
public: //From CActive
    
    /**
    * Handles result
    */
    void RunL();
    
    TInt RunError(TInt aError);
    
    /**
    * Cancel
    */
    void DoCancel();

private: //methods

    /**
    * Constructor
    * @param aObserver
    * @param aPhone    
    */
    CSIPProfileIMSAuthorization( MSIPProfileIMSAuthorizationDataObserver& 
    							aObserver, RMobilePhone& aPhone );

    /**
    * 2nd phase constructor
    */
    void ConstructL();

private: //data

    RMobilePhone::CImsAuthorizationInfoV5* iData;
    CAsyncRetrieveAuthorizationInfo* iRetriever;

private: 
    
    MSIPProfileIMSAuthorizationDataObserver& iObserver; 
    RMobilePhone& iPhone;
    TBool iIMSAuthorizationNotSupported;
    };

#endif // end of SIPPROFILEIMSAUTHORIZATION_H

// End of File
