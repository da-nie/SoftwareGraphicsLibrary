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
 float param[3]={0,0,0};
 Materialfv(SGL_AMBIENT,param);
 Materialfv(SGL_DIFFUSE,param);
 Materialfv(SGL_SPECULAR,param);
 Materialfv(SGL_EMISSION,param);
 Materialfv(SGL_SHININESS,param);

 Disable(SGL_LIGHTING);
 for(uint32_t n=0;n<LIGHT_AMOUNT;n++)
 {
  ITEM_ID id=static_cast<ITEM_ID>(SGL_LIGHT0+n);
  Disable(id);
  Lightfv(id,SGL_POSITION,param);
  Lightfv(id,SGL_AMBIENT,param);
  Lightfv(id,SGL_DIFFUSE,param);
  Lightfv(id,SGL_SPECULAR,param);
  Lightfv(id,SGL_SHININESS,param);
  float v=1;
  Lightfv(id,SGL_CONSTANT_ATTENUATION,&v);
  v=0;
  Lightfv(id,SGL_LINEAR_ATTENUATION,&v);
  Lightfv(id,SGL_QUADRATIC_ATTENUATION,&v);
 }
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
//вычислить цвет точки по источникам света
//----------------------------------------------------------------------------------------------------
void CSGL::CreateLighColor(SGLNVCTPoint &sGLNVCTPoint)
{ 
 sGLNVCTPoint.sGLColor.R=sGLNVCTPoint.sGLMaterial.SGLColor_Emission.R;
 sGLNVCTPoint.sGLColor.G=sGLNVCTPoint.sGLMaterial.SGLColor_Emission.G;
 sGLNVCTPoint.sGLColor.B=sGLNVCTPoint.sGLMaterial.SGLColor_Emission.B;
 for(uint32_t n=0;n<LIGHT_AMOUNT;n++)
 {
  SLight *sLight_Ptr=&sLight[n];
  if (sLight_Ptr->Enabled==false) continue;
  //вычислим расстояние между источником и точкой
  SGLVector4 sGLVector4_Distance;
  sGLVector4_Distance.X=sLight_Ptr->sGLVertex.X-sGLNVCTPoint.sGLVertex.X;
  sGLVector4_Distance.Y=sLight_Ptr->sGLVertex.Y-sGLNVCTPoint.sGLVertex.Y;
  sGLVector4_Distance.Z=sLight_Ptr->sGLVertex.Z-sGLNVCTPoint.sGLVertex.Z;
  sGLVector4_Distance.W=1.0f;
  float dist=GetNorma(sGLVector4_Distance);
  NormaliseSGLVector4(sGLVector4_Distance);
  //определим затухание от расстояния
  float k=sLight_Ptr->ConstantAttenuation+sLight_Ptr->LinearAttenuation*dist+sLight_Ptr->QuadraticAttenuation*dist*dist;
  k=1.0f/k;
  //определим скалярное произведение между нормалью к точке и вектором к источнику света
  float scalar=sGLVector4_Distance.X*sGLNVCTPoint.sGLNormal.Nx+sGLVector4_Distance.Y*sGLNVCTPoint.sGLNormal.Ny+sGLVector4_Distance.Z*sGLNVCTPoint.sGLNormal.Nz;
  if (scalar<0) scalar=-scalar;//вершина с другой стороны от источника света, но мы считаем одинаково (так удобнее) 
  float scalar_sh=pow(scalar,sLight_Ptr->Shininess);
  //вычисляем освещённость
  SGLColor sGLColor_Local;
  float ar=sGLNVCTPoint.sGLMaterial.SGLColor_Ambient.R*sLight_Ptr->SGLColor_Ambient.R;
  float dr=sGLNVCTPoint.sGLMaterial.SGLColor_Diffuse.R*sLight_Ptr->SGLColor_Diffuse.R;
  float sr=sGLNVCTPoint.sGLMaterial.SGLColor_Specular.R*sLight_Ptr->SGLColor_Specular.R;

  float ag=sGLNVCTPoint.sGLMaterial.SGLColor_Ambient.G*sLight_Ptr->SGLColor_Ambient.G;
  float dg=sGLNVCTPoint.sGLMaterial.SGLColor_Diffuse.G*sLight_Ptr->SGLColor_Diffuse.G;
  float sg=sGLNVCTPoint.sGLMaterial.SGLColor_Specular.G*sLight_Ptr->SGLColor_Specular.G;

  float ab=sGLNVCTPoint.sGLMaterial.SGLColor_Ambient.B*sLight_Ptr->SGLColor_Ambient.B;
  float db=sGLNVCTPoint.sGLMaterial.SGLColor_Diffuse.B*sLight_Ptr->SGLColor_Diffuse.B;
  float sb=sGLNVCTPoint.sGLMaterial.SGLColor_Specular.B*sLight_Ptr->SGLColor_Specular.B;

  sGLColor_Local.R=(ar+dr*scalar+sr*scalar_sh)*k;
  sGLColor_Local.G=(ag+dg*scalar+sg*scalar_sh)*k;
  sGLColor_Local.B=(ab+db*scalar+sb*scalar_sh)*k;

  sGLNVCTPoint.sGLColor.R+=sGLColor_Local.R;
  sGLNVCTPoint.sGLColor.G+=sGLColor_Local.G;
  sGLNVCTPoint.sGLColor.B+=sGLColor_Local.B;
 }
 if (sGLNVCTPoint.sGLColor.R>1) sGLNVCTPoint.sGLColor.R=1;
 if (sGLNVCTPoint.sGLColor.G>1) sGLNVCTPoint.sGLColor.G=1;
 if (sGLNVCTPoint.sGLColor.B>1) sGLNVCTPoint.sGLColor.B=1;

 if (sGLNVCTPoint.sGLColor.R<0) sGLNVCTPoint.sGLColor.R=0;
 if (sGLNVCTPoint.sGLColor.G<0) sGLNVCTPoint.sGLColor.G=0;
 if (sGLNVCTPoint.sGLColor.B<0) sGLNVCTPoint.sGLColor.B=0;
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
 const uint16_t BUFFERS=2;//два буфера
 const uint16_t POINTS_IN_TRIANGLE=3;//три точки для каждого треугольника
 const uint16_t POINTS_TO_NEW_POINTS=2;//каждая точка может дать две точки при отсечении
 //выполняем отсечение геометрии для всех ограничивающих плоскостей
 SGLNVCTPoint sGLNVCTPoint[BUFFERS][POINTS_IN_TRIANGLE*POINTS_TO_NEW_POINTS*FRUSTRUM_PLANE];//список хранимых вершин

 sGLNVCTPoint[0][0]=A;
 sGLNVCTPoint[0][1]=B;
 sGLNVCTPoint[0][2]=C;

 uint16_t point_amount=POINTS_IN_TRIANGLE;
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
 if (point_amount<POINTS_IN_TRIANGLE) return;//нечего рисовать
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
 SGLNCTPoint sGLNCTPoint_1;
 SGLNCTPoint sGLNCTPoint_2;
 for(int32_t sy=starty;sy<=endy;sy++,cx1+=dcx1,cx2+=dcx2,cz1+=dcz1,cz2+=dcz2, cr1+=dcr1,cr2+=dcr2, cg1+=dcg1,cg2+=dcg2, cb1+=dcb1,cb2+=dcb2, cu1+=dcu1,cu2+=dcu2, cv1+=dcv1,cv2+=dcv2)
 {
  float x1,x2;
  float z1,z2;

  x1=cx1;
  z1=cz1;
  sGLNCTPoint_1.sGLColor.R=cr1;
  sGLNCTPoint_1.sGLColor.G=cg1;
  sGLNCTPoint_1.sGLColor.B=cb1;
  sGLNCTPoint_1.sGLTexture.U=cu1;
  sGLNCTPoint_1.sGLTexture.V=cv1;

  if (sy<bp.Y)
  {
   x2=cx2;
   z2=cz2;

   sGLNCTPoint_2.sGLColor.R=cr2;
   sGLNCTPoint_2.sGLColor.G=cg2;
   sGLNCTPoint_2.sGLColor.B=cb2;
   sGLNCTPoint_2.sGLTexture.U=cu2;
   sGLNCTPoint_2.sGLTexture.V=cv2;
  }
  else
  {
   if (cp.Y==bp.Y)
   {
    x2=bp.X;
    z2=1.0f/b.sGLVertex.Z;

    sGLNCTPoint_2.sGLColor=b.sGLColor;
    sGLNCTPoint_2.sGLTexture.U=b.sGLTexture.U/b.sGLVertex.Z;
    sGLNCTPoint_2.sGLTexture.V=b.sGLTexture.V/b.sGLVertex.Z;
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

    sGLNCTPoint_2.sGLColor.R=cr2;
    sGLNCTPoint_2.sGLColor.G=cg2;
    sGLNCTPoint_2.sGLColor.B=cb2;

    sGLNCTPoint_2.sGLTexture.U=cu2;
    sGLNCTPoint_2.sGLTexture.V=cv2;
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

   SGLNCTPoint sGLNCTPoint_tmp=sGLNCTPoint_1;
   sGLNCTPoint_1=sGLNCTPoint_2;
   sGLNCTPoint_2=sGLNCTPoint_tmp;
  }
  //чертим линию треугольника
  DrawLine(sy,x1,x2,z1,z2,sGLNCTPoint_1,sGLNCTPoint_2);
 }
}
//----------------------------------------------------------------------------------------------------
//отрисовка линии
//----------------------------------------------------------------------------------------------------
void CSGL::DrawLine(int32_t y,int32_t x1,int32_t x2,float z1,float z2,const SGLNCTPoint &sGLNCTPoint_1,const SGLNCTPoint &sGLNCTPoint_2)
{
 float u1=sGLNCTPoint_1.sGLTexture.U;
 float v1=sGLNCTPoint_1.sGLTexture.V;

 float r1=sGLNCTPoint_1.sGLColor.R;
 float g1=sGLNCTPoint_1.sGLColor.G;
 float b1=sGLNCTPoint_1.sGLColor.B;

 float u2=sGLNCTPoint_2.sGLTexture.U;
 float v2=sGLNCTPoint_2.sGLTexture.V;

 float r2=sGLNCTPoint_2.sGLColor.R;
 float g2=sGLNCTPoint_2.sGLColor.G;
 float b2=sGLNCTPoint_2.sGLColor.B;

 //отрицательная координата текстуры - это текстура в другом направлении
 //поэтому коррекции не требуется - достаточно просто отбросить знак

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
 CGLScreenColor *vptr=ImageMap+(x1+(ScreenHeight-y-1)*ScreenWidth);
 float *invdepthptr=InvZBuffer+x1+(int32_t)(ScreenHeight-y-1)*ScreenWidth;

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
   uint32_t tui=static_cast<uint32_t>(abs(tu));
   uint32_t tvi=static_cast<uint32_t>(abs(tv));

   tui&=mask_texture_width;
   tvi&=mask_texture_height;

   SGLRGBAByteColor &sGLRGBAByteColor=sGLTextureObject_Current.sGLRGBAByteColor_Ptr[tui+tvi*sGLTextureObject_Current.Width];

   uint8_t cr=(uint8_t)(r*sGLRGBAByteColor.R);
   uint8_t cg=(uint8_t)(g*sGLRGBAByteColor.G);
   uint8_t cb=(uint8_t)(b*sGLRGBAByteColor.B);

   vptr->SetColor((uint8_t)(cr)&0xff,(uint8_t)(cg)&0xff,(uint8_t)(cb)&0xff);
   *(invdepthptr)=z;   
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
 ImageMap=new CGLScreenColor[screen_width*screen_height];
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

 SGLMatrix4 matrix;

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
 SGLMatrix4 matrix;
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

 SGLMatrix4 matrix;
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
 if (EnableLighting==true) CreateLighColor(sGLNVCTPointArray[PointArrayAmount]);//вычисляем освещённость точки для режима работы с источниками света
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
  CGLScreenColor *ptr=ImageMap;
  uint8_t r=static_cast<uint8_t>(sGLColor_Clear.R*MAX_COLOR_VALUE);
  uint8_t g=static_cast<uint8_t>(sGLColor_Clear.G*MAX_COLOR_VALUE);
  uint8_t b=static_cast<uint8_t>(sGLColor_Clear.B*MAX_COLOR_VALUE);
  for(uint32_t n=0;n<ScreenWidth*ScreenHeight;n++,ptr++) ptr->SetColor(r,g,b);
 }
 if (mode&SGL_DEPTH_BUFFER_BIT)
 {
  float *ptr=InvZBuffer;
  for(uint32_t n=0;n<ScreenWidth*ScreenHeight;n++,ptr++) *ptr=static_cast<float>(MIN_INV_Z_VALUE);
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
//----------------------------------------------------------------------------------------------------
//разрешить
//----------------------------------------------------------------------------------------------------
void CSGL::Enable(ITEM_ID mode)
{
 if (mode==SGL_LIGHTING) EnableLighting=true;
 if (mode==SGL_LIGHT0) sLight[0].Enabled=true;
 if (mode==SGL_LIGHT1) sLight[1].Enabled=true;
 if (mode==SGL_LIGHT2) sLight[2].Enabled=true;
 if (mode==SGL_LIGHT3) sLight[3].Enabled=true;
 if (mode==SGL_LIGHT4) sLight[4].Enabled=true;
 if (mode==SGL_LIGHT5) sLight[5].Enabled=true;
 if (mode==SGL_LIGHT6) sLight[6].Enabled=true;
 if (mode==SGL_LIGHT7) sLight[7].Enabled=true;
}
//----------------------------------------------------------------------------------------------------
//запретить
//----------------------------------------------------------------------------------------------------
void CSGL::Disable(ITEM_ID mode)
{
 if (mode==SGL_LIGHTING) EnableLighting=false;
 if (mode==SGL_LIGHT0) sLight[0].Enabled=false;
 if (mode==SGL_LIGHT1) sLight[1].Enabled=false;
 if (mode==SGL_LIGHT2) sLight[2].Enabled=false;
 if (mode==SGL_LIGHT3) sLight[3].Enabled=false;
 if (mode==SGL_LIGHT4) sLight[4].Enabled=false;
 if (mode==SGL_LIGHT5) sLight[5].Enabled=false;
 if (mode==SGL_LIGHT6) sLight[6].Enabled=false;
 if (mode==SGL_LIGHT7) sLight[7].Enabled=false;
}
//----------------------------------------------------------------------------------------------------
//задать параметры источника света
//----------------------------------------------------------------------------------------------------
void CSGL::Lightfv(ITEM_ID light,PARAM_ID param,float *ptr)
{
 SLight *sLight_Ptr=NULL;
 if (light==SGL_LIGHT0) sLight_Ptr=&sLight[0];
 if (light==SGL_LIGHT1) sLight_Ptr=&sLight[1];
 if (light==SGL_LIGHT2) sLight_Ptr=&sLight[2];
 if (light==SGL_LIGHT3) sLight_Ptr=&sLight[3];
 if (light==SGL_LIGHT4) sLight_Ptr=&sLight[4];
 if (light==SGL_LIGHT5) sLight_Ptr=&sLight[5];
 if (light==SGL_LIGHT6) sLight_Ptr=&sLight[6];
 if (light==SGL_LIGHT7) sLight_Ptr=&sLight[7];
 if (sLight_Ptr==NULL) return;
 if (param==SGL_POSITION)
 {
  sLight_Ptr->sGLVertex.X=*(ptr+0);
  sLight_Ptr->sGLVertex.Y=*(ptr+1);
  sLight_Ptr->sGLVertex.Z=*(ptr+2);

  //вычислим положение источника света с учётом матрицы моделирования
  SGLVector4 vector_in;
  SGLVector4 vector_out;

  vector_in.X=sLight_Ptr->sGLVertex.X;
  vector_in.Y=sLight_Ptr->sGLVertex.Y;
  vector_in.Z=sLight_Ptr->sGLVertex.Z;
  vector_in.W=1.0f;

  MultiplySGLVector4ToSGLMatrix4(vector_in,ModelViewMatrix,vector_out);

  sLight_Ptr->sGLVertex.X=vector_out.X/vector_out.W;
  sLight_Ptr->sGLVertex.Y=vector_out.Y/vector_out.W;
  sLight_Ptr->sGLVertex.Z=vector_out.Z/vector_out.W;
  return;
 }
 if (param==SGL_AMBIENT)
 {
  sLight_Ptr->SGLColor_Ambient.R=*(ptr+0);
  sLight_Ptr->SGLColor_Ambient.G=*(ptr+1);
  sLight_Ptr->SGLColor_Ambient.B=*(ptr+2);
  return;
 }
 if (param==SGL_DIFFUSE)
 {
  sLight_Ptr->SGLColor_Diffuse.R=*(ptr+0);
  sLight_Ptr->SGLColor_Diffuse.G=*(ptr+1);
  sLight_Ptr->SGLColor_Diffuse.B=*(ptr+2);
  return;
 }
 if (param==SGL_SPECULAR)
 {
  sLight_Ptr->SGLColor_Specular.R=*(ptr+0);
  sLight_Ptr->SGLColor_Specular.G=*(ptr+1);
  sLight_Ptr->SGLColor_Specular.B=*(ptr+2);
  return;
 }
 if (param==SGL_SHININESS)
 {
  sLight_Ptr->Shininess=*(ptr+0);
  return;
 }
 if (param==SGL_CONSTANT_ATTENUATION)
 {
  sLight_Ptr->ConstantAttenuation=*(ptr+0);
  return;
 }
 if (param==SGL_LINEAR_ATTENUATION)
 {
  sLight_Ptr->LinearAttenuation=*(ptr+0);
  return;
 }
 if (param==SGL_QUADRATIC_ATTENUATION)
 {
  sLight_Ptr->QuadraticAttenuation=*(ptr+0);
  return;
 }
}
//----------------------------------------------------------------------------------------------------
//задать параметры материала
//----------------------------------------------------------------------------------------------------
void CSGL::Materialfv(PARAM_ID param,float *ptr)
{
 if (param==SGL_AMBIENT)
 {
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Ambient.R=*(ptr+0);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Ambient.G=*(ptr+1);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Ambient.B=*(ptr+2);
  return;
 }
 if (param==SGL_DIFFUSE)
 {
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Diffuse.R=*(ptr+0);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Diffuse.G=*(ptr+1);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Diffuse.B=*(ptr+2);
  return;
 }
 if (param==SGL_SPECULAR)
 {
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Specular.R=*(ptr+0);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Specular.G=*(ptr+1);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Specular.B=*(ptr+2);
  return;
 }
 if (param==SGL_EMISSION)
 {
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Emission.R=*(ptr+0);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Emission.G=*(ptr+1);
  sGLNVCTPoint_Current.sGLMaterial.SGLColor_Emission.B=*(ptr+2);
  return;
 }
 if (param==SGL_SHININESS)
 {
  sGLNVCTPoint_Current.sGLMaterial.Shininess=*(ptr+0);
  return;
 }
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

