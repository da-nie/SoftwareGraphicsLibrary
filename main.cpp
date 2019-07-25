#include "stdafx.h"
#include <math.h>

//------------------------------------------------------------------------------
#include "cwnd_main.h"

//------------------------------------------------------------------------------
class CWinApp_Main:public CWinApp
{
 protected:
  //-Переменные класса-------------------------------------------------------
  //-Функции класса----------------------------------------------------------
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CWinApp_Main(void);
  //-Деструктор класса-------------------------------------------------------
  ~CWinApp_Main();
  //-Переменные класса-------------------------------------------------------
  //-Функции класса----------------------------------------------------------
  BOOL InitInstance(void);
  //-Прочее------------------------------------------------------------------
};
//-Конструктор класса--------------------------------------------------------
CWinApp_Main::CWinApp_Main(void)
{
}
//-Деструктор класса---------------------------------------------------------
CWinApp_Main::~CWinApp_Main()
{
}
//-Функции класса------------------------------------------------------------
BOOL CWinApp_Main::InitInstance(void)
{
 CWnd_Main *cWnd_Main=new CWnd_Main;
 HCURSOR hCursor=LoadStandardCursor(IDC_ARROW);
 HICON hIcon=LoadStandardIcon(IDI_APPLICATION);
 HBRUSH hBrush=(HBRUSH)COLOR_WINDOW;
 LPCSTR ClassName=AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,hCursor,hBrush,hIcon);
 CRect cRect(0,0,640,480);
 AdjustWindowRect(&cRect,WS_CAPTION|WS_SYSMENU|WS_OVERLAPPED|WS_MINIMIZEBOX,NULL);
 cWnd_Main->CreateEx(0,ClassName,"Приложение на MFC",WS_CAPTION|WS_SYSMENU|WS_OVERLAPPED|WS_MINIMIZEBOX,0,0,cRect.right-cRect.left,cRect.bottom-cRect.top,NULL,NULL);
 cWnd_Main->ShowWindow(m_nCmdShow);
 m_pMainWnd=cWnd_Main;
 return TRUE;
}

CWinApp_Main cWinApp_Main;


