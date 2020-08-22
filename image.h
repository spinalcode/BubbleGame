

// Dump screenshot to SD Card as 16bpp bitmap
void saveBMP(char* filename, const uint8_t *bitmap){

    int w= bitmap[0]; //Pokitto::Display::width;
    int h= bitmap[1]; //Pokitto::Display::height;
    
    FILE *f;
    int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int
    
    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};
    
    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);
    
    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);
    
    f = fopen(filename,"wb");
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);

    for(int y=0; y<h; y++){
        for(int x=0; x<w; x++){
            int pix = bitmap[2+(x+w*(h-y-1))]; //Pokitto::Display::getPixel(x,h-y-1);
            int color = Pokitto::Display::palette[pix];
            char r = ((((color >> 11) & 0x1F) * 527) + 23) >> 6;
            char g = ((((color >> 5) & 0x3F) * 259) + 33) >> 6;
            char b = (((color & 0x1F) * 527) + 23) >> 6;
            fwrite(&b , 1 , sizeof(b) , f);
            fwrite(&g , 1 , sizeof(g) , f);
            fwrite(&r , 1 , sizeof(r) , f);
        }
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }
    
    fclose(f);
}
