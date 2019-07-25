//****************************************************************************************************
//подключаемые библиотеки
//****************************************************************************************************
#include "csgl.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

//****************************************************************************************************
//глобальные переменные
//****************************************************************************************************

//****************************************************************************************************
//константы
//****************************************************************************************************
static const float PI=3.1415926535897932384626433832795f;

//****************************************************************************************************
//макроопределения
//****************************************************************************************************

//****************************************************************************************************
//конструктор и деструктор
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//конструктор
//----------------------------------------------------------------------------------------------------
CSGL::CSGL(void)
{
 MatrixMode(SGL_MATRIX_TEXTURE);
 LoadIdentity();
 MatrixMode(SGL_MATRIX_PROJECTION);
 LoadIdentity();
 MatrixMode(SGL_MATRIX_MODELVIEW);
 LoadIdentity();
 ClearColor(0,0,0);
 DrawMode=false;
 PointArrayAmount=0;
 Color3f(1,1,1);
 Normal3f(1,0,0);
 TexCoordf(0,0);

 ScreenWidth=0;
 ScreenHeight=0;
 ImageMap=NULL;
 InvZBuffer=NULL;

 BindTexture(0,0,NULL);
}
//----------------------------------------------------------------------------------------------------
//деструктор
//----------------------------------------------------------------------------------------------------
CSGL::~CSGL()
{
 if (ImageMap!=NULL) delete[](ImageMap);
 if (InvZBuffer!=NULL) delete[](InvZBuffer);
}

//****************************************************************************************************
//закрытые функции
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//вычислить плоскости отсечения
//----------------------------------------------------------------------------------------------------
void CSGL::CreateFrustrumPlane(void)
{
 //вычисляем общую матрицу modelview-projection
 //ВНИМАНИЕ!!!
 //так как координаты в буфере (Vertex3f) уже переведены с учётом матрицы моделирования, строить плоскости нужно только по матрице проецирования!
 //ВНИМАНИЕ!!!

 SGLMatrix4 projection_model_view_matrix=ProjectionMatrix;
 //вычисляем четыре плоскости отсечения по проекции (верх, низ, лево, право)
 //левая
 FrustumPlane[0].X=projection_model_view_matrix.X.W+projection_model_view_matrix.X.X;
 FrustumPlane[0].Y=projection_model_view_matrix.Y.W+projection_model_view_matrix.Y.X;
 FrustumPlane[0].Z=projection_model_view_matrix.Z.W+projection_model_view_matrix.Z.X;
 FrustumPlane[0].W=projection_model_view_matrix.W.W+projection_model_view_matrix.W.X;
 NormaliseSGLVector4(FrustumPlane[0]);
 //правая
 FrustumPlane[1].X=projection_model_view_matrix.X.W-projection_model_view_matrix.X.X;
 FrustumPlane[1].Y=projection_model_view_matrix.Y.W-projection_model_view_matrix.Y.X;
 FrustumPlane[1].Z=projection_model_view_matrix.Z.W-projection_model_view_matrix.Z.X;
 FrustumPlane[1].W=projection_model_view_matrix.W.W-projection_model_view_matrix.W.X;
 NormaliseSGLVector4(FrustumPlane[1]);
 //верхняя
 FrustumPlane[2].X=projection_model_view_matrix.X.W-projection_model_view_matrix.X.Y;
 FrustumPlane[2].Y=projection_model_view_matrix.Y.W-projection_model_view_matrix.Y.Y;
 FrustumPlane[2].Z=projection_model_view_matrix.Z.W-projection_model_view_matrix.Z.Y;
 FrustumPlane[2].W=projection_model_view_matrix.W.W-projection_model_view_matrix.W.Y;
 NormaliseSGLVector4(FrustumPlane[2]);
 //нижняя
 FrustumPlane[3].X=projection_model_view_matrix.X.W+projection_model_view_matrix.X.Y;
 FrustumPlane[3].Y=projection_model_view_matrix.Y.W+projection_model_view_matrix.Y.Y;
 FrustumPlane[3].Z=projection_model_view_matrix.Z.W+projection_model_view_matrix.Z.Y;
 FrustumPlane[3].W=projection_model_view_matrix.W.W+projection_model_view_matrix.W.Y;
 NormaliseSGLVector4(FrustumPlane[3]);
 //передняя
 FrustumPlane[4].X=projection_model_view_matrix.X.W+projection_model_view_matrix.X.Z;
 FrustumPlane[4].Y=projection_model_view_matrix.Y.W+projection_model_view_matrix.Y.Z;
 FrustumPlane[4].Z=projection_model_view_matrix.Z.W+projection_model_view_matrix.Z.Z;
 FrustumPlane[4].W=projection_model_view_matrix.W.W+projection_model_view_matrix.W.Z;
 NormaliseSGLVector4(FrustumPlane[4]);
 
 //задняя
 FrustumPlane[5].X=projection_model_view_matrix.X.W-projection_model_view_matrix.X.Z;
 FrustumPlane[5].Y=projection_model_view_matrix.Y.W-projection_model_view_matrix.Y.Z;
 FrustumPlane[5].Z=projection_model_view_matrix.Z.W-projection_model_view_matrix.Z.Z;
 FrustumPlane[5].W=projection_model_view_matrix.W.W-projection_model_view_matrix.W.Z;
 NormaliseSGLVector4(FrustumPlane[5]);
 
}
//----------------------------------------------------------------------------------------------------
//получить точку пересечения прямой и плоскости
//----------------------------------------------------------------------------------------------------
void CSGL::GetIntersectionPlaneAndLine(const SGLNVCTPoint& A,const SGLNVCTPoint& B,SGLNVCTPoint& new_point,float nx,float ny,float nz,float w)
{
 new_point=A;
 float ax=A.sGLVertex.X;
 float ay=A.sGLVertex.Y;
 float az=A.sGLVertex.Z;
 float au=A.sGLTexture.U;
 float av=A.sGLTexture.V;
 float ar=A.sGLColor.R;
 float ag=A.sGLColor.G;
 float ab=A.sGLColor.B;

 float bx=B.sGLVertex.X;
 float by=B.sGLVertex.Y;
 float bz=B.sGLVertex.Z;
 float bu=B.sGLTexture.U;
 float bv=B.sGLTexture.V;
 float br=B.sGLColor.R;
 float bg=B.sGLColor.G;
 float bb=B.sGLColor.B;

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
 SetVertexCoord(new_point.sGLVertex,vx,vy,vz);
 SetTextureCoord(new_point.sGLTexture,vu,vv);
 SetColorValue(new_point.sGLColor,(uint8_t)r,(uint8_t)g,(uint8_t)b);
}

//----------------------------------------------------------------------------------------------------
//выполнить коррекцию координат
//----------------------------------------------------------------------------------------------------
void CSGL::Clip(const SGLNVCTPoint *point_array_input,uint16_t point_amount_input,SGLNVCTPoint *point_array_output,uint16_t &point_amount_output,float nx,float ny,float nz,float w)
{
 point_amount_output=0; 
 for(uint16_t n=0;n<point_amount_input;n++)
 {
  long next_p=n+1;
  if (next_p>=point_amount_input) next_p-=point_amount_input;

  const SGLNVCTPoint *sGLNVCTPoint_Current_Ptr=&(point_array_input[n]);
  float current_vx=sGLNVCTPoint_Current_Ptr->sGLVertex.X;
  float current_vy=sGLNVCTPoint_Current_Ptr->sGLVertex.Y;
  float current_vz=sGLNVCTPoint_Current_Ptr->sGLVertex.Z;
  //определяем положение относительно плоскости отсечения
  float current_ret=current_vx*nx+current_vy*ny+current_vz*nz+w;

  const SGLNVCTPoint *sGLNVCTPoint_Next_Ptr=&(point_array_input[next_p]);
  float next_vx=sGLNVCTPoint_Next_Ptr->sGLVertex.X;
  float next_vy=sGLNVCTPoint_Next_Ptr->sGLVertex.Y;
  float next_vz=sGLNVCTPoint_Next_Ptr->sGLVertex.Z;
  //определяем положение относительно плоскости отсечения
  float next_ret=next_vx*nx+next_vy*ny+next_vz*nz+w;

  if (current_ret>0)//текущая точка видима
  {
   if (next_ret>0)//следующая точка видима
   {
    point_array_output[point_amount_output]=*sGLNVCTPoint_Next_Ptr;
	point_amount_output++;
   }
   else
   {
    //добавляем новую точку пересечения
    SGLNVCTPoint sGLNVCTPoint_New;
    GetIntersectionPlaneAndLine(*sGLNVCTPoint_Current_Ptr,*sGLNVCTPoint_Next_Ptr,sGLNVCTPoint_New,nx,ny,nz,w);
    point_array_output[point_amount_output]=sGLNVCTPoint_New;
	point_amount_output++;
   }
  }
  else//текущая точка не видна
  {
   if (next_ret>0)//следующая точка видна
   {
    //добавляем новую точку пересечения
    SGLNVCTPoint sGLNVCTPoint_New;
    GetIntersectionPlaneAndLine(*sGLNVCTPoint_Current_Ptr,*sGLNVCTPoint_Next_Ptr,sGLNVCTPoint_New,nx,ny,nz,w);
    point_array_output[point_amount_output]=sGLNVCTPoint_New;
	point_amount_output++;
    //добавляем сдедующую точку
    point_array_output[point_amount_output]=*sGLNVCTPoint_Next_Ptr;
	point_amount_output++;
   }
  }
 }
}

//----------------------------------------------------------------------------------------------------
//вывести треугольник
//----------------------------------------------------------------------------------------------------
void CSGL::OutputTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C)
{
 //выполняем отсечение геометрии для всех ограничивающих плоскостей 
 SGLNVCTPoint sGLNVCTPoint[2][3*2*4];//список хранимых вершин
 
 sGLNVCTPoint[0][0]=A;
 sGLNVCTPoint[0][1]=B;
 sGLNVCTPoint[0][2]=C;
 
 uint16_t point_amount=3;
 uint8_t input_index=0;
 uint8_t output_index=1;

 for(uint8_t n=0;n<FRUSTRUM_PLANE;n++)
 {
  float nx=FrustumPlane[n].X;
  float ny=FrustumPlane[n].Y;
  float nz=FrustumPlane[n].Z;
  float w=FrustumPlane[n].W;
  Clip(sGLNVCTPoint[input_index],point_amount,sGLNVCTPoint[output_index],point_amount,nx,ny,nz,w);
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
  DrawTriangle(sGLNVCTPoint[input_index][0],sGLNVCTPoint[input_index][i1],sGLNVCTPoint[input_index][i2]);
 } 
}
//----------------------------------------------------------------------------------------------------
//отрисовка треугольника
//----------------------------------------------------------------------------------------------------
void CSGL::DrawTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C)
{
 SGLVector4 sGLVector4_A;
 SGLVector4 sGLVector4_B;
 SGLVector4 sGLVector4_C;
	
 //вычислим проекции вершин треугольника
 sGLVector4_A.X=A.sGLVertex.X;
 sGLVector4_A.Y=A.sGLVertex.Y;
 sGLVector4_A.Z=A.sGLVertex.Z;
 sGLVector4_A.W=1;

 sGLVector4_B.X=B.sGLVertex.X;
 sGLVector4_B.Y=B.sGLVertex.Y;
 sGLVector4_B.Z=B.sGLVertex.Z;
 sGLVector4_B.W=1;

 sGLVector4_C.X=C.sGLVertex.X;
 sGLVector4_C.Y=C.sGLVertex.Y;
 sGLVector4_C.Z=C.sGLVertex.Z;
 sGLVector4_C.W=1;

 SGLVector4 sGLVector4_A_New;
 SGLVector4 sGLVector4_B_New;
 SGLVector4 sGLVector4_C_New;

 MultiplySGLMatrix4ToSGLVector4(ProjectionMatrix,sGLVector4_A,sGLVector4_A_New);
 MultiplySGLMatrix4ToSGLVector4(ProjectionMatrix,sGLVector4_B,sGLVector4_B_New);
 MultiplySGLMatrix4ToSGLVector4(ProjectionMatrix,sGLVector4_C,sGLVector4_C_New);

 SGLScreenPoint ap;
 SGLScreenPoint bp;
 SGLScreenPoint cp;

 ap.X=(int32_t)((sGLVector4_A_New.X/sGLVector4_A_New.W+1)*ViewPort.Z/2+ViewPort.X);
 ap.Y=(int32_t)((1-sGLVector4_A_New.Y/sGLVector4_A_New.W)*ViewPort.W/2+ViewPort.Y);

 bp.X=(int32_t)((sGLVector4_B_New.X/sGLVector4_B_New.W+1)*ViewPort.Z/2+ViewPort.X);
 bp.Y=(int32_t)((1-sGLVector4_B_New.Y/sGLVector4_B_New.W)*ViewPort.W/2+ViewPort.Y);

 cp.X=(int32_t)((sGLVector4_C_New.X/sGLVector4_C_New.W+1)*ViewPort.Z/2+ViewPort.X);
 cp.Y=(int32_t)((1-sGLVector4_C_New.Y/sGLVector4_C_New.W)*ViewPort.W/2+ViewPort.Y);

 RenderTriangle(A,B,C,ap,bp,cp);
}
//----------------------------------------------------------------------------------------------------
//растеризация треугольника на экране
//----------------------------------------------------------------------------------------------------
void CSGL::RenderTriangle(SGLNVCTPoint &a,SGLNVCTPoint &b,SGLNVCTPoint &c,SGLScreenPoint &ap,SGLScreenPoint &bp,SGLScreenPoint &cp)
{
 SGLNVCTPoint tmp_point;
 SGLScreenPoint tmp_screen;
 if (ap.Y>cp.Y)
 {
  tmp_point=a;
  a=c;
  c=tmp_point;

  tmp_screen=ap;
  ap=cp;
  cp=tmp_screen;
 }
 if (ap.Y>bp.Y)
 {
  tmp_point=a;
  a=b;
  b=tmp_point;

  tmp_screen=ap;
  ap=bp;
  bp=tmp_screen;
 }
 if (cp.Y<bp.Y)
 {
  tmp_point=c;
  c=b;
  b=tmp_point;

  tmp_screen=cp;
  cp=bp;
  bp=tmp_screen;
 }

 //проверим, попадает ли треугольник в порт просмотра
 int32_t starty=ap.Y;
 int32_t endy=cp.Y;
 if (starty==endy) return;//нечего рисовать
 if (starty>=ViewPort.Y+ViewPort.W) return;//треугольник не виден
 if (endy<ViewPort.X) return;//треугольник не виден
 if (starty<ViewPort.X) starty=ViewPort.X;
 if (endy>=ViewPort.Y+ViewPort.W) endy=ViewPort.Y+ViewPort.W-1;

 //смещаем позицию с учётом обрезания по видовому порту
 float lyca=(cp.Y-ap.Y);
 float lyba=(bp.Y-ap.Y);
 float offset=starty-ap.Y;
 //x
 float dcx1=(cp.X-ap.X)/lyca;
 float dcx2=(bp.X-ap.X)/lyba;
 float cx1=ap.X+offset*dcx1;
 float cx2=ap.X+offset*dcx2;
 //z
 float dcz1=(1.0f/c.sGLVertex.Z-1.0f/a.sGLVertex.Z)/lyca;
 float dcz2=(1.0f/b.sGLVertex.Z-1.0f/a.sGLVertex.Z)/lyba;
 float cz1=1.0f/a.sGLVertex.Z+offset*dcz1;
 float cz2=1.0f/a.sGLVertex.Z+offset*dcz2;
 //r
 float dcr1=(c.sGLColor.R-a.sGLColor.R)/lyca;
 float dcr2=(b.sGLColor.R-a.sGLColor.R)/lyba;
 float cr1=a.sGLColor.R+offset*dcr1;
 float cr2=a.sGLColor.R+offset*dcr2;
 //g
 float dcg1=(c.sGLColor.G-a.sGLColor.G)/lyca;
 float dcg2=(b.sGLColor.G-a.sGLColor.G)/lyba;
 float cg1=a.sGLColor.G+offset*dcg1;
 float cg2=a.sGLColor.G+offset*dcg2;
 //b
 float dcb1=(c.sGLColor.B-a.sGLColor.B)/lyca;
 float dcb2=(b.sGLColor.B-a.sGLColor.B)/lyba;
 float cb1=a.sGLColor.B+offset*dcb1;
 float cb2=a.sGLColor.B+offset*dcb2;

 //u
 float dcu1=(c.sGLTexture.U/c.sGLVertex.Z-a.sGLTexture.U/a.sGLVertex.Z)/lyca;
 float dcu2=(b.sGLTexture.U/b.sGLVertex.Z-a.sGLTexture.U/a.sGLVertex.Z)/lyba;
 float cu1=a.sGLTexture.U/a.sGLVertex.Z+offset*dcu1;
 float cu2=a.sGLTexture.U/a.sGLVertex.Z+offset*dcu2;

 //v
 float dcv1=(c.sGLTexture.V/c.sGLVertex.Z-a.sGLTexture.V/a.sGLVertex.Z)/lyca;
 float dcv2=(b.sGLTexture.V/b.sGLVertex.Z-a.sGLTexture.V/a.sGLVertex.Z)/lyba;
 float cv1=a.sGLTexture.V/a.sGLVertex.Z+offset*dcv1;
 float cv2=a.sGLTexture.V/a.sGLVertex.Z+offset*dcv2;

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

  if (sy<bp.Y)
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
   if (cp.Y==bp.Y)
   {
	x2=bp.X;
	z2=1.0f/b.sGLVertex.Z;

	r2=b.sGLColor.R;
	g2=b.sGLColor.G;
	b2=b.sGLColor.B;
		 
 	u2=b.sGLTexture.U/b.sGLVertex.Z;
    v2=b.sGLTexture.V/b.sGLVertex.Z;
   }
   else
   {
    if (first_half==true)
   	{
     float lycb=(cp.Y-bp.Y);
     float offset=sy-bp.Y;

	 dcx2=(cp.X-bp.X)/lycb;
	 dcz2=(1.0f/c.sGLVertex.Z-1.0f/b.sGLVertex.Z)/lycb;
     cx2=bp.X+offset*dcx2;
     cz2=1.0f/b.sGLVertex.Z+offset*dcz2;

     dcr2=(c.sGLColor.R-b.sGLColor.R)/lycb;
     cr2=b.sGLColor.R+offset*dcr2;

     dcg2=(c.sGLColor.G-b.sGLColor.G)/lycb;
     cg2=b.sGLColor.G+offset*dcg2;

     dcb2=(c.sGLColor.B-b.sGLColor.B)/lycb;
     cb2=b.sGLColor.B+offset*dcb2;
	
     dcu2=(c.sGLTexture.U/c.sGLVertex.Z-b.sGLTexture.U/b.sGLVertex.Z)/lycb;
     cu2=b.sGLTexture.U/b.sGLVertex.Z+offset*dcu2;

     dcv2=(c.sGLTexture.V/c.sGLVertex.Z-b.sGLTexture.V/b.sGLVertex.Z)/lycb;
     cv2=b.sGLTexture.V/b.sGLVertex.Z+offset*dcv2;
	
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
 u1*=sGLTextureObject_Current.Width;
 v1*=sGLTextureObject_Current.Height;

 u2*=sGLTextureObject_Current.Width;
 v2*=sGLTextureObject_Current.Height;

 uint32_t mask_texture_width=sGLTextureObject_Current.Width-1;
 uint32_t mask_texture_height=sGLTextureObject_Current.Height-1;

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
 if (x2>=ViewPort.X+ViewPort.Z) x2=ViewPort.X+ViewPort.Z-1;
 if (x1<ViewPort.X)
 {  
  float offset=(ViewPort.X-x1);
  x1=ViewPort.X;
  z1+=offset*dz;
  r1+=offset*dr;
  g1+=offset*dg;
  b1+=offset*db;
  u1+=offset*du;
  v1+=offset*dv;
 }
 SGLScreenColor *vptr=ImageMap+(x1+(ScreenHeight-y-1)*ScreenWidth);
 float *invdepthptr=InvZBuffer+x1+(int32_t)(ScreenHeight-y-1)*ScreenWidth;

 bool EnableDepthTest=true;

 if (EnableDepthTest==false)//тест глубины не производится
 {
  float z=z1;
  float r=r1;
  float g=g1;
  float b=b1;
  float u=u1;
  float v=v1;
  for(float x=x1;x<=x2;x++,z+=dz,r+=dr,g+=dg,b+=db,u+=du,v+=dv,vptr++,invdepthptr++)
  {
 
   vptr->R=(int8_t)(r*255.0)&0xff;
   vptr->G=(int8_t)(g*255.0)&0xff;
   vptr->B=(int8_t)(b*255.0)&0xff;
   
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

  //используем линейную интерполяцию текстуры через заданное количество пикселей
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

   if (*(invdepthptr)>z)//условие теста глубины выполняется
   {
    int32_t tui=static_cast<int32_t>(tu);
	int32_t tvi=static_cast<int32_t>(tv);

	if (tui<0)
	{
	 tui^=0xFFFFFFFF;
	 tui+=1;
	 tui=sGLTextureObject_Current.Width-tui;
	}
	if (tvi<0) 
	{
	 tvi^=0xFFFFFFFF;
	 tvi+=1;
	 tvi=sGLTextureObject_Current.Height-tvi;
	}

	tui&=mask_texture_width;
	tvi&=mask_texture_height;

	SGLRGBAByteColor &sGLRGBAByteColor=sGLTextureObject_Current.sGLRGBAByteColor_Ptr[tui+tvi*sGLTextureObject_Current.Width];

	uint8_t cr=(int8_t)(r*sGLRGBAByteColor.R);
	uint8_t cg=(int8_t)(g*sGLRGBAByteColor.G);
	uint8_t cb=(int8_t)(b*sGLRGBAByteColor.B);

	vptr->R=(int8_t)(cr)&0xff;
	vptr->G=(int8_t)(cg)&0xff;
	vptr->B=(int8_t)(cb)&0xff;		 

    *(invdepthptr)=z;
   }
  }
 }
}



//****************************************************************************************************
//открытые функции
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//инициализировать
//----------------------------------------------------------------------------------------------------
void CSGL::Init(uint32_t screen_width,uint32_t screen_height)
{
 ScreenWidth=screen_width;
 ScreenHeight=screen_height;
 ImageMap=new SGLScreenColor[screen_width*screen_height];
 InvZBuffer=new float[screen_width*screen_height];
}


//----------------------------------------------------------------------------------------------------
//сделать матрицу единичной
//----------------------------------------------------------------------------------------------------
void CSGL::LoadIdentity(void)
{
 sGLMatrix4_Ptr->X.X=1.0f; 
 sGLMatrix4_Ptr->X.Y=0;
 sGLMatrix4_Ptr->X.Z=0;
 sGLMatrix4_Ptr->X.W=0;	 
	 
 sGLMatrix4_Ptr->Y.X=0; 
 sGLMatrix4_Ptr->Y.Y=1.0f;
 sGLMatrix4_Ptr->Y.Z=0;
 sGLMatrix4_Ptr->Y.W=0;	 

 sGLMatrix4_Ptr->Z.X=0; 
 sGLMatrix4_Ptr->Z.Y=0;
 sGLMatrix4_Ptr->Z.Z=1.0f;
 sGLMatrix4_Ptr->Z.W=0;	 

 sGLMatrix4_Ptr->W.X=0; 
 sGLMatrix4_Ptr->W.Y=0;
 sGLMatrix4_Ptr->W.Z=0;
 sGLMatrix4_Ptr->W.W=1.0f;
}
//----------------------------------------------------------------------------------------------------
//умножить текущую матрицу на матрицу поворота вокруг вектора
//----------------------------------------------------------------------------------------------------
void CSGL::Rotatef(float angle,float nx,float ny,float nz)
{
 SGLVector4 vector;	
 vector.X=nx;
 vector.Y=ny;
 vector.Z=nz;
 vector.W=0;	
 NormaliseSGLVector4(vector);
 nx=vector.X;	
 ny=vector.Y;	
 nz=vector.Z;	
	
 float cf=cosf(PI/180.0f*angle);
 float sf=sinf(PI/180.0f*angle);

 SGLMatrix4	matrix;
 
 matrix.X.X=nx*nx+cf*(1.0f-nx*nx);
 matrix.X.Y=nx*(1.0f-cf)*ny+nz*sf;
 matrix.X.Z=nx*(1.0f-cf)*nz-ny*sf;
 matrix.X.W=0;

 matrix.Y.X=nx*(1.0f-cf)*ny-nz*sf;
 matrix.Y.Y=ny*ny+cf*(1.0f-ny*ny);
 matrix.Y.Z=ny*(1.0f-cf)*nz+nx*sf;
 matrix.Y.W=0;

 matrix.Z.X=nx*(1.0f-cf)*nz+ny*sf;
 matrix.Z.Y=ny*(1.0f-cf)*nz-nx*sf;
 matrix.Z.Z=nz*nz+cf*(1.0f-nz*nz);
 matrix.Z.W=0;

 matrix.W.X=0;
 matrix.W.Y=0;
 matrix.W.Z=0;
 matrix.W.W=1.0f;

 SGLMatrix4 tmp_matrix=*sGLMatrix4_Ptr;
 MultiplySGLMatrix4(matrix,tmp_matrix,*sGLMatrix4_Ptr);
}
//----------------------------------------------------------------------------------------------------
//умножить текущую матрицу на матрицу смещения
//----------------------------------------------------------------------------------------------------
void CSGL::Translatef(float nx,float ny,float nz)
{
 SGLMatrix4	matrix;
 matrix.X.X=1.0f;	
 matrix.X.Y=0;	
 matrix.X.Z=0;	
 matrix.X.W=0;	

 matrix.Y.X=0;	
 matrix.Y.Y=1.0f;	
 matrix.Y.Z=0;	
 matrix.Y.W=0;	

 matrix.Z.X=0;	
 matrix.Z.Y=0;	
 matrix.Z.Z=1.0f;	
 matrix.Z.W=0;	
	
 matrix.W.X=nx;	
 matrix.W.Y=ny;	
 matrix.W.Z=nz;	
 matrix.W.W=1.0f;	
	
 SGLMatrix4 tmp_matrix=*sGLMatrix4_Ptr;
 MultiplySGLMatrix4(matrix,tmp_matrix,*sGLMatrix4_Ptr);
}
//----------------------------------------------------------------------------------------------------
//выбрать матрицу
//----------------------------------------------------------------------------------------------------
void CSGL::MatrixMode(MATRIX_MODE matrix)
{
 if (matrix==SGL_MATRIX_MODELVIEW) sGLMatrix4_Ptr=&ModelViewMatrix;
 if (matrix==SGL_MATRIX_PROJECTION) sGLMatrix4_Ptr=&ProjectionMatrix;
 if (matrix==SGL_MATRIX_TEXTURE) sGLMatrix4_Ptr=&TextureMatrix;
}

//----------------------------------------------------------------------------------------------------
//задать плоскости отсечения в матрицу проецирования
//----------------------------------------------------------------------------------------------------
void CSGL::Frustrum(float left,float right,float bottom,float top,float near,float far)
{
 if (right==left) return;
 if (top==bottom) return;
 if (near==far) return;

 MatrixMode(SGL_MATRIX_PROJECTION);
 LoadIdentity();

 SGLMatrix4	matrix;
 matrix.X.X=2.0f*near/(right-left);
 matrix.X.Y=0;	
 matrix.X.Z=(right+left)/(right-left);	
 matrix.X.W=0;	

 matrix.Y.X=0;	
 matrix.Y.Y=2.0f*near/(top-bottom);	
 matrix.Y.Z=(top+bottom)/(top-bottom);	
 matrix.Y.W=0;	

 matrix.Z.X=0;	
 matrix.Z.Y=0;	
 matrix.Z.Z=-(far+near)/(far-near);	
 matrix.Z.W=-2.0f*far*near/(far-near);	
	
 matrix.W.X=0;	
 matrix.W.Y=0;	
 matrix.W.Z=-1.0f;	
 matrix.W.W=0;	

 SGLMatrix4 tmp_matrix=ProjectionMatrix;	
 MultiplySGLMatrix4(tmp_matrix,matrix,ProjectionMatrix);
}
//----------------------------------------------------------------------------------------------------
//задать видовой порт
//----------------------------------------------------------------------------------------------------
void CSGL::SetViewport(float x,float y,float len,float hgt)
{
 ViewPort.X=x;
 ViewPort.Y=y;
 ViewPort.Z=len;
 ViewPort.W=hgt;
}
//----------------------------------------------------------------------------------------------------
//задать матрицу проецирования
//----------------------------------------------------------------------------------------------------
void CSGL::Perspective(float fovy,float aspect,float near,float far)
{
 float top=tanf(PI/180.0f*fovy/2.0f)*near;
 float bottom=-top;
 float right=top*aspect;
 float left=-right;
 Frustrum(left,right,bottom,top,near,far);
}
//----------------------------------------------------------------------------------------------------
//задать цвет точки
//----------------------------------------------------------------------------------------------------
void CSGL::Color3f(float r,float g,float b)
{
 sGLNVCTPoint_Current.sGLColor.R=r;
 sGLNVCTPoint_Current.sGLColor.G=g;
 sGLNVCTPoint_Current.sGLColor.B=b;
}
//----------------------------------------------------------------------------------------------------
//задать текстурные координаты точки
//----------------------------------------------------------------------------------------------------
void CSGL::TexCoordf(float u,float v)
{
 //применяем матрицу текстурирования
 SGLVector4 vector_in;
 SGLVector4 vector_out;		
 vector_in.X=u;
 vector_in.Y=v;
 vector_in.Z=0;
 vector_in.W=1;	
 MultiplySGLVector4ToSGLMatrix4(vector_in,TextureMatrix,vector_out);

 sGLNVCTPoint_Current.sGLTexture.U=vector_out.X/vector_out.W;
 sGLNVCTPoint_Current.sGLTexture.V=vector_out.Y/vector_out.W;
}
//----------------------------------------------------------------------------------------------------
//задать нормаль в точке
//----------------------------------------------------------------------------------------------------
void CSGL::Normal3f(float nx,float ny,float nz)
{
 //применяем матрицу моделирования к нормали
 SGLVector4 vector_in;
 SGLVector4 vector_out;

 vector_in.X=nx;
 vector_in.Y=ny;
 vector_in.Z=nz;
 vector_in.W=1;

 MultiplySGLVector4ToSGLMatrix4(vector_in,ModelViewMatrix,vector_out);
 //вычитаем смещение
 sGLNVCTPoint_Current.sGLNormal.Nx=(vector_out.X-ModelViewMatrix.W.X)/vector_out.W;
 sGLNVCTPoint_Current.sGLNormal.Ny=(vector_out.Y-ModelViewMatrix.W.Y)/vector_out.W;
 sGLNVCTPoint_Current.sGLNormal.Nz=(vector_out.Z-ModelViewMatrix.W.Z)/vector_out.W;
}
//----------------------------------------------------------------------------------------------------
//начать рисование
//----------------------------------------------------------------------------------------------------
void CSGL::Begin(void)
{
 PointArrayAmount=0;
 DrawMode=true;
 CreateFrustrumPlane();
}
//----------------------------------------------------------------------------------------------------
//закончить рисование
//----------------------------------------------------------------------------------------------------
void CSGL::End(void)
{
 DrawMode=false;
}
//----------------------------------------------------------------------------------------------------
//задать координату точки
//----------------------------------------------------------------------------------------------------
void CSGL::Vertex3f(float x,float y,float z)
{
 if (DrawMode==false) return;//режим рисования не активирован 
 //выполним преобразование точек треугольника с учётом матрицы моделирования
 SGLVector4 vector_in;		
 SGLVector4 vector_out;	
	
 vector_in.X=x;
 vector_in.Y=y;
 vector_in.Z=z;
 vector_in.W=1.0f;
	
 MultiplySGLVector4ToSGLMatrix4(vector_in,ModelViewMatrix,vector_out);
 //занесём то, что получилось в список точек
 sGLNVCTPoint_Current.sGLVertex.X=vector_out.X/vector_out.W;
 sGLNVCTPoint_Current.sGLVertex.Y=vector_out.Y/vector_out.W;
 sGLNVCTPoint_Current.sGLVertex.Z=vector_out.Z/vector_out.W;
 sGLNVCTPointArray[PointArrayAmount]=sGLNVCTPoint_Current;
 PointArrayAmount++;
 //отрисовываем вершины
 if (PointArrayAmount==VERTEX_POINT_ARRAY)//уже есть точки на треугольник
 {
  OutputTriangle(sGLNVCTPointArray[0],sGLNVCTPointArray[1],sGLNVCTPointArray[2]);
  sGLNVCTPointArray[VERTEX_POINT_ARRAY-2]=sGLNVCTPointArray[VERTEX_POINT_ARRAY-1];//смещаем вершины
  PointArrayAmount=VERTEX_POINT_ARRAY-1;//отменяем последнюю вершину  
 }
}
//----------------------------------------------------------------------------------------------------
//задать цвет очистки фона
//----------------------------------------------------------------------------------------------------
void CSGL::ClearColor(float r,float g,float b)
{
 SetColorValue(sGLColor_Clear,r,g,b);
}
//----------------------------------------------------------------------------------------------------
//очистить буфер
//----------------------------------------------------------------------------------------------------
void CSGL::Clear(uint32_t mode)
{
 if (mode&SGL_COLOR_BUFFER_BIT)
 {  
  memset(ImageMap,0,ScreenWidth*ScreenHeight*sizeof(SGLScreenColor));
 }
 if (mode&SGL_DEPTH_BUFFER_BIT) 
 {
  for(uint32_t n=0;n<ScreenWidth*ScreenHeight;n++) InvZBuffer[n]=static_cast<float>(MIN_INV_Z_VALUE);
 }
}
//----------------------------------------------------------------------------------------------------
//задать текстуру
//----------------------------------------------------------------------------------------------------
void CSGL::BindTexture(uint32_t width,uint32_t height,SGLRGBAByteColor *sGLRGBAByteColor_Ptr_Set)
{
 sGLTextureObject_Current.Width=width;
 sGLTextureObject_Current.Height=height;
 sGLTextureObject_Current.sGLRGBAByteColor_Ptr=sGLRGBAByteColor_Ptr_Set;
}



//****************************************************************************************************
//статические функции
//****************************************************************************************************

//----------------------------------------------------------------------------------------------------
//задать координаты вершины
//----------------------------------------------------------------------------------------------------
void CSGL::SetVertexCoord(SGLVertex &sGLVertex,float x,float y,float z)
{
 sGLVertex.X=x;
 sGLVertex.Y=y;
 sGLVertex.Z=z;
}
//----------------------------------------------------------------------------------------------------
//задать координаты нормали
//----------------------------------------------------------------------------------------------------
void CSGL::SetNormalCoord(SGLNormal &sGLNormal,float nx,float ny,float nz)
{
 sGLNormal.Nx=nx;
 sGLNormal.Ny=ny;
 sGLNormal.Nz=nz;
}
//----------------------------------------------------------------------------------------------------
//задать координаты текстуры
//----------------------------------------------------------------------------------------------------
void CSGL::SetTextureCoord(SGLTexture &sGLTexture,float u,float v)
{
 sGLTexture.U=u;
 sGLTexture.V=v;
}
//----------------------------------------------------------------------------------------------------
//задать цвет
//----------------------------------------------------------------------------------------------------
void CSGL::SetColorValue(SGLColor &sGLColor,float r,float g,float b)
{
 sGLColor.R=r;
 sGLColor.G=g;
 sGLColor.B=b;
}
//----------------------------------------------------------------------------------------------------
//умножение вектора типа SGLVector4 на матрицу типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
void CSGL::MultiplySGLVector4ToSGLMatrix4(const SGLVector4& v,const SGLMatrix4& m,SGLVector4& v_out)
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
void CSGL::MultiplySGLMatrix4ToSGLVector4(const SGLMatrix4& m,const SGLVector4& v,SGLVector4& v_out)
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
void CSGL::MultiplySGLMatrix4(const SGLMatrix4& a,const SGLMatrix4& b,SGLMatrix4& out)
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
//нормирование вектора типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
void CSGL::NormaliseSGLVector4(SGLVector4& v)
{
 const float norma=sqrtf(v.X*v.X+v.Y*v.Y+v.Z*v.Z);
 const float scale=1.0f/norma;
 v.X*=scale;
 v.Y*=scale;
 v.Z*=scale;
 v.W*=scale;
}
//----------------------------------------------------------------------------------------------------
//вычислить определитель матрицы типа SGLMatrix4
//----------------------------------------------------------------------------------------------------
double CSGL::GetDeterminantSGLMatrix4(const SGLMatrix4& matrix)
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
double CSGL::GetDeterminantSGLMatrix3(const SGLMatrix3& matrix)
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
void CSGL::GetTruncatedMatrixSGLMatrix4(long y,long x,const SGLMatrix4& input_matrix,SGLMatrix3& output_matrix)
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
bool CSGL::CreateInvertMatrixSGLMatrix4(const SGLMatrix4& input_matrix,SGLMatrix4& output_matrix)
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


/*

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
 CurrentColor.R=0xff;
 CurrentColor.G=0xff;
 CurrentColor.B=0xff;
 CurrentSelectedMatrix=MATRIX_MODE_PROJECTION;
 LoadIdentity();
 CurrentSelectedMatrix=MATRIX_MODE_MODEL_VIEW;
 LoadIdentity();
 PointArrayAmount=0;
 DrawModeActive=false;
 EnableDepthText=false;
 CurrentTexture.U=0;
 CurrentTexture.V=0;
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
 ImageMap=new SGLScreenColor[screen_width*screen_height+1];
 InvZBuffer=new float[screen_width*screen_height+1];
 ScreenWidth=screen_width;
 ScreenHeight=screen_height;
 return(true);
}
bool CSGL::LoadIdentity(void)
{
 if (CurrentSelectedMatrix==MATRIX_MODE_PROJECTION)
 {
  ProjectionMatrix.X.X=1; 
  ProjectionMatrix.X.Y=0;
  ProjectionMatrix.X.Z=0;
  ProjectionMatrix.X.W=0;	 
	 
  ProjectionMatrix.Y.X=0; 
  ProjectionMatrix.Y.Y=1;
  ProjectionMatrix.Y.Z=0;
  ProjectionMatrix.Y.W=0;	 

  ProjectionMatrix.Z.X=0; 
  ProjectionMatrix.Z.Y=0;
  ProjectionMatrix.Z.Z=1;
  ProjectionMatrix.Z.W=0;	 

  ProjectionMatrix.W.X=0; 
  ProjectionMatrix.W.Y=0;
  ProjectionMatrix.W.Z=0;
  ProjectionMatrix.W.W=1;	 
 }
 if (CurrentSelectedMatrix==MATRIX_MODE_MODEL_VIEW)
 {
  ModelViewMatrix.X.X=1; 
  ModelViewMatrix.X.Y=0;
  ModelViewMatrix.X.Z=0;
  ModelViewMatrix.X.W=0;	 
	 
  ModelViewMatrix.Y.X=0; 
  ModelViewMatrix.Y.Y=1;
  ModelViewMatrix.Y.Z=0;
  ModelViewMatrix.Y.W=0;	 

  ModelViewMatrix.Z.X=0; 
  ModelViewMatrix.Z.Y=0;
  ModelViewMatrix.Z.Z=1;
  ModelViewMatrix.Z.W=0;	 

  ModelViewMatrix.W.X=0; 
  ModelViewMatrix.W.Y=0;
  ModelViewMatrix.W.Z=0;
  ModelViewMatrix.W.W=1;	 
 }
 CreateFrustrumPlane();
 return(true);
}
bool CSGL::Frustrum(float left,float right,float bottom,float top,float near,float far)
{
 if (right==left) return(false);
 if (top==bottom) return(false);
 if (near==far) return(false);
	
 SGLMatrix4	matrix;
 matrix.X.X=2*near/(right-left);
 matrix.X.Y=0;	
 matrix.X.Z=(right+left)/(right-left);	
 matrix.X.W=0;	

 matrix.Y.X=0;	
 matrix.Y.Y=2*near/(top-bottom);	
 matrix.Y.Z=(top+bottom)/(top-bottom);	
 matrix.Y.W=0;	

 matrix.Z.X=0;	
 matrix.Z.Y=0;	
 matrix.Z.Z=-(far+near)/(far-near);	
 matrix.Z.W=-2*far*near/(far-near);	
	
 matrix.W.X=0;	
 matrix.W.Y=0;	
 matrix.W.Z=-1;	
 matrix.W.W=0;	
	
 SGLMatrix4 tmp_matrix=ProjectionMatrix;	
 MultiplySGLMatrix4(tmp_matrix,matrix,ProjectionMatrix);
 CreateFrustrumPlane();
 return(true);
}
bool CSGL::SetViewport(float x,float y,float len,float hgt)
{  
 ViewPort.X=x;
 ViewPort.Y=y;
 ViewPort.Z=len;
 ViewPort.W=hgt;
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
 SGLVector4 vector;	
 vector.X=nx;
 vector.Y=ny;
 vector.Z=nz;
 vector.W=0;	
 NormaliseSGLVector4(vector);
 nx=vector.X;	
 ny=vector.Y;	
 nz=vector.Z;	
	
 float cf=(float)cos(M_PI/180*angle);
 float sf=(float)sin(M_PI/180*angle);
 SGLMatrix4	matrix;
 
 matrix.X.X=nx*nx+cf*(1-nx*nx);
 matrix.X.Y=nx*(1-cf)*ny+nz*sf;
 matrix.X.Z=nx*(1-cf)*nz-ny*sf;
 matrix.X.W=0;

 matrix.Y.X=nx*(1-cf)*ny-nz*sf;
 matrix.Y.Y=ny*ny+cf*(1-ny*ny);
 matrix.Y.Z=ny*(1-cf)*nz+nx*sf;
 matrix.Y.W=0;

 matrix.Z.X=nx*(1-cf)*nz+ny*sf;
 matrix.Z.Y=ny*(1-cf)*nz-nx*sf;
 matrix.Z.Z=nz*nz+cf*(1-nz*nz);
 matrix.Z.W=0;

 matrix.W.X=0;
 matrix.W.Y=0;
 matrix.W.Z=0;
 matrix.W.W=1;

 if (CurrentSelectedMatrix==MATRIX_MODE_PROJECTION)
 {
  SGLMatrix4 tmp_matrix=ProjectionMatrix;
  MultiplySGLMatrix4(matrix,tmp_matrix,ProjectionMatrix);
 }
 if (CurrentSelectedMatrix==MATRIX_MODE_MODEL_VIEW) 
 {
  SGLMatrix4 tmp_matrix=ModelViewMatrix;		 
  MultiplySGLMatrix4(matrix,tmp_matrix,ModelViewMatrix);
 }
 CreateFrustrumPlane();
 return(true);
}
bool CSGL::Translatef(float nx,float ny,float nz)
{
 SGLMatrix4	matrix;
 matrix.X.X=1;	
 matrix.X.Y=0;	
 matrix.X.Z=0;	
 matrix.X.W=0;	

 matrix.Y.X=0;	
 matrix.Y.Y=1;	
 matrix.Y.Z=0;	
 matrix.Y.W=0;	

 matrix.Z.X=0;	
 matrix.Z.Y=0;	
 matrix.Z.Z=1;	
 matrix.Z.W=0;	
	
 matrix.W.X=nx;	
 matrix.W.Y=ny;	
 matrix.W.Z=nz;	
 matrix.W.W=1;	
	
 if (CurrentSelectedMatrix==MATRIX_MODE_PROJECTION) 
 {
  SGLMatrix4 tmp_matrix=ProjectionMatrix;
  MultiplySGLMatrix4(matrix,tmp_matrix,ProjectionMatrix);
 }
 if (CurrentSelectedMatrix==MATRIX_MODE_MODEL_VIEW) 
 {
  SGLMatrix4 tmp_matrix=ModelViewMatrix;		 
  MultiplySGLMatrix4(matrix,tmp_matrix,ModelViewMatrix);
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
 CurrentColor.R=r;
 CurrentColor.G=g;
 CurrentColor.B=b;
 return(true);
}
bool CSGL::TexCoord(float u,float v)
{
 CurrentTexture.U=u;	
 CurrentTexture.V=v;
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
 SGLVector4 vector_in;		
 SGLVector4 vector_out;	
	
 vector_in.X=x;
 vector_in.Y=y;
 vector_in.Z=z;
 vector_in.W=1;
	
 MultiplySGLVector4ToSGLMatrix4(vector_in,ModelViewMatrix,vector_out);
 //занесём то, что получилось в список
 sGLNVCTPointArray[PointArrayAmount].sGLVertex.X=vector_out.X/vector_out.W;
 sGLNVCTPointArray[PointArrayAmount].sGLVertex.Y=vector_out.Y/vector_out.W;
 sGLNVCTPointArray[PointArrayAmount].sGLVertex.Z=vector_out.Z/vector_out.W;
 sGLNVCTPointArray[PointArrayAmount].sGLColor=CurrentColor;
 sGLNVCTPointArray[PointArrayAmount].sGLTexture=CurrentTexture;

 PointArrayAmount++;

 //отрисовываем вершины
 if (PointArrayAmount==3)//уже есть точки на треугольник
 {  
  OutputTriangle(sGLNVCTPointArray[0],sGLNVCTPointArray[1],sGLNVCTPointArray[2]);
  sGLNVCTPointArray[1]=sGLNVCTPointArray[2];//смещаем вершины
  PointArrayAmount=2;//отменяем последнюю вершину  
 }
 return(true);
}
bool CSGL::Clear(unsigned int mode)
{
 if (mode&SGL_COLOR_BUFFER_BIT && ImageMap!=NULL) memset(ImageMap,0,ScreenWidth*ScreenHeight*sizeof(SGLScreenColor));
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
//вычислить плоскости отсечения
//----------------------------------------------------------------------------------------------------
void CSGL::CreateFrustrumPlane(void)
{
 //вычисляем общую матрицу modelview-projection
 //ВНИМАНИЕ!!!
 //так как координаты в буфере (Vertex3f) уже переведены с учётом матрицы моделирования, строить плоскости нужно только по матрице проецирования!
 //ВНИМАНИЕ!!!

 SGLMatrix4 projection_model_view_matrix=ProjectionMatrix;
 //MultiplySGLMatrix4(ModelViewMatrix,ProjectionMatrix,projection_model_view_matrix);
 //вычисляем четыре плоскости отсечения по проекции (верх, низ, лево, право)
 //левая
 FrustumPlane[0].X=projection_model_view_matrix.X.W+projection_model_view_matrix.X.X;
 FrustumPlane[0].Y=projection_model_view_matrix.Y.W+projection_model_view_matrix.Y.X;
 FrustumPlane[0].Z=projection_model_view_matrix.Z.W+projection_model_view_matrix.Z.X;
 FrustumPlane[0].W=projection_model_view_matrix.W.W+projection_model_view_matrix.W.X;
 NormaliseSGLVector4(FrustumPlane[0]);
 //правая
 FrustumPlane[1].X=projection_model_view_matrix.X.W-projection_model_view_matrix.X.X;
 FrustumPlane[1].Y=projection_model_view_matrix.Y.W-projection_model_view_matrix.Y.X;
 FrustumPlane[1].Z=projection_model_view_matrix.Z.W-projection_model_view_matrix.Z.X;
 FrustumPlane[1].W=projection_model_view_matrix.W.W-projection_model_view_matrix.W.X;
 NormaliseSGLVector4(FrustumPlane[1]);
 //верхняя
 FrustumPlane[2].X=projection_model_view_matrix.X.W-projection_model_view_matrix.X.Y;
 FrustumPlane[2].Y=projection_model_view_matrix.Y.W-projection_model_view_matrix.Y.Y;
 FrustumPlane[2].Z=projection_model_view_matrix.Z.W-projection_model_view_matrix.Z.Y;
 FrustumPlane[2].W=projection_model_view_matrix.W.W-projection_model_view_matrix.W.Y;
 NormaliseSGLVector4(FrustumPlane[2]);
 //нижняя
 FrustumPlane[3].X=projection_model_view_matrix.X.W+projection_model_view_matrix.X.Y;
 FrustumPlane[3].Y=projection_model_view_matrix.Y.W+projection_model_view_matrix.Y.Y;
 FrustumPlane[3].Z=projection_model_view_matrix.Z.W+projection_model_view_matrix.Z.Y;
 FrustumPlane[3].W=projection_model_view_matrix.W.W+projection_model_view_matrix.W.Y;
 NormaliseSGLVector4(FrustumPlane[3]);
 //передняя
 FrustumPlane[4].X=projection_model_view_matrix.X.W+projection_model_view_matrix.X.Z;
 FrustumPlane[4].Y=projection_model_view_matrix.Y.W+projection_model_view_matrix.Y.Z;
 FrustumPlane[4].Z=projection_model_view_matrix.Z.W+projection_model_view_matrix.Z.Z;
 FrustumPlane[4].W=projection_model_view_matrix.W.W+projection_model_view_matrix.W.Z;
 NormaliseSGLVector4(FrustumPlane[4]);
 
 //задняя
 FrustumPlane[5].X=projection_model_view_matrix.X.W-projection_model_view_matrix.X.Z;
 FrustumPlane[5].Y=projection_model_view_matrix.Y.W-projection_model_view_matrix.Y.Z;
 FrustumPlane[5].Z=projection_model_view_matrix.Z.W-projection_model_view_matrix.Z.Z;
 FrustumPlane[5].W=projection_model_view_matrix.W.W-projection_model_view_matrix.W.Z;
 NormaliseSGLVector4(FrustumPlane[5]);
 
}
//----------------------------------------------------------------------------------------------------
//получить точку пересечения прямой и плоскости
//----------------------------------------------------------------------------------------------------
void CSGL::GetIntersectionPlaneAndLine(const SGLNVCTPoint& A,const SGLNVCTPoint& B,SGLNVCTPoint& new_point,float nx,float ny,float nz,float w)
{
 new_point=A;
 float ax=A.sGLVertex.X;
 float ay=A.sGLVertex.Y;
 float az=A.sGLVertex.Z;
 float au=A.sGLTexture.U;
 float av=A.sGLTexture.V;
 float ar=A.sGLColor.R;
 float ag=A.sGLColor.G;
 float ab=A.sGLColor.B;

 float bx=B.sGLVertex.X;
 float by=B.sGLVertex.Y;
 float bz=B.sGLVertex.Z;
 float bu=B.sGLTexture.U;
 float bv=B.sGLTexture.V;
 float br=B.sGLColor.R;
 float bg=B.sGLColor.G;
 float bb=B.sGLColor.B;

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
 SetVertexCoord(new_point.sGLVertex,vx,vy,vz);
 SetTextureCoord(new_point.sGLTexture,vu,vv);
 SetColorValue(new_point.sGLColor,(uint8_t)r,(uint8_t)g,(uint8_t)b);
}

//----------------------------------------------------------------------------------------------------
//выполнить коррекцию координат
//----------------------------------------------------------------------------------------------------
void CSGL::Clip(const SGLNVCTPoint *point_array_input,uint16_t point_amount_input,SGLNVCTPoint *point_array_output,uint16_t &point_amount_output,float nx,float ny,float nz,float w)
{
 point_amount_output=0; 
 for(uint16_t n=0;n<point_amount_input;n++)
 {
  long next_p=n+1;
  if (next_p>=point_amount_input) next_p-=point_amount_input;

  const SGLNVCTPoint *sGLNVCTPoint_Current_Ptr=&(point_array_input[n]);
  float current_vx=sGLNVCTPoint_Current_Ptr->sGLVertex.X;
  float current_vy=sGLNVCTPoint_Current_Ptr->sGLVertex.Y;
  float current_vz=sGLNVCTPoint_Current_Ptr->sGLVertex.Z;
  //определяем положение относительно плоскости отсечения
  float current_ret=current_vx*nx+current_vy*ny+current_vz*nz+w;

  const SGLNVCTPoint *sGLNVCTPoint_Next_Ptr=&(point_array_input[next_p]);
  float next_vx=sGLNVCTPoint_Next_Ptr->sGLVertex.X;
  float next_vy=sGLNVCTPoint_Next_Ptr->sGLVertex.Y;
  float next_vz=sGLNVCTPoint_Next_Ptr->sGLVertex.Z;
  //определяем положение относительно плоскости отсечения
  float next_ret=next_vx*nx+next_vy*ny+next_vz*nz+w;

  if (current_ret>0)//текущая точка видима
  {
   if (next_ret>0)//следующая точка видима
   {
    point_array_output[point_amount_output]=*sGLNVCTPoint_Next_Ptr;
	point_amount_output++;
   }
   else
   {
    //добавляем новую точку пересечения
    SGLNVCTPoint sGLNVCTPoint_New;
    GetIntersectionPlaneAndLine(*sGLNVCTPoint_Current_Ptr,*sGLNVCTPoint_Next_Ptr,sGLNVCTPoint_New,nx,ny,nz,w);
    point_array_output[point_amount_output]=sGLNVCTPoint_New;
	point_amount_output++;
   }
  }
  else//текущая точка не видна
  {
   if (next_ret>0)//следующая точка видна
   {
    //добавляем новую точку пересечения
    SGLNVCTPoint sGLNVCTPoint_New;
    GetIntersectionPlaneAndLine(*sGLNVCTPoint_Current_Ptr,*sGLNVCTPoint_Next_Ptr,sGLNVCTPoint_New,nx,ny,nz,w);
    point_array_output[point_amount_output]=sGLNVCTPoint_New;
	point_amount_output++;
    //добавляем сдедующую точку
    point_array_output[point_amount_output]=*sGLNVCTPoint_Next_Ptr;
	point_amount_output++;
   }
  }
 }
}

//----------------------------------------------------------------------------------------------------
//вывести треугольник
//----------------------------------------------------------------------------------------------------
void CSGL::OutputTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C)
{
 //выполняем отсечение геометрии для всех ограничивающих плоскостей 
 SGLNVCTPoint sGLNVCTPoint[2][3*2*4];//список хранимых вершин
 
 sGLNVCTPoint[0][0]=A;
 sGLNVCTPoint[0][1]=B;
 sGLNVCTPoint[0][2]=C;
 
 uint16_t point_amount=3;
 uint8_t input_index=0;
 uint8_t output_index=1;
 
 for(uint8_t n=0;n<5;n++)
 {
  float nx=FrustumPlane[n].X;
  float ny=FrustumPlane[n].Y;
  float nz=FrustumPlane[n].Z;
  float w=FrustumPlane[n].W;
  Clip(sGLNVCTPoint[input_index],point_amount,sGLNVCTPoint[output_index],point_amount,nx,ny,nz,w);
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
  DrawTriangle(sGLNVCTPoint[input_index][0],sGLNVCTPoint[input_index][i1],sGLNVCTPoint[input_index][i2]);
 } 
}
//----------------------------------------------------------------------------------------------------
//отрисовка треугольника
//----------------------------------------------------------------------------------------------------
void CSGL::DrawTriangle(SGLNVCTPoint A,SGLNVCTPoint B,SGLNVCTPoint C)
{
 SGLVector4 sGLVector4_A;
 SGLVector4 sGLVector4_B;
 SGLVector4 sGLVector4_C;
	
 //вычислим проекции вершин треугольника
 sGLVector4_A.X=A.sGLVertex.X;
 sGLVector4_A.Y=A.sGLVertex.Y;
 sGLVector4_A.Z=A.sGLVertex.Z;
 sGLVector4_A.W=1;

 sGLVector4_B.X=B.sGLVertex.X;
 sGLVector4_B.Y=B.sGLVertex.Y;
 sGLVector4_B.Z=B.sGLVertex.Z;
 sGLVector4_B.W=1;

 sGLVector4_C.X=C.sGLVertex.X;
 sGLVector4_C.Y=C.sGLVertex.Y;
 sGLVector4_C.Z=C.sGLVertex.Z;
 sGLVector4_C.W=1;

 SGLVector4 sGLVector4_A_New;
 SGLVector4 sGLVector4_B_New;
 SGLVector4 sGLVector4_C_New;

 MultiplySGLMatrix4ToSGLVector4(ProjectionMatrix,sGLVector4_A,sGLVector4_A_New);
 MultiplySGLMatrix4ToSGLVector4(ProjectionMatrix,sGLVector4_B,sGLVector4_B_New);
 MultiplySGLMatrix4ToSGLVector4(ProjectionMatrix,sGLVector4_C,sGLVector4_C_New);

 SGLScreenPoint ap;
 SGLScreenPoint bp;
 SGLScreenPoint cp;

 ap.X=(int32_t)((sGLVector4_A_New.X/sGLVector4_A_New.W+1)*ViewPort.Z/2+ViewPort.X);
 ap.Y=(int32_t)((1-sGLVector4_A_New.Y/sGLVector4_A_New.W)*ViewPort.W/2+ViewPort.Y);

 bp.X=(int32_t)((sGLVector4_B_New.X/sGLVector4_B_New.W+1)*ViewPort.Z/2+ViewPort.X);
 bp.Y=(int32_t)((1-sGLVector4_B_New.Y/sGLVector4_B_New.W)*ViewPort.W/2+ViewPort.Y);

 cp.X=(int32_t)((sGLVector4_C_New.X/sGLVector4_C_New.W+1)*ViewPort.Z/2+ViewPort.X);
 cp.Y=(int32_t)((1-sGLVector4_C_New.Y/sGLVector4_C_New.W)*ViewPort.W/2+ViewPort.Y);

 RenderTriangle(A,B,C,ap,bp,cp);
}
//----------------------------------------------------------------------------------------------------
//растеризация треугольника на экране
//----------------------------------------------------------------------------------------------------
void CSGL::RenderTriangle(SGLNVCTPoint &a,SGLNVCTPoint &b,SGLNVCTPoint &c,SGLScreenPoint &ap,SGLScreenPoint &bp,SGLScreenPoint &cp)
{
 SGLNVCTPoint tmp_point;
 SGLScreenPoint tmp_screen;
 if (ap.Y>cp.Y)
 {
  tmp_point=a;
  a=c;
  c=tmp_point;

  tmp_screen=ap;
  ap=cp;
  cp=tmp_screen;
 }
 if (ap.Y>bp.Y)
 {
  tmp_point=a;
  a=b;
  b=tmp_point;

  tmp_screen=ap;
  ap=bp;
  bp=tmp_screen;
 }
 if (cp.Y<bp.Y)
 {
  tmp_point=c;
  c=b;
  b=tmp_point;

  tmp_screen=cp;
  cp=bp;
  bp=tmp_screen;
 }

 //проверим, попадает ли треугольник в порт просмотра
 int32_t starty=ap.Y;
 int32_t endy=cp.Y;
 if (starty==endy) return;//нечего рисовать
 if (starty>=ViewPort.Y+ViewPort.W) return;//треугольник не виден
 if (endy<ViewPort.X) return;//треугольник не виден
 if (starty<ViewPort.X) starty=ViewPort.X;
 if (endy>=ViewPort.Y+ViewPort.W) endy=ViewPort.Y+ViewPort.W-1;

 //смещаем позицию с учётом обрезания по видовому порту
 float lyca=(cp.Y-ap.Y);
 float lyba=(bp.Y-ap.Y);
 float offset=starty-ap.Y;
 //x
 float dcx1=(cp.X-ap.X)/lyca;
 float dcx2=(bp.X-ap.X)/lyba;
 float cx1=ap.X+offset*dcx1;
 float cx2=ap.X+offset*dcx2;
 //z
 float dcz1=(1.0f/c.sGLVertex.Z-1.0f/a.sGLVertex.Z)/lyca;
 float dcz2=(1.0f/b.sGLVertex.Z-1.0f/a.sGLVertex.Z)/lyba;
 float cz1=1.0f/a.sGLVertex.Z+offset*dcz1;
 float cz2=1.0f/a.sGLVertex.Z+offset*dcz2;
 //r
 float dcr1=(c.sGLColor.R-a.sGLColor.R)/lyca;
 float dcr2=(b.sGLColor.R-a.sGLColor.R)/lyba;
 float cr1=a.sGLColor.R+offset*dcr1;
 float cr2=a.sGLColor.R+offset*dcr2;
 //g
 float dcg1=(c.sGLColor.G-a.sGLColor.G)/lyca;
 float dcg2=(b.sGLColor.G-a.sGLColor.G)/lyba;
 float cg1=a.sGLColor.G+offset*dcg1;
 float cg2=a.sGLColor.G+offset*dcg2;
 //b
 float dcb1=(c.sGLColor.B-a.sGLColor.B)/lyca;
 float dcb2=(b.sGLColor.B-a.sGLColor.B)/lyba;
 float cb1=a.sGLColor.B+offset*dcb1;
 float cb2=a.sGLColor.B+offset*dcb2;

 //u
 float dcu1=(c.sGLTexture.U/c.sGLVertex.Z-a.sGLTexture.U/a.sGLVertex.Z)/lyca;
 float dcu2=(b.sGLTexture.U/b.sGLVertex.Z-a.sGLTexture.U/a.sGLVertex.Z)/lyba;
 float cu1=a.sGLTexture.U/a.sGLVertex.Z+offset*dcu1;
 float cu2=a.sGLTexture.U/a.sGLVertex.Z+offset*dcu2;

 //v
 float dcv1=(c.sGLTexture.V/c.sGLVertex.Z-a.sGLTexture.V/a.sGLVertex.Z)/lyca;
 float dcv2=(b.sGLTexture.V/b.sGLVertex.Z-a.sGLTexture.V/a.sGLVertex.Z)/lyba;
 float cv1=a.sGLTexture.V/a.sGLVertex.Z+offset*dcv1;
 float cv2=a.sGLTexture.V/a.sGLVertex.Z+offset*dcv2;

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

  if (sy<bp.Y)
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
   if (cp.Y==bp.Y)
   {
	x2=bp.X;
	z2=1.0f/b.sGLVertex.Z;

	r2=b.sGLColor.R;
	g2=b.sGLColor.G;
	b2=b.sGLColor.B;
		 
 	u2=b.sGLTexture.U/b.sGLVertex.Z;
    v2=b.sGLTexture.V/b.sGLVertex.Z;
   }
   else
   {
    if (first_half==true)
   	{
     float lycb=(cp.Y-bp.Y);
     float offset=sy-bp.Y;

	 dcx2=(cp.X-bp.X)/lycb;
	 dcz2=(1.0f/c.sGLVertex.Z-1.0f/b.sGLVertex.Z)/lycb;
     cx2=bp.X+offset*dcx2;
     cz2=1.0f/b.sGLVertex.Z+offset*dcz2;

     dcr2=(c.sGLColor.R-b.sGLColor.R)/lycb;
     cr2=b.sGLColor.R+offset*dcr2;

     dcg2=(c.sGLColor.G-b.sGLColor.G)/lycb;
     cg2=b.sGLColor.G+offset*dcg2;

     dcb2=(c.sGLColor.B-b.sGLColor.B)/lycb;
     cb2=b.sGLColor.B+offset*dcb2;
	
     dcu2=(c.sGLTexture.U/c.sGLVertex.Z-b.sGLTexture.U/b.sGLVertex.Z)/lycb;
     cu2=b.sGLTexture.U/b.sGLVertex.Z+offset*dcu2;

     dcv2=(c.sGLTexture.V/c.sGLVertex.Z-b.sGLTexture.V/b.sGLVertex.Z)/lycb;
     cv2=b.sGLTexture.V/b.sGLVertex.Z+offset*dcv2;
	
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
 if (x2>=ViewPort.X+ViewPort.Z) x2=ViewPort.X+ViewPort.Z-1;
 if (x1<ViewPort.X)
 {  
  float offset=(ViewPort.X-x1);
  x1=ViewPort.X;
  z1+=offset*dz;
  r1+=offset*dr;
  g1+=offset*dg;
  b1+=offset*db;
  u1+=offset*du;
  v1+=offset*dv;
 }
 SGLScreenColor *vptr=ImageMap+(x1+(ScreenHeight-y-1)*ScreenWidth);
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

  //используем линейную интерполяцию текстуры через заданное количество пикселей
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
*/