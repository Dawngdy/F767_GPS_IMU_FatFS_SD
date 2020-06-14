#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "malloc.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//±¾³ÌĞòÖ»¹©Ñ§Ï°Ê¹ÓÃ£¬Î´¾­×÷ÕßĞí¿É£¬²»µÃÓÃÓÚÆäËüÈÎºÎÓÃÍ¾
//ALIENTEK STM32¿ª·¢°å
//FATFS À©Õ¹´úÂë	   
//ÕıµãÔ­×Ó@ALIENTEK
//¼¼ÊõÂÛÌ³:www.openedv.com
//´´½¨ÈÕÆÚ:2016/1/7
//°æ±¾£ºV1.1
//°æÈ¨ËùÓĞ£¬µÁ°æ±Ø¾¿¡£
//Copyright(C) ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾ 2014-2024
//All rights reserved	
//********************************************************************************
//Éı¼¶ËµÃ÷
//V1.1
//ĞŞÕıexf_copyº¯Êı,ÎÄ¼ş½ø¶ÈÏÔÊ¾´íÎóµÄbug
////////////////////////////////////////////////////////////////////////////////// 	
////////////////////////////////////////////////////////////////////////////////// 	
//gdy
//FATFS fatsd;
//FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
//FRESULT fr;
//UINT brs;
//DIR dirobj;    //Ä¿Â¼¶ÔÏó
//FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
/////////////////////////////////////////////

#define FILE_MAX_TYPE_NUM		7	//×î¶àFILE_MAX_TYPE_NUM¸ö´óÀà
#define FILE_MAX_SUBT_NUM		4	//×î¶àFILE_MAX_SUBT_NUM¸öĞ¡Àà

 //ÎÄ¼şÀàĞÍÁĞ±í
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BINÎÄ¼ş
{"LRC"},			//LRCÎÄ¼ş
{"NES","SMS"},		//NES/SMSÎÄ¼ş
{"TXT","C","H"},	//ÎÄ±¾ÎÄ¼ş
{"WAV","MP3","APE","FLAC"},//Ö§³ÖµÄÒôÀÖÎÄ¼ş
{"BMP","JPG","JPEG","GIF"},//Í¼Æ¬ÎÄ¼ş
{"AVI"},			//ÊÓÆµÎÄ¼ş
};
///////////////////////////////¹«¹²ÎÄ¼şÇø,Ê¹ÓÃmallocµÄÊ±ºò////////////////////////////////////////////
FATFS *fs[_VOLUMES];//Âß¼­´ÅÅÌ¹¤×÷Çø.	 
FIL *file;	  		//ÎÄ¼ş1
FIL *ftemp;	  		//ÎÄ¼ş2.
UINT br,bw;			//¶ÁĞ´±äÁ¿
FILINFO fileinfo;	//ÎÄ¼şĞÅÏ¢
DIR dir;  			//Ä¿Â¼

u8 *fatbuf;			//SD¿¨Êı¾İ»º´æÇø
///////////////////////////////////////////////////////////////////////////////////////
//ÎªexfunsÉêÇëÄÚ´æ
//·µ»ØÖµ:0,³É¹¦
//1,Ê§°Ü
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Îª´ÅÅÌi¹¤×÷ÇøÉêÇëÄÚ´æ	
		if(!fs[i])break;
	}
	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//ÎªfileÉêÇëÄÚ´æ
	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//ÎªftempÉêÇëÄÚ´æ
	fatbuf=(u8*)mymalloc(SRAMIN,512);				//ÎªfatbufÉêÇëÄÚ´æ
	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //ÉêÇëÓĞÒ»¸öÊ§°Ü,¼´Ê§°Ü.
	else return 1;	
}

//½«Ğ¡Ğ´×ÖÄ¸×ªÎª´óĞ´×ÖÄ¸,Èç¹ûÊÇÊı×Ö,Ôò±£³Ö²»±ä.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//Êı×Ö,±£³Ö²»±ä.
	if(c>='a')return c-0x20;//±äÎª´óĞ´.
	else return c;//´óĞ´,±£³Ö²»±ä
}	      
//±¨¸æÎÄ¼şµÄÀàĞÍ
//fname:ÎÄ¼şÃû
//·µ»ØÖµ:0XFF,±íÊ¾ÎŞ·¨Ê¶±ğµÄÎÄ¼şÀàĞÍ±àºÅ.
//		 ÆäËû,¸ßËÄÎ»±íÊ¾ËùÊô´óÀà,µÍËÄÎ»±íÊ¾ËùÊôĞ¡Àà.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//ºó×ºÃû
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//Æ«ÒÆµ½ÁË×îºóÁË.
		fname++;
	}
	if(i==250)return 0XFF;//´íÎóµÄ×Ö·û´®.
 	for(i=0;i<5;i++)//µÃµ½ºó×ºÃû
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);//copy
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//È«²¿±äÎª´óĞ´ 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//´óÀà¶Ô±È
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//×ÓÀà¶Ô±È
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//´Ë×éÒÑ¾­Ã»ÓĞ¿É¶Ô±ÈµÄ³ÉÔ±ÁË.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//ÕÒµ½ÁË
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//Ã»ÕÒµ½		 			   
}	 

//µÃµ½´ÅÅÌÊ£ÓàÈİÁ¿
//drv:´ÅÅÌ±àºÅ("0:"/"1:")
//total:×ÜÈİÁ¿	 £¨µ¥Î»KB£©
//free:Ê£ÓàÈİÁ¿	 £¨µ¥Î»KB£©
//·µ»ØÖµ:0,Õı³£.ÆäËû,´íÎó´úÂë
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //µÃµ½´ÅÅÌĞÅÏ¢¼°¿ÕÏĞ´ØÊıÁ¿
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//µÃµ½×ÜÉÈÇøÊı
	    fre_sect=fre_clust*fs1->csize;			//µÃµ½¿ÕÏĞÉÈÇøÊı	   
#if _MAX_SS!=512				  				//ÉÈÇø´óĞ¡²»ÊÇ512×Ö½Ú,Ôò×ª»»Îª512×Ö½Ú
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//µ¥Î»ÎªKB
		*free=fre_sect>>1;	//µ¥Î»ÎªKB 
 	}
	return res;
}		   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ÎÄ¼ş¸´ÖÆ
//×¢ÒâÎÄ¼ş´óĞ¡²»Òª³¬¹ı4GB.
//½«psrcÎÄ¼ş,copyµ½pdst.
//fcpymsg,º¯ÊıÖ¸Õë,ÓÃÓÚÊµÏÖ¿½±´Ê±µÄĞÅÏ¢ÏÔÊ¾
//        pname:ÎÄ¼ş/ÎÄ¼ş¼ĞÃû
//        pct:°Ù·Ö±È
//        mode:
//			[0]:¸üĞÂÎÄ¼şÃû
//			[1]:¸üĞÂ°Ù·Ö±Èpct
//			[2]:¸üĞÂÎÄ¼ş¼Ğ
//			[3~7]:±£Áô
//psrc,pdst:Ô´ÎÄ¼şºÍÄ¿±êÎÄ¼ş
//totsize:×Ü´óĞ¡(µ±totsizeÎª0µÄÊ±ºò,±íÊ¾½ö½öÎªµ¥¸öÎÄ¼ş¿½±´)
//cpdsize:ÒÑ¸´ÖÆÁËµÄ´óĞ¡.
//fwmode:ÎÄ¼şĞ´ÈëÄ£Ê½
//0:²»¸²¸ÇÔ­ÓĞµÄÎÄ¼ş
//1:¸²¸ÇÔ­ÓĞµÄÎÄ¼ş
//·µ»ØÖµ:0,Õı³£
//    ÆäËû,´íÎó,0XFF,Ç¿ÖÆÍË³ö
u8 exf_copy(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8 *psrc,u8 *pdst,u32 totsize,u32 cpdsize,u8 fwmode)
{
	u8 res;
    u16 br=0;
	u16 bw=0;
	FIL *fsrc=0;
	FIL *fdst=0;
	u8 *fbuf=0;
	u8 curpct=0;
	unsigned long long lcpdsize=cpdsize; 
 	fsrc=(FIL*)mymalloc(SRAMIN,sizeof(FIL));//ÉêÇëÄÚ´æ
 	fdst=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	fbuf=(u8*)mymalloc(SRAMIN,8192);
  	if(fsrc==NULL||fdst==NULL||fbuf==NULL)res=100;//Ç°ÃæµÄÖµÁô¸øfatfs
	else
	{   
		if(fwmode==0)fwmode=FA_CREATE_NEW;//²»¸²¸Ç
		else fwmode=FA_CREATE_ALWAYS;	  //¸²¸Ç´æÔÚµÄÎÄ¼ş
		 
	 	res=f_open(fsrc,(const TCHAR*)psrc,FA_READ|FA_OPEN_EXISTING);	//´ò¿ªÖ»¶ÁÎÄ¼ş
	 	if(res==0)res=f_open(fdst,(const TCHAR*)pdst,FA_WRITE|fwmode); 	//µÚÒ»¸ö´ò¿ª³É¹¦,²Å¿ªÊ¼´ò¿ªµÚ¶ş¸ö
		if(res==0)//Á½¸ö¶¼´ò¿ª³É¹¦ÁË
		{
			if(totsize==0)//½ö½öÊÇµ¥¸öÎÄ¼ş¸´ÖÆ
			{
				totsize=fsrc->obj.objsize;
				lcpdsize=0;
				curpct=0;
		 	}else curpct=(lcpdsize*100)/totsize;	//µÃµ½ĞÂ°Ù·Ö±È
			fcpymsg(psrc,curpct,0X02);			//¸üĞÂ°Ù·Ö±È
			while(res==0)//¿ªÊ¼¸´ÖÆ
			{
				res=f_read(fsrc,fbuf,8192,(UINT*)&br);	//Ô´Í·¶Á³ö512×Ö½Ú
				if(res||br==0)break;
				res=f_write(fdst,fbuf,(UINT)br,(UINT*)&bw);	//Ğ´ÈëÄ¿µÄÎÄ¼ş
				lcpdsize+=bw;
				if(curpct!=(lcpdsize*100)/totsize)//ÊÇ·ñĞèÒª¸üĞÂ°Ù·Ö±È
				{
					curpct=(lcpdsize*100)/totsize;
					if(fcpymsg(psrc,curpct,0X02))//¸üĞÂ°Ù·Ö±È
					{
						res=0XFF;//Ç¿ÖÆÍË³ö
						break;
					}
				}			     
				if(res||bw<br)break;       
			}
		    f_close(fsrc);
		    f_close(fdst);
		}
	}
	myfree(SRAMIN,fsrc);//ÊÍ·ÅÄÚ´æ
	myfree(SRAMIN,fdst);
	myfree(SRAMIN,fbuf);
	return res;
}

//µÃµ½Â·¾¶ÏÂµÄÎÄ¼ş¼Ğ
//·µ»ØÖµ:0,Â·¾¶¾ÍÊÇ¸ö¾í±êºÅ.
//    ÆäËû,ÎÄ¼ş¼ĞÃû×ÖÊ×µØÖ·
u8* exf_get_src_dname(u8* dpfn)
{
	u16 temp=0;
 	while(*dpfn!=0)
	{
		dpfn++;
		temp++;	
	}
	if(temp<4)return 0; 
	while((*dpfn!=0x5c)&&(*dpfn!=0x2f))dpfn--;	//×·Êöµ½µ¹ÊıµÚÒ»¸ö"\"»òÕß"/"´¦ 
	return ++dpfn;
}
//µÃµ½ÎÄ¼ş¼Ğ´óĞ¡
//×¢ÒâÎÄ¼ş¼Ğ´óĞ¡²»Òª³¬¹ı4GB.
//·µ»ØÖµ:0,ÎÄ¼ş¼Ğ´óĞ¡Îª0,»òÕß¶ÁÈ¡¹ı³ÌÖĞ·¢ÉúÁË´íÎó.
//    ÆäËû,ÎÄ¼ş¼Ğ´óĞ¡.
u32 exf_fdsize(u8 *fdname)
{
#define MAX_PATHNAME_DEPTH	512+1	//×î´óÄ¿±êÎÄ¼şÂ·¾¶+ÎÄ¼şÃûÉî¶È
	u8 res=0;	  
    DIR *fddir=0;		//Ä¿Â¼
	FILINFO *finfo=0;	//ÎÄ¼şĞÅÏ¢
	u8 * pathname=0;	//Ä¿±êÎÄ¼ş¼ĞÂ·¾¶+ÎÄ¼şÃû
 	u16 pathlen=0;		//Ä¿±êÂ·¾¶³¤¶È
	u32 fdsize=0;

	fddir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));//ÉêÇëÄÚ´æ
 	finfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
   	if(fddir==NULL||finfo==NULL)res=100;
	if(res==0)
	{ 
 		pathname=mymalloc(SRAMIN,MAX_PATHNAME_DEPTH);	    
 		if(pathname==NULL)res=101;	   
 		if(res==0)
		{
			pathname[0]=0;	    
			strcat((char*)pathname,(const char*)fdname); //¸´ÖÆÂ·¾¶	
		    res=f_opendir(fddir,(const TCHAR*)fdname); 		//´ò¿ªÔ´Ä¿Â¼
		    if(res==0)//´ò¿ªÄ¿Â¼³É¹¦ 
			{														   
				while(res==0)//¿ªÊ¼¸´ÖÆÎÄ¼ş¼ĞÀïÃæµÄ¶«¶«
				{
			        res=f_readdir(fddir,finfo);						//¶ÁÈ¡Ä¿Â¼ÏÂµÄÒ»¸öÎÄ¼ş
			        if(res!=FR_OK||finfo->fname[0]==0)break;		//´íÎóÁË/µ½Ä©Î²ÁË,ÍË³ö
			        if(finfo->fname[0]=='.')continue;     			//ºöÂÔÉÏ¼¶Ä¿Â¼
					if(finfo->fattrib&0X10)//ÊÇ×ÓÄ¿Â¼(ÎÄ¼şÊôĞÔ,0X20,¹éµµÎÄ¼ş;0X10,×ÓÄ¿Â¼;)
					{
 						pathlen=strlen((const char*)pathname);		//µÃµ½µ±Ç°Â·¾¶µÄ³¤¶È
						strcat((char*)pathname,(const char*)"/");	//¼ÓĞ±¸Ü
						strcat((char*)pathname,(const char*)finfo->fname);	//Ô´Â·¾¶¼ÓÉÏ×ÓÄ¿Â¼Ãû×Ö
 						//printf("\r\nsub folder:%s\r\n",pathname);	//´òÓ¡×ÓÄ¿Â¼Ãû
						fdsize+=exf_fdsize(pathname);				//µÃµ½×ÓÄ¿Â¼´óĞ¡,µİ¹éµ÷ÓÃ
						pathname[pathlen]=0;						//¼ÓÈë½áÊø·û
					}else fdsize+=finfo->fsize;						//·ÇÄ¿Â¼,Ö±½Ó¼ÓÉÏÎÄ¼şµÄ´óĞ¡
						
				} 
		    }	  
  			myfree(SRAMIN,pathname);	     
		}
 	}
	myfree(SRAMIN,fddir);    
	myfree(SRAMIN,finfo);
	if(res)return 0;
	else return fdsize;
}	  
//ÎÄ¼ş¼Ğ¸´ÖÆ
//×¢ÒâÎÄ¼ş¼Ğ´óĞ¡²»Òª³¬¹ı4GB.
//½«psrcÎÄ¼ş¼Ğ,copyµ½pdstÎÄ¼ş¼Ğ.
//pdst:±ØĞëĞÎÈç"X:"/"X:XX"/"X:XX/XX"Ö®ÀàµÄ.¶øÇÒÒªÊµÏÖÈ·ÈÏÉÏÒ»¼¶ÎÄ¼ş¼Ğ´æÔÚ
//fcpymsg,º¯ÊıÖ¸Õë,ÓÃÓÚÊµÏÖ¿½±´Ê±µÄĞÅÏ¢ÏÔÊ¾
//        pname:ÎÄ¼ş/ÎÄ¼ş¼ĞÃû
//        pct:°Ù·Ö±È
//        mode:
//			[0]:¸üĞÂÎÄ¼şÃû
//			[1]:¸üĞÂ°Ù·Ö±Èpct
//			[2]:¸üĞÂÎÄ¼ş¼Ğ
//			[3~7]:±£Áô
//psrc,pdst:Ô´ÎÄ¼ş¼ĞºÍÄ¿±êÎÄ¼ş¼Ğ
//totsize:×Ü´óĞ¡(µ±totsizeÎª0µÄÊ±ºò,±íÊ¾½ö½öÎªµ¥¸öÎÄ¼ş¿½±´)
//cpdsize:ÒÑ¸´ÖÆÁËµÄ´óĞ¡.
//fwmode:ÎÄ¼şĞ´ÈëÄ£Ê½
//0:²»¸²¸ÇÔ­ÓĞµÄÎÄ¼ş
//1:¸²¸ÇÔ­ÓĞµÄÎÄ¼ş
//·µ»ØÖµ:0,³É¹¦
//    ÆäËû,´íÎó´úÂë;0XFF,Ç¿ÖÆÍË³ö
u8 exf_fdcopy(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8 *psrc,u8 *pdst,u32 *totsize,u32 *cpdsize,u8 fwmode)
{
#define MAX_PATHNAME_DEPTH	512+1	//×î´óÄ¿±êÎÄ¼şÂ·¾¶+ÎÄ¼şÃûÉî¶È
	u8 res=0;	  
    DIR *srcdir=0;		//Ô´Ä¿Â¼
	DIR *dstdir=0;		//Ô´Ä¿Â¼
	FILINFO *finfo=0;	//ÎÄ¼şĞÅÏ¢
	u8 *fn=0;   		//³¤ÎÄ¼şÃû

	u8 * dstpathname=0;	//Ä¿±êÎÄ¼ş¼ĞÂ·¾¶+ÎÄ¼şÃû
	u8 * srcpathname=0;	//Ô´ÎÄ¼ş¼ĞÂ·¾¶+ÎÄ¼şÃû
	
 	u16 dstpathlen=0;	//Ä¿±êÂ·¾¶³¤¶È
 	u16 srcpathlen=0;	//Ô´Â·¾¶³¤¶È

  
	srcdir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));//ÉêÇëÄÚ´æ
 	dstdir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));
	finfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));

   	if(srcdir==NULL||dstdir==NULL||finfo==NULL)res=100;
	if(res==0)
	{ 
 		dstpathname=mymalloc(SRAMIN,MAX_PATHNAME_DEPTH);
		srcpathname=mymalloc(SRAMIN,MAX_PATHNAME_DEPTH);
 		if(dstpathname==NULL||srcpathname==NULL)res=101;	   
 		if(res==0)
		{
			dstpathname[0]=0;
			srcpathname[0]=0;
			strcat((char*)srcpathname,(const char*)psrc); 	//¸´ÖÆÔ­Ê¼Ô´ÎÄ¼şÂ·¾¶	
			strcat((char*)dstpathname,(const char*)pdst); 	//¸´ÖÆÔ­Ê¼Ä¿±êÎÄ¼şÂ·¾¶	
		    res=f_opendir(srcdir,(const TCHAR*)psrc); 		//´ò¿ªÔ´Ä¿Â¼
		    if(res==0)//´ò¿ªÄ¿Â¼³É¹¦ 
			{
  				strcat((char*)dstpathname,(const char*)"/");//¼ÓÈëĞ±¸Ü
 				fn=exf_get_src_dname(psrc);
				if(fn==0)//¾í±ê¿½±´
				{
					dstpathlen=strlen((const char*)dstpathname);
					dstpathname[dstpathlen]=psrc[0];	//¼ÇÂ¼¾í±ê
					dstpathname[dstpathlen+1]=0;		//½áÊø·û 
				}else strcat((char*)dstpathname,(const char*)fn);//¼ÓÎÄ¼şÃû		
 				fcpymsg(fn,0,0X04);//¸üĞÂÎÄ¼ş¼ĞÃû
				res=f_mkdir((const TCHAR*)dstpathname);//Èç¹ûÎÄ¼ş¼ĞÒÑ¾­´æÔÚ,¾Í²»´´½¨.Èç¹û²»´æÔÚ¾Í´´½¨ĞÂµÄÎÄ¼ş¼Ğ.
				if(res==FR_EXIST)res=0;
				while(res==0)//¿ªÊ¼¸´ÖÆÎÄ¼ş¼ĞÀïÃæµÄ¶«¶«
				{
			        res=f_readdir(srcdir,finfo);					//¶ÁÈ¡Ä¿Â¼ÏÂµÄÒ»¸öÎÄ¼ş
			        if(res!=FR_OK||finfo->fname[0]==0)break;		//´íÎóÁË/µ½Ä©Î²ÁË,ÍË³ö
			        if(finfo->fname[0]=='.')continue;     			//ºöÂÔÉÏ¼¶Ä¿Â¼
					fn=(u8*)finfo->fname; 							//µÃµ½ÎÄ¼şÃû
					dstpathlen=strlen((const char*)dstpathname);	//µÃµ½µ±Ç°Ä¿±êÂ·¾¶µÄ³¤¶È
					srcpathlen=strlen((const char*)srcpathname);	//µÃµ½Ô´Â·¾¶³¤¶È

					strcat((char*)srcpathname,(const char*)"/");//Ô´Â·¾¶¼ÓĞ±¸Ü
 					if(finfo->fattrib&0X10)//ÊÇ×ÓÄ¿Â¼(ÎÄ¼şÊôĞÔ,0X20,¹éµµÎÄ¼ş;0X10,×ÓÄ¿Â¼;)
					{
						strcat((char*)srcpathname,(const char*)fn);		//Ô´Â·¾¶¼ÓÉÏ×ÓÄ¿Â¼Ãû×Ö
						res=exf_fdcopy(fcpymsg,srcpathname,dstpathname,totsize,cpdsize,fwmode);	//¿½±´ÎÄ¼ş¼Ğ
					}else //·ÇÄ¿Â¼
					{
						strcat((char*)dstpathname,(const char*)"/");//Ä¿±êÂ·¾¶¼ÓĞ±¸Ü
						strcat((char*)dstpathname,(const char*)fn);	//Ä¿±êÂ·¾¶¼ÓÎÄ¼şÃû
						strcat((char*)srcpathname,(const char*)fn);	//Ô´Â·¾¶¼ÓÎÄ¼şÃû
 						fcpymsg(fn,0,0X01);//¸üĞÂÎÄ¼şÃû
						res=exf_copy(fcpymsg,srcpathname,dstpathname,*totsize,*cpdsize,fwmode);//¸´ÖÆÎÄ¼ş
						*cpdsize+=finfo->fsize;//Ôö¼ÓÒ»¸öÎÄ¼ş´óĞ¡
					}
					srcpathname[srcpathlen]=0;//¼ÓÈë½áÊø·û
					dstpathname[dstpathlen]=0;//¼ÓÈë½áÊø·û	    
				} 
		    }	  
  			myfree(SRAMIN,dstpathname);
 			myfree(SRAMIN,srcpathname); 
		}
 	}
	myfree(SRAMIN,srcdir);
	myfree(SRAMIN,dstdir);
	myfree(SRAMIN,finfo);
    return res;	  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//´ò¿ªÂ·¾¶ÎªfilepathµÄÎÄ¼ş£¬²¢½«ÎÄ¼şµÄÄÚÈİ±£´æÔÚfdatasÖĞ

void printffile(u8 * filepath)
{
FATFS fatsd;
FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
FRESULT fr;
UINT brs;
DIR dirobj;    //Ä¿Â¼¶ÔÏó
FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	u8 fdatas[255];   //±£´æÓÃµÄÊı×é 
	u32 size=0;
	//1¡¢Ïë´ò¿ªÎÄ¼ş
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);   //£¨ÎÄ¼ş¶ÔÏó,´ò¿ªÎÄ¼şÂ·¾¶£¬´ò¿ªµÄÄ£Ê½£©
	if(fr==FR_OK)
	{
		//2¡¢Ê¹ÓÃÎÄ¼ş¶ÔÏófileobj¹ØÁªÆğÀ´£¬½«ÉÏÃæ´ò¿ªµÄÎÄ¼şÀïÃæµÄÊı¾İ¶¼³öÀ´ºó±£´æÔÚfdatasÖĞ
		size=f_size(&fileobj);  //»ñÈ¡ÎÄ¼şµÄ´óĞ¡
		f_read(&fileobj,fdatas,size,&brs);    //(ÎÄ¼ş¶ÔÏó,±£´æ¶Á³öÊı¾İÓÃµÄbuff£¬¶Á¶àÉÙ¸öÊı¾İ£¬Ö¸ÏòÊı¾İ¸öÊıµÄÖ¸Õë¬)
	}
	f_close(&fileobj);  //3¡¢¹Ø±ÕÕâ¸öÎÄ¼ş¶ÔÏó
	//ÎÄ¼ş±£´æÔÚfdatasÖĞ
	printf("------------------------\r\n");	
	printf("File Path : %s \r\n",filepath);
	printf("File Content : %s\r\n",fdatas);
	printf("------------------------\r\n");	
}
//´ÓÎÄ¼şµÄ¿ªÍ·½øĞĞĞ´datasÖĞµÄÊı¾İ£¬³¤¶Èlength
void testWrite(u8 * filepath,u8 * datas,u32 length)
{
	FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	if(fr==FR_OK)
	{
		fr=f_write(&fileobj,datas,length,&brs);
	}
	f_close(&fileobj);
	printffile(filepath);  //½«Ğ´½øÈ¥µÄÄÚÈİ´òÓ¡³öÀ´£¬¿´Ğ´µÄ¶Ô²»¶Ô
}
//´ÓÎÄ¼şÖ¸¶¨Î»ÖÃ¿ªÊ¼Ğ´
void testSeek(u8 * filepath,u32 index,u8 *datas,u32 length)
{ 
		FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	f_lseek(&fileobj, index);//ÒÆ¶¯¹â±êÖ¸ÕëÖÁindexÎ»ÖÃ£¬´ËÊ±fileobjÖĞ±£´æÁË¹â±êÖ¸ÕëÎ»ÖÃ
	// f_lseek(&fileobj,f_size(&fileobj)); //½«Ö¸ÕëÖ¸ÏòÎÄ¼şÄ©Î²
	fr=f_write(&fileobj,datas,length,&brs);   //
	f_close(&fileobj);
	printffile(filepath);
}
//¶Á    Ê¹ÓÃf_gets´ÓÎÄ¼şÖĞ¶ÁÈ¡Ò»¸ö×Ö·û´®£¬f_getsÊÇ¶ÁÈ¡×Ö·û´®£¬¶øf_read²»Çø·ÖÀàĞÍ
void testGets(u8 * filepath)
{	
		FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	u8 fdatas[255];
	u32 size=0;
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	size=f_size(&fileobj);
	f_gets((TCHAR*)fdatas,size,&fileobj);  //f_gets¶ÁÈ¡×Ö·û´®£¨±£´æ¶Á³öÊı¾İÓÃµÄbuff£¬¶Á¶àÉÙ¸öÊı¾İ£¬ÎÄ¼ş¶ÔÏó£©
	f_close(&fileobj);
	printf("Test Gets: %s \r\n",fdatas);
}
//Ğ´Ò»¸ö×Ö·û/×Ö·û´®µ½ÎÄ¼ş
void testPutsPutc(u8 * filepath,u8 *sDatas,u8 cdata)  //£¨´ı²Ù×÷ÎÄ¼şÂ·¾¶£¬×Ö·û´®£¬×Ö·û£©
{
		FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	f_puts((const TCHAR*)sDatas,&fileobj);   //f_putsĞ´Ò»¸ö×Ö·û´®µ½ÎÄ¼ş
	f_putc((TCHAR)cdata,&fileobj);   //f_putcĞ´Ò»¸ö×Ö·ûµ½ÎÄ¼ş
	f_close(&fileobj);
	printffile(filepath);
}
//Ğ´   f_printf :   ¸ñÊ½»¯Ğ´Ò»¸östringµ½ÎÄ¼şÖĞ
void testFprintf(u8 * filepath)
{
		FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	f_printf(&fileobj, "%d", 1234);            /* "1234" */   
	f_printf(&fileobj, "%6d,%3d%%", -200, 5);  /* "  -200,  5%" */
    f_printf(&fileobj, "%ld", 12345L);         /* "12345" */
	
	f_close(&fileobj);
	printffile(filepath);
	
}

//Ä¿Â¼·ÃÎÊ²Ù×÷µÄº¯Êı   f_opendir    f_readdir       f_closedir 
//ÎÄ¼şÉ¨Ãè£¬´òÓ¡µ±Ç°Â·¾¶dirpathÏÂµÄÎÄ¼şÄ¿Â¼
void testDirScan(u8 * dirpath)
{
		FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	int index=0;
	fr=f_opendir(&dirobj,(const TCHAR*)dirpath);
	
	if(fr==FR_OK)
	{
		while(1)
		{
			fr=f_readdir(&dirobj,&fileinfoobj);  //£¨Ä¿Â¼¶ÔÏó£¬±£´æÄ¿Â¼ĞÅÏ¢£©  
			if(fr!=FR_OK||fileinfoobj.fname[0]==0) break;

			 printf("filename %d = %s\r\n",index,(u8 *)fileinfoobj.fname);  //Ë÷ÒıºÅindex
			 index++;
		}
		
	}
	f_closedir(&dirobj);
}
//f_mkdir  ĞÂ½¨Ä¿Â¼     f_unlink É¾³ıÄ¿Â¼     f_rename Ä¿Â¼ÖØÃüÃû 
void testMK_UNlinkDir(void)
{
		FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
	DIR recdir;	
	f_mkdir("0:/123");   //ĞÂ½¨Ä¿Â¼ÎÄ¼ş
	f_mkdir("0:/123/Bedgy");
	f_mkdir("0:/123/DEFRGTH");	
//	testDirScan("0:/AAAA");
//	
//	f_unlink("0:/AAAA/CCCC");   //f_unlink É¾³ıÄ¿Â¼¡°0:/AAAA/CCCC¡±
//	testDirScan("0:/AAAA");
//	
//	f_rename("0:/AAAA/BBBB","0:/AAAA/BBCC");  //½« Ç°ÃæµÄÂ·¾­ ÖØÃüÃûÎª ºóÃæµÄÂ·¾¶
//	testDirScan("0:/AAAA");
	
}

//ÎÄ¼ş²éÕÒ   ´ÓÄ¿Â¼dirpathÖĞ²éÕÒÎÄ¼şfilenamePattern
void testfindFirst(u8 * dirpath,u8 * filenamePattern)
{
		FATFS fatsd;
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ä¿Â¼¶ÔÏó
	FILINFO fileinfoobj;  //ÓÃÀ´±£´æÄ¿Â¼ÎÄ¼şĞÅÏ¢
/*×¢Òâ£¬ÕâÁ½¸öº¯ÊıÊ¹ÓÃ£¬±ØĞëÉèÖÃffconf.hÀïÃæµÄ_USE_FIND±êÊ¶·ûµÄÖµÎª1 £¬Ò²¾ÍÊÇ£º#define _USE_FIND 1  */
 
	fr = f_findfirst(&dirobj, &fileinfoobj, (const TCHAR*)dirpath, (const TCHAR*)filenamePattern);
    while (fr == FR_OK && fileinfoobj.fname[0]) 
	{         /* Repeat while an item is found */
        printf("matched:%s\r\n", fileinfoobj.fname);                /* Display the object name */
        fr = f_findnext(&dirobj, &fileinfoobj);               /* Search for next item */
  }
    f_closedir(&dirobj);
}


//////////////
void sd_creatfile(void)
{
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	DIR recdir;
	FRESULT fr;
	char *sDatas ="-------------------system reset-----------------\r\n";

		while(f_opendir(&recdir,"0:/SaveData"))//´ò¿ªÎÄ¼ş¼Ğ
 	{	
		f_mkdir("0:/SaveData");				//´´½¨¸ÃÄ¿Â¼   
		f_open(&fileobj, "0:SaveData/GpsData.txt", FA_CREATE_NEW);			//´´½¨¸ÃÄ¿Â¼ 
		f_open(&fileobj, "0:SaveData/ImuData.txt", FA_CREATE_NEW);			//´´½¨¸ÃÄ¿Â¼ 		 
	}
	//ÍùGpsData.txtÖĞĞ´ÈëÖØĞÂÉÏµç¿ªÍ·
		fr=f_open(&fileobj,"0:SaveData/GpsData.txt",FA_READ|FA_WRITE);
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//½«Ö¸ÕëÖ¸ÏòÎÄ¼şÄ©Î²
		f_puts((const TCHAR*)sDatas,&fileobj);   //f_puts	
	}
	f_close(&fileobj);
		//ÍùImuData.txtÖĞĞ´ÈëÖØĞÂÉÏµç¿ªÍ·
		fr=f_open(&fileobj,"0:SaveData/ImuData.txt",FA_READ|FA_WRITE);
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//½«Ö¸ÕëÖ¸ÏòÎÄ¼şÄ©Î²
		f_puts((const TCHAR*)sDatas,&fileobj);   //f_puts	
	}
	f_close(&fileobj);
	
}


//void sd_savegps(u8 * filepath,u8 * datas,u32 length)
void sd_savegps(u8 * filepath,u8 * datas)
{
	FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
	FRESULT fr;
	UINT brs;
	DIR recdir;	
//	const u8 newline[]={0X0D,0X0A}; 
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);   //£¨ÎÄ¼ş¶ÔÏó,´ò¿ªÎÄ¼şÂ·¾¶£¬´ò¿ªµÄÄ£Ê½£©
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//½«Ö¸ÕëÖ¸ÏòÎÄ¼şÄ©Î²
		//fr=f_write(&fileobj,datas,length,&brs);
		f_printf(&fileobj, "%s\n", datas);   //Ê¹ÓÃf_printf½â¾öÁËºóÃæÂÒÂëµÄÎÊÌâ
	//	f_lseek(&fileobj,f_size(&fileobj)); //½«Ö¸ÕëÖ¸ÏòÎÄ¼şÄ©Î²
  //	f_write(&fileobj, newline,2, &brs);  //»»ĞĞ
	//	f_printf(&fileobj, "%s\n", newline); 
	}
		f_close(&fileobj);
}

//sd_saveimu("0:SaveData/GpsData.txt",imu_massage2->imu_data);
void sd_saveimu(u8 * filepath,u8 * datas)
{
		FIL fileobj;   //ÎÄ¼ş¶ÔÏó£¬±£´æµ±Ç°²Ù×÷µÄÎÄ¼ş×´¿ö
		FRESULT fr;
		UINT brs;
		DIR recdir;
		fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);   //£¨ÎÄ¼ş¶ÔÏó,´ò¿ªÎÄ¼şÂ·¾¶£¬´ò¿ªµÄÄ£Ê½£©
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//½«Ö¸ÕëÖ¸ÏòÎÄ¼şÄ©Î²
		f_printf(&fileobj, " ACC_DATA X: %d,   Y: %d   Z:%d\n",datas[0],datas[1],datas[2]);   //Ê¹ÓÃf_printf½â¾öÁËºóÃæÂÒÂëµÄÎÊÌâ
		f_printf(&fileobj, " GYR_DATA X: %d,   Y: %d   Z:%d\n",datas[3],datas[4],datas[5]);   		
	}
		f_close(&fileobj);
}









