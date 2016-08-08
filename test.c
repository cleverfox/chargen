#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "chargen.h"
#include "verdana8.h"

char *showbin(uint32_t c, uint8_t w);
char *showbin8(uint8_t c, uint8_t w);
char *showbin8s(uint8_t c, uint8_t w);
uint8_t rb(uint8_t x);

int main(int argc, char** argv){
    FILE *f;
    const FONT_INFO font = verdana_8ptFontInfo;
    char *s=malloc(101);
    bzero(s,101);
    //strcpy(s,"\x01\x8f\xe0\xa8\xa2\xa5\xe2? ");
    int i;
    for(i=0;i<100;i++){
        s[i]=(i%10)+'0';
    }
    int line;
    uint8_t bin[72];
    f=fopen("x1.xbm","w");

    fprintf(f,"#define x_width %d\n",72*8);
    fprintf(f,"#define x_height %d\n",font.height);
    fprintf(f,"static unsigned char x_bits[] = {\n");

    for(line=0;line<font.height;line++){
        bzero(bin,72);
        int byte=render_line(line, s, 100, font, bin, 72, 0,2);
        printf("%d\n",byte);
        for(i=0;i<72;i++){
            if(line==0){
                bin[i]|=0x80;
            }
            if(i==71 && line==font.height-1)
                fprintf(f,"0x%x",rb(bin[i]));
            else
                fprintf(f,"0x%x,",rb(bin[i]));
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
