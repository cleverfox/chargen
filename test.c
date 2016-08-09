#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "chargen.h"
#include "verdana8.h"
#include "tahoma8.h"
#include "lucida10.h"
#include "timesnewroman8.h"
#include "trebuchet8.h"

char *showbin(uint32_t c, uint8_t w);
char *showbin8(uint8_t c, uint8_t w);
char *showbin8s(uint8_t c, uint8_t w);
uint8_t rb(uint8_t x);

int main(int argc, char** argv){
    FILE *f;
    char *s=malloc(200);
    bzero(s,180);
    strcpy(s,"\x01\x8f\xe0\xa8\xa2\xa5\xe2!        ");
    //strcpy(s,"\xff\x02\x01\x8f\xe0\xa8\xa2\xa5\xe2@\xff\x03\xff\x12  ");
    //strcpy(s,"lucida10\xff\x12       ");
    int i;
    for(i=0;i<100;i++){
        s[i+12]=i%10+'0';
    }
    uint8_t bin[72];
    f=fopen("x1.xbm","w");

    const FONT_INFO *fonts[]={
        &verdana_8ptFontInfo,
        &tahoma_8ptFontInfo,
        &lucidaConsole_10ptFontInfo,
        &timesNewRoman_8ptFontInfo,
        &trebuchetMS_8ptFontInfo
    };

    fprintf(f,"#define x_width %d\n",72*8);
    fprintf(f,"#define x_height %d\n",fonts[0]->height);
    fprintf(f,"static unsigned char x_bits[] = {\n");

    uint8_t fc[FCLEN];
    uint8_t fc2[FCLEN];
    bzero(fc,FCLEN);
    fc[0]=0x12;
    fc[1]=0x24;

    for(int line=0;line<verdana_8ptFontInfo.height;line++){
        bzero(bin,72);
        int byte=render_line(line, s, 100, fonts, bin, 72, fc, fc2);
        printf("%d\n",byte);
        for(i=0;i<72;i++){
            if(line==0){
                bin[i]|=0x80;
            }
            if(i==71 && line==fonts[0]->height-1)
                fprintf(f,"0x%02x",rb(bin[i]));
            else
                fprintf(f,"0x%02x,",rb(bin[i]));
        }
        fprintf(f,"\n");
        printf(".\n");
    }
    fprintf(f,"};\n");
    fclose(f);

    /*
    for(line=0;line<font.height;line++){
        memset(bin,0,72);
        int byte=render_line(line, s, strlen(s), font, bin, 0, 2);
        for(i=0;i<=byte;i++){
            printf("%s",showbin8s(bin[i],8));
        }
        printf("\n");
    }
    */

    return 0;
};

char *showbin8s(uint8_t c, uint8_t w){
    int i=0;
    static char tmpbuf[9]=".";
    for(i=0;i<=w;i++){
        tmpbuf[i]=(c&0x80)?'*':(i==0?'.':' ');
        c<<=1;
    }
    tmpbuf[w]='\0';
    return tmpbuf;
}
char *showbin8(uint8_t c, uint8_t w){
    int i=0;
    static char tmpbuf[10];
    for(i=0;i<=w;i++){
        tmpbuf[i]=(c&0x80)?'*':' ';
        c<<=1;
    }
    tmpbuf[w]='\0';
    return tmpbuf;
}
char *showbin(uint32_t c, uint8_t w){
    int i=0;
    static char tmpbuf[17];
    for(i=0;i<w;i++){
        tmpbuf[i]=(c&0x8000)?'*':' ';
        c<<=1;
    }
    tmpbuf[w]='\0';
    return tmpbuf;
}
uint8_t rb(uint8_t x){
    int i=8;
    uint8_t o=0;
    while(i--){
        o<<=1;
        o|=x&1;
        x>>=1;
    }
    return o;
}
