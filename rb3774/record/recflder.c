
/*************************************************************************/
/*                                                                       */
/* ITSC Redbook OS/2 v2.0 Sample Program                                 */
/*                                                                       */
/*                                                                       */
/* RECORD.C  -  This is the C implementation of a WPS Folder class       */
/*              with an extra context menu option to perform a           */
/*              Telephone List find, by calling a method from the        */
/*              Record Class.                                            */
/*                                                                       */
/*************************************************************************/
/*
 * This file was generated by the SOM Compiler.
 * FileName: recflder.c.
 * Generated using:
 *     SOM Precompiler spc: 1.21
 *     SOM Emitter emitc: 1.23
 */

#define INCL_WIN
#define INCL_DOS
#define INCL_GPIBITMAPS
#define INCL_WPCLASS
#define INCL_WPFOLDER
#include <os2.h>

#include <pmwp.h>  /* eventually will be #define INCL_WINWORKPLACE */

#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "record.h" /* Include Record file .h so we can invoke */
                    /* the Record class method to do the query */

#include "res.h"

void SendQueryToRecordClass(SOMAny *Folder, PSZ szQuery);

/* default title when created without a title */
PSZ DefaultClassTitle = "Record Folder";

/* Handle to classes module */
HMODULE  hmodThisClass;

#define RecordFolder_Class_Source
#include "recflder.ih"

#undef SOM_CurrentClass
#define SOM_CurrentClass SOMInstance
/*
 *
 *  METHOD:   UpdateLastQuery                             PRIVATE
 *
 *  PURPOSE:  Saves the last query that return a result
 *            from the Record class
 *
 */

SOM_Scope BOOL   SOMLINK recflder_UpdateLastQuery(RecordFolder *somSelf,
                PSZ szQuery)
{
    RecordFolderData *somThis = RecordFolderGetData(somSelf);
    RecordFolderMethodDebug("RecordFolder","recflder_UpdateLastQuery");

    return (BOOL) 0;
}

/*
 *
 *  OVERRIDE: wpInitData                                  PUBLIC
 *
 *  PURPOSE:  Initialize our instance variables.
 *
 */

SOM_Scope void   SOMLINK recflder_wpInitData(RecordFolder *somSelf)
{
    RecordFolderData *somThis = RecordFolderGetData(somSelf);
    RecordFolderMethodDebug("RecordFolder","recflder_wpInitData");

    parent_wpInitData(somSelf);
}

/*
 *
 *  OVERRIDE: wpModifyPopupMenu                           PUBLIC
 *
 *  PURPOSE:  Add the extra find option to
 *            the context menu.
 *
 */

SOM_Scope BOOL   SOMLINK recflder_wpModifyPopupMenu(RecordFolder *somSelf,
                HWND hwndMenu,
                HWND hwndCnr,
                ULONG iPosition)
{
    RecordFolderData *somThis = RecordFolderGetData(somSelf);
    RecordFolderMethodDebug("RecordFolder","recflder_wpModifyPopupMenu");

    _wpInsertPopupMenuItems( somSelf, hwndMenu, iPosition,
                               hmodThisClass, ID_CXTMENU_RECFLDER, 0 );

    return (parent_wpModifyPopupMenu(somSelf,hwndMenu,hwndCnr,iPosition));
}

/*
 *
 *  OVERRIDE: wpMenuItemSelected                          PUBLIC
 *
 *  PURPOSE:  Process input from the extra
 *            menu option that we have added.
 *
 */

SOM_Scope void   SOMLINK recflder_wpMenuItemSelected(RecordFolder *somSelf,
                HWND hwndFrame,
                ULONG MenuId)
{
    RecordFolderData *somThis = RecordFolderGetData(somSelf);
    RecordFolderMethodDebug("RecordFolder","recflder_wpMenuItemSelected");

    switch( MenuId )
    {
       case IDM_QUERY:
          SendQueryToRecordClass(somSelf, "");
          break;

       case IDM_RESTART_SERVER:
          {
             SOMAny   *RecordClass;

             /* First find the Record class */
             if ( RecordClass = _somFindClass( SOMClassMgrObject, SOM_IdFromString("Record"), 1, 1 ) )
             {
                _clsRestartServer(RecordClass);
             }
             else
             {
                DebugBox("Restart Server Error", "Could not find Record class");
             }
          }
          break;

       default:
          parent_wpMenuItemSelected(somSelf, hwndFrame, MenuId);
          break;
    }

}

/*
 *
 *  OVERRIDE: wpSaveState                                 PUBLIC
 *
 *  PURPOSE:  Here we can store our persistent instance
 *            data so that it can be restored from
 *            wpRestoreState
 *
 */

SOM_Scope BOOL   SOMLINK recflder_wpSaveState(RecordFolder *somSelf)
{
    RecordFolderData *somThis = RecordFolderGetData(somSelf);
    RecordFolderMethodDebug("RecordFolder","recflder_wpSaveState");

    return (parent_wpSaveState(somSelf));
}

/*
 *
 *  OVERRIDE: wpRestoreState                              PUBLIC
 *
 *  PURPOSE:  Allow us to resend the last query to
 *            the Record class
 *
 */

SOM_Scope BOOL   SOMLINK recflder_wpRestoreState(RecordFolder *somSelf,
                ULONG ulReserved)
{
    RecordFolderData *somThis = RecordFolderGetData(somSelf);
    RecordFolderMethodDebug("RecordFolder","recflder_wpRestoreState");

    return (parent_wpRestoreState(somSelf,ulReserved));
}

#undef SOM_CurrentClass
#define SOM_CurrentClass SOMMeta
/*
 *
 *  METHOD: wpclsQueryTitle                               PUBLIC
 *
 *  PURPOSE:
 *    Return the string "Record Folder"
 *
 */

SOM_Scope PSZ   SOMLINK recfldercls_wpclsQueryTitle(M_RecordFolder *somSelf)
{
    /* M_RecordFolderData *somThis = M_RecordFolderGetData(somSelf); */
    M_RecordFolderMethodDebug("M_RecordFolder","recfldercls_wpclsQueryTitle");

    return (DefaultClassTitle);
}

/*
 *
 *  METHOD: wpclsInitData                                 PUBLIC
 *
 *  PURPOSE:
 *    We can do class specific initialisation here
 *
 */

SOM_Scope void   SOMLINK recfldercls_wpclsInitData(M_RecordFolder *somSelf)
{
    CHAR ErrorBuffer[100];

    /* M_RecordFolderData *somThis = M_RecordFolderGetData(somSelf); */
    M_RecordFolderMethodDebug("M_RecordFolder","recfldercls_wpclsInitData");

    DosLoadModule((PSZ) ErrorBuffer,
                  sizeof(ErrorBuffer),
                  "RECFLDER", &hmodThisClass);

    parent_wpclsInitData(somSelf);
}

/*
 *
 *  METHOD: wpclsUnInitData                               PUBLIC
 *
 *  PURPOSE:
 *    Here we release any class resources
 *
 */

SOM_Scope void   SOMLINK recfldercls_wpclsUnInitData(M_RecordFolder *somSelf)
{
    /* M_RecordFolderData *somThis = M_RecordFolderGetData(somSelf); */
    M_RecordFolderMethodDebug("M_RecordFolder","recfldercls_wpclsUnInitData");

    DosFreeModule(hmodThisClass);

    parent_wpclsUnInitData(somSelf);
}

/*********************** End of Methods Section ****************************/



void SendQueryToRecordClass(SOMAny *Folder, PSZ szQuery)
{
   SOMAny   *RecordClass;

   /* First find the Record class */
   if ( RecordClass = _somFindClass( SOMClassMgrObject, SOM_IdFromString("Record"), 1, 1 ) )
   {
      _clsQueryDatabase(RecordClass, szQuery, Folder);
   }
   else
   {
      DebugBox("Send Query Error", "Could not find Record class");
   }
}
