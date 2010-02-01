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
// Name        : TTCPMsgState.cpp
// Part of     : ConnectionMgr
// Version     : SIP/4.0
//



#include "TTCPMsgState.h"
#include "MMsgAssemblerContext.h"


// ----------------------------------------------------------------------------
// TTCPMsgState::TTCPMsgState
// ----------------------------------------------------------------------------
//
TTCPMsgState::TTCPMsgState( MMsgAssemblerContext& aMsgAssembler )
	: iMsgAssembler( aMsgAssembler )
	{
	}
	
// ----------------------------------------------------------------------------
// TTCPMsgState::FindHeaderStartPosition
// ----------------------------------------------------------------------------
//
TInt TTCPMsgState::FindHeaderStartPosition( TDesC8& aData )
	{
	TLex8 lex(aData);
	lex.SkipSpace();
	return lex.Offset();
	}

// ----------------------------------------------------------------------------
// TTCPMsgState::FindHeaderEndPosition
// ----------------------------------------------------------------------------
//
TInt TTCPMsgState::FindHeaderEndPosition( TDesC8& aData )
	{
	_LIT8(KCRLFCRLF, "\r\n\r\n");
	_LIT8(KCRCRLF,   "\r\r\n");
	_LIT8(KLFCRLF,   "\n\r\n");

	TInt endPosition = KErrNotFound;
	TInt cRLFCRLFPosition = aData.Find(KCRLFCRLF); 
	TInt cRCRLFPosition   = aData.Find(KCRCRLF);
	TInt lFCRLFPosition   = aData.Find(KLFCRLF);

	if (cRLFCRLFPosition >= 0) 
		{
		endPosition = cRLFCRLFPosition + KCRLFCRLF().Length();		
		}

	if (cRCRLFPosition >= 0)
		{
		GetHeaderPartEndPosition(cRCRLFPosition,
								 KCRCRLF().Length(),
								 endPosition);
		}

	if (lFCRLFPosition >= 0)
		{
		GetHeaderPartEndPosition(lFCRLFPosition,
								 KLFCRLF().Length(),
								 endPosition);
		}

	return endPosition;
	}

// ----------------------------------------------------------------------------
// TTCPMsgState::GetHeaderPartEndPosition
// ----------------------------------------------------------------------------
//
void TTCPMsgState::GetHeaderPartEndPosition ( TInt  aEndMarkPosition, 
		                                      TInt  aEndMarkLength,  
		                                      TInt& aHeaderEndPosition )
	{
	aEndMarkPosition = aEndMarkPosition + aEndMarkLength;
		
	if (aHeaderEndPosition > 0)
		{
		if (aEndMarkPosition < aHeaderEndPosition )
			{
			aHeaderEndPosition  = aEndMarkPosition;
			}
		}
	else
		{
		aHeaderEndPosition = aEndMarkPosition;
		}
	}
