#include "cwnd_main.h"

//-Функции обработки сообщений класса----------------------------------------
BEGIN_MESSAGE_MAP(CWnd_Main,CWnd)
 ON_WM_CREATE()
 ON_WM_DESTROY()
 ON_WM_PAINT()
 ON_WM_TIMER()
END_MESSAGE_MAP()
//-Конструктор класса--------------------------------------------------------
CWnd_Main::CWnd_Main(void)
{
 cSGL.Create(640,480);
 cSGL.Perspective(60,(float)(640.0/480.0),1,100000000);
 cSGL.SetViewport(0,0,640,480);
}
//-Деструктор класса---------------------------------------------------------
CWnd_Main::~CWnd_Main()
{
}
//-Функции класса------------------------------------------------------------
void CWnd_Main::VectorProduct(float *xv1,float *yv1,float *zv1,float xv2,float yv2,float zv2)
{
 float x1=*xv1;
 float y1=*yv1;
 float z1=*zv1;
 float x2=xv2;
 float y2=yv2;
 float z2=zv2;
 float xn=y1*z2-z1*y2;
 float yn=-(x1*z2-z1*x2);
 float zn=x1*y2-y1*x2;
 float len=(float)sqrt(xn*xn+yn*yn+zn*zn);
 if (len!=0)
 {
  xn=xn/len;
  yn=yn/len;
  zn=zn/len;
 }
 *xv1=xn;
 *yv1=yn;
 *zv1=zn;
}
void CWnd_Main::Octahedron(float height)
{
 float a=height/2;
 float x1=0;
 float z1=0;
 float y1=0;
 float x2=0;
 float z2=0;
 float y2=height;
 float x3=a;
 float z3=0;
 float y3=a;
 float x4=-a;
 float z4=0;
 float y4=a;
 float x5=0;
 float z5=a;
 float y5=a;
 float x6=0;
 float z6=-a;
 float y6=a;
 float nx,ny,nz;
  nx=x3-x1;
  ny=y3-y1;
  nz=z3-z1;
  VectorProduct(&nx,&ny,&nz,x5-x1,y5-y1,z5-z1);
  cSGL.Color3i(255,255,255);
  cSGL.Begin();
   cSGL.Vertex3f(x3,y3,z3);
   cSGL.Vertex3f(x5,y5,z5);
   cSGL.Vertex3f(x1,y1,z1);
  cSGL.End();
  //========================================
  nx=x5-x1;
  ny=y5-y1;
  nz=z5-z1;
  VectorProduct(&nx,&ny,&nz,x4-x1,y4-y1,z4-z1);
  cSGL.Color3i(255,255,0);
  cSGL.Begin();
   cSGL.Vertex3f(x5,y5,z5);
   cSGL.Vertex3f(x4,y4,z4);
   cSGL.Vertex3f(x1,y1,z1);
  cSGL.End();
  //========================================
  nx=x4-x1;
  ny=y4-y1;
  nz=z4-z1;
  VectorProduct(&nx,&ny,&nz,x6-x1,y6-y1,z6-z1);
  cSGL.Color3i(255,0,255);
  cSGL.Begin();
   cSGL.Vertex3f(x4,y4,z4);
   cSGL.Vertex3f(x6,y6,z6);
   cSGL.Vertex3f(x1,y1,z1);
  cSGL.End();
  //========================================
  nx=x6-x1;
  ny=y6-y1;
  nz=z6-z1;
  VectorProduct(&nx,&ny,&nz,x3-x1,y3-y1,z3-z1);
  cSGL.Color3i(0,255,255);
  cSGL.Begin();
   cSGL.Vertex3f(x6,y6,z6);
   cSGL.Vertex3f(x3,y3,z3);
   cSGL.Vertex3f(x1,y1,z1);
  cSGL.End();
  //========================================
  nx=x4-x2;
  ny=y4-y2;
  nz=z4-z2;
  VectorProduct(&nx,&ny,&nz,x5-x2,y5-y2,z5-z2);
  cSGL.Color3i(255,0,0);
  cSGL.Begin();
   cSGL.Vertex3f(x4,y4,z4);
   cSGL.Vertex3f(x5,y5,z5);
   cSGL.Vertex3f(x2,y2,z2);
  cSGL.End();
  //========================================
  nx=x6-x2;
  ny=y6-y2;
  nz=z6-z2;
  VectorProduct(&nx,&ny,&nz,x4-x2,y4-y2,z4-z2);
  cSGL.Color3i(0,255,0);
  cSGL.Begin();
   cSGL.Vertex3f(x6,y6,z6);
   cSGL.Vertex3f(x4,y4,z4);
   cSGL.Vertex3f(x2,y2,z2);
  cSGL.End();
  //========================================
  nx=x3-x2;
  ny=y3-y2;
  nz=z3-z2;
  VectorProduct(&nx,&ny,&nz,x6-x2,y6-y2,z6-z2);
  cSGL.Color3i(0,0,255);
  cSGL.Begin();
   cSGL.Vertex3f(x3,y3,z3);
   cSGL.Vertex3f(x6,y6,z6);
   cSGL.Vertex3f(x2,y2,z2);
  cSGL.End();
  //========================================
  nx=x5-x2;
  ny=y5-y2;
  nz=z5-z2;
  VectorProduct(&nx,&ny,&nz,x3-x2,y3-y2,z3-z2);
  cSGL.Color3i(127,127,255);
  cSGL.Begin();
   cSGL.Vertex3f(x5,y5,z5);
   cSGL.Vertex3f(x3,y3,z3);
   cSGL.Vertex3f(x2,y2,z2);
  cSGL.End();
  //========================================
}
//-Функции обработки сообщений класса----------------------------------------
int CWnd_Main::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
 angle=0;
 SetTimer(100,50,NULL);
 return(CWnd::OnCreate(lpCreateStruct));
}
void CWnd_Main::OnDestroy(void)
{
 KillTimer(100);
 CWnd::OnDestroy(); 
}
afx_msg void CWnd_Main::OnPaint(void)
{
 cSGL.Clear(SGL_COLOR_BUFFER_BIT|SGL_DEPTH_BUFFER_BIT);
 cSGL.Enable(SGL_DEPTH_TEST);
 cSGL.MatrixMode(SGL_MATRIX_MODELVIEW);
 cSGL.LoadIdentity();
 cSGL.Translatef(0,0,-20);
 cSGL.Rotatef(angle,1,1,0);
 cSGL.Translatef(0,-5,0);
 //нарисуем фигуру
 Octahedron(10);
 //выведем картинку на экран
 CPaintDC dc(this);
 BITMAPINFOHEADER bmih;
 bmih.biSize=sizeof(BITMAPINFOHEADER);
 bmih.biWidth=640;
 bmih.biHeight=480;
 bmih.biPlanes=1;
 bmih.biBitCount=24;
 bmih.biCompression=BI_RGB;
 bmih.biSizeImage=0;
 bmih.biXPelsPerMeter=300;
 bmih.biYPelsPerMeter=300;
 bmih.biClrUsed=0;
 bmih.biClrImportant=0;
 BITMAPINFO info;
 info.bmiHeader=bmih;
 StretchDIBits(dc.m_hDC,0,0,640,480,0,0,640,480,cSGL.ImageMap,&info,DIB_RGB_COLORS,SRCCOPY);
 CWnd::OnPaint();
}
afx_msg void CWnd_Main::OnTimer(UINT nIDEvent)
{
 if (nIDEvent==100)
 {
  angle+=(float)5;
  InvalidateRect(NULL,FALSE);
 }
 else CWnd::OnTimer(nIDEvent);
}
//-Прочее--------------------------------------------------------------------
