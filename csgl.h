#ifndef SGL_H
#define SGL_H

#include <stdint.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#pragma pack(1)
//координата точки
struct SGuVertex
{
 float x;
 float y;
 float z;
};
//нормаль к точке
struct SGuNormal
{
 float nx;
 float ny;
 float nz;
};
//текстурные координаты
struct SGuTexture
{
 float u;
 float v;
};
//цвет точки
struct SGuColor
{
 //uint16_t Color;
 uint8_t r;
 uint8_t g;	
 uint8_t b;	
};
//цвет точки экранного буфера
struct SGuScreenColor
{
 uint8_t r;
 uint8_t g;	
 uint8_t b;	
};
//точка с текстурой, цветом, нормалью, координатами
struct SGuNVCTPoint
{
 SGuTexture sGuTexture;
 SGuColor sGuColor;
 SGuNormal sGuNormal;
 SGuVertex sGuVertex;
};
//вектор 4
struct SGuVector4
{
 float x;
 float y;
 float z;
 float w;
};
//вектор 3
struct SGuVector3
{
 float x;
 float y;
 float z;
};
//матрица 4x4
struct SGuMatrix4
{
 SGuVector4 x;
 SGuVector4	y;
 SGuVector4 z;
 SGuVector4 w;
};
//матрица 3x3
struct SGuMatrix3
{
 SGuVector3 x;
 SGuVector3	y;
 SGuVector3 z;
};

#pragma pack()



#define SGL_MATRIX_PROJECTION 100
#define SGL_MATRIX_MODELVIEW  101

#define SGL_COLOR_BUFFER_BIT  1
#define SGL_DEPTH_BUFFER_BIT  2 

#define SGL_DEPTH_TEST		  1

class CSGL
{	
 protected:
  enum MATRIX_MODE
  {
   MATRIX_MODE_MODEL_VIEW,
   MATRIX_MODE_PROJECTION		
  }; 
  //-Переменные класса-------------------------------------------------------
  SGuMatrix4 ProjectionMatrix;//матрица проектирования
  SGuMatrix4 ModelViewMatrix;//матрица моделирования
  SGuVector4 ViewPort;//вектор видового порта
	
  SGuNVCTPoint sGuNVCTPointArray[3];//список хранимых вершин
  uint16_t PointArrayAmount;//размер данных в списке
  MATRIX_MODE CurrentSelectedMatrix;//текущая выбранная матрица
  bool DrawModeActive;//true-была выполнена команда Begin
  SGuVector4 FrustumPlane[4];//четыре плоскости отсечения (каждая четверка чисел описывает плоскость: ax+by+cz+d=0)
  //-Функции класса----------------------------------------------------------
  //-Прочее------------------------------------------------------------------
 public:
  //-Конструктор класса------------------------------------------------------
  CSGL(void);
  //-Деструктор класса-------------------------------------------------------
  ~CSGL();
  //-Переменные класса-------------------------------------------------------
  SGuScreenColor* ImageMap;
  float *ZBuffer;
  int32_t ScreenWidth;
  int32_t ScreenHeight;
  //цвета
  SGuColor CurrentColor;//текущий цвет
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
  bool Vertex3f(float x,float y,float z);
  //функции очистки экранных массивов
  bool Clear(unsigned int mode);
  //функции включения/выключения режимов
  bool Enable(unsigned int mode);
  bool Disable(unsigned int mode);	
	
  void SetVertexCoord(SGuVertex &sGuVertex,float x,float y,float z);//задать координаты вершины
  void SetNormalCoord(SGuNormal &sGuNormal,float nx,float ny,float nz);//задать координаты нормали
  void SetTextureCoord(SGuTexture &sGuTexture,float u,float v);//задать координаты текстуры
  void SetColorValue(SGuColor &sGuColor,uint8_t r,uint8_t g,uint8_t b);//задать цвет
  void MultiplySGuVector4ToSGuMatrix4(const SGuVector4& v,const SGuMatrix4& m,SGuVector4& v_out);//умножение вектора типа SGuVector4 на матрицу типа SGuMatrix4
  void MultiplySGuMatrix4ToSGuVector4(const SGuMatrix4& m,const SGuVector4& v,SGuVector4& v_out);//умножение матрицы типа SGuMatrix4 на вектор типа SGuVector4
  void MultiplySGuMatrix4(const SGuMatrix4& a,const SGuMatrix4& b,SGuMatrix4& out);//умножение двух матриц типа SGuMatrix4
  void NormaliseSGuVector4(SGuVector4& v);//нормирование вектора типа SGuMatrix4
  double GetDeterminantSGuMatrix4(const SGuMatrix4& matrix);//вычислить определитель матрицы типа SGuMatrix4
  double GetDeterminantSGuMatrix3(const SGuMatrix3& matrix);//вычислить определитель матрицы типа SGuMatrix3
  void GetTruncatedMatrixSGuMatrix4(long y,long x,const SGuMatrix4& input_matrix,SGuMatrix3& output_matrix);//вычислить матрицу с исключённой строкой и столбцом по координатам y и x для матрицы типа SGuMatrix4	
  bool CreateInvertMatrixSGuMatrix4(const SGuMatrix4& input_matrix,SGuMatrix4& output_matrix);//вычислить обратную матрицу для матрицы типа SGuMatrix4
  void CreateFrustrumPlane(void);//вычислить плоскости отсечения	
  void GetIntersectionPlaneAndLine(const SGuNVCTPoint& A,const SGuNVCTPoint& B,SGuNVCTPoint& new_point,float nx,float ny,float nz,float w);//получить точку пересечения прямой и плоскости
  void Clip(const SGuNVCTPoint *point_array_input,uint16_t point_amount_input,SGuNVCTPoint *point_array_output,uint16_t &point_amount_output,float nx,float ny,float nz,float w);//выполнить коррекцию координат  


  void OutputTriangle(SGuNVCTPoint A,SGuNVCTPoint B,SGuNVCTPoint C);//вывести треугольник
  bool DrawTriangle(SGuNVCTPoint A,SGuNVCTPoint B,SGuNVCTPoint C);


	//-Прочее------------------------------------------------------------------
};

#endif

