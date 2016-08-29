#include "chargen.h"

uint8_t fromcp866(char x);
uint32_t doubler(uint32_t w, int8_t ow);

struct fontcfg {
    uint8_t fontn;
    uint8_t dbl;
    uint8_t spaces;
    const FONT_INFO *font;
};

void parse_fc(struct fontcfg *fc, uint8_t *fcl, uint8_t len){
    int i;
    for(i=0;i<len;i++){
        uint8_t cw=fcl[i];
        if(cw==2){
            fc->dbl=0; 
        } else if(cw==3) {
            fc->dbl=1; 
        } else if((cw&0xf0) == 0x10) {
            fc->spaces=cw&0x0f;
        } else if((cw&0xf0) == 0x20) {
            fc->fontn=cw&0x0f;
        }

    }
};

void store_fc(struct fontcfg *fc, uint8_t *fcl){
    fcl[0]=fc->fontn|0x20;
    fcl[1]=fc->spaces|0x10;
    fcl[2]=fc->dbl?3:2;
};

int render_line(int lineno, char *s, uint8_t slen, const FONT_INFO *fonts[], uint8_t *bin, uint16_t binlen, uint8_t *fcli, uint8_t *fclo, uint8_t *maxho){
    struct fontcfg fc = {0,0,0};
    uint16_t byte=0;
    int16_t bit=0;
    uint32_t wbuf=0;
    uint8_t finish=0;
    uint8_t maxh=0;
    if(fcli)
        parse_fc(&fc, fcli, FCLEN);
    fc.font=fonts[fc.fontn];
    maxh=fc.font->height;
    int i;
    for(i=0;i<=slen;i++){
        if(s[i]==0){ //NULL terminator
            finish=1;
        }else if(s[i]==-1){ //Control word
            i++;
            parse_fc(&fc, (uint8_t*)s+i, 1);
            fc.font=fonts[fc.fontn];
            maxh=fc.font->height>maxh?fc.font->height:maxh;
        }else{
            FONT_CHAR_INFO ci=fc.font->descriptor[fromcp866(s[i])-fc.font->start];
            uint8_t ciw=ci.width;
            uint16_t char_offset=ci.offset+(lineno*(1+((ciw-1)>>3))); 
            uint8_t pad=0;
            if((bit+ciw) & 0x7){
                pad=1;
            };
            if((byte+((bit+ciw)>>3)+pad)>binlen) finish=1;
            if(!finish){
                uint32_t cword;
                if(lineno<fc.font->height){
                    if(ciw<=8){
                        cword=fc.font->bitmaps[char_offset]>>(8-ciw);
                    }else{
                        cword=((fc.font->bitmaps[char_offset]<<8)|fc.font->bitmaps[char_offset+1])>>(16-ciw);
                    }
                }else{
                    cword=0;
                }
                if(fc.dbl){
                    cword=doubler(cword,ciw);
                    ciw*=2;
                }
                wbuf|=(cword<<(32-ciw-bit));
                bit+=ciw+fc.spaces;
            }
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

    if(fclo)
        store_fc(&fc, fclo);
    if(maxho)
        *maxho=maxh;
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

