//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include "sglmatrix.h"
#include <math.h>
#include <stdio.h>

//****************************************************************************************************
//функции
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//умножение вектора типа SGLVector4 на матрицу типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
void MultiplySGLVector4ToSGLMatrix4(const SGLVector4& v,const SGLMatrix4& m,SGLVector4& v_out)
{
 //первая координата в матрице - y, вторая x
 v_out.X=v.X*m.X.X+v.Y*m.Y.X+v.Z*m.Z.X+v.W*m.W.X;
 v_out.Y=v.X*m.X.Y+v.Y*m.Y.Y+v.Z*m.Z.Y+v.W*m.W.Y;
 v_out.Z=v.X*m.X.Z+v.Y*m.Y.Z+v.Z*m.Z.Z+v.W*m.W.Z;
 v_out.W=v.X*m.X.W+v.Y*m.Y.W+v.Z*m.Z.W+v.W*m.W.W;
}
//----------------------------------------------------------------------------------------------------
//умножение матрицы типа SGLMatrix4 на вектор типа SGLVector4
//----------------------------------------------------------------------------------------------------
void MultiplySGLMatrix4ToSGLVector4(const SGLMatrix4& m,const SGLVector4& v,SGLVector4& v_out)
{
 //первая координата в матрице - y, вторая x
 v_out.X=v.X*m.X.X+v.Y*m.X.Y+v.Z*m.X.Z+v.W*m.X.W;
 v_out.Y=v.X*m.Y.X+v.Y*m.Y.Y+v.Z*m.Y.Z+v.W*m.Y.W;
 v_out.Z=v.X*m.Z.X+v.Y*m.Z.Y+v.Z*m.Z.Z+v.W*m.Z.W;
 v_out.W=v.X*m.W.X+v.Y*m.W.Y+v.Z*m.W.Z+v.W*m.W.W;
}
//----------------------------------------------------------------------------------------------------
//умножение двух матриц типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
void MultiplySGLMatrix4(const SGLMatrix4& a,const SGLMatrix4& b,SGLMatrix4& out)
{
 out.X.X=a.X.X*b.X.X+a.X.Y*b.Y.X+a.X.Z*b.Z.X+a.X.W*b.W.X;
 out.X.Y=a.X.X*b.X.Y+a.X.Y*b.Y.Y+a.X.Z*b.Z.Y+a.X.W*b.W.Y;
 out.X.Z=a.X.X*b.X.Z+a.X.Y*b.Y.Z+a.X.Z*b.Z.Z+a.X.W*b.W.Z;
 out.X.W=a.X.X*b.X.W+a.X.Y*b.Y.W+a.X.Z*b.Z.W+a.X.W*b.W.W;

 out.Y.X=a.Y.X*b.X.X+a.Y.Y*b.Y.X+a.Y.Z*b.Z.X+a.Y.W*b.W.X;
 out.Y.Y=a.Y.X*b.X.Y+a.Y.Y*b.Y.Y+a.Y.Z*b.Z.Y+a.Y.W*b.W.Y;
 out.Y.Z=a.Y.X*b.X.Z+a.Y.Y*b.Y.Z+a.Y.Z*b.Z.Z+a.Y.W*b.W.Z;
 out.Y.W=a.Y.X*b.X.W+a.Y.Y*b.Y.W+a.Y.Z*b.Z.W+a.Y.W*b.W.W;

 out.Z.X=a.Z.X*b.X.X+a.Z.Y*b.Y.X+a.Z.Z*b.Z.X+a.Z.W*b.W.X;
 out.Z.Y=a.Z.X*b.X.Y+a.Z.Y*b.Y.Y+a.Z.Z*b.Z.Y+a.Z.W*b.W.Y;
 out.Z.Z=a.Z.X*b.X.Z+a.Z.Y*b.Y.Z+a.Z.Z*b.Z.Z+a.Z.W*b.W.Z;
 out.Z.W=a.Z.X*b.X.W+a.Z.Y*b.Y.W+a.Z.Z*b.Z.W+a.Z.W*b.W.W;

 out.W.X=a.W.X*b.X.X+a.W.Y*b.Y.X+a.W.Z*b.Z.X+a.W.W*b.W.X;
 out.W.Y=a.W.X*b.X.Y+a.W.Y*b.Y.Y+a.W.Z*b.Z.Y+a.W.W*b.W.Y;
 out.W.Z=a.W.X*b.X.Z+a.W.Y*b.Y.Z+a.W.Z*b.Z.Z+a.W.W*b.W.Z;
 out.W.W=a.W.X*b.X.W+a.W.Y*b.Y.W+a.W.Z*b.Z.W+a.W.W*b.W.W;
}
//----------------------------------------------------------------------------------------------------
//получить норму вектора типа SGLVector4
//----------------------------------------------------------------------------------------------------
double GetNorma(SGLVector4& v)
{
 float norma=sqrtf(v.X*v.X+v.Y*v.Y+v.Z*v.Z);
 return(norma);
}
//----------------------------------------------------------------------------------------------------
//нормирование вектора типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
void NormaliseSGLVector4(SGLVector4& v)
{
 const float norma=GetNorma(v);
 //вариант нулевой нормы не рассматривается ради быстродействия (он не должен встречаться в корректной 3D программе)
 const float scale=1.0f/norma;
 v.X*=scale;
 v.Y*=scale;
 v.Z*=scale;
 v.W*=scale;
}
//----------------------------------------------------------------------------------------------------
//вычислить определитель матрицы типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
double GetDeterminantSGLMatrix4(const SGLMatrix4& matrix)
{
 double a=matrix.X.X;
 double b=matrix.X.Y;
 double c=matrix.X.Z;
 double d=matrix.X.W;

 double e=matrix.Y.X;
 double f=matrix.Y.Y;
 double g=matrix.Y.Z;
 double h=matrix.Y.W;

 double i=matrix.Z.X;
 double j=matrix.Z.Y;
 double k=matrix.Z.Z;
 double l=matrix.Z.W;

 double m=matrix.W.X;
 double n=matrix.W.Y;
 double o=matrix.W.Z;
 double p=matrix.W.W;

 double det=a*(f*(k*p-l*o)+g*(l*n-j*p)+h*(j*o-k*n));
 det+=-b*(e*(k*p-l*o)+g*(l*m-i*p)+h*(i*o-k*m));
 det+=c*(e*(j*p-l*n)+f*(l*m-i*p)+h*(i*n-j*m));
 det+=-d*(e*(j*o-k*n)+f*(k*m-i*o)+g*(i*n-j*m));
 return(det);
}
//----------------------------------------------------------------------------------------------------
//вычислить определитель матрицы типа SGLMatrix3
//----------------------------------------------------------------------------------------------------
double GetDeterminantSGLMatrix3(const SGLMatrix3& matrix)
{
 double a=matrix.X.X;
 double b=matrix.X.Y;
 double c=matrix.X.Z;

 double d=matrix.Y.X;
 double e=matrix.Y.Y;
 double f=matrix.Y.Z;

 double g=matrix.Z.X;
 double h=matrix.Z.Y;
 double i=matrix.Z.Z;

 double det=b*(f*g-d*i)+c*(d*h-e*g)+a*(e*i-f*h);
 return(det);
}

//----------------------------------------------------------------------------------------------------
//вычислить матрицу с исключённой строкой и столбцом по координатам y и x для матрицы типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
void GetTruncatedMatrixSGLMatrix4(long y,long x,const SGLMatrix4& input_matrix,SGLMatrix3& output_matrix)
{
 float i_matrix[4][4];
 i_matrix[0][0]=input_matrix.X.X;
 i_matrix[0][1]=input_matrix.X.Y;
 i_matrix[0][2]=input_matrix.X.Z;
 i_matrix[0][3]=input_matrix.X.W;

 i_matrix[1][0]=input_matrix.Y.X;
 i_matrix[1][1]=input_matrix.Y.Y;
 i_matrix[1][2]=input_matrix.Y.Z;
 i_matrix[1][3]=input_matrix.Y.W;

 i_matrix[2][0]=input_matrix.Z.X;
 i_matrix[2][1]=input_matrix.Z.Y;
 i_matrix[2][2]=input_matrix.Z.Z;
 i_matrix[2][3]=input_matrix.Z.W;

 i_matrix[3][0]=input_matrix.W.X;
 i_matrix[3][1]=input_matrix.W.Y;
 i_matrix[3][2]=input_matrix.W.Z;
 i_matrix[3][3]=input_matrix.W.W;

 float* o_matrix_ptr[3][3];
 o_matrix_ptr[0][0]=&output_matrix.X.X;
 o_matrix_ptr[0][1]=&output_matrix.X.Y;
 o_matrix_ptr[0][2]=&output_matrix.X.Z;

 o_matrix_ptr[1][0]=&output_matrix.Y.X;
 o_matrix_ptr[1][1]=&output_matrix.Y.Y;
 o_matrix_ptr[1][2]=&output_matrix.Y.Z;

 o_matrix_ptr[2][0]=&output_matrix.Z.X;
 o_matrix_ptr[2][1]=&output_matrix.Z.Y;
 o_matrix_ptr[2][2]=&output_matrix.Z.Z;

 long n_i=0;
 for(long i=0;i<4;i++)
 {
  if (i==y) continue;
  long n_j=0;
  for(long j=0;j<4;j++)
  {
   if (j==x) continue;
   float element=i_matrix[i][j];
   *(o_matrix_ptr[n_i][n_j])=element;
   n_j++;
  }
  n_i++;
 }
}
//----------------------------------------------------------------------------------------------------
//вычислить обратную матрицу для матрицы типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
bool CreateInvertMatrixSGLMatrix4(const SGLMatrix4& input_matrix,SGLMatrix4& output_matrix)
{
 float *o_matrix_ptr[4][4];
 o_matrix_ptr[0][0]=&output_matrix.X.X;
 o_matrix_ptr[0][1]=&output_matrix.X.Y;
 o_matrix_ptr[0][2]=&output_matrix.X.Z;
 o_matrix_ptr[0][3]=&output_matrix.X.W;

 o_matrix_ptr[1][0]=&output_matrix.Y.X;
 o_matrix_ptr[1][1]=&output_matrix.Y.Y;
 o_matrix_ptr[1][2]=&output_matrix.Y.Z;
 o_matrix_ptr[1][3]=&output_matrix.Y.W;

 o_matrix_ptr[2][0]=&output_matrix.Z.X;
 o_matrix_ptr[2][1]=&output_matrix.Z.Y;
 o_matrix_ptr[2][2]=&output_matrix.Z.Z;
 o_matrix_ptr[2][3]=&output_matrix.Z.W;

 o_matrix_ptr[3][0]=&output_matrix.W.X;
 o_matrix_ptr[3][1]=&output_matrix.W.Y;
 o_matrix_ptr[3][2]=&output_matrix.W.Z;
 o_matrix_ptr[3][3]=&output_matrix.W.W;

 double determinant=GetDeterminantSGLMatrix4(input_matrix);
 if (determinant==0) return(false);
 SGLMatrix3 matrix3;
 for(long y=0;y<4;y++)
 {
  for(long x=0;x<4;x++)
  {
   GetTruncatedMatrixSGLMatrix4(y,x,input_matrix,matrix3);
   double detsubmatrix=GetDeterminantSGLMatrix3(matrix3);
   double value=detsubmatrix/determinant;
   if ((x+y)%2!=0) value=-value;
   *(o_matrix_ptr[x][y])=static_cast<float>(value);
  }
 }
 return(true);
}

