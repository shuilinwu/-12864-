#include "main.h"
#include "fatfs_task.h"
#include "BMP_decoding.h"


union
{
	uint8_t U8[BITMAP_FILE_INFORMATION_SIZE];						
	BITMAP_FILE_INFORMATION st_bmp;	
}un_BITMAP_FILE_INFORMATION;

//LCD像素大小
#define LCD_Y_MAX				64			//bmp高度,从上到下或从下到上
#define LCD_X_MAX				128			//bmp宽度，从左到右
#define ONE_BYTE_WIDTH			8			//一个bmp数据字节占图片宽度的8个点
/*********************************
  LCD显示坐标
0 |
--|---------> X
  |
  |
  |
  y	
***********************************/


/**
  * @brief  Monochrome bitmap decoding
  * @param  file_path: 文件路径如"0:/badapple/badapple12864__0000.bmp"
  * @param  x128,y64: 显示起始坐标左上角
  *  
  * @retval res = FR_OK; 成功否则失败
  */


uint8_t bmp_tmp, bmp_user, bmp_y128_buf[16];//只要16个字节就可以在屏上显示一行128个点
FRESULT monochrome_bitmap_decoding12864(const TCHAR *file_path, uint8_t x128, uint8_t y64)//只对单色位图解码，如果是12864的屏，就要注意图片的宽度和高度了
{
	/*
		oled像素是12864，如果图片宽高 x128 y64坐标超过12864范围,就不解码了,所以如果图片是12864的，那么X128和Y64都要为0，才能完整的显示一张图片
	*/
	
    FRESULT res = FR_NOT_READY;        /* FatFs function common result code */
    UINT br;         					/* File R/W count */
	
    uint16_t i,j,x,y,k,matrix;
	//uint8_t bmp_tmp, bmp_user, bmp_y128_buf[16];//只要16个字节就可以在屏上显示一行128个点

    f_mount(0, &fs1);//注册一个工作区（文件系统对象）
    res = f_open(&file, file_path, FA_OPEN_EXISTING | FA_READ);	 	//FA_READ 指定读访问对象。可以从文件中读取数据。
    if (FR_OK == res)
    {
        res = f_read(&file, &un_BITMAP_FILE_INFORMATION.U8, BITMAP_FILE_INFORMATION_SIZE, &br);//获取单色位图相关信息
        if (FR_OK == res)
        {
			if (0x4d42 == un_BITMAP_FILE_INFORMATION.st_bmp.bfType)//bmp
			{
				if (0x01 == un_BITMAP_FILE_INFORMATION.st_bmp.biPerPixel)//单色位图
				{
					if ((un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>64) && (un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>128))
					{//图片过大直接显示
						res = FR_NOT_READY;
						return res;
					}
					res = f_lseek(&file, un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits);//跳到bmp的显示数据地址
					y=un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-1;//图片高度0~63
					//用数据大小除以高度就等于要显示的宽度
					matrix = un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage/un_BITMAP_FILE_INFORMATION.st_bmp.biHeight;//bmp文件一行有多少个字节
					while(y64>0)
					{	//计算图片高度空间是否够
						if (y64+un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>=64)
							y64--;
						else
							break;
					}

					while(x128>0)
					{	//计算图片宽度空间是否够
						if (x128+un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>=128)
							x128--;
						else
							break;
					}
					x=0;
					for (i=0; i<un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage; i+=matrix)//图片大小
					{
						f_read(&file, bmp_y128_buf, matrix, &br);//读一行点阵的数据
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
    f_mount(0, NULL);//注销一个工作区（文件系统对象）
    return res;
}



#if (1 == BMP_CECODING_FAST)
//内存大就一次读出整个图片的显示数据
#define BMP_IMAGE_SIZE	1024
uint8_t bmp_buffer[BMP_IMAGE_SIZE];//12864的单色位图数据就是1024BYTES
FRESULT monochrome_bitmap_decoding12864_fast(const TCHAR *file_path, uint8_t x128, uint8_t y64)//只对小于等于12864的单色位图解码
{
	/*
		oled像素是12864，如果图片宽高 x128 y64坐标超过12864范围,显示就会花屏,所以如果图片是12864的，那么X128和Y64都要为0，才能完整的显示一张图片
	*/
	//#define BMP_IMAGE_SIZE	1024
    FRESULT res = FR_NOT_READY;        /* FatFs function common result code */
    UINT br;         					/* File R/W count */
	
    uint32_t i,j,x,y,z;
	uint8_t n,m;
	//uint8_t bmp_buffer[BMP_IMAGE_SIZE];//12864的单色位图数据就是1024BYTES

    f_mount(0, &fs1);//注册一个工作区（文件系统对象）
    res = f_open(&file, file_path, FA_OPEN_EXISTING | FA_READ);	 	//FA_READ 指定读访问对象。可以从文件中读取数据。
    if (FR_OK == res)
    {
        res = f_read(&file, &un_BITMAP_FILE_INFORMATION.U8, BITMAP_FILE_INFORMATION_SIZE, &br);//获取单色位图相关信息
        if (FR_OK == res)
        {
			if (0x4d42 == un_BITMAP_FILE_INFORMATION.st_bmp.bfType)//bmp
			{
				if (0x01 == un_BITMAP_FILE_INFORMATION.st_bmp.biPerPixel)//单色位图
				{
					res = f_lseek(&file, un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits);
					//这里限定了数据大小
					if (un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage>BMP_IMAGE_SIZE)
						un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage=BMP_IMAGE_SIZE;
					//读出单色图片数据
					f_read(&file, bmp_buffer, un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage, &br);//图片的数据坐标是x128=0, y64=63开始
				}
			}
        }
        f_close(&file);
    }
    f_mount(0, NULL);//注销一个工作区（文件系统对象）
	
	if (FR_OK != res)
		return res;
	
	x =0;
	y=un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-1;//图片高度0~63，图片的左下角为第一个数据
	//用数据大小除以高度就等于要显示的宽度
	un_BITMAP_FILE_INFORMATION.st_bmp.biWidth=un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage/un_BITMAP_FILE_INFORMATION.st_bmp.biHeight*8;

	while(y64>0)
	{	//计算图片高度空间是否够
		if (y64+un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>=64)
			y64--;
		else
			break;
	}

	while(x128>0)
	{	//计算图片宽度空间是否够
		if (x128+un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>=128)
			x128--;
		else
			break;
	}
	
	for (i=0; i<un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage; i++)//图片大小
	{
		x = i*8 / un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;	
		z = i*8 % un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;
		for (j=0; j<8; j++)//单色位图只有0/1
		{
			if (bmp_buffer[i]<<j & 0x80)
				OLED_Set_Pos_cache(x128+z+j, y64+y-x, 0);//从0 63坐标开始
			else
				OLED_Set_Pos_cache(x128+z+j, y64+y-x, 1);
		}
	}
	
	
	
    return res;
}
#endif



//FRESULT bmp_decoding(const TCHAR *file_path, uint32_t x128, uint32_t y64)//只对单色位图解码，如果是12864的屏，就要注意图片的宽度和高度了
//{
//	/*
//		图片的启始坐标是从左下角开始，lcd的启始坐标是从左上角开始
//	*/
//	
//    FRESULT res = FR_NOT_READY;        /* FatFs function common result code */
//    UINT br;         					/* File R/W count */
//	
//    uint32_t i,j,x,y,z;
//	uint32_t n,m, x_offset, y_offset, show_y_size, show_x_size;
//	uint32_t bmptmp;

//    f_mount(0, &fs1);//注册一个工作区（文件系统对象）
//    res = f_open(&file, file_path, FA_OPEN_EXISTING | FA_READ);	 	//FA_READ 指定读访问对象。可以从文件中读取数据。
//    if (FR_OK == res)
//    {
//        res = f_read(&file, &un_BITMAP_FILE_INFORMATION.U8, BITMAP_FILE_INFORMATION_SIZE, &br);//获取单色位图相关信息
//        if (FR_OK == res)
//        {
//			if (0x4d42 == un_BITMAP_FILE_INFORMATION.st_bmp.bfType)//bmp
//			{
//				if (0x01 == un_BITMAP_FILE_INFORMATION.st_bmp.biPerPixel)//单色位图
//				{
//					//用数据大小除以高度就等于要显示的宽度
//					un_BITMAP_FILE_INFORMATION.st_bmp.biWidth=un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage/un_BITMAP_FILE_INFORMATION.st_bmp.biHeight*8;
//		
//					x =0;
//					//实际图画可用显示在屏幕上的大小
//					show_y_size = LCD_Y_MAX-y64;
//					show_x_size = LCD_X_MAX-x128;
//					//计算图片高度偏移量
//					if (un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>show_y_size)
//						y_offset = un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-show_y_size; 
//					else
//						y_offset = 0;
//					y=un_BITMAP_FILE_INFORMATION.st_bmp.biHeight-1;//图片高度0~63
//					
//					
//					

//					
//					
//					// un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits是图片显示数据的起始地址
//					res = f_lseek(&file, un_BITMAP_FILE_INFORMATION.st_bmp.bfOffBits);
//					
//					

//					while(y64>0)
//					{	//计算图片高度空间是否够
//						if (y64+un_BITMAP_FILE_INFORMATION.st_bmp.biHeight>=64)
//							y64--;
//						else
//							break;
//					}

//					while(x128>0)
//					{	//计算图片宽度空间是否够
//						if (x128+un_BITMAP_FILE_INFORMATION.st_bmp.biWidth>=128)
//							x128--;
//						else
//							break;
//					}
//					
//					for (i=0; i<un_BITMAP_FILE_INFORMATION.st_bmp.biSizeImage; i++)//图片大小
//					{
//						f_read(&file, &bmptmp, 1, &br);//图片的数据坐标是x128=0, y64=63开始

//						x = i*8 / un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;	
//						z = i*8 % un_BITMAP_FILE_INFORMATION.st_bmp.biWidth;
//						for (j=0; j<8; j++)//单色位图只有0/1
//						{
//							if (bmptmp<<j & 0x80)
//								OLED_Set_Pos_cache(x128+z+j, y64+y-x, 0);//从0 63坐标开始
//							else
//								OLED_Set_Pos_cache(x128+z+j, y64+y-x, 1);
//						}
//					}
//				}
//			}
//        }
//        f_close(&file);
//    }
//    f_mount(0, NULL);//注销一个工作区（文件系统对象）
//    return res;
//}

