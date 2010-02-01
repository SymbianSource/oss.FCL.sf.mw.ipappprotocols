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
* Name        : CSocketOpt.h
* Part of     : ConnectionMgr
* See class definition below.
* Version     : SIP/4.0
*
*/



#ifndef __CSOCKETOPT_H__

/**
* @internalComponent
*/
#define __CSOCKETOPT_H__
  
// INCLUDES
#include <e32base.h>
#include <es_sock.h>
  
class CSocketOpt : public CBase
    {
public: // Constructor and destructor
    
    static CSocketOpt* NewLC( TUint aOptionName,
                              TUint aOptionLevel,
                              TInt aOption );
    
    static CSocketOpt* NewLC( TUint aOptionName,
                              TUint aOptionLevel,
                              const TDesC8& aOption );    
    ~CSocketOpt();
    
private: // Private contruction

    void ConstructL( const TDesC8& aOption = KNullDesC8 );
    
    CSocketOpt( TUint aOptionName,
                TUint aOptionLevel,
                TInt aOption = 0 );
    
public: // Public methods
      
    TUint OptionName() const;
    
    /**
    * Sets socket options 
    */
    CSocketOpt* SetOptL( RSocket& aSocket );

    
private: // Data
    
    TUint iOptionName;
    
    TUint iOptionLevel;
    
    TInt iOption;
    
    HBufC8* iOptionBuf;
    
private: // For unit testing
#ifdef CPPUNIT_TEST
    friend class COwnerSettingsListTest;
#endif
    };
    
#endif // __CSOCKETOPT_H__
