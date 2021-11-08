
单色位图12864解码

stm32f103C8T6上用SSD1306的oled测度
调用形试如下：
for (i = 0; i < 9999; i++)
{

  memset(buf, 0, 40);
  sprintf(buf, "0:/badapple_a/12864_%04d.bmp", i);
  if (FR_OK != monochrome_bitmap_decoding12864(buf, 0, 0))//第一个图片从x0 y0开始


  memset(buf, 0, 40);
  sprintf(buf, "0:/badapple_b/12864_%04d.bmp", i);
  if (FR_OK != monochrome_bitmap_decoding12864(buf, 64, 0))//第二张图片从X64 Y0开始
  break;


  OLED_ScreenUpdating();
}
