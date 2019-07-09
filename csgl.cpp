#include "csgl.h"
#include "fixmath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//-Конструктор класса--------------------------------------------------------
CSGL::CSGL(void)
{ 	
 ImageMap=NULL;
 InvZBuffer=NULL;
 //CurrentColor.Color=0xffff;
 CurrentColor.r=0xff;
 CurrentColor.g=0xff;
 CurrentColor.b=0xff;
 CurrentSelectedMatrix=MATRIX_MODE_PROJECTION;
 LoadIdentity();
 CurrentSelectedMatrix=MATRIX_MODE_MODEL_VIEW;
 LoadIdentity();
 PointArrayAmount=0;
 DrawModeActive=false;
 EnableDepthText=false;
 CurrentTexture.u=0;
 CurrentTexture.v=0;
 CreateFrustrumPlane();
}
//-Деструктор класса---------------------------------------------------------
CSGL::~CSGL(void)
{
 if (ImageMap!=NULL) delete[](ImageMap);
 ImageMap=NULL;
 if (InvZBuffer!=NULL) delete[](InvZBuffer);
 InvZBuffer=NULL;
}
//-Функции класса------------------------------------------------------------
bool CSGL::Create(int screen_width,int screen_height)
{
 if (ImageMap!=NULL) delete[](ImageMap);
 if (InvZBuffer!=NULL) delete[](InvZBuffer);
 ImageMap=new SGuScreenColor[screen_width*screen_height+1];
 InvZBuffer=new float[screen_width*screen_height+1];
 ScreenWidth=screen_width;
 ScreenHeight=screen_height;
 return(true);
}
bool CSGL::LoadIdentity(void)
{
 if (CurrentSelectedMatrix==MATRIX_MODE_PROJECTION)
 {
  ProjectionMatrix.x.x=1; 
  ProjectionMatrix.x.y=0;
  ProjectionMatrix.x.z=0;
  ProjectionMatrix.x.w=0;	 
	 
  ProjectionMatrix.y.x=0; 
  ProjectionMatrix.y.y=1;
  ProjectionMatrix.y.z=0;
  ProjectionMatrix.y.w=0;	 

  ProjectionMatrix.z.x=0; 
  ProjectionMatrix.z.y=0;
  ProjectionMatrix.z.z=1;
  ProjectionMatrix.z.w=0;	 

  ProjectionMatrix.w.x=0; 
  ProjectionMatrix.w.y=0;
  ProjectionMatrix.w.z=0;
  ProjectionMatrix.w.w=1;	 
 }
 if (CurrentSelectedMatrix==MATRIX_MODE_MODEL_VIEW)
 {
  ModelViewMatrix.x.x=1; 
  ModelViewMatrix.x.y=0;
  ModelViewMatrix.x.z=0;
  ModelViewMatrix.x.w=0;	 
	 
  ModelViewMatrix.y.x=0; 
  ModelViewMatrix.y.y=1;
  ModelViewMatrix.y.z=0;
  ModelViewMatrix.y.w=0;	 

  ModelViewMatrix.z.x=0; 
  ModelViewMatrix.z.y=0;
  ModelViewMatrix.z.z=1;
  ModelViewMatrix.z.w=0;	 

  ModelViewMatrix.w.x=0; 
  ModelViewMatrix.w.y=0;
  ModelViewMatrix.w.z=0;
  ModelViewMatrix.w.w=1;	 
 }
 CreateFrustrumPlane();
 return(true);
}
bool CSGL::Frustrum(float left,float right,float bottom,float top,float near,float far)
{
 if (right==left) return(false);
 if (top==bottom) return(false);
 if (near==far) return(false);
	
 SGuMatrix4	matrix;
 matrix.x.x=2*near/(right-left);
 matrix.x.y=0;	
 matrix.x.z=(right+left)/(right-left);	
 matrix.x.w=0;	

 matrix.y.x=0;	
 matrix.y.y=2*near/(top-bottom);	
 matrix.y.z=(top+bottom)/(top-bottom);	
 matrix.y.w=0;	

 matrix.z.x=0;	
 matrix.z.y=0;	
 matrix.z.z=-(far+near)/(far-near);	
 matrix.z.w=-2*far*near/(far-near);	
	
 matrix.w.x=0;	
 matrix.w.y=0;	
 matrix.w.z=-1;	
 matrix.w.w=0;	
	
 SGuMatrix4 tmp_matrix=ProjectionMatrix;	
 MultiplySGuMatrix4(tmp_matrix,matrix,ProjectionMatrix);
 CreateFrustrumPlane();
 return(true);
}
bool CSGL::SetViewport(float x,float y,float len,float hgt)
{  
 ViewPort.x=x;
 ViewPort.y=y;
 ViewPort.z=len;
 ViewPort.w=hgt;
 return(true);
}
bool CSGL::Perspective(float fovy,float aspect,float near,float far)
{
 float top=(float)tan(M_PI/180.0*fovy/2)*near;
 float bottom=-top;
 float right=top*aspect;
 float left=-right;
 return(Frustrum(left,right,bottom,top,near,far));
}
bool CSGL::Rotatef(float angle,float nx,float ny,float nz)
{
 SGuVector4 vector;	
 vector.x=nx;
 vector.y=ny;
 vector.z=nz;
 vector.w=0;	
 NormaliseSGuVector4(vector);
 nx=vector.x;	
 ny=vector.y;	
 nz=vector.z;	
	
 float cf=(float)cos(M_PI/180*angle);
 float sf=(float)sin(M_PI/180*angle);
 SGuMatrix4	matrix;
 
 matrix.x.x=nx*nx+cf*(1-nx*nx);
 matrix.x.y=nx*(1-cf)*ny+nz*sf;
 matrix.x.z=nx*(1-cf)*nz-ny*sf;
 matrix.x.w=0;

 matrix.y.x=nx*(1-cf)*ny-nz*sf;
 matrix.y.y=ny*ny+cf*(1-ny*ny);
 matrix.y.z=ny*(1-cf)*nz+nx*sf;
 matrix.y.w=0;

 matrix.z.x=nx*(1-cf)*nz+ny*sf;
 matrix.z.y=ny*(1-cf)*nz-nx*sf;
 matrix.z.z=nz*nz+cf*(1-nz*nz);
 matrix.z.w=0;

 matrix.w.x=0;
 matrix.w.y=0;
 matrix.w.z=0;
 matrix.w.w=1;

 if (CurrentSelectedMatrix==MATRIX_MODE_PROJECTION)
 {
  SGuMatrix4 tmp_matrix=ProjectionMatrix;
  MultiplySGuMatrix4(matrix,tmp_matrix,ProjectionMatrix);
 }
 if (CurrentSelectedMatrix==MATRIX_MODE_MODEL_VIEW) 
 {
  SGuMatrix4 tmp_matrix=ModelViewMatrix;		 
  MultiplySGuMatrix4(matrix,tmp_matrix,ModelViewMatrix);
 }
 CreateFrustrumPlane();
 return(true);
}
bool CSGL::Translatef(float nx,float ny,float nz)
{
 SGuMatrix4	matrix;
 matrix.x.x=1;	
 matrix.x.y=0;	
 matrix.x.z=0;	
 matrix.x.w=0;	

 matrix.y.x=0;	
 matrix.y.y=1;	
 matrix.y.z=0;	
 matrix.y.w=0;	

 matrix.z.x=0;	
 matrix.z.y=0;	
 matrix.z.z=1;	
 matrix.z.w=0;	
	
 matrix.w.x=nx;	
 matrix.w.y=ny;	
 matrix.w.z=nz;	
 matrix.w.w=1;	
	
 if (CurrentSelectedMatrix==MATRIX_MODE_PROJECTION) 
 {
  SGuMatrix4 tmp_matrix=ProjectionMatrix;
  MultiplySGuMatrix4(matrix,tmp_matrix,ProjectionMatrix);
 }
 if (CurrentSelectedMatrix==MATRIX_MODE_MODEL_VIEW) 
 {
  SGuMatrix4 tmp_matrix=ModelViewMatrix;		 
  MultiplySGuMatrix4(matrix,tmp_matrix,ModelViewMatrix);
 }
 CreateFrustrumPlane();
 return(true);
}
bool CSGL::MatrixMode(int matrix)
{
 if (matrix==SGL_MATRIX_PROJECTION) CurrentSelectedMatrix=MATRIX_MODE_PROJECTION;
 if (matrix==SGL_MATRIX_MODELVIEW) CurrentSelectedMatrix=MATRIX_MODE_MODEL_VIEW;
 return(true);
}
bool CSGL::Color3i(unsigned char r,unsigned char g,unsigned char b)
{
 uint16_t color;	  
 color=r>>3;
 color<<=6;
 color|=g>>2;
 color<<=5;
 color|=b>>3;
// CurrentColor.Color=color;
 CurrentColor.r=r;
 CurrentColor.g=g;
 CurrentColor.b=b;
 return(true);
}
bool CSGL::TexCoord(float u,float v)
{
 CurrentTexture.u=u;	
 CurrentTexture.v=v;
 return(true);
}
bool CSGL::Begin(void)
{
 PointArrayAmount=0;
 DrawModeActive=true;
 return(true);
}
bool CSGL::End(void)
{
 if (DrawModeActive==false) return(false);//режим вообще не активирован 
 DrawModeActive=false;
 PointArrayAmount=0;
 return(true);
}
bool CSGL::Vertex3f(float x,float y,float z)
{
 if (DrawModeActive==false) return(false);//режим вообще не активирован 
 //выполним преобразование точек треугольника с учётом матрицы моделирования
 SGuVector4 vector_in;		
 SGuVector4 vector_out;	
	
 vector_in.x=x;
 vector_in.y=y;
 vector_in.z=z;
 vector_in.w=1;
	
 MultiplySGuVector4ToSGuMatrix4(vector_in,ModelViewMatrix,vector_out);
 //занесём то, что получилось в список
 sGuNVCTPointArray[PointArrayAmount].sGuVertex.x=vector_out.x/vector_out.w;
 sGuNVCTPointArray[PointArrayAmount].sGuVertex.y=vector_out.y/vector_out.w;
 sGuNVCTPointArray[PointArrayAmount].sGuVertex.z=vector_out.z/vector_out.w;
 sGuNVCTPointArray[PointArrayAmount].sGuColor=CurrentColor;
 sGuNVCTPointArray[PointArrayAmount].sGuTexture=CurrentTexture;

 PointArrayAmount++;

 //отрисовываем вершины
 if (PointArrayAmount==3)//уже есть точки на треугольник
 {  
  OutputTriangle(sGuNVCTPointArray[0],sGuNVCTPointArray[1],sGuNVCTPointArray[2]);
  sGuNVCTPointArray[1]=sGuNVCTPointArray[2];//смещаем вершины
  PointArrayAmount=2;//отменяем последнюю вершину  
 }
 return(true);
}
bool CSGL::Clear(unsigned int mode)
{
 if (mode&SGL_COLOR_BUFFER_BIT && ImageMap!=NULL) memset(ImageMap,0,ScreenWidth*ScreenHeight*sizeof(SGuScreenColor));
 if (mode&SGL_DEPTH_BUFFER_BIT && InvZBuffer!=NULL) for(int32_t n=0;n<ScreenWidth*ScreenHeight;n++) InvZBuffer[n]=(float)(MIN_INV_Z_VALUE);
 return(true);
}
bool CSGL::Enable(unsigned int mode)
{
 if (mode==SGL_DEPTH_TEST) EnableDepthText=true;
 return(true);
}
bool CSGL::Disable(unsigned int mode)
{
 if (mode==SGL_DEPTH_TEST) EnableDepthText=false;
 return(true);
}


//----------------------------------------------------------------------------------------------------
//задать координаты вершины
//----------------------------------------------------------------------------------------------------
void CSGL::SetVertexCoord(SGuVertex &sGuVertex,float x,float y,float z)
{
 sGuVertex.x=x;
 sGuVertex.y=y;
 sGuVertex.z=z;
}
//----------------------------------------------------------------------------------------------------
//задать координаты нормали
//----------------------------------------------------------------------------------------------------
void CSGL::SetNormalCoord(SGuNormal &sGuNormal,float nx,float ny,float nz)
{
 sGuNormal.nx=nx;
 sGuNormal.ny=ny;
 sGuNormal.nz=nz;
}
//----------------------------------------------------------------------------------------------------
//задать координаты текстуры
//----------------------------------------------------------------------------------------------------
void CSGL::SetTextureCoord(SGuTexture &sGuTexture,float u,float v)
{
 sGuTexture.u=u;
 sGuTexture.v=v;
}
//----------------------------------------------------------------------------------------------------
//задать цвет
//----------------------------------------------------------------------------------------------------
void CSGL::SetColorValue(SGuColor &sGuColor,uint8_t r,uint8_t g,uint8_t b)
{
 uint16_t color;	  
 color=r>>3;
 color<<=6;
 color|=g>>2;
 color<<=5;
 color|=b>>3;
// sGuColor.Color=color;
 sGuColor.r=r;
 sGuColor.g=g;
 sGuColor.b=b;
}
//----------------------------------------------------------------------------------------------------
//умножение вектора типа SGuVector4 на матрицу типа SGuMatrix4
//----------------------------------------------------------------------------------------------------
void CSGL::MultiplySGuVector4ToSGuMatrix4(const SGuVector4& v,const SGuMatrix4& m,SGuVector4& v_out)
{
 //первая координата в матрице - y, вторая x
 v_out.x=v.x*m.x.x+v.y*m.y.x+v.z*m.z.x+v.w*m.w.x;
 v_out.y=v.x*m.x.y+v.y*m.y.y+v.z*m.z.y+v.w*m.w.y;
 v_out.z=v.x*m.x.z+v.y*m.y.z+v.z*m.z.z+v.w*m.w.z;
 v_out.w=v.x*m.x.w+v.y*m.y.w+v.z*m.z.w+v.w*m.w.w;
}
//----------------------------------------------------------------------------------------------------
//умножение матрицы типа SGuMatrix4 на вектор типа SGuVector4
//----------------------------------------------------------------------------------------------------
void CSGL::MultiplySGuMatrix4ToSGuVector4(const SGuMatrix4& m,const SGuVector4& v,SGuVector4& v_out)
{
 //первая координата в матрице - y, вторая x
 v_out.x=v.x*m.x.x+v.y*m.x.y+v.z*m.x.z+v.w*m.x.w;
 v_out.y=v.x*m.y.x+v.y*m.y.y+v.z*m.y.z+v.w*m.y.w;
 v_out.z=v.x*m.z.x+v.y*m.z.y+v.z*m.z.z+v.w*m.z.w;
 v_out.w=v.x*m.w.x+v.y*m.w.y+v.z*m.w.z+v.w*m.w.w;
}
//----------------------------------------------------------------------------------------------------
//умножение двух матриц типа SGuMatrix4
//----------------------------------------------------------------------------------------------------
void CSGL::MultiplySGuMatrix4(const SGuMatrix4& a,const SGuMatrix4& b,SGuMatrix4& out)
{
 out.x.x=a.x.x*b.x.x+a.x.y*b.y.x+a.x.z*b.z.x+a.x.w*b.w.x;
 out.x.y=a.x.x*b.x.y+a.x.y*b.y.y+a.x.z*b.z.y+a.x.w*b.w.y;
 out.x.z=a.x.x*b.x.z+a.x.y*b.y.z+a.x.z*b.z.z+a.x.w*b.w.z;
 out.x.w=a.x.x*b.x.w+a.x.y*b.y.w+a.x.z*b.z.w+a.x.w*b.w.w;

 out.y.x=a.y.x*b.x.x+a.y.y*b.y.x+a.y.z*b.z.x+a.y.w*b.w.x;
 out.y.y=a.y.x*b.x.y+a.y.y*b.y.y+a.y.z*b.z.y+a.y.w*b.w.y;
 out.y.z=a.y.x*b.x.z+a.y.y*b.y.z+a.y.z*b.z.z+a.y.w*b.w.z;
 out.y.w=a.y.x*b.x.w+a.y.y*b.y.w+a.y.z*b.z.w+a.y.w*b.w.w;

 out.z.x=a.z.x*b.x.x+a.z.y*b.y.x+a.z.z*b.z.x+a.z.w*b.w.x;
 out.z.y=a.z.x*b.x.y+a.z.y*b.y.y+a.z.z*b.z.y+a.z.w*b.w.y;
 out.z.z=a.z.x*b.x.z+a.z.y*b.y.z+a.z.z*b.z.z+a.z.w*b.w.z;
 out.z.w=a.z.x*b.x.w+a.z.y*b.y.w+a.z.z*b.z.w+a.z.w*b.w.w;

 out.w.x=a.w.x*b.x.x+a.w.y*b.y.x+a.w.z*b.z.x+a.w.w*b.w.x;
 out.w.y=a.w.x*b.x.y+a.w.y*b.y.y+a.w.z*b.z.y+a.w.w*b.w.y;
 out.w.z=a.w.x*b.x.z+a.w.y*b.y.z+a.w.z*b.z.z+a.w.w*b.w.z;
 out.w.w=a.w.x*b.x.w+a.w.y*b.y.w+a.w.z*b.z.w+a.w.w*b.w.w;
}
//----------------------------------------------------------------------------------------------------
//нормирование вектора типа SGuMatrix4
//----------------------------------------------------------------------------------------------------
void CSGL::NormaliseSGuVector4(SGuVector4& v)
{
 const float norma=sqrtf(v.x*v.x+v.y*v.y+v.z*v.z);
 const float scale=1.0f/norma;
 v.x*=scale;
 v.y*=scale;
 v.z*=scale;
 v.w*=scale;
}
//----------------------------------------------------------------------------------------------------
//вычислить определитель матрицы типа SGuMatrix4
//----------------------------------------------------------------------------------------------------
double CSGL::GetDeterminantSGuMatrix4(const SGuMatrix4& matrix)
{
 double a=matrix.x.x;
 double b=matrix.x.y;
 double c=matrix.x.z;
 double d=matrix.x.w;

 double e=matrix.y.x;
 double f=matrix.y.y;
 double g=matrix.y.z;
 double h=matrix.y.w;

 double i=matrix.z.x;
 double j=matrix.z.y;
 double k=matrix.z.z;
 double l=matrix.z.w;

 double m=matrix.w.x;
 double n=matrix.w.y;
 double o=matrix.w.z;
 double p=matrix.w.w;

 double det=a*(f*(k*p-l*o)+g*(l*n-j*p)+h*(j*o-k*n));
 det+=-b*(e*(k*p-l*o)+g*(l*m-i*p)+h*(i*o-k*m));
 det+=c*(e*(j*p-l*n)+f*(l*m-i*p)+h*(i*n-j*m));
 det+=-d*(e*(j*o-k*n)+f*(k*m-i*o)+g*(i*n-j*m));
 return(det);
}
//----------------------------------------------------------------------------------------------------
//вычислить определитель матрицы типа SGuMatrix3
//----------------------------------------------------------------------------------------------------
double CSGL::GetDeterminantSGuMatrix3(const SGuMatrix3& matrix)
{
 double a=matrix.x.x;
 double b=matrix.x.y;
 double c=matrix.x.z;

 double d=matrix.y.x;
 double e=matrix.y.y;
 double f=matrix.y.z;

 double g=matrix.z.x;
 double h=matrix.z.y;
 double i=matrix.z.z;

 double det=b*(f*g-d*i)+c*(d*h-e*g)+a*(e*i-f*h);
 return(det);
}

//----------------------------------------------------------------------------------------------------
//вычислить матрицу с исключённой строкой и столбцом по координатам y и x для матрицы типа SGuMatrix4
//----------------------------------------------------------------------------------------------------
void CSGL::GetTruncatedMatrixSGuMatrix4(long y,long x,const SGuMatrix4& input_matrix,SGuMatrix3& output_matrix)
{
 float i_matrix[4][4];
 i_matrix[0][0]=input_matrix.x.x;
 i_matrix[0][1]=input_matrix.x.y;
 i_matrix[0][2]=input_matrix.x.z;
 i_matrix[0][3]=input_matrix.x.w;

 i_matrix[1][0]=input_matrix.y.x;
 i_matrix[1][1]=input_matrix.y.y;
 i_matrix[1][2]=input_matrix.y.z;
 i_matrix[1][3]=input_matrix.y.w;

 i_matrix[2][0]=input_matrix.z.x;
 i_matrix[2][1]=input_matrix.z.y;
 i_matrix[2][2]=input_matrix.z.z;
 i_matrix[2][3]=input_matrix.z.w;

 i_matrix[3][0]=input_matrix.w.x;
 i_matrix[3][1]=input_matrix.w.y;
 i_matrix[3][2]=input_matrix.w.z;
 i_matrix[3][3]=input_matrix.w.w;

 float* o_matrix_ptr[3][3];
 o_matrix_ptr[0][0]=&output_matrix.x.x;
 o_matrix_ptr[0][1]=&output_matrix.x.y;
 o_matrix_ptr[0][2]=&output_matrix.x.z;

 o_matrix_ptr[1][0]=&output_matrix.y.x;
 o_matrix_ptr[1][1]=&output_matrix.y.y;
 o_matrix_ptr[1][2]=&output_matrix.y.z;

 o_matrix_ptr[2][0]=&output_matrix.z.x;
 o_matrix_ptr[2][1]=&output_matrix.z.y;
 o_matrix_ptr[2][2]=&output_matrix.z.z;

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
//вычислить обратную матрицу для матрицы типа SGuMatrix4
//----------------------------------------------------------------------------------------------------
bool CSGL::CreateInvertMatrixSGuMatrix4(const SGuMatrix4& input_matrix,SGuMatrix4& output_matrix)
{
 float *o_matrix_ptr[4][4];
 o_matrix_ptr[0][0]=&output_matrix.x.x;
 o_matrix_ptr[0][1]=&output_matrix.x.y;
 o_matrix_ptr[0][2]=&output_matrix.x.z;
 o_matrix_ptr[0][3]=&output_matrix.x.w;

 o_matrix_ptr[1][0]=&output_matrix.y.x;
 o_matrix_ptr[1][1]=&output_matrix.y.y;
 o_matrix_ptr[1][2]=&output_matrix.y.z;
 o_matrix_ptr[1][3]=&output_matrix.y.w;

 o_matrix_ptr[2][0]=&output_matrix.z.x;
 o_matrix_ptr[2][1]=&output_matrix.z.y;
 o_matrix_ptr[2][2]=&output_matrix.z.z;
 o_matrix_ptr[2][3]=&output_matrix.z.w;

 o_matrix_ptr[3][0]=&output_matrix.w.x;
 o_matrix_ptr[3][1]=&output_matrix.w.y;
 o_matrix_ptr[3][2]=&output_matrix.w.z;
 o_matrix_ptr[3][3]=&output_matrix.w.w;

 double determinant=GetDeterminantSGuMatrix4(input_matrix);
 if (determinant==0) return(false);
 SGuMatrix3 matrix3;
 for(long y=0;y<4;y++)
 {
  for(long x=0;x<4;x++)
  {
   GetTruncatedMatrixSGuMatrix4(y,x,input_matrix,matrix3);
   double detsubmatrix=GetDeterminantSGuMatrix3(matrix3);
   double value=detsubmatrix/determinant;
   if ((x+y)%2!=0) value=-value;
   *(o_matrix_ptr[x][y])=value;
  }
 }
 return(true);
}
//----------------------------------------------------------------------------------------------------
//вычислить плоскости отсечения
//----------------------------------------------------------------------------------------------------
void CSGL::CreateFrustrumPlane(void)
{
 //вычисляем общую матрицу modelview-projection
 //ВНИМАНИЕ!!!
 //так как координаты в буфере (Vertex3f) уже переведены с учётом матрицы моделирования, строить плоскости нужно только по матрице проецирования!
 //ВНИМАНИЕ!!!

 SGuMatrix4 projection_model_view_matrix=ProjectionMatrix;
 //MultiplySGuMatrix4(ModelViewMatrix,ProjectionMatrix,projection_model_view_matrix);
 //вычисляем четыре плоскости отсечения по проекции (верх, низ, лево, право)
 //левая
 FrustumPlane[0].x=projection_model_view_matrix.x.w+projection_model_view_matrix.x.x;
 FrustumPlane[0].y=projection_model_view_matrix.y.w+projection_model_view_matrix.y.x;
 FrustumPlane[0].z=projection_model_view_matrix.z.w+projection_model_view_matrix.z.x;
 FrustumPlane[0].w=projection_model_view_matrix.w.w+projection_model_view_matrix.w.x;
 NormaliseSGuVector4(FrustumPlane[0]);
 //правая
 FrustumPlane[1].x=projection_model_view_matrix.x.w-projection_model_view_matrix.x.x;
 FrustumPlane[1].y=projection_model_view_matrix.y.w-projection_model_view_matrix.y.x;
 FrustumPlane[1].z=projection_model_view_matrix.z.w-projection_model_view_matrix.z.x;
 FrustumPlane[1].w=projection_model_view_matrix.w.w-projection_model_view_matrix.w.x;
 NormaliseSGuVector4(FrustumPlane[1]);
 //верхняя
 FrustumPlane[2].x=projection_model_view_matrix.x.w-projection_model_view_matrix.x.y;
 FrustumPlane[2].y=projection_model_view_matrix.y.w-projection_model_view_matrix.y.y;
 FrustumPlane[2].z=projection_model_view_matrix.z.w-projection_model_view_matrix.z.y;
 FrustumPlane[2].w=projection_model_view_matrix.w.w-projection_model_view_matrix.w.y;
 NormaliseSGuVector4(FrustumPlane[2]);
 //нижняя
 FrustumPlane[3].x=projection_model_view_matrix.x.w+projection_model_view_matrix.x.y;
 FrustumPlane[3].y=projection_model_view_matrix.y.w+projection_model_view_matrix.y.y;
 FrustumPlane[3].z=projection_model_view_matrix.z.w+projection_model_view_matrix.z.y;
 FrustumPlane[3].w=projection_model_view_matrix.w.w+projection_model_view_matrix.w.y;
 NormaliseSGuVector4(FrustumPlane[3]);
 //передняя
 FrustumPlane[4].x=projection_model_view_matrix.x.w+projection_model_view_matrix.x.z;
 FrustumPlane[4].y=projection_model_view_matrix.y.w+projection_model_view_matrix.y.z;
 FrustumPlane[4].z=projection_model_view_matrix.z.w+projection_model_view_matrix.z.z;
 FrustumPlane[4].w=projection_model_view_matrix.w.w+projection_model_view_matrix.w.z;
 NormaliseSGuVector4(FrustumPlane[4]);
 /*
 //задняя
 FrustumPlane[5].x=projection_model_view_matrix.x.w-projection_model_view_matrix.x.z;
 FrustumPlane[5].y=projection_model_view_matrix.y.w-projection_model_view_matrix.y.z;
 FrustumPlane[5].z=projection_model_view_matrix.z.w-projection_model_view_matrix.z.z;
 FrustumPlane[5].w=projection_model_view_matrix.w.w-projection_model_view_matrix.w.z;
 NormaliseSGuVector4(FrustumPlane[5]);
 */
}
//----------------------------------------------------------------------------------------------------
//получить точку пересечения прямой и плоскости
//----------------------------------------------------------------------------------------------------
void CSGL::GetIntersectionPlaneAndLine(const SGuNVCTPoint& A,const SGuNVCTPoint& B,SGuNVCTPoint& new_point,float nx,float ny,float nz,float w)
{
 new_point=A;
 float ax=A.sGuVertex.x;
 float ay=A.sGuVertex.y;
 float az=A.sGuVertex.z;
 float au=A.sGuTexture.u;
 float av=A.sGuTexture.v;
 float ar=A.sGuColor.r;
 float ag=A.sGuColor.g;
 float ab=A.sGuColor.b;

 float bx=B.sGuVertex.x;
 float by=B.sGuVertex.y;
 float bz=B.sGuVertex.z;
 float bu=B.sGuTexture.u;
 float bv=B.sGuTexture.v;
 float br=B.sGuColor.r;
 float bg=B.sGuColor.g;
 float bb=B.sGuColor.b;

 float dx=bx-ax;
 float dy=by-ay;
 float dz=bz-az;
 float du=bu-au;
 float dv=bv-av;
 float dr=br-ar;
 float dg=bg-ag;
 float db=bb-ab;

 float top=(nx*ax)+(ny*ay)+(nz*az)+w;
 float bottom=(nx*dx)+(ny*dy)+(nz*dz);
 float time=-top/bottom;

 float vx=ax+time*dx;
 float vy=ay+time*dy;
 float vz=az+time*dz;
 float vu=au+time*du;
 float vv=av+time*dv;
 float r=ar+time*dr;
 float g=ag+time*dg;
 float b=ab+time*db;
 //добавляем новую точку
 SetVertexCoord(new_point.sGuVertex,vx,vy,vz);
 SetTextureCoord(new_point.sGuTexture,vu,vv);
 SetColorValue(new_point.sGuColor,(uint8_t)r,(uint8_t)g,(uint8_t)b);
}

//----------------------------------------------------------------------------------------------------
//выполнить коррекцию координат
//----------------------------------------------------------------------------------------------------
void CSGL::Clip(const SGuNVCTPoint *point_array_input,uint16_t point_amount_input,SGuNVCTPoint *point_array_output,uint16_t &point_amount_output,float nx,float ny,float nz,float w)
{
 point_amount_output=0; 
 for(uint16_t n=0;n<point_amount_input;n++)
 {
  long next_p=n+1;
  if (next_p>=point_amount_input) next_p-=point_amount_input;

  const SGuNVCTPoint *sGuNVCTPoint_Current_Ptr=&(point_array_input[n]);
  float current_vx=sGuNVCTPoint_Current_Ptr->sGuVertex.x;
  float current_vy=sGuNVCTPoint_Current_Ptr->sGuVertex.y;
  float current_vz=sGuNVCTPoint_Current_Ptr->sGuVertex.z;
  //определяем положение относительно плоскости отсечения
  float current_ret=current_vx*nx+current_vy*ny+current_vz*nz+w;

  const SGuNVCTPoint *sGuNVCTPoint_Next_Ptr=&(point_array_input[next_p]);
  float next_vx=sGuNVCTPoint_Next_Ptr->sGuVertex.x;
  float next_vy=sGuNVCTPoint_Next_Ptr->sGuVertex.y;
  float next_vz=sGuNVCTPoint_Next_Ptr->sGuVertex.z;
  //определяем положение относительно плоскости отсечения
  float next_ret=next_vx*nx+next_vy*ny+next_vz*nz+w;

  if (current_ret>0)//текущая точка видима
  {
   if (next_ret>0)//следующая точка видима
   {
    point_array_output[point_amount_output]=*sGuNVCTPoint_Next_Ptr;
	point_amount_output++;
   }
   else
   {
    //добавляем новую точку пересечения
    SGuNVCTPoint sGuNVCTPoint_New;
    GetIntersectionPlaneAndLine(*sGuNVCTPoint_Current_Ptr,*sGuNVCTPoint_Next_Ptr,sGuNVCTPoint_New,nx,ny,nz,w);
    point_array_output[point_amount_output]=sGuNVCTPoint_New;
	point_amount_output++;
   }
  }
  else//текущая точка не видна
  {
   if (next_ret>0)//следующая точка видна
   {
    //добавляем новую точку пересечения
    SGuNVCTPoint sGuNVCTPoint_New;
    GetIntersectionPlaneAndLine(*sGuNVCTPoint_Current_Ptr,*sGuNVCTPoint_Next_Ptr,sGuNVCTPoint_New,nx,ny,nz,w);
    point_array_output[point_amount_output]=sGuNVCTPoint_New;
	point_amount_output++;
    //добавляем сдедующую точку
    point_array_output[point_amount_output]=*sGuNVCTPoint_Next_Ptr;
	point_amount_output++;
   }
  }
 }
}

//----------------------------------------------------------------------------------------------------
//вывести треугольник
//----------------------------------------------------------------------------------------------------
void CSGL::OutputTriangle(SGuNVCTPoint A,SGuNVCTPoint B,SGuNVCTPoint C)
{
 //выполняем отсечение геометрии для всех ограничивающих плоскостей 
 SGuNVCTPoint sGuNVCTPoint[2][3*2*4];//список хранимых вершин
 
 sGuNVCTPoint[0][0]=A;
 sGuNVCTPoint[0][1]=B;
 sGuNVCTPoint[0][2]=C;
 
 uint16_t point_amount=3;
 uint8_t input_index=0;
 uint8_t output_index=1;
 
 for(uint8_t n=0;n<5;n++)
 {
  float nx=FrustumPlane[n].x;
  float ny=FrustumPlane[n].y;
  float nz=FrustumPlane[n].z;
  float w=FrustumPlane[n].w;
  Clip(sGuNVCTPoint[input_index],point_amount,sGuNVCTPoint[output_index],point_amount,nx,ny,nz,w);
  //переставляем буфера точек
  uint8_t tmp=input_index;
  input_index=output_index;
  output_index=tmp;
 }
 if (point_amount<3) return;//нечего рисовать
 //выполняем отрисовку треугольников
 for(uint8_t n=1;n<point_amount-1;n++)
 {
  uint16_t i1=n;
  uint16_t i2=n+1;
  DrawTriangle(sGuNVCTPoint[input_index][0],sGuNVCTPoint[input_index][i1],sGuNVCTPoint[input_index][i2]);
 } 
}
//----------------------------------------------------------------------------------------------------
//отрисовка треугольника
//----------------------------------------------------------------------------------------------------
void CSGL::DrawTriangle(SGuNVCTPoint A,SGuNVCTPoint B,SGuNVCTPoint C)
{
 SGuVector4 sGuVector4_A;
 SGuVector4 sGuVector4_B;
 SGuVector4 sGuVector4_C;
	
 //вычислим проекции вершин треугольника
 sGuVector4_A.x=A.sGuVertex.x;
 sGuVector4_A.y=A.sGuVertex.y;
 sGuVector4_A.z=A.sGuVertex.z;
 sGuVector4_A.w=1;

 sGuVector4_B.x=B.sGuVertex.x;
 sGuVector4_B.y=B.sGuVertex.y;
 sGuVector4_B.z=B.sGuVertex.z;
 sGuVector4_B.w=1;

 sGuVector4_C.x=C.sGuVertex.x;
 sGuVector4_C.y=C.sGuVertex.y;
 sGuVector4_C.z=C.sGuVertex.z;
 sGuVector4_C.w=1;

 SGuVector4 sGuVector4_A_New;
 SGuVector4 sGuVector4_B_New;
 SGuVector4 sGuVector4_C_New;

 MultiplySGuMatrix4ToSGuVector4(ProjectionMatrix,sGuVector4_A,sGuVector4_A_New);
 MultiplySGuMatrix4ToSGuVector4(ProjectionMatrix,sGuVector4_B,sGuVector4_B_New);
 MultiplySGuMatrix4ToSGuVector4(ProjectionMatrix,sGuVector4_C,sGuVector4_C_New);

 SGuScreenPoint ap;
 SGuScreenPoint bp;
 SGuScreenPoint cp;

 ap.x=(int32_t)((sGuVector4_A_New.x/sGuVector4_A_New.w+1)*ViewPort.z/2+ViewPort.x);
 ap.y=(int32_t)((1-sGuVector4_A_New.y/sGuVector4_A_New.w)*ViewPort.w/2+ViewPort.y);

 bp.x=(int32_t)((sGuVector4_B_New.x/sGuVector4_B_New.w+1)*ViewPort.z/2+ViewPort.x);
 bp.y=(int32_t)((1-sGuVector4_B_New.y/sGuVector4_B_New.w)*ViewPort.w/2+ViewPort.y);

 cp.x=(int32_t)((sGuVector4_C_New.x/sGuVector4_C_New.w+1)*ViewPort.z/2+ViewPort.x);
 cp.y=(int32_t)((1-sGuVector4_C_New.y/sGuVector4_C_New.w)*ViewPort.w/2+ViewPort.y);

 RenderTriangle(A,B,C,ap,bp,cp);
}
//----------------------------------------------------------------------------------------------------
//растеризация треугольника на экране
//----------------------------------------------------------------------------------------------------
void CSGL::RenderTriangle(SGuNVCTPoint &a,SGuNVCTPoint &b,SGuNVCTPoint &c,SGuScreenPoint &ap,SGuScreenPoint &bp,SGuScreenPoint &cp)
{
 SGuNVCTPoint tmp_point;
 SGuScreenPoint tmp_screen;
 if (ap.y>cp.y)
 {
  tmp_point=a;
  a=c;
  c=tmp_point;

  tmp_screen=ap;
  ap=cp;
  cp=tmp_screen;
 }
 if (ap.y>bp.y)
 {
  tmp_point=a;
  a=b;
  b=tmp_point;

  tmp_screen=ap;
  ap=bp;
  bp=tmp_screen;
 }
 if (cp.y<bp.y)
 {
  tmp_point=c;
  c=b;
  b=tmp_point;

  tmp_screen=cp;
  cp=bp;
  bp=tmp_screen;
 }

 //проверим, попадает ли треугольник в порт просмотра
 int32_t starty=ap.y;
 int32_t endy=cp.y;
 if (starty==endy) return;//нечего рисовать
 if (starty>=ViewPort.y+ViewPort.w) return;//треугольник не виден
 if (endy<ViewPort.x) return;//треугольник не виден
 if (starty<ViewPort.x) starty=ViewPort.x;
 if (endy>=ViewPort.y+ViewPort.w) endy=ViewPort.y+ViewPort.w-1;

 //смещаем позицию с учётом обрезания по видовому порту
 float lyca=(cp.y-ap.y);
 float lyba=(bp.y-ap.y);
 float offset=starty-ap.y;
 //x
 float dcx1=(cp.x-ap.x)/lyca;
 float dcx2=(bp.x-ap.x)/lyba;
 float cx1=ap.x+offset*dcx1;
 float cx2=ap.x+offset*dcx2;
 //z
 float dcz1=(1.0f/c.sGuVertex.z-1.0f/a.sGuVertex.z)/lyca;
 float dcz2=(1.0f/b.sGuVertex.z-1.0f/a.sGuVertex.z)/lyba;
 float cz1=1.0f/a.sGuVertex.z+offset*dcz1;
 float cz2=1.0f/a.sGuVertex.z+offset*dcz2;
 //r
 float dcr1=(c.sGuColor.r-a.sGuColor.r)/lyca;
 float dcr2=(b.sGuColor.r-a.sGuColor.r)/lyba;
 float cr1=a.sGuColor.r+offset*dcr1;
 float cr2=a.sGuColor.r+offset*dcr2;
 //g
 float dcg1=(c.sGuColor.g-a.sGuColor.g)/lyca;
 float dcg2=(b.sGuColor.g-a.sGuColor.g)/lyba;
 float cg1=a.sGuColor.g+offset*dcg1;
 float cg2=a.sGuColor.g+offset*dcg2;
 //b
 float dcb1=(c.sGuColor.b-a.sGuColor.b)/lyca;
 float dcb2=(b.sGuColor.b-a.sGuColor.b)/lyba;
 float cb1=a.sGuColor.b+offset*dcb1;
 float cb2=a.sGuColor.b+offset*dcb2;

 //u
 float dcu1=(c.sGuTexture.u/c.sGuVertex.z-a.sGuTexture.u/a.sGuVertex.z)/lyca;
 float dcu2=(b.sGuTexture.u/b.sGuVertex.z-a.sGuTexture.u/a.sGuVertex.z)/lyba;
 float cu1=a.sGuTexture.u/a.sGuVertex.z+offset*dcu1;
 float cu2=a.sGuTexture.u/a.sGuVertex.z+offset*dcu2;

 //v
 float dcv1=(c.sGuTexture.v/c.sGuVertex.z-a.sGuTexture.v/a.sGuVertex.z)/lyca;
 float dcv2=(b.sGuTexture.v/b.sGuVertex.z-a.sGuTexture.v/a.sGuVertex.z)/lyba;
 float cv1=a.sGuTexture.v/a.sGuVertex.z+offset*dcv1;
 float cv2=a.sGuTexture.v/a.sGuVertex.z+offset*dcv2;

 bool first_half=true;
 for(int32_t sy=starty;sy<=endy;sy++,cx1+=dcx1,cx2+=dcx2,cz1+=dcz1,cz2+=dcz2, cr1+=dcr1,cr2+=dcr2, cg1+=dcg1,cg2+=dcg2, cb1+=dcb1,cb2+=dcb2, cu1+=dcu1,cu2+=dcu2, cv1+=dcv1,cv2+=dcv2)
 {
  float x1,x2;
  float z1,z2;
  float r1,r2;
  float g1,g2;
  float b1,b2;
  float u1,u2;
  float v1,v2;

  x1=cx1;
  z1=cz1;
  r1=cr1;
  g1=cg1;
  b1=cb1;
  u1=cu1;
  v1=cv1;

  if (sy<bp.y)
  {
   x2=cx2;
   z2=cz2;

   r2=cr2;
   g2=cg2;
   b2=cb2;
		
   u2=cu2;
   v2=cv2;
  }
  else
  {
   if (cp.y==bp.y)
   {
	x2=bp.x;
	z2=1.0f/b.sGuVertex.z;

	r2=b.sGuColor.r;
	g2=b.sGuColor.g;
	b2=b.sGuColor.b;
		 
 	u2=b.sGuTexture.u/b.sGuVertex.z;
    v2=b.sGuTexture.v/b.sGuVertex.z;
   }
   else
   {
    if (first_half==true)
   	{
     float lycb=(cp.y-bp.y);
     float offset=sy-bp.y;

	 dcx2=(cp.x-bp.x)/lycb;
	 dcz2=(1.0f/c.sGuVertex.z-1.0f/b.sGuVertex.z)/lycb;
     cx2=bp.x+offset*dcx2;
     cz2=1.0f/b.sGuVertex.z+offset*dcz2;

     dcr2=(c.sGuColor.r-b.sGuColor.r)/lycb;
     cr2=b.sGuColor.r+offset*dcr2;

     dcg2=(c.sGuColor.g-b.sGuColor.g)/lycb;
     cg2=b.sGuColor.g+offset*dcg2;

     dcb2=(c.sGuColor.b-b.sGuColor.b)/lycb;
     cb2=b.sGuColor.b+offset*dcb2;
	
     dcu2=(c.sGuTexture.u/c.sGuVertex.z-b.sGuTexture.u/b.sGuVertex.z)/lycb;
     cu2=b.sGuTexture.u/b.sGuVertex.z+offset*dcu2;

     dcv2=(c.sGuTexture.v/c.sGuVertex.z-b.sGuTexture.v/b.sGuVertex.z)/lycb;
     cv2=b.sGuTexture.v/b.sGuVertex.z+offset*dcv2;
	
     first_half=false;
    }	
    x2=cx2;
    z2=cz2;

    r2=cr2;
    g2=cg2;
    b2=cb2;
		
    u2=cu2;
    v2=cv2;		
   }
  }
  if (x1>x2)
  {
   float tmp=x1;
   x1=x2;
   x2=tmp;
   tmp=z1;
   z1=z2;
   z2=tmp;

   tmp=r1;
   r1=r2;
   r2=tmp;

   tmp=g1;
   g1=g2;
   g2=tmp;

   tmp=b1;
   b1=b2;
   b2=tmp;
		
   tmp=u1;
   u1=u2;
   u2=tmp;
		
   tmp=v1;
   v1=v2;
   v2=tmp;
  }
  //чертим линию треугольника
  DrawLine(sy,x1,x2,z1,z2,r1,r2,g1,g2,b1,b2,u1,u2,v1,v2);
 }
}
//----------------------------------------------------------------------------------------------------
//отрисовка линии
//----------------------------------------------------------------------------------------------------
void CSGL::DrawLine(int32_t y,int32_t x1,int32_t x2,float z1,float z2,float r1,float r2,float g1,float g2,float b1,float b2,float u1,float u2,float v1,float v2)
{
 float dz=0;
 float dr=0;
 float dg=0;
 float db=0;
 float du=0;
 float dv=0;
 if (x2!=x1)
 {
  float dx=x2-x1;
  dz=(z2-z1)/dx;
  dr=(r2-r1)/dx;
  dg=(g2-g1)/dx;
  db=(b2-b1)/dx;
  du=(u2-u1)/dx;
  dv=(v2-v1)/dx;
 }
 if (x2>=ViewPort.x+ViewPort.z) x2=ViewPort.x+ViewPort.z-1;
 if (x1<ViewPort.x)
 {  
  float offset=(ViewPort.x-x1);
  x1=ViewPort.x;
  z1+=offset*dz;
  r1+=offset*dr;
  g1+=offset*dg;
  b1+=offset*db;
  u1+=offset*du;
  v1+=offset*dv;
 }
 SGuScreenColor *vptr=ImageMap+(x1+(ScreenHeight-y-1)*ScreenWidth);
 float *invdepthptr=InvZBuffer+x1+(int32_t)(ScreenHeight-y-1)*ScreenWidth;
 if (EnableDepthText==false)//тест глубины не производится
 {
  float z=z1;
  float r=r1;
  float g=g1;
  float b=b1;
  float u=u1;
  float v=v1;
  for(float x=x1;x<=x2;x++,z+=dz,r+=dr,g+=dg,b+=db,u+=du,v+=dv,vptr++,invdepthptr++)
  {
   /*
   uint16_t color;	  
   color=(uint8_t)(r)>>3;
   color<<=6;
   color|=(uint8_t)(g)>>2;
   color<<=5;
   color|=(uint8_t)(b)>>3;		 
   vptr->Color=color;*/
   
   vptr->r=(int8_t)(r)&0xff;
   vptr->g=(int8_t)(g)&0xff;
   vptr->b=(int8_t)(b)&0xff;
   
   *(invdepthptr)=z;
  }
 }
 else//тест глубины производится
 {
  float z=z1;
  float r=r1;
  float g=g1;
  float b=b1;
  float u=u1;
  float v=v1;

  //используем линейную интерполяцию текстуры через 16 пикселей
  int32_t step=16;//шаг интерполяции
  if ((x2-x1)<step) step=x2-x1;
  float dz_step=dz*step;
  float du_step=du*step;
  float dv_step=dv*step;

  float tu1=u/z;
  float tv1=v/z;
  float tu2=(u+du_step)/(z+dz_step);
  float tv2=(v+dv_step)/(z+dz_step);
  float dtu=(tu2-tu1)/step;
  float dtv=(tv2-tv1)/step;

  float tu=tu1;
  float tv=tv1;

  int32_t counter=step;
  for(float x=x1;x<=x2;x++,z+=dz,r+=dr,g+=dg,b+=db,u+=du,v+=dv,vptr++,invdepthptr++,counter--)
  {
   if (counter==0)
   {    		 
    counter=step;
	if ((x2-counter)<step) step=x2-counter;

    dz_step=dz*step;
    du_step=du*step;
    dv_step=dv*step;

    tu1=u/z;
    tv1=v/z;
    tu2=(u+du_step)/(z+dz_step);
    tv2=(v+dv_step)/(z+dz_step);
    dtu=(tu2-tu1)/step;
    dtv=(tv2-tv1)/step;
   }
   tu+=dtu;
   tv+=dtv;

   //float real_z=1.0f/z;
   if (*(invdepthptr)>z)
   {
    int32_t ut=(int)(tu*8.0f);
	int32_t vt=(int)(tv*8.0f);
    
    //uint16_t color;	  
    uint8_t cr=(int8_t)r;
	uint8_t cg=(int8_t)g;
	uint8_t cb=(int8_t)b;
    if ((ut+vt)&1)
		{
		
		}
		else
		{
         cr^=255;
		 cg^=255;
		 cb^=255;
		}
  	//vptr->Color=color;
	
		 
	vptr->r=(int8_t)(cr)&0xff;
	vptr->g=(int8_t)(cg)&0xff;
	vptr->b=(int8_t)(cb)&0xff;
		 

    *(invdepthptr)=z;
   }
  }
 }
}
