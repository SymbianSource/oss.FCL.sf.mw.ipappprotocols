/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name          : MSIPMsgAssemblerOwner.h
* Part of       : ConnectionMgr
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef __MSIPMSGASSEMBLEROWNER_H__
#define __MSIPMSGASSEMBLEROWNER_H__

class CSIPMessage;

class MSIPMsgAssemblerOwner
	{
public:
	virtual ~MSIPMsgAssemblerOwner() {}
	virtual void MessageCompleteL(CSIPMessage* aSIPMessage,
								  TInt aError,
								  TBool aCompressed) = 0;
	};

#endif // end of __MSIPMSGASSEMBLEROWNER_H__

// End of File
