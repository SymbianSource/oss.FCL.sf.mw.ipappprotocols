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
* Name          : MCompartmentUserOwner.h
* Part of       : SIP SigComp Controller
* Version       : SIP/4.0 
*
*/




/**
 @internalComponent
*/


#ifndef __MCOMPARTMENTUSEROWNER_H__
#define __MCOMPARTMENTUSEROWNER_H__

#include <e32base.h>

class CSipSigCompCompartmentCtx;

class MCompartmentUserOwner
	{
public:
	virtual ~MCompartmentUserOwner() {}

    /**
     * Searches whether any existing compartment is related to passed
     * remote address
     *
     * @returns compartmentCtx, ownership is not transferred
     */
	virtual CSipSigCompCompartmentCtx* SearchCompartmentByInetAddr( 
	                                            const TInetAddr& aAddress,
    										    TUint32 aIapId ) = 0;
    
    /**
     * Creates new compartmentCtx (including real sigcomp compartment)
     * for interactions between passed remote address. CompartmentCtx
     *
     * @returns compartmentCtx, ownership is not transferred
     */																		
    virtual CSipSigCompCompartmentCtx* CreateCompartmentL( 
                                                const TInetAddr& aAddress,
                                            	TUint32 aIapId,
	                                            TBool aDynamicCompression ) = 0;
	};

#endif // end of __MCOMPARTMENTUSEROWNER_H__

// End of File
