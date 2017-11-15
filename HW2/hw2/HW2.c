#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>
#include<math.h>
void bmpwrite();
void bmp_file_header();
void gaussian1();
void gaussian2();
void distract();
uint8_t* bmpdata();
void bia();
void dft();
#define roffset 0
#define goffset 1
#define boffset 2
float odata[512*512];
int dta[512*512];
uint8_t map[1024];
uint8_t outputdata[512*512];
uint8_t answer1[512*512];
uint8_t answer2[512*512];
uint8_t answer3[512*512];
float redata[512][512],imdata[512][512],fourre[512][512],fourim[512][512];
float max=0;
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
     FILE * hillimage =fopen("Goldhill.bmp","rb");

     if(!hillimage||!output) {
        printf("read bmp fail\n");
        return 1;
    }
    bmp hill;
    bmp_file_header(&hill,hillimage);
    uint8_t *hilldata;
    hilldata=bmpdata(&hill,hillimage);


    int ans;
    uint8_t *data;
    int16_t *dftdata;
    printf("1.DFT 2.gausian1 3.gausian2 4.gausian1-gausian2\n" );
    scanf("%d",&ans);
    switch (ans) {
        case 1:
         bia(&hill,hilldata);
         dft(&hill,dta);
         data = outputdata;
        break;
        case 2:
        gaussian1(&hill,hilldata);
        data = answer1;
        break;
        case 3:
        gaussian2(&hill,hilldata);
        data = answer2;
        break;
        case 4:
        gaussian1(&hill,hilldata);
        gaussian2(&hill,hilldata);
        distract();
        data = answer3;
        break;
        }

    bmpwrite(&hill,output,data);
system("pause");
return 1;
}
void bia(bmp *image,uint8_t *data){


    for (int i = 0; i < 512; ++i)
	{
		for (int j=0; j<512; ++j){
			if((i+j)%2==0){
             dta[i*512+j]= data[i*512+j];
            }
            else{
                dta[i*512+j]=-data[i*512+j];
            }

        }
	}

}

void dft(bmp *image){
int M = image->height,N = image ->width;

    for(int x=0 ;x<512;x++){
        for( int v=0;v<512;v++){//1-D DFT y->v
            for(int y = 0;y<512;y++){
            redata[x][v] += dta[x*512+y] * cos(2*M_PI*(v*y)/512.0);
            imdata[x][v] -= dta[x*512+y] * sin(2*M_PI*(v*y)/512.0);
            }
        }

    }

  for(int v=0;v<512;v++){
    for(int u=0;u<512;u++){//1-D DFT x->u
        for(int x =0 ;x<512;x++){
            fourre[u][v] += ((redata[x][v]*cos(-2*M_PI*(u*x)/512.0))-(imdata[x][v]*(sin(-2*M_PI*(u*x)/512.0))));
			fourim[u][v] += ((redata[x][v]*sin(-2*M_PI*(u*x)/512.0))+(imdata[x][v]*(cos(-2*M_PI*(x*u)/512.0))));
            }
        }
    }

     for(int x=0,v=0;x<512;x++){
            for(v=0;v<512;v++){
                odata[x*512+v]=sqrt(fourre[x][v]*fourre[x][v]+fourim[x][v]*fourim[x][v]);

            }
        }

        for(int x=0;x<512*512;x++){ //normalize maximum

                if(max>odata[x]){
                    max=max;
                }
                else{
                max = odata[x];
                }
            }
    printf("max:%f ",max);
    max=255.0/(log(1+max));
    printf("max:%f ",max);

    for(int x=0,v=0;x<512;x++){
           for(v=0;v<512;v++){
               odata[x*512+v]=max*log(1+sqrt(fourre[x][v]*fourre[x][v]+fourim[x][v]*fourim[x][v]));

           }
       }


for(int x=0,v=0;x<512;x++){
       for(v=0;v<512;v++){
           outputdata[x*512+v]=(uint8_t)(odata[x*512+v]);

       }
   }

}

void gaussian1(bmp *image,uint8_t *data){
uint8_t pad[514][514];
pad[0][0]= data[512+1];
pad[0][513]= data[512+510];
pad[513][0]= data[510*512+1];
pad[513][513]= data[510*512+510];
    for(int i=1;i<513;i++){
        pad[0][i]=data[512+i-1];
        pad[513][i]=data[510*512+(i-1)];
    }
    for(int j=1;j<513;j++){
        pad[j][0]=data[(j-1)*512+1];
        pad[j][513]=data[(j-1)*512+510];
    }
    for(int i=1;i<513;i++){
        for(int j=1;j<513;j++){
                pad[i][j]=data[(i-1)*512+(j-1)];
        }
    }

uint16_t temp=0;
    for(int i=1;i<513;i++){
        for(int j=1;j<513;j++){
        temp=(pad[i-1][j-1]*1
        +pad[i-1][j]*2
        +pad[i-1][j+1]*1
        +pad[i][j-1]*2
        +pad[i][j]*4
        +pad[i][j+1]*2
        +pad[i+1][j-1]*1
        +pad[i+1][j]*2
        +pad[i+1][j+1]*1)/16;

        answer1[(i-1)*512+(j-1)]=(uint8_t)temp;
        }
    }

}

void gaussian2(bmp *image,uint8_t *data){
uint8_t pad[514][514];

pad[0][0]= data[512+1];
pad[0][513]= data[512+510];
pad[513][0]= data[510*512+1];
pad[513][513]= data[510*512+510];
    for(int i=1;i<513;i++){
        pad[0][i]=data[512+i-1];
        pad[513][i]=data[510*512+(i-1)];
    }
    for(int j=1;j<513;j++){
        pad[j][0]=data[(j-1)*512+1];
        pad[j][513]=data[(j-1)*512+510];
    }
    for(int i=1;i<513;i++){
        for(int j=1;j<513;j++){
                pad[i][j]=data[(i-1)*512+(j-1)];
        }
    }
    uint16_t temp=0;
    for(int i=1;i<513;i++){
        for(int j=1;j<513;j++){
            temp=(pad[i-1][j-1]*1
            +pad[i-1][j]*1
            +pad[i-1][j+1]*1
            +pad[i][j-1]*1
            +pad[i][j]*2
            +pad[i][j+1]*1
            +pad[i+1][j-1]*1
            +pad[i+1][j]*1
            +pad[i+1][j+1]*1)/10;

            answer2[(i-1)*512+(j-1)]=(uint8_t)temp;

            }
        }
}
void distract(){
    for(int i=1;i<513;i++){
        for(int j=1;j<513;j++){
        answer3[(i-1)*512+(j-1)]=(uint8_t)(answer1[(i-1)*512+(j-1)]-answer2[(i-1)*512+(j-1)]);
    }
}

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
}
uint8_t* bmpdata(bmp *image,FILE*im){
    uint8_t* data = malloc((image->width)*(image->height)*((image->bits_per_pixel/8)));

  fseek(im,54,SEEK_SET);
    fread(map,sizeof(uint8_t),image->data_offset-54, im);
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
