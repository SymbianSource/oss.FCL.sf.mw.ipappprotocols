// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Description:
// CStateMachine definition file
// 
//

/**
 @file
 @internalComponent
*/

#ifndef SIP_STATE_MACHINE_BASE_H
#define SIP_STATE_MACHINE_BASE_H




class CStateMachine : public CActive
   {
   public:
      IMPORT_C ~CStateMachine();

      TInt LastError() const;
      void SetLastError( TInt aLastError );
      void SetClientStatus( TRequestStatus* aClientStatus );
      TRequestStatus* ClientStatus() const;
      void SetActiveEvent( CAsynchEvent* aEvent );
      void SetErrorEvent( CAsynchEvent* aEvent );
      void SetSuspendRequest( TBool aSuspendRequest );
      TBool SuspendRequest() const;

      void UpdateHistory( TInt aUpdate );
      TInt History() const;

      void RegisterNotify( MStateMachineNotify* aStateMachineNotify );
      void DeRegisterNotify( MStateMachineNotify* aStateMachineNotify );
   
      IMPORT_C void Start( TRequestStatus* aClientStatus, CAsynchEvent* aErrorEvent, MStateMachineNotify* aStateMachineNotify
         /*the object is NOT taking ownership of the params*/ );
      //CActive::Cancel method should not be used
      IMPORT_C void Cancel( TInt aLastError );

      IMPORT_C HBufC8* ReAllocL( TInt aNewLength );
      HBufC8* Fragment() const;

   protected:
      IMPORT_C CStateMachine();

	   IMPORT_C virtual void DoCancel();
	   IMPORT_C virtual void RunL();
	   IMPORT_C virtual TInt RunError(TInt aError);

      IMPORT_C void OnError();
      IMPORT_C virtual void OnCompletion();

   protected:
      CAsynchEvent* iActiveEvent; //referenced not owned
      CAsynchEvent* iErrorEvent; //referenced not owned
      TInt iHistory;
      TInt iLastError;

      TRequestStatus* iClientStatus; //optional
      HBufC8* iFragment; //shared data fragment buffer owned by this class

      MStateMachineNotify* iStateMachineNotify; //one at the time so far
      TBool iSuspendRequest : 1; //if ETrue causes the state machine to stop after completion of
      //ongoing asynch event (no client request will be completed but MStateMachineNotify::OnCompletion
      //will be called if installed)
   };


#endif

