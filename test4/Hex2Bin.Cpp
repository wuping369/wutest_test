/*******************/
/* for P03 printer */
/*******************/
#include "stdio.h"
//#include <process.h>
#include "stdlib.h"
unsigned long readadr(void);
unsigned char readone(void);
unsigned char readword(void);
void convertFs32(void);
unsigned char exe1Line(void);
void convertDat(void);
unsigned char gethex(unsigned char ch);

FILE *fp,*out_fp;
unsigned long subadr;

int main(int argc, char* argv[])
{
	int mode;

	if((argv[1][0]=='/')&&(argv[1][1]=='?')&&(argc == 2)){
		printf("Convert Hex file to Binary file.\n\n");
		printf("Hex2Bin -mode -address source destination.\n");
		printf("  mode       -Fs32  source file is *.s32\n");
		printf("             -Dat   source file is *.dat\n");
		printf("  address    -0x000000\n");
		return 1;
	}
/*----------------------------------------------------------*/
	if(argc != 5){
		printf("Please enter the file(s) name.\n");
		exit(0);
	}
/*----------------------------------------------------------*/
	if((argv[1][0]=='-')&&(argv[1][1]=='F')&&(argv[1][2]=='s')&&(argv[1][3]=='3')&&(argv[1][4]=='2')){
		mode=1;
	}
	else if((argv[1][0]=='-')&&(argv[1][1]=='D')&&(argv[1][2]=='a')&&(argv[1][3]=='t')){
		mode=2;
	}
	else {
		printf("Invalid switch %s\n",argv[1]);
		return 0;
	}
/*----------------------------------------------------------*/
	printf("Convert Hex to Binary for wutest printer\n");
	printf("Created date:2004-12-22\n");
	printf("Copyright 2004 Start Computer Peripheral Institue All rights reserved\n\n");

	subadr=gethex((unsigned)(argv[2][8]));
	subadr+=gethex((unsigned)(argv[2][7]))*0x10L;
	subadr+=gethex((unsigned)(argv[2][6]))*0x100L;
	subadr+=gethex((unsigned)(argv[2][5]))*0x1000L;
	subadr+=gethex((unsigned)(argv[2][4]))*0x10000L;
	subadr+=gethex((unsigned)(argv[2][3]))*0x100000L;
/*	printf("CheckSum is 0x%06lx.\n",subadr);*/

	if((fp=fopen(argv[3],"rb"))==NULL){
		printf("Cannot open %s file!\n",argv[3]);
		exit(0);
	}

	if((out_fp=fopen(argv[4],"r+b"))==NULL){
		printf("Cannot open %s file!\n",argv[4]);
		exit(0);
	}
/*----------------------------------------------------------*/
	switch(mode){
		case 1:
			convertFs32();
			break;
		case 2:
			convertDat();
			break;
		default:
			break;
	}
	return 1;
}
/*----------------------------------------------------------*/
void convertFs32(){
	while(!feof(fp)){
		if(exe1Line()==0){
			fclose(fp);
			fclose(out_fp);
			break;
		}
	}
}
/*----------------------------------------------------------*/
unsigned char exe1Line(){
	unsigned char num,i;
	unsigned long address;

	getc(fp);			/*S3 or S7*/
	if(getc(fp)=='7') return 0;	/*S7 结束*/
	num=readword();			/*个数*/
	num-=5;
	address =readword()*0x1000000L;	/*address*/
	address+=readword()*0x10000L;
	address+=readword()*0x100L;
	address+=readword();
	address-=subadr;
	fseek(out_fp,address,SEEK_SET);
	for(i=0;i<num;i++) putc(readword(),out_fp);
	getc(fp);getc(fp);		/*校验码*/
	getc(fp);getc(fp);		/*0x0d,0x0a*/
	return 1;
}
/*----------------------------------------------------------*/
void convertDat(){
	while(!feof(fp)){
		switch(getc(fp)){
			case 'H':
				fseek(out_fp,readadr()-subadr,SEEK_SET);
				break;
			case 'D':
				getc(fp);getc(fp);
				while(getc(fp)==0x20) putc(readword(),out_fp);
				break;
/*			case 'B':
			case 0x0d:
			case 0x0a:
			case 0x20:
			case 'w':
			case 'q':
*/
			default:
				break;
		}
	}
	fclose(fp);
	fclose(out_fp);
}
/*----------------------------------------------------------*/
unsigned char readword(){
	unsigned char H,L,value;

	H=readone();
	L=readone();

	value=H*0x10+L;
	return value;
}
/*----------------------------------------------------------*/
unsigned long readadr(){
	unsigned long address;

	address=readone()*0x10000L;
	address+=readword()*0x100L;
	address+=readword();

	return address;
}
/*----------------------------------------------------------*/
unsigned char readone(){
	unsigned char value;

	value=getc(fp);

	if(value>='A' && value<='F') value-=0x37;
	else if(value>='0' && value<='9') value-=0x30;

	return value;
}
/*----------------------------------------------------------*/
unsigned char gethex(unsigned char ch){
	unsigned char value;

	value=ch;

	if(value>='A' && value<='F') value-=0x37;
	if(value>='a' && value<='f') value-=0x57;
	if(value>='0' && value<='9') value-=0x30;

	return value;
}



//SEEK_SET： 文件开头
//　　SEEK_CUR： 当前位置
//　　SEEK_END： 文件结尾
