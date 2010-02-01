/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : SigcompValueConverter.h
* Part of       : SIPSigComp
* Version       : SIP/3.0 
*
*/




/**
 @internalComponent
*/

#ifndef __SIGCOMPVALUECONVERTER_H__
#define __SIGCOMPVALUECONVERTER_H__

#include "sigcomp.h"
#include <e32std.h>

#ifdef CPPUNIT_TEST
#undef IMPORT_C
#define IMPORT_C
#endif

class TSigcompValueConverter
	{
public:

	static CSigComp::TMemorySize ConvertDecompressionMemorySize(TUint aSize);
	static CSigComp::TMemorySize ConvertStateMemorySize(TUint aSize);
	static CSigComp::TCyclesPerBit ConvertCyclesPerBit(TUint aCpb);
	};
#endif // end of __SIGCOMPVALUECONVERTER_H__

// End of File
