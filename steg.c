#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void encrypt(unsigned char* imageData, long imageSize, char* insert, char* outfile, int offset){
    
    int pos = 0; //position of the pointer for the password
    int global_pointer = offset+(8*(strlen(insert)));

    for(int i = offset; i < global_pointer+4; i++){
        int pointer = (i-offset)%8;
        int t = (*(insert+pos) >> (7-pointer)) & 1;
        if(t == 1){ *(imageData+i) |= 0x01; } else { *(imageData+i) &= 0xFE; }
        if(pointer == 7) { pos++; }
    }
    
    FILE* wf = fopen(outfile, "wb");
    fwrite(imageData, sizeof(char), imageSize, wf);
    fclose(wf);
}

void decrypt(unsigned char* imageData, int offset){

    int pos = 0;
    char* len = (char*)malloc(sizeof(char));

    for(int i = offset; i < offset+8; i++){
        *(len) = (*(len)<<1) | *(imageData+i)&1;
    }
    
    int size = *len;
    char* pass = (char*)malloc(size * sizeof(char));

    int global_pointer = offset+(8*(size+1));

    for(int i = offset+8; i < global_pointer; i++) {
        *(pass+pos) = (*(pass+pos)<<1) | *(imageData+i)&1;
        int pointer = (i-offset)%8;
        if(pointer == 7) { printf("%c", *(pass+pos)); }
    }
    printf("\n");
    free(len);
    free(pass);
}

int main(int argc, char *argv[]){

    if(argc < 3){
        printf("Wrong argument structure. Example:\n./steg [image-path] [-e] [password] [-o] [outfile]\n./steg [image-path] [-d]");
        return 1;
    }
    
    // Code to grab the image and read bytes into memory for use //
    FILE* imageFile;
    unsigned char* imageData;
    long imageSize;
    
    imageFile = fopen(argv[1], "rb");

    if(imageFile == NULL){
        perror("There was an error reading image. Please try again.");
        return 69;
    }

    fseek(imageFile, 0, SEEK_END);
    imageSize = ftell(imageFile);
    rewind(imageFile);

    imageData = (unsigned char*)malloc(imageSize);
    if(imageSize == 0){
        perror("Memory allocation failed!");
        return 69;
    }

    fread(imageData, 1, imageSize, imageFile);
    fclose(imageFile);

    ////////////////////////////////////////////////////////////////////
    
    if(strcmp(argv[2], "-e") == 0){
        char* message = (char*)malloc((strlen(argv[3]) + 2) * sizeof(char));
        *message = (char)strlen(argv[3]);
        strcpy(message+1, argv[3]);
        encrypt(imageData, imageSize, message, argv[5], 206);
        free(message);
    }else if(strcmp(argv[2], "-d") == 0){
        decrypt(imageData, 206);
    }
    free(imageData);
    return 0;
}
