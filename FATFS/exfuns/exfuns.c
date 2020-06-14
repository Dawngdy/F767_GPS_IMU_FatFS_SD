#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "malloc.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//FATFS ��չ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/7
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved	
//********************************************************************************
//����˵��
//V1.1
//����exf_copy����,�ļ�������ʾ�����bug
////////////////////////////////////////////////////////////////////////////////// 	
////////////////////////////////////////////////////////////////////////////////// 	
//gdy
//FATFS fatsd;
//FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
//FRESULT fr;
//UINT brs;
//DIR dirobj;    //Ŀ¼����
//FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
/////////////////////////////////////////////

#define FILE_MAX_TYPE_NUM		7	//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		4	//���FILE_MAX_SUBT_NUM��С��

 //�ļ������б�
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			//BIN�ļ�
{"LRC"},			//LRC�ļ�
{"NES","SMS"},		//NES/SMS�ļ�
{"TXT","C","H"},	//�ı��ļ�
{"WAV","MP3","APE","FLAC"},//֧�ֵ������ļ�
{"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
{"AVI"},			//��Ƶ�ļ�
};
///////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
FATFS *fs[_VOLUMES];//�߼����̹�����.	 
FIL *file;	  		//�ļ�1
FIL *ftemp;	  		//�ļ�2.
UINT br,bw;			//��д����
FILINFO fileinfo;	//�ļ���Ϣ
DIR dir;  			//Ŀ¼

u8 *fatbuf;			//SD�����ݻ�����
///////////////////////////////////////////////////////////////////////////////////////
//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
u8 exfuns_init(void)
{
	u8 i;
	for(i=0;i<_VOLUMES;i++)
	{
		fs[i]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Ϊ����i�����������ڴ�	
		if(!fs[i])break;
	}
	file=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊfile�����ڴ�
	ftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));		//Ϊftemp�����ڴ�
	fatbuf=(u8*)mymalloc(SRAMIN,512);				//Ϊfatbuf�����ڴ�
	if(i==_VOLUMES&&file&&ftemp&&fatbuf)return 0;  //������һ��ʧ��,��ʧ��.
	else return 1;	
}

//��Сд��ĸתΪ��д��ĸ,���������,�򱣳ֲ���.
u8 char_upper(u8 c)
{
	if(c<'A')return c;//����,���ֲ���.
	if(c>='a')return c-0x20;//��Ϊ��д.
	else return c;//��д,���ֲ���
}	      
//�����ļ�������
//fname:�ļ���
//����ֵ:0XFF,��ʾ�޷�ʶ����ļ����ͱ��.
//		 ����,����λ��ʾ��������,����λ��ʾ����С��.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';//��׺��
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;//ƫ�Ƶ��������.
		fname++;
	}
	if(i==250)return 0XFF;//������ַ���.
 	for(i=0;i<5;i++)//�õ���׺��
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
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);//ȫ����Ϊ��д 
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//����Ա�
	{
		for(j=0;j<FILE_MAX_SUBT_NUM;j++)//����Ա�
		{
			if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
			if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
			{
				return (i<<4)|j;
			}
		}
	}
	return 0XFF;//û�ҵ�		 			   
}	 

//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
	    fre_sect=fre_clust*fs1->csize;			//�õ�����������	   
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	//��λΪKB
		*free=fre_sect>>1;	//��λΪKB 
 	}
	return res;
}		   
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�ļ�����
//ע���ļ���С��Ҫ����4GB.
//��psrc�ļ�,copy��pdst.
//fcpymsg,����ָ��,����ʵ�ֿ���ʱ����Ϣ��ʾ
//        pname:�ļ�/�ļ�����
//        pct:�ٷֱ�
//        mode:
//			[0]:�����ļ���
//			[1]:���°ٷֱ�pct
//			[2]:�����ļ���
//			[3~7]:����
//psrc,pdst:Դ�ļ���Ŀ���ļ�
//totsize:�ܴ�С(��totsizeΪ0��ʱ��,��ʾ����Ϊ�����ļ�����)
//cpdsize:�Ѹ����˵Ĵ�С.
//fwmode:�ļ�д��ģʽ
//0:������ԭ�е��ļ�
//1:����ԭ�е��ļ�
//����ֵ:0,����
//    ����,����,0XFF,ǿ���˳�
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
 	fsrc=(FIL*)mymalloc(SRAMIN,sizeof(FIL));//�����ڴ�
 	fdst=(FIL*)mymalloc(SRAMIN,sizeof(FIL));
	fbuf=(u8*)mymalloc(SRAMIN,8192);
  	if(fsrc==NULL||fdst==NULL||fbuf==NULL)res=100;//ǰ���ֵ����fatfs
	else
	{   
		if(fwmode==0)fwmode=FA_CREATE_NEW;//������
		else fwmode=FA_CREATE_ALWAYS;	  //���Ǵ��ڵ��ļ�
		 
	 	res=f_open(fsrc,(const TCHAR*)psrc,FA_READ|FA_OPEN_EXISTING);	//��ֻ���ļ�
	 	if(res==0)res=f_open(fdst,(const TCHAR*)pdst,FA_WRITE|fwmode); 	//��һ���򿪳ɹ�,�ſ�ʼ�򿪵ڶ���
		if(res==0)//�������򿪳ɹ���
		{
			if(totsize==0)//�����ǵ����ļ�����
			{
				totsize=fsrc->obj.objsize;
				lcpdsize=0;
				curpct=0;
		 	}else curpct=(lcpdsize*100)/totsize;	//�õ��°ٷֱ�
			fcpymsg(psrc,curpct,0X02);			//���°ٷֱ�
			while(res==0)//��ʼ����
			{
				res=f_read(fsrc,fbuf,8192,(UINT*)&br);	//Դͷ����512�ֽ�
				if(res||br==0)break;
				res=f_write(fdst,fbuf,(UINT)br,(UINT*)&bw);	//д��Ŀ���ļ�
				lcpdsize+=bw;
				if(curpct!=(lcpdsize*100)/totsize)//�Ƿ���Ҫ���°ٷֱ�
				{
					curpct=(lcpdsize*100)/totsize;
					if(fcpymsg(psrc,curpct,0X02))//���°ٷֱ�
					{
						res=0XFF;//ǿ���˳�
						break;
					}
				}			     
				if(res||bw<br)break;       
			}
		    f_close(fsrc);
		    f_close(fdst);
		}
	}
	myfree(SRAMIN,fsrc);//�ͷ��ڴ�
	myfree(SRAMIN,fdst);
	myfree(SRAMIN,fbuf);
	return res;
}

//�õ�·���µ��ļ���
//����ֵ:0,·�����Ǹ�����.
//    ����,�ļ��������׵�ַ
u8* exf_get_src_dname(u8* dpfn)
{
	u16 temp=0;
 	while(*dpfn!=0)
	{
		dpfn++;
		temp++;	
	}
	if(temp<4)return 0; 
	while((*dpfn!=0x5c)&&(*dpfn!=0x2f))dpfn--;	//׷����������һ��"\"����"/"�� 
	return ++dpfn;
}
//�õ��ļ��д�С
//ע���ļ��д�С��Ҫ����4GB.
//����ֵ:0,�ļ��д�СΪ0,���߶�ȡ�����з����˴���.
//    ����,�ļ��д�С.
u32 exf_fdsize(u8 *fdname)
{
#define MAX_PATHNAME_DEPTH	512+1	//���Ŀ���ļ�·��+�ļ������
	u8 res=0;	  
    DIR *fddir=0;		//Ŀ¼
	FILINFO *finfo=0;	//�ļ���Ϣ
	u8 * pathname=0;	//Ŀ���ļ���·��+�ļ���
 	u16 pathlen=0;		//Ŀ��·������
	u32 fdsize=0;

	fddir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));//�����ڴ�
 	finfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));
   	if(fddir==NULL||finfo==NULL)res=100;
	if(res==0)
	{ 
 		pathname=mymalloc(SRAMIN,MAX_PATHNAME_DEPTH);	    
 		if(pathname==NULL)res=101;	   
 		if(res==0)
		{
			pathname[0]=0;	    
			strcat((char*)pathname,(const char*)fdname); //����·��	
		    res=f_opendir(fddir,(const TCHAR*)fdname); 		//��ԴĿ¼
		    if(res==0)//��Ŀ¼�ɹ� 
			{														   
				while(res==0)//��ʼ�����ļ�������Ķ���
				{
			        res=f_readdir(fddir,finfo);						//��ȡĿ¼�µ�һ���ļ�
			        if(res!=FR_OK||finfo->fname[0]==0)break;		//������/��ĩβ��,�˳�
			        if(finfo->fname[0]=='.')continue;     			//�����ϼ�Ŀ¼
					if(finfo->fattrib&0X10)//����Ŀ¼(�ļ�����,0X20,�鵵�ļ�;0X10,��Ŀ¼;)
					{
 						pathlen=strlen((const char*)pathname);		//�õ���ǰ·���ĳ���
						strcat((char*)pathname,(const char*)"/");	//��б��
						strcat((char*)pathname,(const char*)finfo->fname);	//Դ·��������Ŀ¼����
 						//printf("\r\nsub folder:%s\r\n",pathname);	//��ӡ��Ŀ¼��
						fdsize+=exf_fdsize(pathname);				//�õ���Ŀ¼��С,�ݹ����
						pathname[pathlen]=0;						//���������
					}else fdsize+=finfo->fsize;						//��Ŀ¼,ֱ�Ӽ����ļ��Ĵ�С
						
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
//�ļ��и���
//ע���ļ��д�С��Ҫ����4GB.
//��psrc�ļ���,copy��pdst�ļ���.
//pdst:��������"X:"/"X:XX"/"X:XX/XX"֮���.����Ҫʵ��ȷ����һ���ļ��д���
//fcpymsg,����ָ��,����ʵ�ֿ���ʱ����Ϣ��ʾ
//        pname:�ļ�/�ļ�����
//        pct:�ٷֱ�
//        mode:
//			[0]:�����ļ���
//			[1]:���°ٷֱ�pct
//			[2]:�����ļ���
//			[3~7]:����
//psrc,pdst:Դ�ļ��к�Ŀ���ļ���
//totsize:�ܴ�С(��totsizeΪ0��ʱ��,��ʾ����Ϊ�����ļ�����)
//cpdsize:�Ѹ����˵Ĵ�С.
//fwmode:�ļ�д��ģʽ
//0:������ԭ�е��ļ�
//1:����ԭ�е��ļ�
//����ֵ:0,�ɹ�
//    ����,�������;0XFF,ǿ���˳�
u8 exf_fdcopy(u8(*fcpymsg)(u8*pname,u8 pct,u8 mode),u8 *psrc,u8 *pdst,u32 *totsize,u32 *cpdsize,u8 fwmode)
{
#define MAX_PATHNAME_DEPTH	512+1	//���Ŀ���ļ�·��+�ļ������
	u8 res=0;	  
    DIR *srcdir=0;		//ԴĿ¼
	DIR *dstdir=0;		//ԴĿ¼
	FILINFO *finfo=0;	//�ļ���Ϣ
	u8 *fn=0;   		//���ļ���

	u8 * dstpathname=0;	//Ŀ���ļ���·��+�ļ���
	u8 * srcpathname=0;	//Դ�ļ���·��+�ļ���
	
 	u16 dstpathlen=0;	//Ŀ��·������
 	u16 srcpathlen=0;	//Դ·������

  
	srcdir=(DIR*)mymalloc(SRAMIN,sizeof(DIR));//�����ڴ�
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
			strcat((char*)srcpathname,(const char*)psrc); 	//����ԭʼԴ�ļ�·��	
			strcat((char*)dstpathname,(const char*)pdst); 	//����ԭʼĿ���ļ�·��	
		    res=f_opendir(srcdir,(const TCHAR*)psrc); 		//��ԴĿ¼
		    if(res==0)//��Ŀ¼�ɹ� 
			{
  				strcat((char*)dstpathname,(const char*)"/");//����б��
 				fn=exf_get_src_dname(psrc);
				if(fn==0)//��꿽��
				{
					dstpathlen=strlen((const char*)dstpathname);
					dstpathname[dstpathlen]=psrc[0];	//��¼���
					dstpathname[dstpathlen+1]=0;		//������ 
				}else strcat((char*)dstpathname,(const char*)fn);//���ļ���		
 				fcpymsg(fn,0,0X04);//�����ļ�����
				res=f_mkdir((const TCHAR*)dstpathname);//����ļ����Ѿ�����,�Ͳ�����.��������ھʹ����µ��ļ���.
				if(res==FR_EXIST)res=0;
				while(res==0)//��ʼ�����ļ�������Ķ���
				{
			        res=f_readdir(srcdir,finfo);					//��ȡĿ¼�µ�һ���ļ�
			        if(res!=FR_OK||finfo->fname[0]==0)break;		//������/��ĩβ��,�˳�
			        if(finfo->fname[0]=='.')continue;     			//�����ϼ�Ŀ¼
					fn=(u8*)finfo->fname; 							//�õ��ļ���
					dstpathlen=strlen((const char*)dstpathname);	//�õ���ǰĿ��·���ĳ���
					srcpathlen=strlen((const char*)srcpathname);	//�õ�Դ·������

					strcat((char*)srcpathname,(const char*)"/");//Դ·����б��
 					if(finfo->fattrib&0X10)//����Ŀ¼(�ļ�����,0X20,�鵵�ļ�;0X10,��Ŀ¼;)
					{
						strcat((char*)srcpathname,(const char*)fn);		//Դ·��������Ŀ¼����
						res=exf_fdcopy(fcpymsg,srcpathname,dstpathname,totsize,cpdsize,fwmode);	//�����ļ���
					}else //��Ŀ¼
					{
						strcat((char*)dstpathname,(const char*)"/");//Ŀ��·����б��
						strcat((char*)dstpathname,(const char*)fn);	//Ŀ��·�����ļ���
						strcat((char*)srcpathname,(const char*)fn);	//Դ·�����ļ���
 						fcpymsg(fn,0,0X01);//�����ļ���
						res=exf_copy(fcpymsg,srcpathname,dstpathname,*totsize,*cpdsize,fwmode);//�����ļ�
						*cpdsize+=finfo->fsize;//����һ���ļ���С
					}
					srcpathname[srcpathlen]=0;//���������
					dstpathname[dstpathlen]=0;//���������	    
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
//��·��Ϊfilepath���ļ��������ļ������ݱ�����fdatas��

void printffile(u8 * filepath)
{
FATFS fatsd;
FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
FRESULT fr;
UINT brs;
DIR dirobj;    //Ŀ¼����
FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	u8 fdatas[255];   //�����õ����� 
	u32 size=0;
	//1������ļ�
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);   //���ļ�����,���ļ�·�����򿪵�ģʽ��
	if(fr==FR_OK)
	{
		//2��ʹ���ļ�����fileobj����������������򿪵��ļ���������ݶ������󱣴���fdatas��
		size=f_size(&fileobj);  //��ȡ�ļ��Ĵ�С
		f_read(&fileobj,fdatas,size,&brs);    //(�ļ�����,������������õ�buff�������ٸ����ݣ�ָ�����ݸ�����ָ��)
	}
	f_close(&fileobj);  //3���ر�����ļ�����
	//�ļ�������fdatas��
	printf("------------------------\r\n");	
	printf("File Path : %s \r\n",filepath);
	printf("File Content : %s\r\n",fdatas);
	printf("------------------------\r\n");	
}
//���ļ��Ŀ�ͷ����дdatas�е����ݣ�����length
void testWrite(u8 * filepath,u8 * datas,u32 length)
{
	FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	if(fr==FR_OK)
	{
		fr=f_write(&fileobj,datas,length,&brs);
	}
	f_close(&fileobj);
	printffile(filepath);  //��д��ȥ�����ݴ�ӡ��������д�ĶԲ���
}
//���ļ�ָ��λ�ÿ�ʼд
void testSeek(u8 * filepath,u32 index,u8 *datas,u32 length)
{ 
		FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	f_lseek(&fileobj, index);//�ƶ����ָ����indexλ�ã���ʱfileobj�б����˹��ָ��λ��
	// f_lseek(&fileobj,f_size(&fileobj)); //��ָ��ָ���ļ�ĩβ
	fr=f_write(&fileobj,datas,length,&brs);   //
	f_close(&fileobj);
	printffile(filepath);
}
//��    ʹ��f_gets���ļ��ж�ȡһ���ַ�����f_gets�Ƕ�ȡ�ַ�������f_read����������
void testGets(u8 * filepath)
{	
		FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	u8 fdatas[255];
	u32 size=0;
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	size=f_size(&fileobj);
	f_gets((TCHAR*)fdatas,size,&fileobj);  //f_gets��ȡ�ַ�����������������õ�buff�������ٸ����ݣ��ļ�����
	f_close(&fileobj);
	printf("Test Gets: %s \r\n",fdatas);
}
//дһ���ַ�/�ַ������ļ�
void testPutsPutc(u8 * filepath,u8 *sDatas,u8 cdata)  //���������ļ�·�����ַ������ַ���
{
		FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	f_puts((const TCHAR*)sDatas,&fileobj);   //f_putsдһ���ַ������ļ�
	f_putc((TCHAR)cdata,&fileobj);   //f_putcдһ���ַ����ļ�
	f_close(&fileobj);
	printffile(filepath);
}
//д   f_printf :   ��ʽ��дһ��string���ļ���
void testFprintf(u8 * filepath)
{
		FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);
	f_printf(&fileobj, "%d", 1234);            /* "1234" */   
	f_printf(&fileobj, "%6d,%3d%%", -200, 5);  /* "  -200,  5%" */
    f_printf(&fileobj, "%ld", 12345L);         /* "12345" */
	
	f_close(&fileobj);
	printffile(filepath);
	
}

//Ŀ¼���ʲ����ĺ���   f_opendir    f_readdir       f_closedir 
//�ļ�ɨ�裬��ӡ��ǰ·��dirpath�µ��ļ�Ŀ¼
void testDirScan(u8 * dirpath)
{
		FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	int index=0;
	fr=f_opendir(&dirobj,(const TCHAR*)dirpath);
	
	if(fr==FR_OK)
	{
		while(1)
		{
			fr=f_readdir(&dirobj,&fileinfoobj);  //��Ŀ¼���󣬱���Ŀ¼��Ϣ��  
			if(fr!=FR_OK||fileinfoobj.fname[0]==0) break;

			 printf("filename %d = %s\r\n",index,(u8 *)fileinfoobj.fname);  //������index
			 index++;
		}
		
	}
	f_closedir(&dirobj);
}
//f_mkdir  �½�Ŀ¼     f_unlink ɾ��Ŀ¼     f_rename Ŀ¼������ 
void testMK_UNlinkDir(void)
{
		FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
	DIR recdir;	
	f_mkdir("0:/123");   //�½�Ŀ¼�ļ�
	f_mkdir("0:/123/Bedgy");
	f_mkdir("0:/123/DEFRGTH");	
//	testDirScan("0:/AAAA");
//	
//	f_unlink("0:/AAAA/CCCC");   //f_unlink ɾ��Ŀ¼��0:/AAAA/CCCC��
//	testDirScan("0:/AAAA");
//	
//	f_rename("0:/AAAA/BBBB","0:/AAAA/BBCC");  //�� ǰ���·�� ������Ϊ �����·��
//	testDirScan("0:/AAAA");
	
}

//�ļ�����   ��Ŀ¼dirpath�в����ļ�filenamePattern
void testfindFirst(u8 * dirpath,u8 * filenamePattern)
{
		FATFS fatsd;
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR dirobj;    //Ŀ¼����
	FILINFO fileinfoobj;  //��������Ŀ¼�ļ���Ϣ
/*ע�⣬����������ʹ�ã���������ffconf.h�����_USE_FIND��ʶ����ֵΪ1 ��Ҳ���ǣ�#define _USE_FIND 1  */
 
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
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	DIR recdir;
	FRESULT fr;
	char *sDatas ="-------------------system reset-----------------\r\n";

		while(f_opendir(&recdir,"0:/SaveData"))//���ļ���
 	{	
		f_mkdir("0:/SaveData");				//������Ŀ¼   
		f_open(&fileobj, "0:SaveData/GpsData.txt", FA_CREATE_NEW);			//������Ŀ¼ 
		f_open(&fileobj, "0:SaveData/ImuData.txt", FA_CREATE_NEW);			//������Ŀ¼ 		 
	}
	//��GpsData.txt��д�������ϵ翪ͷ
		fr=f_open(&fileobj,"0:SaveData/GpsData.txt",FA_READ|FA_WRITE);
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//��ָ��ָ���ļ�ĩβ
		f_puts((const TCHAR*)sDatas,&fileobj);   //f_puts	
	}
	f_close(&fileobj);
		//��ImuData.txt��д�������ϵ翪ͷ
		fr=f_open(&fileobj,"0:SaveData/ImuData.txt",FA_READ|FA_WRITE);
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//��ָ��ָ���ļ�ĩβ
		f_puts((const TCHAR*)sDatas,&fileobj);   //f_puts	
	}
	f_close(&fileobj);
	
}


//void sd_savegps(u8 * filepath,u8 * datas,u32 length)
void sd_savegps(u8 * filepath,u8 * datas)
{
	FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
	FRESULT fr;
	UINT brs;
	DIR recdir;	
//	const u8 newline[]={0X0D,0X0A}; 
	fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);   //���ļ�����,���ļ�·�����򿪵�ģʽ��
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//��ָ��ָ���ļ�ĩβ
		//fr=f_write(&fileobj,datas,length,&brs);
		f_printf(&fileobj, "%s\n", datas);   //ʹ��f_printf����˺������������
	//	f_lseek(&fileobj,f_size(&fileobj)); //��ָ��ָ���ļ�ĩβ
  //	f_write(&fileobj, newline,2, &brs);  //����
	//	f_printf(&fileobj, "%s\n", newline); 
	}
		f_close(&fileobj);
}

//sd_saveimu("0:SaveData/GpsData.txt",imu_massage2->imu_data);
void sd_saveimu(u8 * filepath,u8 * datas)
{
		FIL fileobj;   //�ļ����󣬱��浱ǰ�������ļ�״��
		FRESULT fr;
		UINT brs;
		DIR recdir;
		fr=f_open(&fileobj,(const TCHAR*)filepath,FA_READ|FA_WRITE);   //���ļ�����,���ļ�·�����򿪵�ģʽ��
	if(fr==FR_OK)
	{
		f_lseek(&fileobj,f_size(&fileobj)); 	//��ָ��ָ���ļ�ĩβ
		f_printf(&fileobj, " ACC_DATA X: %d,   Y: %d   Z:%d\n",datas[0],datas[1],datas[2]);   //ʹ��f_printf����˺������������
		f_printf(&fileobj, " GYR_DATA X: %d,   Y: %d   Z:%d\n",datas[3],datas[4],datas[5]);   		
	}
		f_close(&fileobj);
}









