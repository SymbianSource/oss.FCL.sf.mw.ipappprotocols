/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : sipconcreteprofilecomparison.h
* Part of     : SIP / SIP Profile Agent / SIP Concrete Profile
* Version     : %version: 2.1.1 %
*
*/




/**
 @internalComponent
*/

#ifndef SIPCONCRETEPROFILECOMPARISON_H
#define SIPCONCRETEPROFILECOMPARISON_H

// INCLUDES
#include <e32base.h>
#include "sipconcreteprofile.h"

// CLASS DECLARATION
/**
*  Comparison functions for a SIP concrete profile.
*
*  @lib sipprofile.lib
*/
class SIPConcreteProfileComparison
	{
	public:

		/**
		* Compares a single field of two profiles
		* @param aValue field to be checked
		* @param aOldProfile old concrete profile
		* @param aOldProfile new concrete profile
		* @returns ETrue if only asked field have changed
		*/
		IMPORT_C static TBool ValueModified(
			  const CSIPConcreteProfile::TComparison aValue,
			  CSIPConcreteProfile& aOldProfile,
			  const CSIPConcreteProfile& aNewProfile);

		/**
		* Compares many fields of two profiles
		* @param aValue fields to be checked, like
		*        CSIPConcreteProfile::EId|CSIPConcreteProfile::EIAPId
		* @param aOldProfile old concrete profile
		* @param aOldProfile new concrete profile
		* @returns ETrue if only asked fields have changed
		*/
		IMPORT_C static TBool ValuesModified(
			  const TInt32 aValue,
			  CSIPConcreteProfile& aOldProfile,
			  const CSIPConcreteProfile& aNewProfile);
	};

#endif // SIPCONCRETEPROFILECOMPARISON_H
