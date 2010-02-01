
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



#ifndef __CSIPEXIMDIALOG_H__
#define __CSIPEXIMDIALOG_H__

// INCLUDES
#include <eikdialg.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CSIPAddress;

// CLASS DECLARATION
/**
* The dialog for quering the instant message recipient address
*              and the message.
*/
class CSIPExIMDialog : public CEikDialog
    {
    public:

        /**
        * Static constructor
        * @param aAddress On return contains the recipients address.
        * @param aMsg On return contains the message.
        */
        static CSIPExIMDialog* NewL( TDes& aAddress, TDes& aMsg );

        /**
        * Static constructor. On return the instance is left to the
        * CleanupStack
        * @param aAddress On return contains the recipients address.
        * @param aMsg On return contains the message.
        */
        static CSIPExIMDialog* NewLC( TDes& aAddress, TDes& aMsg );

        /**
        * Destructor
        */
        ~CSIPExIMDialog();

    private:

        /**
        * C++ default constructor
        * @param aAddress On return contains the recipients address.
        * @param aMsg On return contains the message.
        */
        CSIPExIMDialog( TDes& aAddress, TDes& aMsg );

        /**
        * Symbian 2nd phase constructor that can leave.
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
        
    private:
    
        /**
        * Checks if user typed address is valid sip address.
        * @param aSipAddr address which validity is checked
        * Returns true if valid address, otherwise returns false
        */
        TBool AddressValid( const TDesC8& aSipAddr );

    private:
        // Member variables

        TDes& iAddress;
        TDes& iMsg;
    };

#endif // __CSIPEXIMDIALOG_H__

// End of file
