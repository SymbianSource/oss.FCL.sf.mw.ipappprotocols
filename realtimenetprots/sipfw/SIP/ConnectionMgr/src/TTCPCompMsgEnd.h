/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : TTCPCompMsgEnd.h
* Part of     : ConnectionMgr
* Version     : SIP/4.0
*
*/




/**
 @internalComponent
*/

#ifndef __TTCPCOMPMSGEND_H__
#define __TTCPCOMPMSGEND_H__

// INCLUDES
#include "TTCPMsgState.h"

// FORWARD DECLARATIONS
class MMsgAssemblerContext;

class TTCPCompMsgEnd : public TTCPMsgState
	{
public:

    TTCPCompMsgEnd( MMsgAssemblerContext& aMsgAssembler )
    	: TTCPMsgState( aMsgAssembler ) {};

public: 
	void DataReceivedL( TPtr8 aData, TUint&  aNextLength );
	void DecideNextStateL( TPtr8 aData, TUint&  aNextLength );
	};

#endif // end of __TTCPCOMPMSGEND_H__

// end of file
