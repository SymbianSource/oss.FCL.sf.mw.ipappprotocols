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
* Name        : _sipcodecdefs.h
* Part of     : SIP Codec
* Interface   : SDK API, SIP Codec API
* Version     : SIP/4.0 
*
*/



#ifndef SIPCODECDEFS_H
#define SIPCODECDEFS_H

#ifdef CPPUNIT_TEST

#define UNIT_TEST(ClassName) friend class ClassName;

#else 

#define UNIT_TEST(ClassName)

#endif

#endif // SIPCODECDEFS_H
