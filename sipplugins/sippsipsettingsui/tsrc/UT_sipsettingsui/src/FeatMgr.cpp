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
#include "FeatMgr.h"


// =============================================================================
// Stub variables for FeatureManager
// =============================================================================

TBool FeatureManager::iIsCameraSupported = ETrue;
TBool FeatureManager::iIsMultimediaSharingSupported = ETrue;
TBool FeatureManager::iIsIdHelpSupported = ETrue;


// =============================================================================
// Stub functions for FeatureManager
// =============================================================================

EXPORT_C TBool FeatureManager::FeatureSupported( TInt aFeature )
    {
    if (aFeature == KFeatureIdCamera)
       return iIsCameraSupported;
    else if (aFeature == KFeatureIdMultimediaSharing)
       return iIsMultimediaSharingSupported;
    else if ( aFeature == KFeatureIdHelp )
    	return iIsIdHelpSupported;
    else 
       return EFalse;
    }

EXPORT_C void FeatureManager::CameraNotSupported()
    {
    iIsCameraSupported = EFalse;
    }

EXPORT_C void FeatureManager::CameraSupported()
    {
    iIsCameraSupported = ETrue;
    }
	
EXPORT_C void FeatureManager::MultimediaSharingNotSupported()
    {
    iIsMultimediaSharingSupported = EFalse;
    }

EXPORT_C void FeatureManager::MultimediaSharingSupported()
    {
    iIsMultimediaSharingSupported = ETrue;
    }

EXPORT_C void FeatureManager::IDHelpSupported()
	{
	iIsIdHelpSupported = ETrue;
	}
EXPORT_C void FeatureManager::IDHelpNotSupported()
	{
	iIsIdHelpSupported = EFalse;
	}
	
void FeatureManager::InitializeLibL()
    {

    }

void FeatureManager::UnInitializeLib()
    {

    }
