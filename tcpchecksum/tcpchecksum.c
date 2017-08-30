# include <stdio.h>
# include <stdlib.h>

unsigned short checksum(unsigned short *buffer, int size)
{
    unsigned long cksum=0;
    while(size >1)
    {
	printf("size %d \n",size);
	printf("cksum %x \n",cksum);
	printf("buffer %x \n",*buffer);
	printf("\n");
        cksum+=*buffer++;
        size -=sizeof(unsigned short);
    }
    if(size)
        cksum += *(unsigned char*)buffer;

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (unsigned short)(~cksum);
}

int main(int argc, char * argv[])
{
	unsigned short value;
	unsigned short buffer1[18]={
	0xc0a8, 0x01e2, 0xc0a8, 0x01e1, 0x0006, 0x0018,
	0x2ddc, 0x9b85, 0x38c4, 0x197c, 0x0775, 0xefeb,
	0x6012, 0xffff, 0x0000, 0x0000, 0x0204, 0x05b4};
	//0x38c4 

	//value=checksum(buffer1,sizeof(buffer1));
	value=checksum(buffer1,sizeof(buffer1));
	//value=checksum(buffer2,36);
	printf("%x \n",value);
}
