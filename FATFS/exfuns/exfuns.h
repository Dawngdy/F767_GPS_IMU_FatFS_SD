#ifndef __EXFUNS_H
#define __EXFUNS_H 					   
#include <sys.h>
#include "ff.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//FATFS 扩展代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/7
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved	
//********************************************************************************
//升级说明
//V1.1
//修正exf_copy函数,文件进度显示错误的bug

extern FATFS *fs[_VOLUMES];  
extern FIL *file;	 
extern FIL *ftemp;	 
extern UINT br,bw;
extern FILINFO fileinfo;
extern DIR dir;
//////////////
extern u8 *fatbuf;//SD卡数据缓存区
extern FIL fileobj;   //文件对象，保存当前操作的文件状况
extern FRESULT fr;
extern UINT brs;
extern DIR dirobj;    //目录对象
extern FILINFO fileinfoobj;  //用来保存目录文件信息


//f_typetell返回的类型定义
//根据表FILE_TYPE_TBL获得.在exfuns.c里面定义
#define T_BIN		0X00	//bin文件
#define T_LRC		0X10	//lrc文件

#define T_NES		0X20	//nes文件
#define T_SMS		0X21	//sms文件

#define T_TEXT		0X30	//.txt文件
#define T_C			0X31	//.c文件
#define T_H			0X32    //.h文件

#define T_WAV		0X40	//WAV文件
#define T_MP3		0X41	//MP3文件 
#define T_APE		0X42	//APE文件
#define T_FLAC		0X43	//FLAC文件

#define T_BMP		0X50	//bmp文件
#define T_JPG		0X51	//jpg文件
#define T_JPEG		0X52	//jpeg文件		 
#define T_GIF		0X53	//gif文件  
 
#define T_AVI		0X60	//avi文件  

 
u8 exfuns_init(void);							//申请内存
u8 f_typetell(u8 *fname);						//识别文件类型
u8 exf_getfree(u8 *drv,u32 *total,u32 *free);	//得到磁盘总容量和剩余容量
u32 exf_fdsize(u8 *fdname);						//得到文件夹大小	
u8* exf_get_src_dname(u8* dpfn);																		   
u8 exf_copy(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8 *psrc,u8 *pdst,u32 totsize,u32 cpdsize,u8 fwmode);	   //文件复制
u8 exf_fdcopy(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8 *psrc,u8 *pdst,u32 *totsize,u32 *cpdsize,u8 fwmode);//文件夹复制	  

//打开路径为filepath的文件，并将文件的内容保存在fdatas中
void printffile(u8 * filepath);
//从文件的开头进行写datas中的数据，长度length
void testWrite(u8 * filepath,u8 * datas,u32 length);
//从文件指定位置开始写
void testSeek(u8 * filepath,u32 index,u8 *datas,u32 length);
//读    使用f_gets从文件中读取一个字符串，f_gets是读取字符串，而f_read不区分类型
void testGets(u8 * filepath);
//写一个字符/字符串到文件
void testPutsPutc(u8 * filepath,u8 *sDatas,u8 cdata);  //（待操作文件路径，字符串，字符）;
//写   f_printf :   格式化写一个string到文件中
void testFprintf(u8 * filepath);
//目录访问操作的函数   f_opendir    f_readdir       f_closedir 
//文件扫描，打印当前路径dirpath下的文件目录
void testDirScan(u8 * dirpath);
//f_mkdir  新建目录     f_unlink 删除目录     f_rename 目录重命名 
void testMK_UNlinkDir(void);
//文件查找   从目录dirpath中查找文件filenamePattern
void testfindFirst(u8 * dirpath,u8 * filenamePattern);
//创建txt文件
void sd_creatfile();
//保存GPS数据
void sd_savegps(u8 * filepath,u8 * datas);
//保存IMU数据，定制输出
void sd_saveimu(u8 * filepath,u8 * datas);
#endif


