/*
* Copyright (c) 2003 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the panic codes
*
*/


#ifndef GSSIPPANICS_H
#define GSSIPPANICS_H

_LIT( KPanicCategory, "GSUi" );

/**
* Panic code class.
* @since 3.0
*/

enum TPanicCodes
    {
    EComponentControlIndexOOB = -5000,
    EAPListItemZeroCount = -5001,
    ECouldNotReadAPList = -5002,
    ENoDefaultProfile = -5003,
    ETypeInfoArrayIndexOOB = -5004
    };

#endif      // GSSIPPANICS_H   
            
// End of File