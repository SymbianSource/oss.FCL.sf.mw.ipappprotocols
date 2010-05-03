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
* Name          : sipprofilesimauthorization.h
* Part of       : ProfileAgent
* Version       : SIP/4.1 
*
*/




/**
 @internalComponent
*/


#ifndef SIPPROFILESIMAUTHORIZATION_H
#define SIPPROFILESIMAUTHORIZATION_H

// INCLUDES
#include <etelmm.h>
#include <mmretrieve.h>

// FORWARD DECLARATIONS
class CSIPProfileSIMRecord;
class MSIPProfileIMSAuthorizationDataObserver;


//---------------------------------------------------------
/**
 * @brief Class 
 *
 */
class CSIPProfileSIMAuthorization : public CActive
    {
public: //Constructors and destructor
    
    /**
    * Constructs an object
    * @param aObserver 
    * @return new instance, ownership is tranferred
    */
    static CSIPProfileSIMAuthorization* NewL( 
        MSIPProfileIMSAuthorizationDataObserver& aObserver, 
        RMobilePhone& aPhone );
        
    /**
    * Destructor
    */
    ~CSIPProfileSIMAuthorization();

public://new methods

    /**
    * Asyncronous request to get the IMSI from the SIM
    * @param aPhone the mobile phone
    */
    void RequestL();
   
    RMobilePhone::TMobilePhoneSubscriberId& IMSI();
    
    /*
     * Quires whether SIM Authorization is allowed
     */
    TBool IsSIMAuthorizationAllowed();
    
    /*
     * Resets SIM Authorization value
     */
    void ResetSIMAuthorizationAllowed(TBool aStatus);
    
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
    */
    CSIPProfileSIMAuthorization( MSIPProfileIMSAuthorizationDataObserver& 
    								aObserver, RMobilePhone& aPhone);

    /**
    * 2nd phase constructor
    */
    void ConstructL();

private: //data

    
    RMobilePhone::TMobilePhoneSubscriberId  iIMSI;

private: 
    
    /**
    * Observer
    */
    MSIPProfileIMSAuthorizationDataObserver& iObserver; 
    RMobilePhone& iPhone;
    TBool iSIMAuthorizationNotSupported;
    
private: //For testing  purpose
#ifdef CPPUNIT_TEST	
    friend class CSIPProfileSIMAuthorizationStub;
#endif
    };

#endif // end of SIPPROFILESIMAUTHORIZATION_H

// End of File
