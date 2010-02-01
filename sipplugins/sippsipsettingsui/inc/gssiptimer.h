// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:  Timer for Hight light listbox item Container
//
//


#ifndef GSSIP_TIMER_H
#define GSSIP_TIMER_H

//  INCLUDES

#include <coecntrl.h>
#include <eiklbi.h>
#include <aknlists.h>

/**
*  CGSSIPTimer class
*/
class  CGSSIPTimer: public CBase,
                    public MBeating
    {
    public:  // Constructors and destructor
        
       /**
        * Two-phased constructor.
        * @return Pointer to created instance
        */
        static CGSSIPTimer* NewL( const CCoeControl& aContainer, 
                CListItemDrawer& aListItemDrawer );
        
         /**
         * Destructor.
         */
         virtual ~CGSSIPTimer();
                
         /**
         * By default Symbian 2nd phase constructor is private.
         */
         void ConstructL();    
                       
      
        /**
        * Record beat count
        */
         void Beat();
         
        /**
        * Synchronize
        */
         void Synchronize();
         
        /**
        * Start timer
        */
         void StartTimer();
         
        /**
        * Stop timer
        */
         void StopTimer();
         
        /**
        * IsStarted()
        */      
         TBool IsStarted() const;
              
       
    private:        
        
        /**
         * C++ default constructor.        
         */
        CGSSIPTimer( const CCoeControl& aContainer, 
                     CListItemDrawer& aListItemDrawer );                

    public:
        // Timer count 
        TInt iTotal;

    private: 
    
        // Timer
        CHeartbeat* iHeart;
        
        //parameter iContainer
        const CCoeControl& iContainer;
        
        //parameter iListItemDrawer
        CListItemDrawer& iListItemDrawer;
        
        
        // Check if timer is started
        TBool iIfStart;
        /**
         * For testing purposes
         */        
        friend class UT_GSSIPTimer;
    };

#endif      // GSSIP_TIMER_H
            
// End of File
