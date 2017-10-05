#include<stdlib.h>
#include<stdio.h>
#include<inttypes.h>

void bmpwrite();
void bmp_file_header();
uint8_t* bmpdata();
uint8_t* reverse();
uint8_t* gray();
uint8_t* cover();
#define roffset 0
#define goffset 1
#define boffset 2

typedef struct bmpheader
  {
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
     FILE * lenaimage =fopen("lena_drink_reverse.bmp","rb");
     FILE * output = fopen("output.bmp","wb");
     FILE * waterimage = fopen("water.bmp", "rb");
     if(!lenaimage||!waterimage) {
        printf("read bmp fail\n");
        return 1;
    }
    bmp water;
    bmp lena;
    bmp_file_header(&lena,lenaimage);
    uint8_t *lenadata;
    lenadata=bmpdata(&lena,lenaimage);

    bmp_file_header(&water,waterimage);
    uint8_t *waterdata;
    waterdata=bmpdata(&water,waterimage);


    int ans;
    uint8_t *data;
    printf("1.reverse 2.cover water 3.gray\n" );
    scanf("%d",&ans);
    switch (ans) {
        case 1:
        lenadata=reverse(&lena,lenadata);
        data = lenadata;
        break;
        case 2:
        data=cover(&lena,lenadata,&water,waterdata);
        break;
        case 3:
        lenadata=gray(&lena,lenadata);
        data=lenadata;
        break;
    }

    bmpwrite(&lena,output,data);

return 1;
}
uint8_t *cover(bmp *lena,uint8_t*lenadata,bmp *water,uint8_t*waterdata){
int i ,j;
    for(i=0;i<154;i++){
        for ( j =52; j < 130; j++) {
            lenadata[((i+4)*512+j+60)*3+roffset]=waterdata[(i*180+j)*3+roffset];
            lenadata[((i+4)*512+j+60)*3+goffset]=waterdata[(i*180+j)*3+goffset];
            lenadata[((i+4)*512+j+60)*3+boffset]=waterdata[(i*180+j)*3+boffset];

        }
    }

return lenadata;
}
uint8_t *gray(bmp *image,uint8_t* data){ // 相鄰3個照公式相加後取代RGB   graygrid = 0.299*R + 0.587*G + 0.114*B
    uint8_t  graygrid;
    FILE * histo = fopen("histogram.txt","w");
    int i ,j;

    for(i=0;i<(image->height);i++){
        for ( j = 0; j < (image->width); j++) {
            graygrid= (299*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]+587*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]+114*data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset])/1000;
            fprintf(histo, "%d,",(uint8_t)graygrid);
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+roffset]=graygrid;
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+goffset]=graygrid;
            data[(i*(image->height)+j)*((image->bits_per_pixel)/8)+boffset]=graygrid;

        }
        fprintf(histo, "\n" );
    }

    printf("gray end\n" );

     return data;
}

uint8_t *reverse(bmp *image,uint8_t* data){
    int i,j,k;
    uint8_t *out = malloc((image->height)*(image->width)*((image->bits_per_pixel)/8));
    for(i=0;i<=(image->height);i=i+1){
        for(j=(image->width),k=0;j>0;j--,k++){
            // printf("i %d j %d k %d\n",i,j,k );
            out[(i*(image->height)+k)*((image->bits_per_pixel)/8)+goffset]=data[(j+i*(image->height))*((image->bits_per_pixel)/8)+goffset];
            out[(i*(image->height)+k)*((image->bits_per_pixel)/8)+roffset]=data[(j+i*(image->height))*((image->bits_per_pixel)/8)+roffset];
            out[(i*(image->height)+k)*((image->bits_per_pixel)/8)+boffset]=data[(j+i*(image->height))*((image->bits_per_pixel)/8)+boffset];
        }
    }
    return out;
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
  fwrite(data,sizeof(uint8_t), (image->height)*(image->width)*((image->bits_per_pixel)/8),im);
}
