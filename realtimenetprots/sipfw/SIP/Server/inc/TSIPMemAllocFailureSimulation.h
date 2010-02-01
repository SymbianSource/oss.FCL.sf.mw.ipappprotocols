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
* Name          : TSIPMemAllocFailureSimulation.h
* Part of       : SIPServerCore
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef TSIPMEMALLOCFAILURESIMULATION_H
#define TSIPMEMALLOCFAILURESIMULATION_H

#include <e32std.h>
#include "_sipcodecdefs.h"


class TSIPMemAllocFailureSimulation
	{
public:

	static TInt Start();
	static void Stop();

private:

	static TInt ReadParamBufferFromFile (TDes8& aDes);
	static TInt ParseParams (const TDesC8& aDes, TUint& aType, TUint& aValue);

private: // For testing purposes

	UNIT_TEST(CSIPMemAllocFailureSimulationTest)
	};

#endif // TSIPMEMALLOCFAILURESIMULATION_H

// End of File
