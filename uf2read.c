#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

// https://github.com/microsoft/uf2

struct UF2_Block {
    // 32 byte header
    uint32_t magicStart0;
    uint32_t magicStart1;
    uint32_t flags;
    uint32_t targetAddr;
    uint32_t payloadSize;
    uint32_t blockNo;
    uint32_t numBlocks;
    uint32_t fileSize; // or familyID;
    uint8_t data[476];
    uint32_t magicEnd;
} UF2_Block;

int main(int argc, char **argv) {

    if ( argc <= 1 ) {
        perror("invalid arguments\n");
        exit(-1);
    }

    int itercount = 0;
    int blockcount = 1;

    if ( argv[2] != NULL ) {
        blockcount = atoi(argv[2]);
    }

    FILE *fh;
    ssize_t bytes;
    char *filename = argv[1];
    struct UF2_Block block;
    char tbuf[32];

    //char *filename = "CALIB.UF2";

    fh = fopen(filename, "r");

while ( blockcount > 0 ) {
    printf("iter count %d\n", ++itercount);
    bytes = fread(&block, sizeof(struct UF2_Block), 1, fh);

    printf("%s\n", filename);
    printf("magicStart0: 0x%02X\n", block.magicStart0);
    printf("magicStart1: 0x%02X\n", block.magicStart1);
    printf("flags      : 0x%02X\n", block.flags);
    printf("targetAddr : 0x%08X\n", block.targetAddr);
    printf("payloadSize: 0x%02X\n", block.payloadSize);
    printf("blockNo    : 0x%02X\n", block.blockNo);
    printf("numBlocks  : 0x%02X\n", block.numBlocks);
    printf("familID    : 0x%02X\n", block.fileSize);
    printf("magicEnd   : 0x%02X\n", block.magicEnd);

    if ( block.blockNo == 0 ) {                             // new "file" condition, store useful information about it for parsing later.
        blockcount = block.numBlocks;
        printf("new block count: %d\n", blockcount);
    }

    int x=0;

    for ( x=0; x < 256 /* sizeof(block.data) */; x++) { // bytes past 256 in each block are not used by plinky
        if ( x % 16 )                               
            ;
        else {                                          // beginning of new line condition
            printf("%6x ", x * block.payloadSize);      // print a semi useful offset
        }
        if ((x+1) % 16) {                               // 16 bytes per row
            if ( (x+1) % 4 ) {                          // in groups of 4
                printf("0x%02X ", block.data[x]); 
            } else
                printf("0x%02X  ", block.data[x]);      // extra space for the grouping; yes, could just be a white space print alone
        } else {                                        // new line condition
            printf("0x%02X\n", block.data[x]);
        }
    }
    printf("\n");
    blockcount--;
}
    fclose(fh);
    return 0;
}
