#include "chargen.h"

uint8_t fromcp866(char x);
uint32_t doubler(uint32_t w, int8_t ow);

int render_line(int line, char *s, uint8_t slen, const FONT_INFO font, uint8_t *bin, uint16_t binlen, uint8_t dbl, uint8_t spaces){
    int i;
    uint16_t byte=0;
    int16_t bit=0;
    uint32_t wbuf=0;
    uint8_t finish=0;
    for(i=0;i<=slen;i++){
        if(s[i]!=0){
            FONT_CHAR_INFO ci=font.descriptor[fromcp866(s[i])-font.start];
            uint8_t ciw=ci.width;
            uint16_t cho=ci.offset+((ciw<=8)?line:(line*2));
            uint8_t pad=0;
            if((bit+ciw) & 0x7){
                pad=1;
            };
            if((byte+((bit+ciw)>>3)+pad)>binlen) finish=1;
            if(!finish){
                uint32_t cword;
                if(ciw<=8){
                    cword=font.bitmaps[cho]>>(8-ciw);
                }else{
                    cword=((font.bitmaps[cho]<<8)|font.bitmaps[cho+1])>>(16-ciw);
                }
                if(dbl){
                    cword=doubler(cword,ciw);
                    ciw*=2;
                }
                wbuf|=(cword<<(32-ciw-bit));
                bit+=ciw+spaces;
            }
        }else{
            finish=1;
        }
        while(bit>=8 || (bit>0 && finish)){
            bin[byte]=(wbuf>>24)&0xff;
            byte++;
            wbuf<<=8;
            bit-=8;
        }
        if(finish)
            break;
    }
    return i;
};

uint32_t doubler(uint32_t w, int8_t ow){
    uint32_t out=0;
    uint32_t b=(1<<(ow-1));
    while((--ow)>=0){
        out<<=2;
        if(w&b){
            out|=3;
        }
        w<<=1;
    }
    return out;
}

uint8_t fromcp866(char c){
    uint8_t x=(unsigned char)c;
    if(x>=0xe0 && x<=0xef){
        return x-0x30;
    }else if(x==0xf0){ //Captital Yo
        return 127;
    }else if(x==0xf1){ //Regular Yo
        return 192;
    }else if(x>=32 && x<=0xaf){
        return x;
    }else{
        return 193;
    }
}

