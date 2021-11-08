#ifndef __BMP_decoding_H
#define __BMP_decoding_H
#include "main.h"
#include "fatfs_task.h"
//bmp数据是小端模式
//小端模式，是指数据的高字节保存在内存的高地址中，而数据的低字节保存在内存的低地址中。
#define BITMAP_FILE_INFORMATION_SIZE	38		//位图文件信息要占用的内存

//__packed是字节对齐的意思
typedef struct//单色位图解码结构体
{
	__packed uint16_t bfType;			//0000h 文件标识 2 bytes 两字节的内容用来识别位图的类型
	__packed uint32_t bfSize;			//0002h File Size 4 bytes 用字节表示的整个文件的大小
	__packed uint32_t bfReserved1;		//0006h Reserved 4 bytes 保留，必须设置为0
	__packed uint32_t bfOffBits;		//000Ah Bitmap Data Offset 4 bytes 从文件开始到位图数据开始之间的数据（bitmap data）之间的偏移量
	__packed uint32_t bfheader;			//000Eh Bitmap Header Size 4 bytes位图信息头
    __packed uint32_t biWidth;			//0012h Width 4 bytes位图的宽度，以像素为单位
    __packed uint32_t biHeight;			//0016h Height 4 bytes位图的高度，以像素为单位
	__packed uint16_t biPlanes;			//001Ah Planes 2 bytes位图的位面数（注：该值将总是1）
	__packed uint16_t biPerPixel;		//001Ch Bits Per Pixel 2 bytes 每个像素的位数,1 - 单色位图（实际上可有两种颜色，缺省情况下是黑色和白色。你可以自己定义这两种颜色）
	__packed uint32_t biCompression;	//001Eh Compression 4 bytes 压缩说明,0 - 不压缩 （使用BI_RGB表示）
	__packed uint32_t biSizeImage;		//0022h Bitmap Data Size 4 bytes 用字节数表示的位图数据的大小。该数必须是4的倍数
}BITMAP_FILE_INFORMATION;

#define BMP_CECODING_FAST				0

extern FRESULT monochrome_bitmap_decoding12864(const TCHAR *file_path, uint8_t x128, uint8_t y64);
#if (1 == BMP_CECODING_FAST)
extern FRESULT monochrome_bitmap_decoding12864_fast(const TCHAR *file_path, uint8_t x128, uint8_t y64);
#endif


#endif