#include "CameraOCAM.h"

CameraOCAM::CameraOCAM()
{
}

//------------------------------------------------------------------------------
bool CameraOCAM::fromFile(std::string filename)
{
 double *pol        = this->pol;
 double *invpol     = this->invpol;
// double *xc         = &this->xc;
// double *yc         = &this->yc;
 double *xc         = &this->cy;
 double *yc         = &this->cx;
 double *c          = &this->c;
 double *d          = &this->d;
 double *e          = &this->e;
 int    *width      = &this->width;
 int    *height     = &this->height;
 int *length_pol    = &this->length_pol;
 int *length_invpol = &this->length_invpol;
 FILE *f;
 char buf[1024];
 int i;

 //Open file
 const int CMV_MAX_BUF=1024;
 if(!(f=fopen(filename.c_str(),"r")))
 {
   printf("File %s cannot be opened\n", filename.c_str());
   return -1;
 }

 //Read polynomial coefficients
 fgets(buf,1024,f);
 fscanf(f,"\n");
 fscanf(f,"%d", length_pol);
 for (i = 0; i < *length_pol; i++)
 {
     fscanf(f," %lf",&pol[i]);
 }

 //Read inverse polynomial coefficients
 fscanf(f,"\n");
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%d", length_invpol);
 for (i = 0; i < *length_invpol; i++)
 {
     fscanf(f," %lf",&invpol[i]);
 }

 //Read center coordinates
 fscanf(f,"\n");
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%lf %lf\n", xc, yc);

 //Read affine coefficients
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%lf %lf %lf\n", c,d,e);

 //Read image size
 fgets(buf,CMV_MAX_BUF,f);
 fscanf(f,"\n");
 fscanf(f,"%d %d", height, width);

 fclose(f);

 fx=*c;
 fy=*d;
 return 0;
}

//------------------------------------------------------------------------------
void create_panoramic_undistortion_LUT ( CvMat *mapx, CvMat *mapy, float Rmin, float Rmax, float xc, float yc )
{
     int i, j;
     float theta;
     int width = mapx->width;
     int height = mapx->height;
     float *data_mapx = mapx->data.fl;
     float *data_mapy = mapy->data.fl;
     float rho;

     for (i=0; i<height; i++)
         for (j=0; j<width; j++)
         {
             theta = -((float)j)/width*2*M_PI; // Note, if you would like to flip the image, just inverte the sign of theta
             rho   = Rmax - (Rmax-Rmin)/height*i;
             *( data_mapx + i*width+j ) = yc + rho*sin(theta); //in OpenCV "x" is the
             *( data_mapy + i*width+j ) = xc + rho*cos(theta);
         }
}
