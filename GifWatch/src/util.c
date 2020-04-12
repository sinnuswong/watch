/*
 * util.c
 *
 *  Created on: Mar 30, 2020
 *      Author: sinnus
 */

#include <stdio.h>
#include <stdlib.h>
#include <media_content.h>
#include<limits.h>
double getNumber(char a) {
	double res = 0;
	if (a >= 'A' && a <= 'F')
		res = (a - 'A' + 10);
	else
		res = (a - '0');
	return res;
}
double* get_color(char* in) {
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %s",in);
	//int temp = getNumber()*16+(in[2]-'A'+10);
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %d",(in[1]));
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %d",('A'));
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %d",temp);
	double* res = (double*) malloc(3 * sizeof(double));

//		return res;
	double a, b, c;
	a = getNumber(in[1]) * 16 + getNumber(in[2]);
	b = getNumber(in[3]) * 16 + getNumber(in[4]);
	c = getNumber(in[5]) * 16 + getNumber(in[6]);
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %f,%f,%f",a,b,c);
	res[0] = a / 255;
	res[1] = b / 255;
	res[2] = c / 255;
	//dlog_print(DLOG_ERROR, LOG_TAG, "get color %f,%f,%f",res[0],res[1],res[2]);
	return res;
}

//void get_size(int *w, int *h) {
//
//	int res = media_content_connect();
//	dlog_print(DLOG_ERROR, LOG_TAG, "get_size res1 %d", res);
//	if (res == MEDIA_CONTENT_ERROR_NONE) {
//		char *path = "/opt/usr/globalapps/org.eros.gifwatch/res/cxk.gif";
//		res = media_content_scan_file(path);
//		dlog_print(DLOG_ERROR, LOG_TAG, "get_size res2 %d", res);
//
//	}
//	media_info_h h = media_info_get_image();
//	//image_meta_destroy
//}


// 开始代码【PNG文件标志】[8] 可以用于校验文件类型
//FFFFFF89 00000050 0000004E 00000047 0000000D 0000000A 0000001A 0000000A 【第一行】
//结束代码 【IEND】 [12]
//00000000 00000000 00000000 00000000 00000049 00000045 0000004E 00000044
//FFFFFFAE 00000042 00000060 FFFFFF82    【第后行和倒数第二行后四个】
//图片宽高
//00000000 00000000 00000002 FFFFFFD0 ---> 00 00 02 D0 ---> 2*256+13*16 = 720
//00000000 00000000 00000005 00000000 ---> 00 00 05 00 ---> 5*256 	= 1280
#define E_MAX 	255
int * png_size(FILE *fp1) {	//得到PNG文件中二进制数据输出宽高（单位：像素）
	rewind(fp1);			//文件重定向到开始
	int place = 0, index = 0, i;
	char data;
	int size[8];
	int width = 0, height = 0;
	while (place <= 24) {
		place++;
		data = fgetc(fp1);
		if (place > 16) {
			size[index] = (int) data;
			index++;
		}
	}
	for(i=0;i<4;i++) if(size[i]<0) size[i] = (size[i]&INT_MAX)&E_MAX;
	width = 256 * size[2] + size[3];
	height = 256 * size[6] + size[7];
	int *res = (int*)malloc(sizeof(int)*2);
	res[0] = width;
	res[1] = height;
	fclose(fp1);
//	printf("【尺寸】图片宽为%d，长为%d\n", width, height);
	return res;
}

void putout(FILE *fp1) {		//输出文件数据流
	rewind(fp1);
	int place = 0;
	char data;
	while (!feof(fp1)) {
		place++;
		data = fgetc(fp1);
		if (feof(fp1))
			break;
		printf("%08X ", data);
		if (place % 8 == 0)
			printf("\n");
	}
}

int *extract_size(char* path) {
	FILE *fp1;
	fp1 = fopen(path, "rb+");
	if (fp1) {
		printf("文件存在\n");
		printf("----------------------------------\n");
	} else {
		printf("文件丢失\n");
		return 0;
	}
	return png_size(fp1);
//	putout(fp1);

}

