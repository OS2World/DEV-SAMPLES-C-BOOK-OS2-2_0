/**********************************************************/
/**********************************************************/
/***                                                    ***/
/***  Program name: SWAPSIZE.EXE                        ***/
/***                                                    ***/
/***  Created     : February, 1992                      ***/
/***                                                    ***/
/***  Revised     :                                     ***/
/***                                                    ***/
/***  Author      : Darryl Frost                        ***/
/***                                                    ***/
/***  Purpose     : To be used with the Lab Session     ***/
/***                Examples given in Appendix C of the ***/
/***                OS/2 Version 2.0 Volume 1: Control  ***/
/***                Program Document no GG24-3730.      ***/
/***                This program interrogates and       ***/
/***                displays the size of the            ***/
/***                SWAPPER.DAT at regular intervals    ***/
/***                in a PM window. The interval is     ***/
/***                initially set to 10 seconds. This   ***/
/***                may be changed to 30 seconds or     ***/
/***                60 seconds by selecting the         ***/
/***                Interval action bar.                ***/
/***                                                    ***/
/***  Parameter   : If the path of SWAPPER.DAT is other ***/
/***                than C:\OS2\SYSTEM, the full path   ***/
/***                and file name of the swapper file   ***/
/***                must be the first parameter passed  ***/
/***                to the progam when it is started.   ***/
/***                                                    ***/
/**********************************************************/
/**********************************************************/
#define INCL_WIN
#define INCL_GPI

#include <os2.h>                        /* PM header file               */
#include <string.h>
#include <stdlib.h>
#include "swapsize.h"                   /* Resource symbolic identifiers*/

#define STRINGLENGTH 20                 /* Length of string             */

/************************************************************************/
/* Function prototypes                                                  */
/************************************************************************/
INT main(int argc, char *argv[], char *envp[] );
VOID AbortSS(HWND hwndFrame,HWND hwndClient);
MRESULT EXPENTRY SSWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );
ULONG GetSwapperSize (void);
                                        /* Define parameters by type    */
HAB  hab;                               /* PM anchor block handle       */
HWND hwndClient=0L;                     /* Client area window handle    */
HWND hwndFrame=0L;                      /* Frame window handle          */
CHAR *szString;                         /* procedure.                   */
PSZ  pszErrMsg;
char szFname[64] = "C:\\OS2\\SYSTEM\\SWAPPER.DAT";
ULONG swapsize;
ULONG oldswapsize;
ULONG timerinterval = 10 * 1000;
ULONG idTimer = 1;

/******************************************************************************/
/*                                                                            */
/* MAIN initializes the process for OS/2 PM services, and processes the       */
/* application message queue until a WM_QUIT message is received. It          */
/* then destroys all OS/2 PM resources and terminates.                        */
/*                                                                            */
/******************************************************************************/
INT main (int argc, char *argv[], char *envp[])
{
  HMQ  hmq;                             /* Message queue handle         */
  QMSG qmsg;                            /* Message from message queue   */
  ULONG flCreate;                       /* Window creation control flags*/

  if ( argc >= 2 ) {
     strcpy( szFname, argv[1] );
  }

  if ((hab = WinInitialize(0)) == 0L) /* Initialize PM     */
     AbortSS(hwndFrame, hwndClient); /* Terminate the application    */

  if ((hmq = WinCreateMsgQueue( hab, 0 )) == 0L)/* Create a msg queue */
     AbortSS(hwndFrame, hwndClient); /* Terminate the application    */

  if (!WinRegisterClass(                /* Register window class        */
     hab,                               /* Anchor block handle          */
     (PSZ)"SSWindow",                   /* Window class name            */
     (PFNWP)SSWindowProc,               /* Address of window procedure  */
     CS_SIZEREDRAW,                     /* Class style                  */
     0                                  /* No extra window words        */
     ))
     AbortSS(hwndFrame, hwndClient); /* Terminate the application    */

   flCreate = FCF_STANDARD &            /* Set frame control flags to   */
             ~FCF_SHELLPOSITION &       /* standard except for shell    */
             ~FCF_ACCELTABLE;           /* positioning.                 */

  if ((hwndFrame = WinCreateStdWindow(
               HWND_DESKTOP,            /* Desktop window is parent     */
               0,                       /* STD. window styles           */
               &flCreate,               /* Frame control flag           */
               "SSWindow",              /* Client window class name     */
               "",                      /* Window text                  */
               0,                       /* No special class style       */
               (HMODULE)0L,             /* Resource is in .EXE file     */
               ID_WINDOW,               /* Frame window identifier      */
               &hwndClient              /* Client window handle         */
               )) == 0L)
     AbortSS(hwndFrame, hwndClient); /* Terminate the application    */

  WinSetWindowText(hwndFrame, "Swapper Size");

  if (!WinSetWindowPos( hwndFrame,      /* Shows and activates frame    */
                   HWND_TOP,            /* window at position 100, 100, */
                   100, 100, 180, 100,  /* and size 180, 100.           */
                   SWP_SIZE | SWP_MOVE | SWP_ACTIVATE | SWP_SHOW
                 ))
     AbortSS(hwndFrame, hwndClient); /* Terminate the application    */

/************************************************************************/
/* Get and dispatch messages from the application message queue         */
/* until WinGetMsg returns FALSE, indicating a WM_QUIT message.         */
/************************************************************************/

  while( WinGetMsg( hab, &qmsg, 0L, 0, 0 ) )
    WinDispatchMsg( hab, &qmsg );
  WinDestroyWindow(hwndFrame);           /* Tidy up...                   */
  WinDestroyMsgQueue( hmq );             /* Tidy up...                   */
  WinTerminate( hab );                   /* Terminate the application    */
} /* End of main */

/******************************************************************************/
/*                                                                            */
/*  SSWINDOWPROC is the window procedure which continuously monitors the size */
/*  of the swap file and displays the size in the client area.                */
/*                                                                            */
/******************************************************************************/
MRESULT EXPENTRY SSWindowProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{

  switch( msg )
  {
    case WM_CREATE:
      swapsize = GetSwapperSize();

      oldswapsize = swapsize;
      WinStartTimer(hab, hwnd, idTimer, timerinterval);
      break;

    case WM_COMMAND:
      {
      USHORT command;                   /* WM_COMMAND command value     */
      command = SHORT1FROMMP(mp1);      /* Extract the command value    */
      switch (command)
      {
        case ID_10SECS:
          timerinterval = 10 * 1000;
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_10SECS, TRUE);
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_30SECS, FALSE);
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_60SECS, FALSE);
          WinStartTimer(hab, hwnd, idTimer, timerinterval);
          break;
        case ID_30SECS:
          timerinterval = 30 * 1000;
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_10SECS, FALSE);
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_30SECS, TRUE);
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_60SECS, FALSE);
          WinStartTimer(hab, hwnd, idTimer, timerinterval);
          break;
        case ID_60SECS:
          timerinterval = 60 * 1000;
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_10SECS, FALSE);
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_30SECS, FALSE);
          WinCheckMenuItem(WinWindowFromID(hwndFrame,FID_MENU),
                           ID_60SECS, TRUE);
          WinStartTimer(hab, hwnd, idTimer, timerinterval);
          break;
        default:
          return WinDefWindowProc( hwnd, msg, mp1, mp2 );
      }
      break;
      }
   case WM_TIMER:
      swapsize = GetSwapperSize();
      if (swapsize==0) {
         WinMessageBox(HWND_DESKTOP,         /* Parent window is desk top */
            hwndFrame,            /* Owner window is our frame */
            "Cannot locate the Swapper File, Check your parameter.",
            "Error Msg",          /* Title bar message         */
            MSGBOXID,             /* Message identifier        */
            MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL ); /* Flags */
         WinPostMsg( hwnd, WM_CLOSE, (MPARAM)0,(MPARAM)0 );
      }
      if (swapsize != oldswapsize)
      {
         oldswapsize = swapsize;
         WinInvalidateRect( hwnd, (PRECTL)NULL, FALSE);
      }
      break;
    case WM_ERASEBACKGROUND:
      /******************************************************************/
      /* Return TRUE to request PM to paint the window background       */
      /* in SYSCLR_WINDOW.                                              */
      /******************************************************************/
      return (MRESULT)( TRUE );
    case WM_PAINT:
      /******************************************************************/
      /* Window contents are drawn here in WM_PAINT processing.         */
      /******************************************************************/
      {
      HPS    hps;                       /* Presentation Space handle    */
      RECTL  rc;                        /* Rectangle coordinates        */
      POINTL pt;                        /* String screen coordinates    */
                                        /* Create a presentation space  */
      char   buffer[STRINGLENGTH];
      char   temp[STRINGLENGTH];
      int len;

      hps = WinBeginPaint( hwnd, 0L, &rc );
      pt.x = 50; pt.y = 20;             /* Set the text coordinates,    */
      GpiSetColor( hps, CLR_NEUTRAL );         /* colour of the text,   */
      GpiSetBackColor( hps, CLR_BACKGROUND );  /* its background and    */
      GpiSetBackMix( hps, BM_OVERPAINT );      /* how it mixes,         */
                                               /* and draw the string...*/
      WinFillRect( hps, &rc, CLR_BACKGROUND );
      szString = _ltoa (swapsize, buffer, 10);
      if ((len=strlen(buffer)) > 3) {
         memcpy(temp, buffer, len+1);
         memcpy(buffer, temp, len-3);
         buffer[len-3] = ' ';
         strcpy(&buffer[len-2], &temp[len-3]);
         }

      strcat( szString, " KB");
      GpiCharStringAt( hps, &pt, (LONG)strlen( szString ), szString );
      WinEndPaint( hps );                      /* Drawing is complete   */
      break;
      }
    case WM_CLOSE:
      /******************************************************************/
      /* This is the place to put your termination routines             */
      /******************************************************************/
      WinStopTimer( hab, hwnd, idTimer);
      WinPostMsg( hwnd, WM_QUIT, (MPARAM)0,(MPARAM)0 );/* Cause termination*/
      break;
    default:
      /******************************************************************/
      /* Everything else comes here.  This call MUST exist              */
      /* in your window procedure.                                      */
      /******************************************************************/

      return WinDefWindowProc( hwnd, msg, mp1, mp2 );
  }
  return (MRESULT)FALSE;
} /* End of MyWindowProc */

/**************************************************************************/
/* AbortSS    -- report an error returned from an API service.            */
/*                                                                        */
/* The error message is displayed using a message box and the program     */
/* exit will be affected with the error status of 1.                      */
/**************************************************************************/
VOID AbortSS(HWND hwndFrame,HWND hwndClient)
{
PERRINFO  pErrInfoBlk;
PSZ       pszOffSet;
void      stdprint(void);

      DosBeep(100,10);
      if ((pErrInfoBlk = WinGetErrorInfo(hab)) != (PERRINFO)NULL)
      {
      pszOffSet = ((PSZ)pErrInfoBlk) + pErrInfoBlk->offaoffszMsg;
      pszErrMsg = ((PSZ)pErrInfoBlk) + *((PSHORT)pszOffSet);
      if((INT)hwndFrame && (INT)hwndClient)
         WinMessageBox(HWND_DESKTOP,         /* Parent window is desk top */
                       hwndFrame,            /* Owner window is our frame */
                       (PSZ)pszErrMsg,       /* PMWIN Error message       */
                       "Error Msg",          /* Title bar message         */
                       MSGBOXID,             /* Message identifier        */
                       MB_MOVEABLE | MB_CUACRITICAL | MB_CANCEL ); /* Flags */
      WinFreeErrorInfo(pErrInfoBlk);
      }
      WinPostMsg(hwndClient, WM_QUIT, (MPARAM)0, (MPARAM)0);
} /* End of AbortHello */
/* Function which returns swap file size in KB            */
ULONG GetSwapperSize ()
{
   HDIR fhandle;
   unsigned LONG count;
   int fsize;
   USHORT frc;
   FILEFINDBUF buffer;   /* file information struct */

   count = 1;
   fhandle = 0xFFFF;
   frc = DosFindFirst (szFname, &fhandle, 0, &buffer, sizeof(buffer),                       &count, 1L);
   if (frc != 0){
      return(0L);
   } /* endif */
   fsize = buffer.cbFileAlloc / 1024;  /* in KBytes */
   DosFindClose (fhandle);
   return((ULONG)fsize);
}
/*********************** End of the hello.c *******************************/
