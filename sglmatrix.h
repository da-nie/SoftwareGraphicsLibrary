#ifndef SGL_MATRIX_H
#define SGL_MATRIX_H

//****************************************************************************************************
//операции с матрицам для программной растеризации
//****************************************************************************************************

//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include <stdint.h>

//****************************************************************************************************
//структуры
//****************************************************************************************************

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


//****************************************************************************************************
//прототипы функций
//****************************************************************************************************

void MultiplySGLVector4ToSGLMatrix4(const SGLVector4& v,const SGLMatrix4& m,SGLVector4& v_out);//умножение вектора типа SGLVector4 на матрицу типа SGLMatrix4
void MultiplySGLMatrix4ToSGLVector4(const SGLMatrix4& m,const SGLVector4& v,SGLVector4& v_out);//умножение матрицы типа SGLMatrix4 на вектор типа SGLVector4
void MultiplySGLMatrix4(const SGLMatrix4& a,const SGLMatrix4& b,SGLMatrix4& out);//умножение двух матриц типа SGLMatrix4
double GetNorma(SGLVector4& v);//получить норму вектора типа SGLVector4
void NormaliseSGLVector4(SGLVector4& v);//нормирование вектора типа SGLVector4
double GetDeterminantSGLMatrix4(const SGLMatrix4& matrix);//вычислить определитель матрицы типа SGLMatrix4
double GetDeterminantSGLMatrix3(const SGLMatrix3& matrix);//вычислить определитель матрицы типа SGLMatrix3
void GetTruncatedMatrixSGLMatrix4(long y,long x,const SGLMatrix4& input_matrix,SGLMatrix3& output_matrix);//вычислить матрицу с исключённой строкой и столбцом по координатам y и x для матрицы типа SGLMatrix4
bool CreateInvertMatrixSGLMatrix4(const SGLMatrix4& input_matrix,SGLMatrix4& output_matrix);//вычислить обратную матрицу для матрицы типа SGLMatrix4

#endif
