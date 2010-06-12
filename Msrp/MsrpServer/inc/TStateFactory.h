/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html."
* Initial Contributors:
* Nokia Corporation - initial contribution.
* Contributors:
*
* Description:
* MSRP Implementation
*
*/

#ifndef TSTATEFACTORY_H_
#define TSTATEFACTORY_H_

#include "TStates.h"

class CStateFactory : public CBase
	{
	public:
	    static CStateFactory* NewL();
		static CStateFactory* NewLC();
		
		~CStateFactory();
		
	    TStateBase* getStateL(TStates state);	    

	private:
		CStateFactory();		
		void ConstructL();
		
	private:		
		RPointerArray<TStateBase> iStateArray;
		
		#ifdef __UT_TSTATEFACTORY_H__
		friend class UT_TStateFactory;
		#endif
	};


#endif /* TSTATEFACTORY_HPP_ */
