// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name          : TSIPMemAllocFailureSimulation.cpp
// Part of       : SIPServerCore
// Version       : SIP/4.0 
//




#include "TSIPMemAllocFailureSimulation.h"
#include <f32file.h>


_LIT(KIniFileName, "\\sip\\MemAllocFailureSimulation.ini");
const TInt KParamBufferSize = 100;

// -----------------------------------------------------------------------------
// TSIPMemAllocFailureSimulation::Start
// -----------------------------------------------------------------------------
//
TInt TSIPMemAllocFailureSimulation::Start()
	{
	TInt err = KErrNone;
	
	TBuf8<KParamBufferSize> buf;
	err = ReadParamBufferFromFile(buf);
	if (err != KErrNone) 
        {
        return err;
        }
	TUint type;
	TUint value;
	err = ParseParams(buf,type,value); 
	if (err != KErrNone)
        {
        return err;
        }

	__UHEAP_SETFAIL (static_cast<RHeap::TAllocFail>(type),value);

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// TSIPMemAllocFailureSimulation::Stop
// -----------------------------------------------------------------------------
//
void TSIPMemAllocFailureSimulation::Stop()
	{
	__UHEAP_RESET;
	}

// -----------------------------------------------------------------------------
// TSIPMemAllocFailureSimulation::ReadParamBufferFromFile
// -----------------------------------------------------------------------------
//
TInt TSIPMemAllocFailureSimulation::ReadParamBufferFromFile (TDes8& aDes)
	{
	RFs fileSession;
	TInt err = fileSession.Connect();
	if (err != KErrNone) 
        {
        return err;
        }
	RFile file;
	err = file.Open(fileSession, KIniFileName, EFileRead|EFileShareAny);
	if (err != KErrNone)
		{
		fileSession.Close();
		return err;
		}
	err = file.Read(aDes);
	file.Close();
	fileSession.Close();
	return err;
	}

// -----------------------------------------------------------------------------
// TSIPMemAllocFailureSimulation::ParseParams
// -----------------------------------------------------------------------------
//
TInt TSIPMemAllocFailureSimulation::ParseParams (const TDesC8& aDes,
												 TUint& aType,
												 TUint& aValue)
	{
	TLex8 lex(aDes);
	lex.SkipSpace();
	TInt err = lex.Val(aType);
	if (err != KErrNone)
        {
        return err;
        }
	if (aType > RAllocator::EFailNext) 
        {
        return KErrCorrupt;
        }
	lex.SkipSpace();
	return lex.Val(aValue);
	}
