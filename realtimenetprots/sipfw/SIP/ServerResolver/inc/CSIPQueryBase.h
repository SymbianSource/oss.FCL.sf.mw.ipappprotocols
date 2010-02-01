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
* Name          : CSIPQueryBase.h
* Part of       : ServerResolver
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/

#ifndef CSIPQUERYBASE_H
#define CSIPQUERYBASE_H

//INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RStringF;
class MSIPHostResolver;

class CSIPQueryBase : public CBase
	{
	public:

		virtual ~CSIPQueryBase();
		
	public:	
		
		virtual void Query( MSIPHostResolver& aResolver ) = 0; 
		
		virtual void HandleQueryResultL( MSIPHostResolver& aResolver ) = 0;
		
	public:
	
		virtual const TDesC8& ResultTargetL();
		
		virtual RStringF ResultProtocolL();
		
		virtual TUint ResultPortL();
		
		virtual void RemoveElementL( TInt aIndex );
		
		virtual TInt ArrayCount();
		
		virtual void SetTargetL( const TDesC8& aTarget );
	
	protected:

		CSIPQueryBase();
	
	};
#endif // end of CSIPQUERYBASE_H

// End of File
