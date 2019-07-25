#ifndef C_SGL_H
#define C_SGL_H

//****************************************************************************************************
//класс программной растеризации
//****************************************************************************************************

//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include <stdint.h>

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
//точка экрана
struct SGLScreenColor
{
 uint8_t R;
 uint8_t G;	
 uint8_t B;	
};
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
  //вектор 4
  struct SGLVector4
  {
   float X;
   float Y;
   float Z;
   float W;
  };
  //вектор 3
  struct SGLVector3
  {
   float X;
   float Y;
   float Z;
  };
  //матрица 4x4
  struct SGLMatrix4
  {
   SGLVector4 X;
   SGLVector4 Y;
   SGLVector4 Z;
   SGLVector4 W;
  };
  //матрица 3x3
  struct SGLMatrix3
  {
   SGLVector3 X;
   SGLVector3 Y;
   SGLVector3 Z;
  };
  //точка с текстурой, цветом, нормалью, координатами
  struct SGLNVCTPoint
  {
   SGLTexture sGLTexture;
   SGLColor sGLColor;
   SGLNormal sGLNormal;
   SGLVertex sGLVertex;
  };
  //точка на экране
  struct SGLScreenPoint
  {
   int32_t X;
   int32_t Y;
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

  SGLMatrix4 *sGLMatrix4_Ptr;//указатель на матрицу, с которой производится работа

  bool DrawMode;//включён ли режим рисования

  SGLTextureObject sGLTextureObject_Current;//текущая текстура

 public:
  SGLScreenColor* ImageMap;
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

  //-статические функции--------------------------------------------------------------------------------
  static void SetVertexCoord(SGLVertex &sGLVertex,float x,float y,float z);//задать координаты вершины
  static void SetNormalCoord(SGLNormal &sGLNormal,float nx,float ny,float nz);//задать координаты нормали
  static void SetTextureCoord(SGLTexture &sGLTexture,float u,float v);//задать координаты текстуры
  static void SetColorValue(SGLColor &sGLColor,float r,float g,float b);//задать цвет
  static void MultiplySGLVector4ToSGLMatrix4(const SGLVector4& v,const SGLMatrix4& m,SGLVector4& v_out);//умножение вектора типа SGLVector4 на матрицу типа SGLMatrix4
  static void MultiplySGLMatrix4ToSGLVector4(const SGLMatrix4& m,const SGLVector4& v,SGLVector4& v_out);//умножение матрицы типа SGLMatrix4 на вектор типа SGLVector4
  static void MultiplySGLMatrix4(const SGLMatrix4& a,const SGLMatrix4& b,SGLMatrix4& out);//умножение двух матриц типа SGLMatrix4
  static void NormaliseSGLVector4(SGLVector4& v);//нормирование вектора типа SGLMatrix4
  static double GetDeterminantSGLMatrix4(const SGLMatrix4& matrix);//вычислить определитель матрицы типа SGLMatrix4
  static double GetDeterminantSGLMatrix3(const SGLMatrix3& matrix);//вычислить определитель матрицы типа SGLMatrix3
  static void GetTruncatedMatrixSGLMatrix4(long y,long x,const SGLMatrix4& input_matrix,SGLMatrix3& output_matrix);//вычислить матрицу с исключённой строкой и столбцом по координатам y и x для матрицы типа SGLMatrix4	
  static bool CreateInvertMatrixSGLMatrix4(const SGLMatrix4& input_matrix,SGLMatrix4& output_matrix);//вычислить обратную матрицу для матрицы типа SGLMatrix4

 private:
  //-закрытые функции-----------------------------------------------------------------------------------  
  void CreateFrustrumPlane(void);//вычислить плоскости отсечения
  void GetIntersectionPlaneAndLine(const SGLNVCTPoint& A,const SGLNVCTPoint& B,SGLNVCTPoint& new_point,float nx,float ny,float nz,float w);//получить точку пересечения прямой и плоскости
  void Clip(const SGLNVCTPoint *point_array_input,uint16_t point_amount_input,SGLNVCTPoint *point_array_output,uint16_t &point_amount_output,float nx,float ny,float nz,float w);//выполнить коррекцию координат  

  void OutputTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C);//вывести треугольник
  void DrawTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C);//отрисовка треугольника
  void RenderTriangle(SGLNVCTPoint &a,SGLNVCTPoint &b,SGLNVCTPoint &c,SGLScreenPoint &ap,SGLScreenPoint &bp,SGLScreenPoint &cp);//растеризация треугольника на экране
  void DrawLine(int32_t y,int32_t x1,int32_t x2,float z1,float z2,float r1,float r2,float g1,float g2,float b1,float b2,float u1,float u2,float v1,float v2);//отрисовка текстурированной горизонтальной линии
};

#endif


/*

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#pragma pack(1)
//координата точки
struct SGLVertex
{
 float x;
 float y;
 float z;
};

struct SGLScreenColor
{
 //uint16_t Color;
 uint8_t r;
 uint8_t g;	
 uint8_t b;	
};
//точка с текстурой, цветом, нормалью, координатами
struct SGLNVCTPoint
{
 SGLTexture sGLTexture;
 SGLColor sGLColor;
 SGLNormal sGLNormal;
 SGLVertex sGLVertex;
};
struct SGLScreenPoint
{
 int32_t x;
 int32_t y;
};

#pragma pack()


*/

/*
#define SGL_MATRIX_PROJECTION 100
#define SGL_MATRIX_MODELVIEW  101

#define SGL_COLOR_BUFFER_BIT  1
#define SGL_DEPTH_BUFFER_BIT  2 

#define SGL_DEPTH_TEST		  1

class CSGL
{	
 private:
  enum MATRIX_MODE
  {
   MATRIX_MODE_MODEL_VIEW,
   MATRIX_MODE_PROJECTION
  }; 
  //-Переменные класса-------------------------------------------------------
  SGLMatrix4 ProjectionMatrix;//матрица проектирования
  SGLMatrix4 ModelViewMatrix;//матрица моделирования
  SGLVector4 ViewPort;//вектор видового порта
	
  SGLNVCTPoint sGLNVCTPointArray[3];//список хранимых вершин
  uint16_t PointArrayAmount;//размер данных в списке
  MATRIX_MODE CurrentSelectedMatrix;//текущая выбранная матрица
  bool DrawModeActive;//true-была выполнена команда Begin
  SGLVector4 FrustumPlane[5];//пять плоскостей отсечения (каждая четверка чисел описывает плоскость: ax+by+cz+d=0)

  static const int32_t MIN_INV_Z_VALUE=0;//минимальное значение 1/Z

  //-Функции класса----------------------------------------------------------
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CSGL(void);
  //-Деструктор класса-------------------------------------------------------
  ~CSGL();
  //-Переменные класса-------------------------------------------------------
  SGLScreenColor* ImageMap;
  float *InvZBuffer;//буфер 1/z
  int32_t ScreenWidth;
  int32_t ScreenHeight;
  SGLTexture CurrentTexture;//текущие текструрные координаты
 
  //цвета
  SGLColor CurrentColor;//текущий цвет
  //char CurrentR;//текущий
  //char CurrentG;
  //char CurrentB;
  //параметры
  bool EnableDepthText;
  //-Функции класса----------------------------------------------------------
  bool Create(int screen_width,int screen_height);
  //функции работы с матрицами
  bool LoadIdentity(void);
  bool Frustrum(float left,float right,float bottom,float top,float near,float far);
  bool SetViewport(float x,float y,float len,float hgt);
  bool Perspective(float fovy,float aspect,float near,float far);
  bool Rotatef(float angle,float nx,float ny,float nz);
  bool Translatef(float nx,float ny,float nz);
  bool MatrixMode(int matrix);
  //функции задания цвета
  bool Color3i(unsigned char r,unsigned char g,unsigned char b);
  //функции рисования примитивов
  bool Begin(void);
  bool End(void);
	bool TexCoord(float u,float v);
  bool Vertex3f(float x,float y,float z);
  //функции очистки экранных массивов
  bool Clear(unsigned int mode);
  //функции включения/выключения режимов
  bool Enable(unsigned int mode);
  bool Disable(unsigned int mode);	
	
  void CreateFrustrumPlane(void);//вычислить плоскости отсечения	
  void GetIntersectionPlaneAndLine(const SGLNVCTPoint& A,const SGLNVCTPoint& B,SGLNVCTPoint& new_point,float nx,float ny,float nz,float w);//получить точку пересечения прямой и плоскости
  void Clip(const SGLNVCTPoint *point_array_input,uint16_t point_amount_input,SGLNVCTPoint *point_array_output,uint16_t &point_amount_output,float nx,float ny,float nz,float w);//выполнить коррекцию координат  

  void OutputTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C);//вывести треугольник
  void DrawTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C);//отрисовка треугольника
  void RenderTriangle(SGLNVCTPoint &a,SGLNVCTPoint &b,SGLNVCTPoint &c,SGLScreenPoint &ap,SGLScreenPoint &bp,SGLScreenPoint &cp);//растеризация треугольника на экране
  void DrawLine(int32_t y,int32_t x1,int32_t x2,float z1,float z2,float r1,float r2,float g1,float g2,float b1,float b2,float u1,float u2,float v1,float v2);//отрисовка линии


	//-Прочее------------------------------------------------------------------
};


#endif
*/