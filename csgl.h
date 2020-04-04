#ifndef C_SGL_H
#define C_SGL_H

//****************************************************************************************************
//класс программной растеризации
//****************************************************************************************************

//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include <stdint.h>
#include <stdio.h>

#include "sglmatrix.h"
#include "cglscreencolor.h"

//****************************************************************************************************
//макроопределения
//****************************************************************************************************

//****************************************************************************************************
//константы
//****************************************************************************************************

//****************************************************************************************************
//предварительные объявления
//****************************************************************************************************

#pragma pack(1)
//цвет точки RGBA в целых числах
struct SGLRGBAByteColor
{
 uint8_t R;
 uint8_t G;
 uint8_t B;
 uint8_t A;
};
#pragma pack()


//****************************************************************************************************
//класс программной растеризации
//****************************************************************************************************
class CSGL
{
 public:
  //-перечисления---------------------------------------------------------------------------------------
  //варианты элементов
  enum ITEM_ID
  {
   SGL_LIGHTING,//расчёт освещения
   SGL_LIGHT0,//источник света 0
   SGL_LIGHT1,//источник света 1
   SGL_LIGHT2,//источник света 2
   SGL_LIGHT3,//источник света 3
   SGL_LIGHT4,//источник света 4
   SGL_LIGHT5,//источник света 5
   SGL_LIGHT6,//источник света 6
   SGL_LIGHT7,//источник света 7
  };
  //параметры
  enum PARAM_ID
  {
   SGL_POSITION,//позиция
   SGL_AMBIENT,//фоновый цвет
   SGL_DIFFUSE,//рассеянный свет
   SGL_SPECULAR,//отражённый свет
   SGL_SHININESS,//уровень отражения
	 SGL_EMISSION,//уровень излучения
   SGL_CONSTANT_ATTENUATION,//постоянное затухание
   SGL_LINEAR_ATTENUATION,//линейное затухание
   SGL_QUADRATIC_ATTENUATION//квадратичное затухание
  };
  //варианты выбора матриц
  enum MATRIX_MODE
  {
   SGL_MATRIX_MODELVIEW,//выбрана матрица моделирования
   SGL_MATRIX_PROJECTION,//выбрана матрица проектирования
   SGL_MATRIX_TEXTURE//выбрана матрица текстурирования
  };
  //варианты очистки
  enum CLEAR_MODE
  {
   SGL_COLOR_BUFFER_BIT=(1<<0),
   SGL_DEPTH_BUFFER_BIT=(1<<1)
  };
  //-константы------------------------------------------------------------------------------------------
  static const uint8_t MAX_COLOR_VALUE=255;//максимальное значение цвета
 public:
  //-структуры------------------------------------------------------------------------------------------
  //координата точки
  struct SGLVertex
  {
   float X;
   float Y;
   float Z;
  };
  //нормаль к точке
  struct SGLNormal
  {
   float Nx;
   float Ny;
   float Nz;
  };
  //текстурные координаты
  struct SGLTexture
  {
   float U;
   float V;
  };
  //цвет точки
  struct SGLColor
  {
   float R;
   float G;
   float B;
  };  
  //параметры материала
  struct SGLMatherial
  {
   SGLColor SGLColor_Ambient;
   SGLColor SGLColor_Diffuse;
   SGLColor SGLColor_Specular;
   SGLColor SGLColor_Emission;
   float Shininess;
  };
  //источник света
  struct SLight
  {
   bool Enabled;
   SGLVertex sGLVertex;
   SGLColor SGLColor_Ambient;
   SGLColor SGLColor_Diffuse;
   SGLColor SGLColor_Specular;
   float Shininess;
   float ConstantAttenuation;
   float LinearAttenuation;
   float QuadraticAttenuation;
  };
  //точка с текстурой, цветом, нормалью, координатами, материалом
  struct SGLNVCTPoint
  {
   SGLTexture sGLTexture;
   SGLColor sGLColor;
   SGLNormal sGLNormal;
   SGLVertex sGLVertex;
   SGLMatherial sGLMaterial;
  };
  //точка на экране
  struct SGLScreenPoint
  {
   int32_t X;
   int32_t Y;
  };
  //точка с текстурой и цветом
  struct SGLNCTPoint
  {
   SGLTexture sGLTexture;
   SGLColor sGLColor;
  };
 private:
  struct SGLTextureObject
  {
   uint32_t Width;//ширина
   uint32_t Height;//высота
   SGLRGBAByteColor *sGLRGBAByteColor_Ptr;//данные текстуры
  };
 private:
  //-константы------------------------------------------------------------------------------------------
  static const uint32_t VERTEX_POINT_ARRAY=3;//размер буфера вершин
  static const uint32_t FRUSTRUM_PLANE=6;//количество плоскостей отсечения
  static const int32_t MIN_INV_Z_VALUE=0;//минимальное значение 1/Z
  static const uint32_t LIGHT_AMOUNT=8;//количество источников света
 private:
  //-переменные-----------------------------------------------------------------------------------------
  SGLMatrix4 ProjectionMatrix;//матрица проектирования
  SGLMatrix4 ModelViewMatrix;//матрица моделирования
  SGLMatrix4 TextureMatrix;//матрица текстурирования
  SGLVector4 ViewPort;//вектор видового порта  
  SGLNVCTPoint sGLNVCTPointArray[VERTEX_POINT_ARRAY];//список хранимых вершин
  size_t PointArrayAmount;//размер данных в списке хранимых вершин
  SGLVector4 FrustumPlane[FRUSTRUM_PLANE];//набор плоскостей отсечения (каждая четверка чисел описывает плоскость: ax+by+cz+d=0)
  SGLNVCTPoint sGLNVCTPoint_Current;//параметры текущей точки
  SGLColor sGLColor_Clear;//цвет очистки фона

  bool EnableLighting;//разрешение на общий расчёт освещения
  SLight sLight[LIGHT_AMOUNT];//источники света

  SGLMatrix4 *sGLMatrix4_Ptr;//указатель на матрицу, с которой производится работа

  bool DrawMode;//включён ли режим рисования

  SGLTextureObject sGLTextureObject_Current;//текущая текстура
 public:
  CGLScreenColor* ImageMap;
  uint32_t ScreenWidth;//размеры экрана
  uint32_t ScreenHeight;
  float *InvZBuffer;//буфер 1/z

 public:
  //-конструктор----------------------------------------------------------------------------------------
  CSGL(void);
  //-деструктор-----------------------------------------------------------------------------------------
  ~CSGL();
 public:
  //-открытые функции-----------------------------------------------------------------------------------
  void Init(uint32_t screen_width,uint32_t screen_height);//инициализировать

  void LoadIdentity(void);//сделать матрицу единичной
  void Rotatef(float angle,float nx,float ny,float nz);//умножить текущую матрицу на матрицу поворота вокруг вектора
  void Translatef(float nx,float ny,float nz);//умножить текущую матрицу на матрицу смещения
  void MatrixMode(MATRIX_MODE matrix);//выбрать матрицу

  void Frustrum(float left,float right,float bottom,float top,float near,float far);//задать плоскости отсечения в матрицу проецирования
  void SetViewport(float x,float y,float len,float hgt);//задать видовой порт
  void Perspective(float fovy,float aspect,float near,float far);//задать матрицу проецирования

  void Color3f(float r,float g,float b);//задать цвет точки
  void TexCoordf(float u,float v);//задать текстурные координаты точки
  void Normal3f(float nx,float ny,float nz);//задать нормаль в точке
  void Vertex3f(float x,float y,float z);//задать координату точки
  void Begin(void);//начать рисование
  void End(void);//закончить рисование
  void ClearColor(float r,float g,float b);//задать цвет очистки фона
  void Clear(uint32_t mode);//очистить буфер
  void BindTexture(uint32_t width,uint32_t height,SGLRGBAByteColor *sGLRGBAByteColor_Ptr_Set);//задать текстуру
  void Enable(ITEM_ID mode);//разрешить
  void Disable(ITEM_ID mode);//запретить
  void Lightfv(ITEM_ID light,PARAM_ID param,float *ptr);//задать параметры источника света
  void Materialfv(PARAM_ID param,float *ptr);//задать параметры материала

  //-статические функции--------------------------------------------------------------------------------
  static void SetVertexCoord(SGLVertex &sGLVertex,float x,float y,float z);//задать координаты вершины
  static void SetNormalCoord(SGLNormal &sGLNormal,float nx,float ny,float nz);//задать координаты нормали
  static void SetTextureCoord(SGLTexture &sGLTexture,float u,float v);//задать координаты текстуры
  static void SetColorValue(SGLColor &sGLColor,float r,float g,float b);//задать цвет
 private:
  //-закрытые функции-----------------------------------------------------------------------------------
  void CreateLighColor(SGLNVCTPoint &sGLNVCTPoint);//вычислить цвет точки по источникам света
  void CreateFrustrumPlane(void);//вычислить плоскости отсечения
  void GetIntersectionPlaneAndLine(const SGLNVCTPoint& A,const SGLNVCTPoint& B,SGLNVCTPoint& new_point,float nx,float ny,float nz,float w);//получить точку пересечения прямой и плоскости
  void Clip(const SGLNVCTPoint *point_array_input,uint16_t point_amount_input,SGLNVCTPoint *point_array_output,uint16_t &point_amount_output,float nx,float ny,float nz,float w);//выполнить коррекцию координат

  void OutputTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C);//вывести треугольник
  void DrawTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C);//отрисовка треугольника
  void RenderTriangle(SGLNVCTPoint &a,SGLNVCTPoint &b,SGLNVCTPoint &c,SGLScreenPoint &ap,SGLScreenPoint &bp,SGLScreenPoint &cp);//растеризация треугольника на экране
  void DrawLine(int32_t y,int32_t x1,int32_t x2,float z1,float z2,const SGLNCTPoint &sGLNCTPoint_1,const SGLNCTPoint &sGLNCTPoint_2);//отрисовка текстурированной горизонтальной линии
};

#endif
