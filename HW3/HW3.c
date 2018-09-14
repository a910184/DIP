#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>
#include<math.h>
void bmpwrite();
uint8_t *ero();
uint8_t *dia();
void bmp_file_header();
uint8_t *rgb2ycrcb();
uint8_t *rgb2ycrcb1();
uint8_t *bmpdata();
#define boffset 0
#define goffset 1
#define roffset 2
uint8_t map[1024];

typedef struct bmpheader {
      uint16_t id;
      uint32_t filesize;
      uint32_t reserved;
      uint32_t data_offset;
      uint32_t bitmap_header_size;
      uint32_t width;
      uint32_t height;
      uint16_t planes;
      uint16_t bits_per_pixel;
      uint32_t compression;
      uint32_t bitmap_data_size;
      uint32_t Hresolution;
      uint32_t Vresolution;
      uint32_t usedcolor;
      uint32_t important_color;

  } bmp;

int main()
{
     FILE * output = fopen("output.bmp","wb");
     FILE * kobeimage =fopen("kobe.bmp","rb");
     FILE * angelimage =fopen("angel.bmp","rb");
     if(!kobeimage||!output||!angelimage) {
        printf("read bmp fail\n");
        system("pause");
        return 1;
    }
    bmp kobe;
    bmp_file_header(&kobe,kobeimage);
    uint8_t *kobedata;
    kobedata=bmpdata(&kobe,kobeimage);
    bmp angel;
    bmp_file_header(&angel,angelimage);
    uint8_t *angeldata;
    angeldata=bmpdata(&angel,angelimage);


    int ans;
    uint8_t *data,*ycbcr;
    float *hsv;
    printf("1.angella face detection 2.kobe face detection \n" );
    scanf("%d",&ans);
    switch (ans) {
        case 1:
        data= rgb2ycrcb(&angel,angeldata);
        data = dia(&angel,data);
        data = dia(&angel,data);

        break;
        case 2:
        data = rgb2ycrcb1(&kobe,kobedata);
        data = ero(&angel,data);
        data = ero(&angel,data);
        data = dia(&angel,data);
        data = dia(&angel,data);
        data = ero(&angel,data);
        data = dia(&angel,data);
        data = dia(&angel,data);
        data = dia(&angel,data);
        data = dia(&angel,data);
        data = dia(&angel,data);
        data = dia(&angel,data);
        break;
        case 3:
        break;
        case 4:
        break;
    }

    bmpwrite(&kobe,output,data);

system("pause");
return 1;
}
uint8_t *rgb2ycrcb1(bmp *image,uint8_t *data){

float ycrcb[image->height][image->width][image->bits_per_pixel/8];
    for(int i=0;i<image->height;i++){
        for (int j = 0; j <image->width; j++) {
            ycrcb[i][j][0] =((0.299*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset])+(0.578*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset])+(1*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]));
            ycrcb[i][j][1] =((0.5*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset])+(-0.4187*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset])+(-0.0813*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset])+128);
            ycrcb[i][j][2] =((-0.1687*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset])+(-0.3313*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset])+(0.5*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset])+128);
        }
    }

    for(int i=0;i<(image->height);i++){
        for (int j = 0; j <(image->width); j++) {
            if (
            ycrcb[i][j][2]>=77&&127>=ycrcb[i][j][2]&&133<=ycrcb[i][j][1]&&ycrcb[i][j][1]<=173 && 35<=ycrcb[i][j][0]&&255>=ycrcb[i][j][0]
            ){
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]=255;
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]=255;
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]=255;
            }
            else{
                data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]=0;
                data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]=0;
                data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]=0;

            }
        }

    }
    printf("here\n" );
    return data;
}


uint8_t *rgb2ycrcb(bmp *image,uint8_t *data){

float ycrcb[image->height][image->width][image->bits_per_pixel/8];
    for(int i=0;i<image->height;i++){
        for (int j = 0; j <image->width; j++) {
            ycrcb[i][j][0] =((0.299*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset])+(0.578*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset])+(1*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]));
            ycrcb[i][j][1] =((0.5*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset])+(-0.4187*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset])+(-0.0813*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset])+128);
            ycrcb[i][j][2] =((-0.1687*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset])+(-0.3313*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset])+(0.5*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset])+128);
        }
    }

    for(int i=0;i<(image->height);i++){
        for (int j = 0; j <(image->width); j++) {
            if (
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset] > 95 &&
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset] > 40 &&
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset] > 20 &&
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset] > data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset] &&
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset] > data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset] &&
            abs(data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset] - data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]) > 15 &&
            ycrcb[i][j][1]> 135 &&
            ycrcb[i][j][2]> 85 &&
            ycrcb[i][j][0] > 80 &&
            ycrcb[i][j][1] <= (1.5862*ycrcb[i][j][2]+ 20)  &&
            ycrcb[i][j][1]>= (0.3448*ycrcb[i][j][2] + 76.2069) &&
            ycrcb[i][j][1] >= (-4.5652*ycrcb[i][j][2] + 234.5652) &&
            ycrcb[i][j][1] <= (-1.15*ycrcb[i][j][2] + 301.75) &&
            ycrcb[i][j][1]<= (-2.2857*ycrcb[i][j][2] + 432.85)
            ){
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]=255;
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]=255;
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]=255;
            }
            else{
                data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]=0;
                data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]=0;
                data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]=0;

            }
        }

    }
    printf("here\n" );
    return data;
}
uint8_t *ero(bmp*image,uint8_t*data){//SMALLSMALLSMALLSMALL
    uint8_t dilmax[image->height][image->width];
    int count;
    for (int i = 1; i < image->height-1; i++){
        for (int j = 1;j < image->width-1; j++){
            count=0;
            for (int a = -1; a <2; a++) {
                for (int b = -1 ; b < 2; b++) {
                    if(data[((i+a)*(image->height)+(j+b))*((image->bits_per_pixel)/8)+boffset]==255){count+=1;
                    }
                }
            }
        if(count>=9){dilmax[i][j]=255;}
        else{dilmax[i][j]=0;}
        }
    }
    for (int i = 1; i < image->height-1; i++){
        for (int j = 1;j < image->width-1; j++){
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]=dilmax[i][j];
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]=dilmax[i][j];
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]=dilmax[i][j];
        }
    }
return data;
}
uint8_t *dia(bmp*image,uint8_t*data){
    uint8_t dilmax[image->height][image->width];
    int count;
    for (int i = 1; i < image->height-1; i++){
        for (int j = 1;j < image->width-1; j++){
            count=0;
            for (int a = -1; a <2; a++) {
                for (int b = -1 ; b < 2; b++) {
                    if(data[((i+a)*(image->height)+(j+b))*((image->bits_per_pixel)/8)+boffset]==255){count+=1;
                    }
                }
            }
        if(count>=1){dilmax[i][j]=255;}
        else{dilmax[i][j]=0;}
        }
    }
    for (int i = 1; i < image->height-1; i++){
        for (int j = 1;j < image->width-1; j++){
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]=dilmax[i][j];
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]=dilmax[i][j];
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]=dilmax[i][j];
        }
    }
return data;
}
void bmp_file_header(bmp *image,FILE*im){

  fread(&image->id,sizeof(uint8_t),sizeof(uint16_t),im);
  fread(&image->filesize,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->reserved,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->data_offset,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->bitmap_header_size,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->width,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->height,sizeof(uint8_t),sizeof(uint32_t),im);

  fread(&image->planes,sizeof(uint8_t),sizeof(uint16_t),im);
  fread(&image->bits_per_pixel,sizeof(uint8_t),sizeof(uint16_t),im);
  fread(&image->compression,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->bitmap_data_size,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->Hresolution,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->Vresolution,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->usedcolor,sizeof(uint8_t),sizeof(uint32_t),im);
  fread(&image->important_color,sizeof(uint8_t),sizeof(uint32_t),im);
printf("%d\n",image->data_offset );
}
uint8_t* bmpdata(bmp *image,FILE*im){
    uint8_t* data = malloc(((image->width)*(image->height)*(image->bits_per_pixel)/8));

    fseek(im,54,SEEK_SET);
    fread(map,sizeof(uint8_t),(image->data_offset-54), im);
    fseek(im,image->data_offset,SEEK_SET);
    fread(data,sizeof(uint8_t),(image->height)*(image->width)*((image->bits_per_pixel)/8), im);


    return data;
}
void bmpwrite(bmp *image,FILE*im,uint8_t *data){
  fwrite(&image->id,sizeof(uint8_t),sizeof(uint16_t),im);
  fwrite(&image->filesize,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->reserved,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->data_offset,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->bitmap_header_size,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->width,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->height,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->planes,sizeof(uint8_t),sizeof(uint16_t),im);
  fwrite(&image->bits_per_pixel,sizeof(uint8_t),sizeof(uint16_t),im);
  fwrite(&image->compression,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->bitmap_data_size,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->Hresolution,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->Vresolution,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->usedcolor,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(&image->important_color,sizeof(uint8_t),sizeof(uint32_t),im);
  fwrite(map,sizeof(uint8_t), (image->data_offset) - 54,im);
  fwrite(data,sizeof(uint8_t), (image->height)*(image->width)*((image->bits_per_pixel)/8),im);
}
