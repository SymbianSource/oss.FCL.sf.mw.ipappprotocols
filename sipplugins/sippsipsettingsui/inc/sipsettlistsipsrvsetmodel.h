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
* Description:  The model of the server setting views
*
*/


#ifndef SIP_SETT_LIST_SIP_SRV_SET_MODEL_H
#define SIP_SETT_LIST_SIP_SRV_SET_MODEL_H

//  INCLUDES

#include <e32base.h>
#include <aknsettingitemlist.h>
#include <sipprofiletypeinfo.h>
#include "tgssipserverdata.h"

// FORWARD DECLARATIONS

class CGSSIPModel;
class CSIPRouteHeader;

// CLASS DECLARATION

/**
*  CSIPSettListSIPSrvSetModel model class
*  @since 3.0
*  model class for SIP Server settings view
*/
class CSIPSettListSIPSrvSetModel : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.        
        * @param aData The used server data structure
        * @param aProfileType for profile type
        * @param aLocked for profile lock info
        */
        static CSIPSettListSIPSrvSetModel* NewL( TGSSIPServerData& aData, 
            TSIPProfileTypeInfo::TSIPProfileClass aProfileType,
            TBool aLocked );
        
        /**
        * Destructor.
        */
        virtual ~CSIPSettListSIPSrvSetModel();

    public: // New functions

        /**
        * Fetches the settings of the modified server
        * @return Pointer to the server data
        */        
        TGSSIPServerData* Settings();        

        /**
        * Returns selected service profile type
        * @return type of selected service profile
        */
        TSIPProfileTypeInfo::TSIPProfileClass ProfileType();

        /**
        * Checks is currently shown profile locked
        * @return ETrue if profile is locked
        */
        TBool ProfileLocked();

    private:

        /**
        * C++ default constructor.        
        * @param aData Data structure for initialization
        * @param aProfileType for profile type
        * @param aLocked for profile lock info
        */
        CSIPSettListSIPSrvSetModel( TGSSIPServerData& aData,
            TSIPProfileTypeInfo::TSIPProfileClass,
            TBool aLocked );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data    

        // The temporary data structure
        TGSSIPServerData& iData;    
        // Type of selected service profile type
        TSIPProfileTypeInfo::TSIPProfileClass iProfileType;
        // Profile lock info
        TBool iLocked;
    };

#endif      // SIP_SETT_LIST_SIP_SRV_SET_MODEL_H   
            
// End of File
