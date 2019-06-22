#ifndef C_WND_MAIN_H
#define C_WND_MAIN_H

#include "stdafx.h"
#include "csgl.h"

class CWnd_Main:public CWnd
{
 protected:
  //-Переменные класса-------------------------------------------------------
  CSGL cSGL;
  float angle;
  //-Функции класса----------------------------------------------------------
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CWnd_Main(void);
  //-Деструктор класса-------------------------------------------------------
  ~CWnd_Main();
  //-Переменные класса-------------------------------------------------------
  //-Функции класса----------------------------------------------------------
  void VectorProduct(float *xv1,float *yv1,float *zv1,float xv2,float yv2,float zv2);
  void Octahedron(float height);
  //-Функции обработки сообщений класса--------------------------------------
  DECLARE_MESSAGE_MAP()
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy(void);

  afx_msg void OnPaint(void);
  afx_msg void OnTimer(UINT nIDEvent);

  //-Прочее------------------------------------------------------------------
};
#endif