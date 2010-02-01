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
* Name          : sipimsprofilemccandmnc.h
* Part of       : ProfileAgent
* Version       : SIP/4.1 
*
*/




/**
 @internalComponent
*/

#ifndef SIPIMSPROFILEMCCANDMNC_H
#define SIPIMSPROFILEMCCANDMNC_H

// INCLUDES
#include <etelmm.h>
#include <mmretrieve.h>

// FORWARD DECLARATIONS
class CSIPProfileSIMRecord;
class MSIPProfileIMSAuthorizationDataObserver;
//---------------------------------------------------------
/**
 * @brief Class 
 * Requested to get the MCC & MNC from the SIM
 * @class CSIPIMSProfileMCCandMNC
 *
 */
class CSIPIMSProfileMCCandMNC : public CActive
    {
public: //Constructors and destructor
    
    /**
    * Constructs an object
    * @param aObserver 
    * @return new instance, ownership is tranferred
    */
    static CSIPIMSProfileMCCandMNC* NewL( 
        MSIPProfileIMSAuthorizationDataObserver& aObserver, 
        RMobilePhone& aPhone );
        
    /**
    * Destructor
    */
    ~CSIPIMSProfileMCCandMNC();

public://new methods

    /**
    * Asyncronous request to CAsyncRetrieveAuthorizationInfo
    * @param aPhone the mobile phone
    */
    void RequestL();
	
	/**
    * Get the MCC of the SIM
    */
	RMobilePhone::TMobilePhoneNetworkCountryCode& NetworkCountryCode();
	
	/**
    * Get the MNC of the SIM
    */
	RMobilePhone::TMobilePhoneNetworkIdentity& NetworkIdentityCode();


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
    CSIPIMSProfileMCCandMNC( MSIPProfileIMSAuthorizationDataObserver&
    								 aObserver, RMobilePhone& aPhone);

    /**
    * 2nd phase constructor
    */
    void ConstructL();

private: //data
    
   RMobilePhone::TMobilePhoneNetworkCountryCode  iCountryCode;
   RMobilePhone::TMobilePhoneNetworkIdentity     iNetworkCode;
   TPckgBuf<RMobilePhone::TMobilePhoneNetworkInfoV1> iNetworkInfoPckg;

private: 
    
    MSIPProfileIMSAuthorizationDataObserver& iObserver;
    RMobilePhone& iPhone;
private: //For testing purposes
#ifdef CPPUNIT_TEST	
    friend class CSIPIMSProfileMCCandMNCStub;
#endif
    };


#endif // end of SIPIMSPROFILEMCCANDMNC_H

// End of File
