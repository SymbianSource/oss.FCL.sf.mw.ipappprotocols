
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
*
*/



#ifndef __CSIPLINEADDRQUERYDLG_H__
#define __CSIPLINEADDRQUERYDLG_H__

// INCLUDES
#include    <eikdialg.h>


// FORWARD DECLARATIONS
class CSIPAddress;

// CLASS DECLARATION
/**
* A recipient's address query dialog for game invitation.
*/
class CSIPExAddrQueryDlg : public CEikDialog
    {
    public:

        /**
        * Static constructor
        * @param aAddr On return contains the other player's address.
        */
        static CSIPExAddrQueryDlg* NewL( TDes& aAddr );

        /**
        * Static constructor. On return the instance is left to the
        * CleanupStack.
        * @param aAddr On return contains the other player's address.
        */
        static CSIPExAddrQueryDlg* NewLC( TDes& aAddr );

        /**
        * Destructor
        */
        ~CSIPExAddrQueryDlg();

    private:

        /**
        * C++ default constructor
        * @param aAddr On return contains the other player's address.
        */
        CSIPExAddrQueryDlg( TDes& aAddr );
        
        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();
    
    private: // From base classes

        /**
        * From CEikDialog
        */
        TBool OkToExitL(TInt aKeyCode);

        /**
        * From CEikDialog
        */
        virtual void PreLayoutDynInitL();
        
    private: // New functions
    
        /**
        * Checks if user typed address is valid sip address.
        * @param aSipAddr address which validity is checked
        * Returns true if valid address, otherwise returns false
        */
        TBool AddressValid( const TDesC8& aSipAddr );

    private:
        // Member variables
        TDes&   iAddress;
    };

#endif // __CSIPLINEADDRQUERYDLG_H__

// End of file
