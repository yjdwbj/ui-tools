#include "mainwindow.h"
#include <QApplication>

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QPushButton>
#include <signal.h>
#include <iostream>

using namespace std;
#ifdef __WIN32
#include <windows.h>
#include <windef.h>
#include <dbghelp.h>
LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
//   cout << "Enter TopLevelExceptionFilter Function" << endl;
    HANDLE hFile = CreateFile( TEXT("project.dmp"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
     if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE)) {
         MINIDUMP_EXCEPTION_INFORMATION mdei;

         mdei.ThreadId          = GetCurrentThreadId();
         mdei.ExceptionPointers = pExceptionInfo;
         mdei.ClientPointers    = FALSE;

         MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                           hFile, MiniDumpNormal, &mdei, NULL, NULL);
         CloseHandle(hFile);
     }
     return EXCEPTION_EXECUTE_HANDLER;
 }

#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

     QDesktopWidget *pDwgt = a.desktop();
    QRect desk_rect = pDwgt->screenGeometry(pDwgt->screenNumber(QCursor::pos()));
    w.setMaximumSize(desk_rect.size());
    w.showMaximized();
    w.show();
#ifdef __WIN32
    SetUnhandledExceptionFilter(TopLevelExceptionFilter);
#endif

    return a.exec();
}
