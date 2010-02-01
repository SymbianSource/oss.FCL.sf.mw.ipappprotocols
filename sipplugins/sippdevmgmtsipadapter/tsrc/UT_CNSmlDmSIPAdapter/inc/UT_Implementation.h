/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef UT_IMPLEMENTATION_H
#define UT_IMPLEMENTATION_H

#include <e32base.h>

class TImplementation : public CBase, public MSmlDmDDFObject
     {
public:
     TImplementation()
         {
         iImpl = NULL;
         }
     
     ~TImplementation()
         {
         delete iImpl;
         }
 
     void SetAccessTypesL( TSmlDmAccessTypes /*aAccessTypes*/ )
         {
         }
     
     void SetDefaultValueL( const TDesC8& /*aDefaultValue*/ )
         {
         }
     
     void SetDescriptionL( const TDesC8& /*aDescription*/ )
         {
         }
     
     void SetDFFormatL( TDFFormat /*aFormat*/ )
         {
         }
     
     void SetOccurenceL( TOccurence /*aOccurence*/ )
         {
         }
     
     void SetScopeL( TScope /*aScope*/ )
         {
         }
     
     void SetDFTitleL( const TDesC8& /*aTitle*/ )
         {
         }
     
     void AddDFTypeMimeTypeL( const TDesC8& /*aMimeType*/ )
         {
         }
     
     void SetAsObjectGroup()
         {
         }
     
     MSmlDmDDFObject& AddChildObjectL(const TDesC8& /*aNodeName*/ )
         {
         if ( !iImpl )
             {
             iImpl = new ( ELeave ) TImplementation;
             }
         return *iImpl;
         }
     
     MSmlDmDDFObject& AddChildObjectGroupL()
         {
         if ( !iImpl )
             {
             iImpl = new ( ELeave ) TImplementation;
             }
         
         return *iImpl;
         }
     
     TImplementation* iImpl;
     };

#endif // UT_IMPLEMENTATION_H
