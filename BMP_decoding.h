#ifndef __BMP_decoding_H
#define __BMP_decoding_H
#include "main.h"
#include "fatfs_task.h"
//bmp������С��ģʽ
//С��ģʽ����ָ���ݵĸ��ֽڱ������ڴ�ĸߵ�ַ�У������ݵĵ��ֽڱ������ڴ�ĵ͵�ַ�С�
#define BITMAP_FILE_INFORMATION_SIZE	38		//λͼ�ļ���ϢҪռ�õ��ڴ�

//__packed���ֽڶ������˼
typedef struct//��ɫλͼ����ṹ��
{
	__packed uint16_t bfType;			//0000h �ļ���ʶ 2 bytes ���ֽڵ���������ʶ��λͼ������
	__packed uint32_t bfSize;			//0002h File Size 4 bytes ���ֽڱ�ʾ�������ļ��Ĵ�С
	__packed uint32_t bfReserved1;		//0006h Reserved 4 bytes ��������������Ϊ0
	__packed uint32_t bfOffBits;		//000Ah Bitmap Data Offset 4 bytes ���ļ���ʼ��λͼ���ݿ�ʼ֮������ݣ�bitmap data��֮���ƫ����
	__packed uint32_t bfheader;			//000Eh Bitmap Header Size 4 bytesλͼ��Ϣͷ
    __packed uint32_t biWidth;			//0012h Width 4 bytesλͼ�Ŀ�ȣ�������Ϊ��λ
    __packed uint32_t biHeight;			//0016h Height 4 bytesλͼ�ĸ߶ȣ�������Ϊ��λ
	__packed uint16_t biPlanes;			//001Ah Planes 2 bytesλͼ��λ������ע����ֵ������1��
	__packed uint16_t biPerPixel;		//001Ch Bits Per Pixel 2 bytes ÿ�����ص�λ��,1 - ��ɫλͼ��ʵ���Ͽ���������ɫ��ȱʡ������Ǻ�ɫ�Ͱ�ɫ��������Լ�������������ɫ��
	__packed uint32_t biCompression;	//001Eh Compression 4 bytes ѹ��˵��,0 - ��ѹ�� ��ʹ��BI_RGB��ʾ��
	__packed uint32_t biSizeImage;		//0022h Bitmap Data Size 4 bytes ���ֽ�����ʾ��λͼ���ݵĴ�С������������4�ı���
}BITMAP_FILE_INFORMATION;

#define BMP_CECODING_FAST				0

extern FRESULT monochrome_bitmap_decoding12864(const TCHAR *file_path, uint8_t x128, uint8_t y64);
#if (1 == BMP_CECODING_FAST)
extern FRESULT monochrome_bitmap_decoding12864_fast(const TCHAR *file_path, uint8_t x128, uint8_t y64);
#endif


#endif