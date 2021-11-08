#include "main.h"
#include "fatfs_task.h"
#include "BMP_decoding.h"


union
{
	uint8_t U8[BITMAP_FILE_INFORMATION_SIZE];						
	BITMAP_FILE_INFORMATION st_bmp;	
}un_BITMAP_FILE_INFORMATION;

//LCD���ش�С
#define LCD_Y_MAX				64			//bmp�߶�,���ϵ��»���µ���
#define LCD_X_MAX				128			//bmp��ȣ�������
#define ONE_BYTE_WIDTH			8			//һ��bmp�����ֽ�ռͼƬ��ȵ�8����
/*********************************
  LCD��ʾ����
0 |
--|---------> X
  |
  |
  |
  y	
***********************************/


/**
  * @brief  Monochrome bitmap decoding
  * @param  file_path: �ļ�·����"0:/badapple/badapple12864__0000.bmp"
  * @param  x128,y64: ��ʾ��ʼ�������Ͻ�
  *  
  * @retval res = FR_OK; �ɹ�����ʧ��
  */


uint8_t bmp_tmp, bmp_user, bmp_y128_buf[16];//ֻҪ16���ֽھͿ�����������ʾһ��128����
FRESULT monochrome_bitmap_decoding12864(const TCHAR *file_path, uint8_t x128, uint8_t y64)//ֻ�Ե�ɫλͼ���룬�����12864��������Ҫע��ͼƬ�Ŀ�Ⱥ͸߶���
{
	/*
		oled������12864�����ͼƬ��� x128 y64���곬��12864��Χ,�Ͳ�������,�������ͼƬ��12864�ģ���ôX128��Y64��ҪΪ0��������������ʾһ��ͼƬ
	*/
	
    FRESULT res = FR_NOT_READY;        /* FatFs function common result code */
    UINT br;         					/* File R/W count */
	
    uint16_t i,j,x,y,k,matrix;
	//uint8_t bmp_tmp, bmp_user, bmp_y128_buf[16];//ֻҪ16���ֽھͿ�����������ʾһ��128����

    f_mount(0, &fs1);//ע��һ�����������ļ�ϵͳ����
    res = f_open(&file, file_path, FA_OPEN_EXISTING | FA_READ);	 	//FA_READ ָ�������ʶ��󡣿��Դ��ļ��ж�ȡ���ݡ�
    if (FR_OK == res)
    {
        res = f_read(&file, &un_BITMAP_FILE_INFORMATION.U8, BITMAP_FILE_INFORMATION_SIZE, &br);//��ȡ��ɫλͼ�����Ϣ
        if (FR_OK == res)
        {
			if (0x4d42 == un_BITMAP_FILE_INFORMATION.st_bmp.bfType)//bmp
			{
				if (0x01 == un_BITMAP_FILE_INFORMATION.st_bmp.biPerPixel)//��ɫλͼ
				{
					if ((un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>64) && (un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>128))
					{//ͼƬ����ֱ����ʾ
						res = FR_NOT_READY;
						return res;
					}
					res = f_lseek(&file, un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits);//����bmp����ʾ���ݵ�ַ
					y=un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-1;//ͼƬ�߶�0~63
					//�����ݴ�С���Ը߶Ⱦ͵���Ҫ��ʾ�Ŀ��
					matrix = un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage/un_BITMAP_FILE_INFORMATION.st_bmp.biHeight;//bmp�ļ�һ���ж��ٸ��ֽ�
					while(y64>0)
					{	//����ͼƬ�߶ȿռ��Ƿ�
						if (y64+un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>=64)
							y64--;
						else
							break;
					}

					while(x128>0)
					{	//����ͼƬ��ȿռ��Ƿ�
						if (x128+un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>=128)
							x128--;
						else
							break;
					}
					x=0;
					for (i=0; i<un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage; i+=matrix)//ͼƬ��С
					{
						f_read(&file, bmp_y128_buf, matrix, &br);//��һ�е��������
						for (k=0; k<un_BITMAP_FILE_INFORMATION.st_bmp.biWidth; k++)
						{
							if (bmp_y128_buf[k/8] != bmp_tmp)
							{
								bmp_tmp = bmp_y128_buf[k/8];
								bmp_user = bmp_tmp;
							}
								
							j = k%8;
							if ((bmp_user << j) & 0x80)
								OLED_Set_Pos_cache(x128+k, y64+y-x, 0);
							else
								OLED_Set_Pos_cache(x128+k, y64+y-x, 1);
						}
						x++;
					}
				}
			}
        }
        f_close(&file);
    }
    f_mount(0, NULL);//ע��һ�����������ļ�ϵͳ����
    return res;
}



#if (1 == BMP_CECODING_FAST)
//�ڴ���һ�ζ�������ͼƬ����ʾ����
#define BMP_IMAGE_SIZE	1024
uint8_t bmp_buffer[BMP_IMAGE_SIZE];//12864�ĵ�ɫλͼ���ݾ���1024BYTES
FRESULT monochrome_bitmap_decoding12864_fast(const TCHAR *file_path, uint8_t x128, uint8_t y64)//ֻ��С�ڵ���12864�ĵ�ɫλͼ����
{
	/*
		oled������12864�����ͼƬ��� x128 y64���곬��12864��Χ,��ʾ�ͻỨ��,�������ͼƬ��12864�ģ���ôX128��Y64��ҪΪ0��������������ʾһ��ͼƬ
	*/
	//#define BMP_IMAGE_SIZE	1024
    FRESULT res = FR_NOT_READY;        /* FatFs function common result code */
    UINT br;         					/* File R/W count */
	
    uint32_t i,j,x,y,z;
	uint8_t n,m;
	//uint8_t bmp_buffer[BMP_IMAGE_SIZE];//12864�ĵ�ɫλͼ���ݾ���1024BYTES

    f_mount(0, &fs1);//ע��һ�����������ļ�ϵͳ����
    res = f_open(&file, file_path, FA_OPEN_EXISTING | FA_READ);	 	//FA_READ ָ�������ʶ��󡣿��Դ��ļ��ж�ȡ���ݡ�
    if (FR_OK == res)
    {
        res = f_read(&file, &un_BITMAP_FILE_INFORMATION.U8, BITMAP_FILE_INFORMATION_SIZE, &br);//��ȡ��ɫλͼ�����Ϣ
        if (FR_OK == res)
        {
			if (0x4d42 == un_BITMAP_FILE_INFORMATION.st_bmp.bfType)//bmp
			{
				if (0x01 == un_BITMAP_FILE_INFORMATION.st_bmp.biPerPixel)//��ɫλͼ
				{
					res = f_lseek(&file, un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits);
					//�����޶������ݴ�С
					if (un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage>BMP_IMAGE_SIZE)
						un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage=BMP_IMAGE_SIZE;
					//������ɫͼƬ����
					f_read(&file, bmp_buffer, un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage, &br);//ͼƬ������������x128=0, y64=63��ʼ
				}
			}
        }
        f_close(&file);
    }
    f_mount(0, NULL);//ע��һ�����������ļ�ϵͳ����
	
	if (FR_OK != res)
		return res;
	
	x =0;
	y=un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-1;//ͼƬ�߶�0~63��ͼƬ�����½�Ϊ��һ������
	//�����ݴ�С���Ը߶Ⱦ͵���Ҫ��ʾ�Ŀ��
	un_BITMAP_FILE_INFORMATION.st_bmp.biWidth=un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage/un_BITMAP_FILE_INFORMATION.st_bmp.biHeight*8;

	while(y64>0)
	{	//����ͼƬ�߶ȿռ��Ƿ�
		if (y64+un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>=64)
			y64--;
		else
			break;
	}

	while(x128>0)
	{	//����ͼƬ��ȿռ��Ƿ�
		if (x128+un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>=128)
			x128--;
		else
			break;
	}
	
	for (i=0; i<un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage; i++)//ͼƬ��С
	{
		x = i*8 / un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;	
		z = i*8 % un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;
		for (j=0; j<8; j++)//��ɫλͼֻ��0/1
		{
			if (bmp_buffer[i]<<j & 0x80)
				OLED_Set_Pos_cache(x128+z+j, y64+y-x, 0);//��0 63���꿪ʼ
			else
				OLED_Set_Pos_cache(x128+z+j, y64+y-x, 1);
		}
	}
	
	
	
    return res;
}
#endif



//FRESULT bmp_decoding(const TCHAR *file_path, uint32_t x128, uint32_t y64)//ֻ�Ե�ɫλͼ���룬�����12864��������Ҫע��ͼƬ�Ŀ�Ⱥ͸߶���
//{
//	/*
//		ͼƬ����ʼ�����Ǵ����½ǿ�ʼ��lcd����ʼ�����Ǵ����Ͻǿ�ʼ
//	*/
//	
//    FRESULT res = FR_NOT_READY;        /* FatFs function common result code */
//    UINT br;         					/* File R/W count */
//	
//    uint32_t i,j,x,y,z;
//	uint32_t n,m, x_offset, y_offset, show_y_size, show_x_size;
//	uint32_t bmptmp;

//    f_mount(0, &fs1);//ע��һ�����������ļ�ϵͳ����
//    res = f_open(&file, file_path, FA_OPEN_EXISTING | FA_READ);	 	//FA_READ ָ�������ʶ��󡣿��Դ��ļ��ж�ȡ���ݡ�
//    if (FR_OK == res)
//    {
//        res = f_read(&file, &un_BITMAP_FILE_INFORMATION.U8, BITMAP_FILE_INFORMATION_SIZE, &br);//��ȡ��ɫλͼ�����Ϣ
//        if (FR_OK == res)
//        {
//			if (0x4d42 == un_BITMAP_FILE_INFORMATION.st_bmp.bfType)//bmp
//			{
//				if (0x01 == un_BITMAP_FILE_INFORMATION.st_bmp.biPerPixel)//��ɫλͼ
//				{
//					//�����ݴ�С���Ը߶Ⱦ͵���Ҫ��ʾ�Ŀ��
//					un_BITMAP_FILE_INFORMATION.st_bmp.biWidth=un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage/un_BITMAP_FILE_INFORMATION.st_bmp.biHeight*8;
//		
//					x =0;
//					//ʵ��ͼ��������ʾ����Ļ�ϵĴ�С
//					show_y_size = LCD_Y_MAX-y64;
//					show_x_size = LCD_X_MAX-x128;
//					//����ͼƬ�߶�ƫ����
//					if (un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>show_y_size)
//						y_offset = un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-show_y_size; 
//					else
//						y_offset = 0;
//					y=un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-1;//ͼƬ�߶�0~63
//					
//					
//					

//					
//					
//					// un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits��ͼƬ��ʾ���ݵ���ʼ��ַ
//					res = f_lseek(&file, un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits);
//					
//					

//					while(y64>0)
//					{	//����ͼƬ�߶ȿռ��Ƿ�
//						if (y64+un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>=64)
//							y64--;
//						else
//							break;
//					}

//					while(x128>0)
//					{	//����ͼƬ��ȿռ��Ƿ�
//						if (x128+un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>=128)
//							x128--;
//						else
//							break;
//					}
//					
//					for (i=0; i<un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage; i++)//ͼƬ��С
//					{
//						f_read(&file, &bmptmp, 1, &br);//ͼƬ������������x128=0, y64=63��ʼ

//						x = i*8 / un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;	
//						z = i*8 % un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;
//						for (j=0; j<8; j++)//��ɫλͼֻ��0/1
//						{
//							if (bmptmp<<j & 0x80)
//								OLED_Set_Pos_cache(x128+z+j, y64+y-x, 0);//��0 63���꿪ʼ
//							else
//								OLED_Set_Pos_cache(x128+z+j, y64+y-x, 1);
//						}
//					}
//				}
//			}
//        }
//        f_close(&file);
//    }
//    f_mount(0, NULL);//ע��һ�����������ļ�ϵͳ����
//    return res;
//}

