#ifndef C_SGL_H
#define C_SGL_H

#include "stdafx.h"

#define SGL_MATRIX_PROJECTION 100
#define SGL_MATRIX_MODELVIEW  101

#define SGL_COLOR_BUFFER_BIT  1
#define SGL_DEPTH_BUFFER_BIT  2 

#define SGL_DEPTH_TEST		  1

#define M_PI 3.14159265358979323

struct SSGLPoint
{
 float X;
 float Y;
 float Z;

 char R;
 char G;
 char B;
};

class CSGL
{
 protected:
  //-Переменные класса-------------------------------------------------------
 float ProjectionMatrix[16];//матрица проектирования
 float ModelViewMatrix[16];//матрица моделирования
 float Viewport[4];//вектор видового порта
 int CurrentSelectedMatrix;//текущая выбранная матрица
 SSGLPoint SSGLPointArray[100];//список хранимых вершин
 int PointArrayAmount;//размер данных в списке
 BOOL DrawModeActive;//TRUE=была выполнена команда Begin
  //-Функции класса----------------------------------------------------------
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CSGL(void);
  //-Деструктор класса-------------------------------------------------------
  ~CSGL();
  //-Переменные класса-------------------------------------------------------
  char *ImageMap;
  float *ZBuffer;
  int ScreenWidth;
  int ScreenHeight;
  //цвета
  char CurrentR;//текущий
  char CurrentG;
  char CurrentB;
  //параметры
  BOOL EnableDepthText;
  //-Функции класса----------------------------------------------------------
  BOOL Create(int screen_width,int screen_height);
  //функции работы с матрицами
  BOOL MatrixProductfv(float* Matrix1,float* Matrix2,float* Matrix3);
  BOOL VectorProductMatrixfv(float *Vector,float* Matrix,float *ReturnVector);
  BOOL MatrixProductVectorfv(float* Matrix,float *Vector,float *ReturnVector);
  BOOL LoadIdentity(void);
  BOOL Frustrum(float Left,float Right,float Bottom,float Top,float Near,float Far);
  BOOL SetViewport(float x,float y,float len,float hgt);
  BOOL Perspective(float Fovy,float Aspect,float Near,float Far);
  BOOL Rotatef(float angle,float nx,float ny,float nz);
  BOOL Translatef(float nx,float ny,float nz);
  BOOL MatrixMode(int matrix);
  //функции задания цвета
  BOOL Color3i(int r,int g,int b);
  //функции рисования примитивов
  BOOL Begin(void);
  BOOL End(void);
  BOOL Vertex3f(float x,float y,float z);
  BOOL PutTriangle(SSGLPoint *TVertex);
  BOOL DrawTriangle(SSGLPoint A,SSGLPoint B,SSGLPoint C);
  //функции очистки экранных массивов
  BOOL Clear(unsigned int mode);
  //функции включения/выключения режимов
  BOOL Enable(unsigned int mode);
  BOOL Disable(unsigned int mode);
  //-Прочее------------------------------------------------------------------
};
#endif