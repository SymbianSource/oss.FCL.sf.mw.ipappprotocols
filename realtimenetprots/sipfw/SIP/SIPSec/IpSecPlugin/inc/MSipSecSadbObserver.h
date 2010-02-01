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
* Name          : MSipSecSadbObserver.h
* Part of       : SIPSec
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef MSIPSECSADBOBSERVER_H
#define MSIPSECSADBOBSERVER_H

/**
 * @brief Observer for callbacks of sadb handler
 *
 * 
 */
class MSipSecSadbObserver
	{

public:

    virtual void SAsAddedL( TBool aSuccess ) = 0;

	virtual void SAsDeletedL() = 0;

	};

#endif // MSIPSECSADBOBSERVER_H


// End of File