/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
// FeatMgr.h
//
//stubsfor Feature Manager

#ifndef FEATUREMANAGER_H
#define FEATUREMANAGER_H


//INCLUDES
#include <e32std.h>


class FeatureManager
    {
    public: 

		/**
        * Fetches information whether a certain feature is supported.
        * @return feature support status
        */
        IMPORT_C static TBool FeatureSupported( TInt aFeature );
		
        static void InitializeLibL();
        static void UnInitializeLib();

        // functions to set/unset camera feature
        IMPORT_C static void CameraNotSupported();
        IMPORT_C static void CameraSupported();
		
		// functions to set/unset MultimediaSharing feature
        IMPORT_C static void MultimediaSharingNotSupported();
        IMPORT_C static void MultimediaSharingSupported();
        
        // functions to set/unset MultimediaSharing feature
        IMPORT_C static void IDHelpSupported();
        IMPORT_C static void IDHelpNotSupported();

    private:
        // member to 
        static TBool iIsCameraSupported;
        static TBool iIsMultimediaSharingSupported;
        static TBool iIsIdHelpSupported;
    };

#endif      // FEATUREMANAGER_H
            
// End of File
