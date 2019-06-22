#ifndef C_WND_MAIN_H
#define C_WND_MAIN_H

#include "stdafx.h"
#include "csgl.h"

class CWnd_Main:public CWnd
{
 protected:
  //-���������� ������-------------------------------------------------------
  CSGL cSGL;
  float angle;
  //-������� ������----------------------------------------------------------
  //-������------------------------------------------------------------------
 public:
  //-����������� ������------------------------------------------------------
  CWnd_Main(void);
  //-���������� ������-------------------------------------------------------
  ~CWnd_Main();
  //-���������� ������-------------------------------------------------------
  //-������� ������----------------------------------------------------------
  void VectorProduct(float *xv1,float *yv1,float *zv1,float xv2,float yv2,float zv2);
  void Octahedron(float height);
  //-������� ��������� ��������� ������--------------------------------------
  DECLARE_MESSAGE_MAP()
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy(void);

  afx_msg void OnPaint(void);
  afx_msg void OnTimer(UINT nIDEvent);

  //-������------------------------------------------------------------------
};
#endif