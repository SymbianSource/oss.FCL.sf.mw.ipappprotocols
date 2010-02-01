// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name          : SigcompValueConverter.cpp
// Part of       : SIPSigComp
// Version       : SIP/3.0 
//




#include "sigcompvalueconverter.h"
// -----------------------------------------------------------------------------
// TSigcompValueConverter::ConvertDecompressionMemorySize
// -----------------------------------------------------------------------------
//
CSigComp::TMemorySize TSigcompValueConverter::ConvertDecompressionMemorySize(TUint aSize)
	{
	CSigComp::TMemorySize enumSize;

	switch (aSize)
		{
		case 2048:
			enumSize = CSigComp::E2048;
			break;
		case 4096:
			enumSize = CSigComp::E4096;
			break;
		case 8192:
			enumSize = CSigComp::E8192;
			break;
		case 16384:
			enumSize = CSigComp::E16384;
			break;
		case 32768:
			enumSize = CSigComp::E32768;
			break;
		case 65536:
			enumSize = CSigComp::E65536;
			break;
		case 131072:
			enumSize = CSigComp::E131072;
			break;
		default:
			enumSize = CSigComp::E8192;
			break;
		}
	return enumSize;
	}
// -----------------------------------------------------------------------------
// TSigcompValueConverter::ConvertStateMemorySize
// -----------------------------------------------------------------------------
//
CSigComp::TMemorySize TSigcompValueConverter::ConvertStateMemorySize(TUint aSize)
	{
		CSigComp::TMemorySize enumSize;

	switch (aSize)
		{
		case 2048:
			enumSize = CSigComp::E2048;
			break;
		case 4096:
			enumSize = CSigComp::E4096;
			break;
		case 8192:
			enumSize = CSigComp::E8192;
			break;
		case 16384:
			enumSize = CSigComp::E16384;
			break;
		case 32768:
			enumSize = CSigComp::E32768;
			break;
		case 65536:
			enumSize = CSigComp::E65536;
			break;
		case 131072:
			enumSize = CSigComp::E131072;
			break;
		default:
			enumSize = CSigComp::E2048;
			break;
		}
	return enumSize;
	}

// -----------------------------------------------------------------------------
// TSigcompValueConverter::ConvertCyclesPerBit
// -----------------------------------------------------------------------------
//
CSigComp::TCyclesPerBit TSigcompValueConverter::ConvertCyclesPerBit(TUint aCpb)
	{
	CSigComp::TCyclesPerBit enumValue;

	switch (aCpb)
		{
		case 16:
			enumValue = CSigComp::E16;
			break;
		case 32:
			enumValue = CSigComp::E32;
			break;
		case 64:
			enumValue = CSigComp::E64;
			break;
		case 128:
			enumValue = CSigComp::E128;
			break;
		default:
			enumValue = CSigComp::E16;
			break;
		}
	return enumValue;
	}
