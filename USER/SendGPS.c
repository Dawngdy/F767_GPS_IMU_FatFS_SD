#include "SendGPS.h"
#include "exfuns.h"  //SD
#include "VariableType.h"
#include "GlobalVars.h"
#include "DataProc.h"
#include "GINavMain.h"
#include "BasicFunc.h"


uint8_t   GGA_Data[30]; 
uint8_t   GGB_Data[8];           
uint8_t   GGS_Data[15];          
uint8_t   RMC_TX_DATA[200];
uint8_t   GGA_TX_DATA[200];
//------------------------------------------------
//void ProcessLSFTOA(float GGAIData,uint8_t bit)
//------------------------------------------------
uint8_t ProcessLSFTOA(float GGAIData,uint8_t bit)
{
 static uint8_t AVRNum; 

 static uint8_t Degree;    
 static uint8_t ZF_Flag;  
 static uint8_t i;       

 static int16_t  Angle;       
 static float    AngleDot;


 if(GGAIData>=0)  
 {
  ZF_Flag=1;
  
 }
 else
 {
  ZF_Flag=0;  
 }

 Angle    = GGAIData;			  //-3.14  => -3
 AngleDot = GGAIData - Angle;	  //-3.14+3=> -0.14

//-------------------------------------
  if(abs(Angle)>=100)
 {
  Degree=3;
 }
 else if(abs(Angle)>=10)
 {
  Degree=2;
 }
 else if(abs(Angle)>=1)
 {
  Degree=1;
 }
 else
 {
  Degree=0;
 }

 //--------------------------------------------------------------
  if(ZF_Flag==0)
  {
   Angle    = - Angle;
   AngleDot = - AngleDot;
  }
 //--------------------------------------------------------------
  if(Degree==3)                                                //132
  {
	 GGB_Data[0] = (Angle)/100;	                                //百位    
   GGB_Data[1] = (Angle)/10    - ((uint16_t)GGB_Data[0])*10;   //十位        
	 GGB_Data[2] = (Angle)       - ((uint16_t)GGB_Data[0])*100 - ((uint16_t)GGB_Data[1]*10) ;  //个数        		    

	 ProcessSSFTOA(AngleDot,bit);							   

	 if(ZF_Flag)
	 {
    GGA_Data[0]='+';
	 }
	 else
	 {
	  GGA_Data[0]='-';
	 }

   GGA_Data[1] = GGB_Data[0] + 0x30;
	 GGA_Data[2] = GGB_Data[1] + 0x30;
	 GGA_Data[3] = GGB_Data[2] + 0x30;
	 GGA_Data[4] = '.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[5+i] =GGS_Data[i];
	 }

	 AVRNum = 5+bit;

	}
 //--------------------------------------------------------------		 								   
	else if(Degree==2)                                          //13
	{
	 GGB_Data[0] = (Angle)/10;	                                //十位    
   GGB_Data[1] = (Angle)    - ((uint16_t)GGB_Data[0])*10;     //个位        	

	 ProcessSSFTOA(AngleDot,bit);							   

	 if(ZF_Flag)
	 {
    GGA_Data[0]='+';
	 }
	 else
	 {
	  GGA_Data[0]='-';
	 }

   GGA_Data[1] = GGB_Data[0] + 0x30;
	 GGA_Data[2] = GGB_Data[1] + 0x30;	 
	 GGA_Data[3] ='.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[4+i] =GGS_Data[i];
	 }

	 AVRNum = 4+bit;
	}
   	else if(Degree==1)                                          //1.32==>132==>1.32  //0.32==>32==>0.32    //0.03==3==>0.03	                                                                  
	{
    GGB_Data[0] = Angle;	                                    //个位

	 ProcessSSFTOA(AngleDot,bit);

	 if(ZF_Flag)
	 {
    GGA_Data[0]='+';
	 }
	 else
	 {
	  GGA_Data[0]='-';
	 }

   GGA_Data[1] = GGB_Data[0] + 0x30;
	 GGA_Data[2] ='.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[3+i] =GGS_Data[i];
	 }

	 AVRNum = 3 + bit;
	}
   //-----------------------------
	else
	{
     GGB_Data[0] = 0;

	 ProcessSSFTOA(AngleDot,bit);

	 if(ZF_Flag)
	 {
    GGA_Data[0]='+';
	 }
	 else
	 {
	  GGA_Data[0]='-';
	 }

   GGA_Data[1] = GGB_Data[0] + 0x30;	 
	 GGA_Data[2] ='.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[3+i] =GGS_Data[i];
	 }

	 AVRNum = 3+bit;
	}
  
   return AVRNum;										
}


//------------------------------------------------
//void ProcessLSFTOA(float GGAIData,uint8_t bit)
//------------------------------------------------
uint8_t ProcessPSFTOA(float GGAIData,uint8_t bit)
{
 static uint8_t AVRNum; 

 static uint8_t Degree;    
 static uint8_t ZF_Flag;  
 static uint8_t i;       

 static int16_t  Angle;       
 static float    AngleDot;


 if(GGAIData>=0)  
 {
  ZF_Flag=1;
  
 }
 else
 {
  ZF_Flag=0;  
 }

 Angle    = GGAIData;			  //-3.14  => -3
 AngleDot = GGAIData - Angle;	  //-3.14+3=> -0.14

//-------------------------------------
  if(abs(Angle)>=100)
 {
  Degree=3;
 }
 else if(abs(Angle)>=10)
 {
  Degree=2;
 }
 else if(abs(Angle)>=1)
 {
  Degree=1;
 }
 else
 {
  Degree=0;
 }

 //--------------------------------------------------------------
  if(ZF_Flag==0)
  {
   Angle    = - Angle;
   AngleDot = - AngleDot;
  }
 //--------------------------------------------------------------
  if(Degree==3)                                                //132
  {
	 GGB_Data[0] = (Angle)/100;	                                //百位    
   GGB_Data[1] = (Angle)/10    - ((uint16_t)GGB_Data[0])*10;   //十位        
	 GGB_Data[2] = (Angle)       - ((uint16_t)GGB_Data[0])*100 - ((uint16_t)GGB_Data[1]*10) ;  //个数        		    

	 ProcessSSFTOA(AngleDot,bit);							   

	 if(ZF_Flag)
	 {
	  GGA_Data[0] = GGB_Data[0] + 0x30;
	  GGA_Data[1] = GGB_Data[1] + 0x30;
	  GGA_Data[2] = GGB_Data[2] + 0x30;
	  GGA_Data[3] = '.';

	  for(i=0;i<bit;i++)
	  {
	  GGA_Data[4+i] =GGS_Data[i];
	  }
	  AVRNum = 4+bit;
		
	 }
	 else
	 {
	  GGA_Data[0]='-';	 

    GGA_Data[1] = GGB_Data[0] + 0x30;
	  GGA_Data[2] = GGB_Data[1] + 0x30;
	  GGA_Data[3] = GGB_Data[2] + 0x30;
	  GGA_Data[4] = '.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[5+i] =GGS_Data[i];
	  }
	  AVRNum = 5+bit;
   }
	}
 //--------------------------------------------------------------		 								   
	else if(Degree==2)                                          //13
	{
	 GGB_Data[0] = (Angle)/10;	                                //十位    
   GGB_Data[1] = (Angle)    - ((uint16_t)GGB_Data[0])*10;     //个位        	

	 ProcessSSFTOA(AngleDot,bit);							   

	 if(ZF_Flag)
	 {
    GGA_Data[0] = GGB_Data[0] + 0x30;
	  GGA_Data[1] = GGB_Data[1] + 0x30;	 
	  GGA_Data[2] ='.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[3+i] =GGS_Data[i];
	  }
	   AVRNum = 3+bit;
	 }
	 else
	 {
	  GGA_Data[0]='-';
	 
    GGA_Data[1] = GGB_Data[0] + 0x30;
	  GGA_Data[2] = GGB_Data[1] + 0x30;	 
	  GGA_Data[3] ='.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[4+i] =GGS_Data[i];
	  }
	   AVRNum = 4+bit;
   }
	}
   	else if(Degree==1)                                          //1.32==>132==>1.32  //0.32==>32==>0.32    //0.03==3==>0.03	                                                                  
	{
    GGB_Data[0] = Angle;	                                    //个位

	  ProcessSSFTOA(AngleDot,bit);

	  if(ZF_Flag)
	  {
     GGA_Data[0] = GGB_Data[0] + 0x30;
	   GGA_Data[1] ='.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[2+i] =GGS_Data[i];
	  }
	  AVRNum = 2 + bit;
	 }
	 else
	 {
	  GGA_Data[0]='-';
    GGA_Data[1] = GGB_Data[0] + 0x30;
	  GGA_Data[2] ='.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[3+i] =GGS_Data[i];
	  }
	  AVRNum = 3 + bit;
   }
	}
   //-----------------------------
	else
	{
   GGB_Data[0] = 0;

	 ProcessSSFTOA(AngleDot,bit);

	 if(ZF_Flag)
	 {
    GGA_Data[0] = GGB_Data[0] + 0x30;	 
	  GGA_Data[1] ='.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[2+i] =GGS_Data[i];
	  }

	  AVRNum = 2+bit;
	 }
	 else
	 {
	  GGA_Data[0]='-';
	  GGA_Data[1] = GGB_Data[0] + 0x30;	 
	  GGA_Data[2] ='.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[3+i] =GGS_Data[i];
	  }

	  AVRNum = 3+bit;
	 }
  }
   return AVRNum;										
}

//------------------------------------------------
//void ProcessLatToA(double GGAIData)
//------------------------------------------------
void ProcessLatToA(double GGAIData)
{
static   uint8_t  i;
static   uint8_t  Latitude;

static  double  DLatitudePart;
static  uint64_t LatitudePart;
	
	 if(GGAIData<0)
	 {
	  GGAIData=-GGAIData;
	 }

   Latitude    = GGAIData;
   												     	   //31.2754477==>31+0.2754477*60==>31+16.52686 ==> 3116.52686;

    GGA_Data[0] = Latitude/10 ;					   //d 3
    GGA_Data[1] = Latitude    - ((uint16_t)GGA_Data[0])*10 ;      //d 1

    DLatitudePart= (GGAIData - Latitude)*60.0;
    LatitudePart = (DLatitudePart)*100000.0;      //degree;
   
    GGA_Data[2] = LatitudePart/1000000 ;                                         //m
    GGA_Data[3] = LatitudePart/100000  - GGA_Data[2]*10;                       //m
    GGA_Data[4] = 0x2E;									                              //.
    GGA_Data[5] = LatitudePart/10000   - ((uint32_t)GGA_Data[2])*100    - ((uint32_t)GGA_Data[3])*10 ;     //m
    GGA_Data[6] = LatitudePart/1000    - ((uint32_t)GGA_Data[2])*1000   - ((uint32_t)GGA_Data[3])*100    - ((uint32_t)GGA_Data[5])*10;     //m
    GGA_Data[7] = LatitudePart/100     - ((uint32_t)GGA_Data[2])*10000  - ((uint32_t)GGA_Data[3])*1000   - ((uint32_t)GGA_Data[5])*100   - ((uint32_t)GGA_Data[6])*10;     //m
    GGA_Data[8] = LatitudePart/10      - ((uint32_t)GGA_Data[2])*100000 - ((uint32_t)GGA_Data[3])*10000  - ((uint32_t)GGA_Data[5])*1000  - ((uint32_t)GGA_Data[6])*100  -  ((uint32_t)GGA_Data[7])*10;     //m
    GGA_Data[9] = LatitudePart         - ((uint32_t)GGA_Data[2])*1000000- ((uint32_t)GGA_Data[3])*100000 - ((uint32_t)GGA_Data[5])*10000 - ((uint32_t)GGA_Data[6])*1000 -  ((uint32_t)GGA_Data[7])*100 -  ((uint32_t)GGA_Data[8])*10;     //m

		 
    for (i=0;i<4;i++)
    {
     GGA_Data[i] = GGA_Data[i]+0x30;  
    }

    for (i=5;i<10;i++)
    {
     GGA_Data[i] = GGA_Data[i]+0x30;  
    }   
}
//-------------------------------------------------
//void ProcessSSFTOA(float GGAIData,uint8_t bit)
//-------------------------------------------------
void ProcessSSFTOA(double GGAIData,uint8_t bit)
{
 static double  AngDot;
 static uint8_t i;
 static uint64_t Anglel;
	
   AngDot= GGAIData;	
	
	
   if(bit==11)			       //bit==11
	 {
	  Anglel=AngDot*100000000000;
		 
    GGS_Data[0] = (Anglel)/10000000000;	                              //千位    	
    GGS_Data[1] = (Anglel)/1000000000  - ((uint32_t)GGS_Data[0]*10);     //百位        				                             //.
	  GGS_Data[2] = (Anglel)/100000000   - ((uint32_t)GGS_Data[0]*100)         - ((uint32_t)GGS_Data[1]*10) ;  //十数        	
	  GGS_Data[3] = (Anglel)/10000000    - ((uint32_t)GGS_Data[0]*1000)        - ((uint32_t)GGS_Data[1]*100)        - ((uint32_t)GGS_Data[2]*10) ;  //十数        	
		GGS_Data[4] = (Anglel)/1000000     - ((uint32_t)GGS_Data[0]*10000)       - ((uint32_t)GGS_Data[1]*1000)       - ((uint32_t)GGS_Data[2]*100)       - ((uint32_t)GGS_Data[3]*10) ;  //十数        	
    GGS_Data[5] = (Anglel)/100000      - ((uint32_t)GGS_Data[0]*100000)      - ((uint32_t)GGS_Data[1]*10000)      - ((uint32_t)GGS_Data[2]*1000)      - ((uint32_t)GGS_Data[3]*100)     - ((uint32_t)GGS_Data[4]*10);  //十数        	
		GGS_Data[6] = (Anglel)/10000       - ((uint32_t)GGS_Data[0]*1000000)     - ((uint32_t)GGS_Data[1]*100000)     - ((uint32_t)GGS_Data[2]*10000)     - ((uint32_t)GGS_Data[3]*1000)    - ((uint32_t)GGS_Data[4]*100)     - ((uint32_t)GGS_Data[5]*10);  //十数        	
		GGS_Data[7] = (Anglel)/1000        - ((uint32_t)GGS_Data[0]*10000000)    - ((uint32_t)GGS_Data[1]*1000000)    - ((uint32_t)GGS_Data[2]*100000)    - ((uint32_t)GGS_Data[3]*10000)   - ((uint32_t)GGS_Data[4]*1000)    - ((uint32_t)GGS_Data[5]*100)    - ((uint32_t)GGS_Data[6]*10);  //十数        	 
		GGS_Data[8] = (Anglel)/100         - ((uint32_t)GGS_Data[0]*100000000)   - ((uint32_t)GGS_Data[1]*10000000)   - ((uint32_t)GGS_Data[2]*1000000)   - ((uint32_t)GGS_Data[3]*100000)  - ((uint32_t)GGS_Data[4]*10000)   - ((uint32_t)GGS_Data[5]*1000)   - ((uint32_t)GGS_Data[6]*100)   - ((uint32_t)GGS_Data[7]*10);  //十数        	  
		GGS_Data[9] = (Anglel)/10          - ((uint64_t)GGS_Data[0]*1000000000)  - ((uint64_t)GGS_Data[1]*100000000)  - ((uint32_t)GGS_Data[2]*10000000)  - ((uint32_t)GGS_Data[3]*1000000) - ((uint32_t)GGS_Data[4]*100000)  - ((uint32_t)GGS_Data[5]*10000)  - ((uint32_t)GGS_Data[6]*1000)  - ((uint32_t)GGS_Data[7]*100)  - ((uint32_t)GGS_Data[8]*10);  //十数        	  
		GGS_Data[10]= (Anglel)             - ((uint64_t)GGS_Data[0]*10000000000) - ((uint64_t)GGS_Data[1]*1000000000) - ((uint32_t)GGS_Data[2]*100000000) - ((uint32_t)GGS_Data[3]*10000000)- ((uint32_t)GGS_Data[4]*1000000) - ((uint32_t)GGS_Data[5]*100000) - ((uint32_t)GGS_Data[6]*10000) - ((uint32_t)GGS_Data[7]*1000) - ((uint32_t)GGS_Data[8]*100)- ((uint32_t)GGS_Data[9]*10);  //十数        	  
		 
		for(i=0;i<11;i++)
	  {
	   if(GGS_Data[i]==0xFF)		
			GGS_Data[i]=0;
		
     GGS_Data[i] = GGS_Data[i] + 0x30;   
		}
   }
	 if(bit==9)			       //bit==11
	 {
	  Anglel=AngDot*1000000000;
		 
    GGS_Data[0] = (Anglel)/100000000;	                              //千位    	
    GGS_Data[1] = (Anglel)/10000000  - ((uint32_t)GGS_Data[0]*10);     //百位        				                             //.
	  GGS_Data[2] = (Anglel)/1000000   - ((uint32_t)GGS_Data[0]*100)         - ((uint32_t)GGS_Data[1]*10) ;  //十数        	
	  GGS_Data[3] = (Anglel)/100000    - ((uint32_t)GGS_Data[0]*1000)        - ((uint32_t)GGS_Data[1]*100)        - ((uint32_t)GGS_Data[2]*10) ;  //十数        	
		GGS_Data[4] = (Anglel)/10000     - ((uint32_t)GGS_Data[0]*10000)       - ((uint32_t)GGS_Data[1]*1000)       - ((uint32_t)GGS_Data[2]*100)       - ((uint32_t)GGS_Data[3]*10) ;  //十数        	
    GGS_Data[5] = (Anglel)/1000      - ((uint32_t)GGS_Data[0]*100000)      - ((uint32_t)GGS_Data[1]*10000)      - ((uint32_t)GGS_Data[2]*1000)      - ((uint32_t)GGS_Data[3]*100)     - ((uint32_t)GGS_Data[4]*10);  //十数        	
		GGS_Data[6] = (Anglel)/100       - ((uint32_t)GGS_Data[0]*1000000)     - ((uint32_t)GGS_Data[1]*100000)     - ((uint32_t)GGS_Data[2]*10000)     - ((uint32_t)GGS_Data[3]*1000)    - ((uint32_t)GGS_Data[4]*100)     - ((uint32_t)GGS_Data[5]*10);  //十数        	
		GGS_Data[7] = (Anglel)/10        - ((uint32_t)GGS_Data[0]*10000000)    - ((uint32_t)GGS_Data[1]*1000000)    - ((uint32_t)GGS_Data[2]*100000)    - ((uint32_t)GGS_Data[3]*10000)   - ((uint32_t)GGS_Data[4]*1000)    - ((uint32_t)GGS_Data[5]*100)    - ((uint32_t)GGS_Data[6]*10);  //十数        	 
		GGS_Data[8] = (Anglel)           - ((uint32_t)GGS_Data[0]*100000000)   - ((uint32_t)GGS_Data[1]*10000000)   - ((uint32_t)GGS_Data[2]*1000000)   - ((uint32_t)GGS_Data[3]*100000)  - ((uint32_t)GGS_Data[4]*10000)   - ((uint32_t)GGS_Data[5]*1000)   - ((uint32_t)GGS_Data[6]*100)   - ((uint32_t)GGS_Data[7]*10);  //十数        	  
		 
		for(i=0;i<9;i++)
	  {
	   GGS_Data[i] = GGS_Data[i] + 0x30;   
		}
   }
   else if(bit==4)			       //bit==4
   {
    GGS_Data[0] = (AngDot*10000)/1000;	                              //千位    	
    GGS_Data[1] = (AngDot*10000)/100  - ((uint16_t)GGS_Data[0])*10;     //百位        				                             //.
	  GGS_Data[2] = (AngDot*10000)/10   - ((uint16_t)GGS_Data[0])*100 - ((uint16_t)GGS_Data[1]*10) ;  //十数        	
	  GGS_Data[3] = (AngDot*10000)      - ((uint16_t)GGS_Data[0])*1000- ((uint16_t)GGS_Data[1]*100)- ((uint16_t)GGS_Data[2]*10) ;  //十数        	

    GGS_Data[0] = GGS_Data[0] + 0x30;   
    GGS_Data[1] = GGS_Data[1] + 0x30;   
    GGS_Data[2] = GGS_Data[2] + 0x30;       
	  GGS_Data[3] = GGS_Data[3] + 0x30;  
   }
   else	                        //bit==3
   {
    GGS_Data[0] = (AngDot*1000)/100;	                              //千位    	
    GGS_Data[1] = (AngDot*1000)/10  - ((uint16_t)GGS_Data[0]*10);     //百位        				                             //.
	  GGS_Data[2] = (AngDot*1000)     - ((uint16_t)GGS_Data[0])*100-((uint16_t)GGS_Data[1]*10);
	

    GGS_Data[0] = GGS_Data[0] + 0x30;   
    GGS_Data[1] = GGS_Data[1] + 0x30;   
    GGS_Data[2] = GGS_Data[2] + 0x30;       	  
   }													  
}

//-------------------------------------------------------
//uint8_t ProcessZLSFTOA(float GGAIData,uint8_t bit)
//-------------------------------------------------------
uint8_t ProcessZLSFTOA(float GGAIData,uint8_t bit)
{
 static uint8_t AVRNum; 

 static uint8_t Degree;     
 static uint8_t i;       

 static int16_t  Angle;       
 static float    AngleDot;



 Angle    = GGAIData;			  //-3.14  => -3
 AngleDot = GGAIData - Angle;	  //-3.14+3=> -0.14

//-------------------------------------
 if(abs(Angle)>=1000)
 {
  Degree=4;
 }
  if(abs(Angle)>=100)
 {
  Degree=3;
 }
 else if(abs(Angle)>=10)
 {
  Degree=2;
 }
 else if(abs(Angle)>=1)
 {
  Degree=1;
 }
 else
 {
  Degree=0;
 }


 //--------------------------------------------------------------
  if(Degree==4)                                                //132
  {
	 GGB_Data[0] = (Angle)/1000;	                                //千位    
   GGB_Data[1] = (Angle)/100    - ((uint16_t)GGB_Data[0])*10;   //百位     
	 GGB_Data[2] = (Angle)/10     - ((uint16_t)GGB_Data[0])*100 -  ((uint16_t)GGB_Data[1]*10) ;  //十位
   GGB_Data[3] = (Angle)        - ((uint16_t)GGB_Data[0])*1000 - ((uint16_t)GGB_Data[1]*100) -GGB_Data[2]*10;  //个		

	 ProcessSSFTOA(AngleDot,bit);							   

	
   GGA_Data[0] = GGB_Data[0] + 0x30;
	 GGA_Data[1] = GGB_Data[1] + 0x30;
	 GGA_Data[2] = GGB_Data[2] + 0x30;
	 GGA_Data[3] = GGB_Data[3] + 0x30;
	 GGA_Data[4] = '.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[5+i] =GGS_Data[i];
	 }

	 AVRNum = 5+bit;

	}
	
  else if(Degree==3)                                                //132
  {
	 GGB_Data[0] = (Angle)/100;	                                //百位    
   GGB_Data[1] = (Angle)/10    - ((uint16_t)GGB_Data[0])*10;   //十位        
	 GGB_Data[2] = (Angle)       - ((uint16_t)GGB_Data[0])*100 - ((uint16_t)GGB_Data[1]*10) ;  //个数        		    

	 ProcessSSFTOA(AngleDot,bit);							   

	
   GGA_Data[0] = GGB_Data[0] + 0x30;
	 GGA_Data[1] = GGB_Data[1] + 0x30;
	 GGA_Data[2] = GGB_Data[2] + 0x30;
	 GGA_Data[3] = '.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[4+i] =GGS_Data[i];
	 }

	 AVRNum = 4+bit;

	}
 //--------------------------------------------------------------		 								   
	else if(Degree==2)                                          //13
	{
	 GGB_Data[0] = (Angle)/10;	                                //十位    
   GGB_Data[1] = (Angle)    - ((uint16_t)GGB_Data[0])*10;     //个位        	

	 ProcessSSFTOA(AngleDot,bit);							   

	 
   GGA_Data[0] = GGB_Data[0] + 0x30;
	 GGA_Data[1] = GGB_Data[1] + 0x30;	 
	 GGA_Data[2] ='.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[3+i] =GGS_Data[i];
	 }

	 AVRNum = 3+bit;
	}
   	else if(Degree==1)                                          //1.32==>132==>1.32  //0.32==>32==>0.32    //0.03==3==>0.03	                                                                  
	{
    GGB_Data[0] = Angle;	                                    //个位

	  ProcessSSFTOA(AngleDot,bit);


    GGA_Data[0] = GGB_Data[0] + 0x30;
	  GGA_Data[1] ='.';

	  for(i=0;i<bit;i++)
	  {
	   GGA_Data[2+i] =GGS_Data[i];
	  }

	  AVRNum = 2 + bit;
	}
   //-----------------------------
	else
	{
    GGB_Data[0] = 0;

	  ProcessSSFTOA(AngleDot,bit);	 

    GGA_Data[0] = GGB_Data[0] + 0x30;	 
	  GGA_Data[1] ='.';

	 for(i=0;i<bit;i++)
	 {
	  GGA_Data[2+i] =GGS_Data[i];
	 }

	 AVRNum = 2+bit;
	}
  
   return AVRNum;										
}

//------------------------------------------------
//void ProcessITOA(uint8_t GGAIData,uint8_t bit)
//------------------------------------------------
void ProcessVTOA(uint16_t VData)
{									
 static  uint8_t  i;
  			     
	GGA_Data[0] = (VData)/1000;	                                    //??    
  GGA_Data[1] = (VData)/100  - ((uint16_t)GGA_Data[0])*10;        //??        
	GGA_Data[2] = (VData)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //??        	
	GGA_Data[3] = (VData)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[1]*100)- ((uint16_t)GGA_Data[2]*10) ;  //??        	
	
    for(i=0;i<4;i++)
    {
     GGA_Data[i] = GGA_Data[i] + 0x30;   
    }
}

//------------------------------------------------
//void ProcessLongToA(double GGAIData)
//------------------------------------------------
void ProcessLongToA(double GGAIData)
{
static   uint8_t  i,Longitude;
static   uint64_t LongitudePart;

	 if(GGAIData<0)
	 {
	  GGAIData=-GGAIData;
	 }

	 
   Longitude   = GGAIData;			     	   //121.452914===121+0.452914*60 =121+27.17484==12127.17484;

   GGA_Data[0] = Longitude/100;					                       //d 1
   GGA_Data[1] = Longitude/10  - ((uint16_t)GGA_Data[0])*10;                       //d 2
   GGA_Data[2] = Longitude     - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1])*10;      //d 1

 
    LongitudePart = ((GGAIData - Longitude)*60)*100000;      //degree;
   
    GGA_Data[3] = LongitudePart/1000000;                                        //m
    GGA_Data[4] = LongitudePart/100000  - GGA_Data[3]*10;                       //m
    GGA_Data[5] = 0x2E;									                              //.
    GGA_Data[6] = LongitudePart/10000   - ((uint32_t)GGA_Data[3])*100    - ((uint32_t)GGA_Data[4])*10 ;     //m
    GGA_Data[7] = LongitudePart/1000    - ((uint32_t)GGA_Data[3])*1000   - ((uint32_t)GGA_Data[4])*100    - ((uint32_t)GGA_Data[6])*10;     //m
    GGA_Data[8] = LongitudePart/100     - ((uint32_t)GGA_Data[3])*10000  - ((uint32_t)GGA_Data[4])*1000   - ((uint32_t)GGA_Data[6])*100   - ((uint32_t)GGA_Data[7])*10;     //m
    GGA_Data[9] = LongitudePart/10      - ((uint32_t)GGA_Data[3])*100000 - ((uint32_t)GGA_Data[4])*10000  - ((uint32_t)GGA_Data[6])*1000  - ((uint32_t)GGA_Data[7])*100  -  ((uint32_t)GGA_Data[8])*10;     //m
    GGA_Data[10]= LongitudePart         - ((uint32_t)GGA_Data[3])*1000000- ((uint32_t)GGA_Data[4])*100000 - ((uint32_t)GGA_Data[6])*10000 - ((uint32_t)GGA_Data[7])*1000 -  ((uint32_t)GGA_Data[8])*100 -  ((uint32_t)GGA_Data[9])*10;     //m

		 
    for (i=0;i<5;i++)
    {
     GGA_Data[i] = GGA_Data[i]+0x30;  
    }

    for (i=6;i<11;i++)
    {
    GGA_Data[i] = GGA_Data[i]+0x30;  
    }
   
   
}


//----------------------------------------
//void ProcessCheckToA(uint8_t CheckNum)
//----------------------------------------
void ProcessCheckToA(uint8_t CheckNum)
{									
static   uint8_t  i;
  			     
   GGA_Data[0] = CheckNum/16 ; 			       //十位
   GGA_Data[1] = CheckNum -GGA_Data[0]*16;	   //个位

   for(i=0;i<2;i++)
   {
    if(GGA_Data[i] <= 9)
	{
	 GGA_Data[i] = (GGA_Data[i] + '0') ;
	}
	else
	{
	 GGA_Data[i] = ((GGA_Data[i]-10) + 'A') ;
	}   
   }
}



//------------------------------------
//void ProcessCheck(uint8_t DNum)
//------------------------------------
uint8_t ProcessCheckResult(uint16_t UpDum,uint16_t DNum)
{
static  uint16_t i;
static  uint8_t CheckNum;

 CheckNum=0;
 for(i=UpDum+1;i<UpDum+DNum;i++)
 {
  CheckNum^=RMC_TX_DATA[i]; 
 }   
 return CheckNum;
}


uint8_t ProcessATOA(uint16_t GGAIData)
{
 static uint8_t Degree;   
	
 if(abs(GGAIData)>=100)
 {
  Degree=3;
 }
 else if(abs(GGAIData)>=10)
 {
  Degree=2;
 }
 else 
 {
  Degree=1;
 }
 
 
 if(Degree==3)                                                 //132
	{
	  GGB_Data[0] = (GGAIData)/100;	                                //百位    
    GGB_Data[1] = (GGAIData)/10    - ((uint16_t)GGB_Data[0])*10;    //十位        
	  GGB_Data[2] = (GGAIData)       - ((uint16_t)GGB_Data[0])*100 - ((uint16_t)GGB_Data[1]*10) ;  //个数        	

	  GGA_Data[0] = 0x30;   
	  GGA_Data[1] = GGB_Data[0] + 0x30;   
    GGA_Data[2] = GGB_Data[1] + 0x30;   
    GGA_Data[3] = GGB_Data[2] + 0x30;       
	}	 								   
	else if(Degree==2)                                         //13
	{
	  GGB_Data[0] = (GGAIData)/10;	                           //十位    
    GGB_Data[1] = (GGAIData)    -  GGB_Data[0]*10;   //个位        
	   	
    GGA_Data[0] = 0x30;   
		GGA_Data[1] = 0x30;   
	  GGA_Data[2] = GGB_Data[0] + 0x30;   
    GGA_Data[3] = GGB_Data[1] + 0x30;  
	}
   	else                                                                                                                  
	{
    GGB_Data[0] = (GGAIData);	                              //个位      

		GGA_Data[0] = 0x30;   
		GGA_Data[1] = 0x30; 
		GGA_Data[2] = 0x30; 
	  GGA_Data[3] = GGB_Data[0] + 0x30; 
	}

 return Degree;
}
//------------------------------------------------
//void ProcessSFTOA(float GGAIData,uint8_t bit)
//------------------------------------------------
uint8_t ProcessSFTOA(float GGAIData,uint8_t bit)
{

 static uint8_t Degree;    
 static uint8_t ZF_Flag;   
 
 if(GGAIData>=0)  
 {
  ZF_Flag=1;
 }
 else
 {
  ZF_Flag=0;
 }
//-------------------------------------
 if(abs(GGAIData)>=1000)
 {
  Degree=4;
 }
 else if(abs(GGAIData)>=100)
 {
  Degree=3;
 }
 else if(abs(GGAIData)>=10)
 {
  Degree=2;
 }
 else if(abs(GGAIData)>=1)
 {
  Degree=1;
 }
 else
 {
  Degree=0;
 }

 //--------------------------------------------------------------
   if(ZF_Flag==0)
   {
    GGAIData = -GGAIData;
   }
 //--------------------------------------------------------------  
   if(bit==2)	   				                       //"两位数字" + "." = "三位"
   {
    if(Degree==2)                                      //13.2==> 013;
	{
	  GGA_Data[0] = '0';	                               //百位    						          //.																	  	
    GGA_Data[1] = (GGAIData)/10;	                   //十位    						          //.
    GGA_Data[2] = (GGAIData)    - ((uint16_t)GGA_Data[0])*10;  //个位  		

	  GGA_Data[1] = GGA_Data[0] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;   
	}
	else if(Degree==1)                                 //9.8==>98==>9.8
	{
    GGA_Data[0] = (GGAIData*10)/10;	                               //十位
    GGA_Data[1] = 0x2E;						                       //.
    GGA_Data[2] = (GGAIData*10)    - ((uint16_t)GGA_Data[0])*10;   //个位  

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;   
	}
	else                                              //0.8==>08==>0.8
	{
	  GGA_Data[0] = (GGAIData*10)/10;	                               //十位
    GGA_Data[1] = 0x2E;						                       //.
    GGA_Data[2] = (GGAIData*10)    - ((uint16_t)GGA_Data[0])*10;   //个位  

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;   	
	}
   }												  
//--------------------------------------------------------------
   else	 if(bit==3)									              //"三位数字" + "." = "四位"
   {
    if(Degree==3)                                                 //132==>132==>132
	{
	  GGA_Data[0] = (GGAIData)/100;	                                //百位    
    GGA_Data[1] = (GGAIData)/10    - ((uint16_t)GGA_Data[0])*10;    //十位        
	  GGA_Data[2] = (GGAIData)       - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //个数        	
	  GGA_Data[3] = 0x2E;						                          //.

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[1] = GGA_Data[1] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;       
	}	 								   
	else if(Degree==2)                                            //13.2==>132==>13.2
	{
	  GGA_Data[0] = (GGAIData*10)/100;	                              //十位    
    GGA_Data[1] = (GGAIData*10)/10    - ((uint16_t)GGA_Data[0])*10;   //个位        
	  GGA_Data[2] = 0x2E;						                          //.
	  GGA_Data[3] = (GGAIData*10)       - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //小数        	

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[1] = GGA_Data[1] + 0x30;   
    GGA_Data[3] = GGA_Data[3] + 0x30;       
	}
   	else                                                              //1.32==>132==>1.32  //0.32==>32==>0.32    //0.03==3==>0.03	                                                                  
	{
    GGA_Data[0] = (GGAIData*100)/100;	                              //个位
    GGA_Data[1] = 0x2E;						                          //.
    GGA_Data[2] = (GGAIData*100)/10    - ((uint16_t)GGA_Data[0])*10;  //小位        
	  GGA_Data[3] = (GGAIData*100)       - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[2])*10 ;  //小位        

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;   
    GGA_Data[3] = GGA_Data[3] + 0x30;       
	}						
  }
//--------------------------------------------------------------
   else	if(bit==4)									                  //"四位数字" + "." = "五位"
   {
    if(Degree==4)                                                     //1321==>1321==>1321.
  	{
	  GGA_Data[0] = (GGAIData)/1000;	                                  //千位    
    GGA_Data[1] = (GGAIData)/100  - ((uint16_t)GGA_Data[0])*10;       //百位        
	  GGA_Data[2] = (GGAIData)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //十数        	
	  GGA_Data[3] = (GGAIData)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[1]*100)- ((uint16_t)GGA_Data[2]*10) ;  //十数        	
	  GGA_Data[4] = 0x2E;						                          //.

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[1] = GGA_Data[1] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;       
	  GGA_Data[3] = GGA_Data[3] + 0x30;       
	  }	 	
   	else if(Degree==3)                                                       //132.1==>1321==>132.1
	  {
    GGA_Data[0] = (GGAIData*10)/1000;	                                 //千位    
    GGA_Data[1] = (GGAIData*10)/100  - ((uint16_t)GGA_Data[0])*10;       //百位        
	  GGA_Data[2] = (GGAIData*10)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //十数        	
	  GGA_Data[3] = 0x2E;						                             //.
	  GGA_Data[4] = (GGAIData*10)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[1]*100)- ((uint16_t)GGA_Data[2]*10) ;  //??        	

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[1] = GGA_Data[1] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;       
	  GGA_Data[4] = GGA_Data[4] + 0x30;        
	  }	 
   	else if(Degree==2)                                                   //13.21==>1321==>13.21
	 {
    GGA_Data[0] = (GGAIData*100)/1000;	                                 //千位    
    GGA_Data[1] = (GGAIData*100)/100  - ((uint16_t)GGA_Data[0])*10;      //百位        
	  GGA_Data[2] = 0x2E;						                             //.
	  GGA_Data[3] = (GGAIData*100)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //十数        	
	  GGA_Data[4] = (GGAIData*100)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[1]*100)- ((uint16_t)GGA_Data[3]*10) ;  //十数        	

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[1] = GGA_Data[1] + 0x30;   
    GGA_Data[3] = GGA_Data[3] + 0x30;       
	  GGA_Data[4] = GGA_Data[4] + 0x30;        
	 }	 
	 else if(Degree==1)				  							        //1.321==>1321==>1.321    //0.132==>132==>0.132
	 {																											
    GGA_Data[0] = (GGAIData*1000)/1000;	                                 //千位    
	  GGA_Data[1] = 0x2E;													 //.
    GGA_Data[2] = (GGAIData*1000)/100  - ((uint16_t)GGA_Data[0])*10;     //百位        				                             //.
	  GGA_Data[3] = (GGAIData*1000)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[2]*10) ;  //十数        	
	  GGA_Data[4] = (GGAIData*1000)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[2]*100)- ((uint16_t)GGA_Data[3]*10) ;  //十数        	

	  GGA_Data[0] = GGA_Data[0] + 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;   
    GGA_Data[3] = GGA_Data[3] + 0x30;       
	  GGA_Data[4] = GGA_Data[4] + 0x30;        
	 }
	 else		               
	 {											 	                     //0.321==>321==>0.321    //0.321==>321==>0.321
    GGA_Data[0] = (GGAIData*1000)/1000;	                                 //千位    
	  GGA_Data[1] = 0x2E;													 //.
    GGA_Data[2] = (GGAIData*1000)/100  - ((uint16_t)GGA_Data[0])*10;     //百位        				                             //.
	  GGA_Data[3] = (GGAIData*1000)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[2]*10) ;  //十数        	
	  GGA_Data[4] = (GGAIData*1000)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[2]*100)- ((uint16_t)GGA_Data[3]*10) ;  //十数        	

	  GGA_Data[0] = 0x30;   
    GGA_Data[2] = GGA_Data[2] + 0x30;   
    GGA_Data[3] = GGA_Data[3] + 0x30;       
	  GGA_Data[4] = GGA_Data[4] + 0x30;        		
	  }										   
   }
   
   return ZF_Flag;										
}
//------------------------------------------------
//void ProcessITOA(uint8_t GGAIData,uint8_t bit)
//------------------------------------------------
void ProcessITOA(uint8_t GGAIData)
{									
 static  uint8_t  i;
  			     
   GGA_Data[0] = GGAIData/10 ;				   //十位
   GGA_Data[1] = GGAIData -GGA_Data[0]*10;	   //个位

   for(i=0;i<2;i++)
   {
    GGA_Data[i] = GGA_Data[i] + 0x30;   
   }
}



//------------------------------------------------
//void ProcessLZFTOA(float GGAIData,uint8_t bit)
//------------------------------------------------
uint8_t ProcessLZFTOA(uint16_t GGAIData)
{

 static uint8_t Degree,Num;    
 static uint8_t i;   
 

//-------------------------------------
 if(abs(GGAIData)>=10000)
 {
  Degree=5;
 }
 else if(abs(GGAIData)>=1000)
 {
  Degree=4;
 }
 else if(abs(GGAIData)>=100)
 {
  Degree=3;
 }
 else if(abs(GGAIData)>=10)
 {
  Degree=2;
 }
 else 
 {
  Degree=1;
 }
 

 //--------------------------------------------------------------  
    if(Degree==5)                                                   
  	{			
	  GGA_Data[0] = (GGAIData)/10000;	                                   
    GGA_Data[1] = (GGAIData)/1000  - ((uint16_t)GGA_Data[0])*10;        
	  GGA_Data[2] = (GGAIData)/100   - ((uint16_t)GGA_Data[0])*100 -  ((uint16_t)GGA_Data[1]*10) ;  //十数        	
	  GGA_Data[3] = (GGAIData)/10    - ((uint16_t)GGA_Data[0])*1000-  ((uint16_t)GGA_Data[1]*100) - ((uint16_t)GGA_Data[2]*10) ;  //十数        	
    GGA_Data[4] = (GGAIData)       - ((uint16_t)GGA_Data[0])*10000- ((uint16_t)GGA_Data[1]*1000)- ((uint16_t)GGA_Data[2]*100) -((uint16_t)GGA_Data[3]*10) ;  //十数         
	  }	 	
		
   else if(Degree==4)                                                   
  	{			
	  GGA_Data[0] = (GGAIData)/1000;	                                   
    GGA_Data[1] = (GGAIData)/100  - ((uint16_t)GGA_Data[0])*10;        
	  GGA_Data[2] = (GGAIData)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //十数        	
	  GGA_Data[3] = (GGAIData)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[1]*100)- ((uint16_t)GGA_Data[2]*10) ;  //十数        	
      
	  }	 	
   	else if(Degree==3)                                                  
	  {
    GGA_Data[0] = (GGAIData*10)/1000;	                                  
    GGA_Data[1] = (GGAIData*10)/100  - ((uint16_t)GGA_Data[0])*10;              
	  GGA_Data[2] = (GGAIData*10)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //十数        	
       
	  }	 
   	else if(Degree==2)                                                  
	 {
    GGA_Data[0] = (GGAIData*100)/1000;	                                    
    GGA_Data[1] = (GGAIData*100)/100  - ((uint16_t)GGA_Data[0])*10;            
       
	 }	 
	 else if(Degree==1)				  							                         
	 {																											
    GGA_Data[0] = (GGAIData*1000)/1000;	                             
	 }

	 //-----------------------------------------		

	  for(i=0;i<Degree;i++)
    {
      GGA_Data[i] = GGA_Data[i] + 0x30;   
    }
		
	  Num = Degree;   		
  
   
   return Num;										
}


//------------------------------------------------
//void ProcessZFTOA(float GGAIData,uint8_t bit)
//------------------------------------------------
uint8_t ProcessZFTOA(int16_t GGAIData)
{

 static uint8_t Degree,Num;    
 static uint8_t ZF_Flag,i;   
 
 if(GGAIData>=0)  
 {
  ZF_Flag=1;
 }
 else
 {
  ZF_Flag=0;
 }
//-------------------------------------
 if(abs(GGAIData)>=10000)
 {
  Degree=5;
 }
 else if(abs(GGAIData)>=1000)
 {
  Degree=4;
 }
 else if(abs(GGAIData)>=100)
 {
  Degree=3;
 }
 else if(abs(GGAIData)>=10)
 {
  Degree=2;
 }
 else 
 {
  Degree=1;
 }
 

 //--------------------------------------------------------------
   if(ZF_Flag==0)
   {
    GGAIData = -GGAIData;
   }
 //--------------------------------------------------------------  
  
    if(Degree==4)                                                   
  	{			
	  GGA_Data[0] = (GGAIData)/1000;	                                   
    GGA_Data[1] = (GGAIData)/100  - ((uint16_t)GGA_Data[0])*10;        
	  GGA_Data[2] = (GGAIData)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //十数        	
	  GGA_Data[3] = (GGAIData)      - ((uint16_t)GGA_Data[0])*1000- ((uint16_t)GGA_Data[1]*100)- ((uint16_t)GGA_Data[2]*10) ;  //十数        	
      
	  }	 	
   	else if(Degree==3)                                                  
	  {
    GGA_Data[0] = (GGAIData*10)/1000;	                                  
    GGA_Data[1] = (GGAIData*10)/100  - ((uint16_t)GGA_Data[0])*10;              
	  GGA_Data[2] = (GGAIData*10)/10   - ((uint16_t)GGA_Data[0])*100 - ((uint16_t)GGA_Data[1]*10) ;  //十数        	
       
	  }	 
   	else if(Degree==2)                                                  
	 {
    GGA_Data[0] = (GGAIData*100)/1000;	                                    
    GGA_Data[1] = (GGAIData*100)/100  - ((uint16_t)GGA_Data[0])*10;            
       
	 }	 
	 else if(Degree==1)				  							                         
	 {																											
    GGA_Data[0] = (GGAIData*1000)/1000;	                             
	 }

	 //-----------------------------------------		
   if(ZF_Flag==1)
	 {
	   for(i=0;i<Degree;i++)
    {
      GGA_Data[i] = GGA_Data[i] + 0x30;   
    }
		
		 Num = Degree;
	 }
	 else
	 {
		 for(i=0;i<Degree;i++)
    {
      GGB_Data[i] = GGA_Data[i];   
    }
		 
		 for(i=0;i<Degree;i++)
    {
      GGA_Data[i+1] = GGB_Data[i]+0x30;   
    }
	 
		 GGA_Data[0]='-';
		
	   Num = Degree+1;
	 }
    		
  
   
   return Num;										
}
/*
//------------------------------------------------
//void ProcessITOA(uint8_t GGAIData,uint8_t bit)
//------------------------------------------------
uint8_t ProcessZFTOA(int16_t GGAIData)
{									
 static  uint8_t  i,Len;
  			     
	 if(GGAIData>=0)
	 {
	   if(GGAIData>100)
		 {
		   GGAIData=100;
		 }
		 
		 GGA_Data[0] = GGAIData/10 ;				   //十位
		 GGA_Data[1] = GGAIData -GGA_Data[0]*10;	   //个位

		 for(i=0;i<2;i++)
		 {
			GGA_Data[i] = GGA_Data[i] + 0x30;   
		 } 
		 
		 Len=2;
	 }
	 else
	 {
	   if(GGAIData<-99)
		 {
		   GGAIData=-99;
		 }
		 
		 GGAIData = -GGAIData;
		 
		 GGA_Data[1] = GGAIData/10 ;				   //十位
		 GGA_Data[2] = GGAIData -GGA_Data[1]*10;	   //个位

		 for(i=1;i<3;i++)
		 {
			GGA_Data[i] = GGA_Data[i] + 0x30;   
		 } 
		 
		 GGA_Data[0]='-';
		 
		 Len=3;
	 } 

	 
	 return Len;
}
*/

//------------------------------------------------
//void ProcessITOA(uint8_t GGAIData,uint8_t bit)
//------------------------------------------------
void ProcessIITOA(uint16_t GGAIData)
{									
 static  uint8_t  i;
  			     
   GGA_Data[0] = GGAIData/1000 ;				   //十位
   GGA_Data[1] = GGAIData/100 - GGA_Data[0]*10;	   //个位
   GGA_Data[2] = GGAIData/10  - GGA_Data[0]*100  - GGA_Data[1]*10;	   //个位
	 GGA_Data[3] = GGAIData     - GGA_Data[0]*1000 - GGA_Data[1]*100 - GGA_Data[2]*10;	   //个位
	 
   for(i=0;i<4;i++) 
   {
    GGA_Data[i] = GGA_Data[i] + 0x30;   
   }
}




uint8_t ProcessAgeTOA(uint16_t GGAIData)
{
 static uint8_t Degree;   
	
 if(abs(GGAIData)>=100)
 {
  Degree=3;
 }
 else if(abs(GGAIData)>=10)
 {
  Degree=2;
 }
 else 
 {
  Degree=1;
 }
 
 
 if(Degree==3)                                                 //132
	{
	  GGB_Data[0] = (GGAIData)/100;	                                //百位    
    GGB_Data[1] = (GGAIData)/10    - ((uint16_t)GGB_Data[0])*10;    //十位        
	  GGB_Data[2] = (GGAIData)       - ((uint16_t)GGB_Data[0])*100 - ((uint16_t)GGB_Data[1]*10) ;  //个数        	
	  
	  GGA_Data[0] = GGB_Data[0] + 0x30;   
    GGA_Data[1] = GGB_Data[1] + 0x30;   
		GGA_Data[2] = 0x2E;	 
    GGA_Data[3] = GGB_Data[2] + 0x30;       
	}	 								   
	else if(Degree==2)                                         //13
	{
	  GGB_Data[0] = (GGAIData)/10;	                           //十位    
    GGB_Data[1] = (GGAIData)    -  GGB_Data[0]*10;   //个位        
	
		GGA_Data[0] = GGB_Data[0] + 0x30;   
		GGA_Data[1] = 0x2E;	 
    GGA_Data[2] = GGB_Data[1] + 0x30; 
    GGA_Data[3] = 0x30;   
	}
  else                                                                                                                  
	{     
		GGA_Data[0] = 0x30;   
		GGA_Data[1] = 0x2E;	 
		GGA_Data[2] = 0x30; 
	  GGA_Data[3] = 0x30; 
	}

 return Degree;
}

void  GetGPRMC(GPRMC_DATA GPRMCData)
{

	float MisAngleT;
		
	uint8_t  CheckNumData;	
		

	uint8_t i;
	uint8_t ZF_Flag;	
	uint8_t  Hea_Data[6];         //$GPGGA    = 6              ??
	uint8_t  TEN_Data[1];
		
	//----------------RMC-------------------------------------------------
	uint8_t  UTC_Data[9];          //hhmmss.ss  =  9            UTC??
	uint8_t  Lat_Data[15];         //ddmm.mmmmmmm = 12          ??
	uint8_t  SNth_Data[1];		     //N S
	uint8_t  Lon_Data[15];         //dddmm.mmmmmmm= 13          ??
	uint8_t  WEst_Data[1];         //W E

	uint8_t  Vad_Data[1];          //A V		 
	uint8_t  Spd_Data[5];          //138.1/13.81/1.381/0.1831   //Speed Over Ground, Knots  1.852??/??;??????100knots ?????? ???4?;
	uint8_t  Tra_Data[9];          //128.1/12.81/1.281/0.1281   //Track made good; Degree						 
	uint8_t  Dat_Data[6];          //ddmmyy                     //??
	uint8_t  Mag_Data[4];          //980/098/9.8/0.9            //Magnetic Variatio     ??????	???3?
	uint8_t  Mod_Data[1];          //A,D,E,N                    //Mode Indicator        Ublox
	uint8_t  BAngle_Data[5];
	float    RMCGroundSpeed;
	//-------------------------------
	//          GPRMC
	//-------------------------------

	//--------字段----数-------------

//	 if(GPRMCData.Gp_PN_Kind==1)
//	 {
//		Hea_Data[0] = '$';
//		Hea_Data[1] = 'G';
//		Hea_Data[2] = 'P';
//		Hea_Data[3] = 'R';
//		Hea_Data[4] = 'M';
//		Hea_Data[5] = 'C';
//	 }
//	 else if(GPRMCData.Gp_PN_Kind==2)
//	 {
//		Hea_Data[0] = '$';
//		Hea_Data[1] = 'G';
//		Hea_Data[2] = 'L';
//		Hea_Data[3] = 'R';
//		Hea_Data[4] = 'M';
//		Hea_Data[5] = 'C';  
//	 }
//	 else if(GPRMCData.Gp_PN_Kind==3)
//	 {
//		Hea_Data[0] = '$';
//		Hea_Data[1] = 'B';
//		Hea_Data[2] = 'D';
//		Hea_Data[3] = 'R';
//		Hea_Data[4] = 'M';
//		Hea_Data[5] = 'C';  
//	 }
//	 else 
//	 {
		Hea_Data[0] = '$';
		Hea_Data[1] = 'G';
		Hea_Data[2] = 'N';
		Hea_Data[3] = 'R';
		Hea_Data[4] = 'M';
		Hea_Data[5] = 'C';  
//	 }


	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
		 Vad_Data[0] = GPRMCData.DataValid; 
		 
		
	 }



	 //----------1-----9----------------
	//UTC时间
	//--------------------------------

		//ProcessITOA(GINavResult.UtcTime.Hour);
		ProcessITOA(GPRMCData.Hour);
		UTC_Data[0]  = GGA_Data[0];    //0
		UTC_Data[1]  = GGA_Data[1];    //1

		//ProcessITOA(GINavResult.UtcTime.Minute);
		ProcessITOA(GPRMCData.Minute);
		UTC_Data[2]  = GGA_Data[0];	//2
		UTC_Data[3] = GGA_Data[1];	//3
	 
		
		//ProcessITOA(GINavResult.UtcTime.Second);
		ProcessITOA(GPRMCData.Second);
		UTC_Data[4] = GGA_Data[0];	//4
		UTC_Data[5] = GGA_Data[1];	//5

		UTC_Data[6] = '.';	        //6

		//ProcessITOA(GINavResult.UtcTime.MillSecond/10);
		if(GPRMCData.MSecond>=100)
		{
		 GPRMCData.MSecond=GPRMCData.MSecond/10;
		}
		
		ProcessITOA(GPRMCData.MSecond);

		UTC_Data[7] = GGA_Data[0];	//7
		UTC_Data[8] = GGA_Data[1];	//8


	//----------2------10----------------
	//维度
	//--------------------------------	

	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
		/*
		 if(GINavResult.GpsStatus&&GINavResult.Gpsvalid)
		 {
			Vad_Data[0] = GPRMCData.DataValid; 
		 }
		 else
		 {
			Vad_Data[0] ='V';
		 }*/
		 
			Vad_Data[0] = GPRMCData.DataValid; 
	 }
	 else
	 {
		 Vad_Data[0] = 'A'; 
	 }
	//----------2------10----------------
	//维度
	//--------------------------------
	//----------2------10----------------
	//维度
	//--------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {  
			ProcessLatToA(GPRMCData.Latitude);   //10-19;		
	 }
	 else
	 {		
			ProcessLatToA(GINavResult.Position.Lat*RAD2DEG);   //10-19;	  	
	 }
	 
	 
		for(i=0;i<10;i++)
		{
		 Lat_Data[i] = GGA_Data[i];  
		}	
		
												 
		
		//----------3------1----------------	
	//南北
	//--------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
		{		
			if((GPRMCData.SNth=='N')||(GPRMCData.SNth=='S'))
			{
			 SNth_Data[0] = GPRMCData.SNth;	         //21; < 0 = South, > 0 = North
			}
		}
		else
		{
		}

		
	 //--------------------------------
	 //如果车库启动
	 //--------------------------------
		if((GPRMCData.SNth=='N')||(GPRMCData.SNth=='S'))
		{
			 SNth_Data[0] = GPRMCData.SNth;	         //21; < 0 = South, > 0 = North
			
		}
		else
		{
		
		 }
		
	//----------4-------11----------------	
	//经度
	//--------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
			ProcessLongToA(GPRMCData.Longitude);   //10-19	 
	 }
	 else
	 { 
			ProcessLongToA(GINavResult.Position.Lon*RAD2DEG);   //10-19;	 	 	 
	 }
	 
	 
		for(i=0;i<11;i++)
		{
		 Lon_Data[i] = GGA_Data[i];  
		}											 
		
		

		
		
	//----------5-------1----------------	
	//东西
	//--------------------------------

		
	//----------5-------1----------------	
	//东西
	//--------------------------------

		
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
		{
		 if((GPRMCData.WEst=='W')||(GPRMCData.WEst=='E'))
		 {
			WEst_Data[0] = GPRMCData.WEst;	           //35;	< 0 = West, > 0 = East
		 }
		 
			
		}
		else
		{
			if((GPRMCData.WEst=='W')||(GPRMCData.WEst=='E'))
			{
			 WEst_Data[0] = GPRMCData.WEst;	           //35;	< 0 = West, > 0 = East
			}
				
		}	
		
		
	 //----------------------------------------
	 //
	 //---------------------------------------- 
		
	 
	 
	 //--------------------------------
	 //如果车库启动
	 //--------------------------------
		if((GPRMCData.SNth=='N')||(GPRMCData.SNth=='S'))
		{
			 SNth_Data[0] = GPRMCData.SNth;	         //21; < 0 = South, > 0 = North
		}
		else
		{
		 
		}
		
	//----------7-----5------------------   
	// 地速
	//---------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
			RMCGroundSpeed=GPRMCData.GroundSpeed;	
	 }
	 else
	 {
		 RMCGroundSpeed=0/0.5144444444444;	 
		
	 }	

	 
	 
		ProcessSFTOA(GPRMCData.GroundSpeed,4);  
	 
		Spd_Data[0]= GGA_Data[0];
		Spd_Data[1]= GGA_Data[1];
		Spd_Data[2]= GGA_Data[2];
		Spd_Data[3]= GGA_Data[3];
		Spd_Data[4]= GGA_Data[4];

	//----------8-----5------------------
	// 方向
	//---------------------------------
	 if (!IS_INS_ALIGNED(g_GINavInfo.INSState)) 
		{
		 ZF_Flag=ProcessSFTOA(GPRMCData.Course,4);  
		}
		else
		{
		 ZF_Flag=ProcessSFTOA(GINavResult.Attitude.Heading,4);  
		}
		
		if(ZF_Flag)
		{
		 Tra_Data[0]= GGA_Data[0];
		 Tra_Data[1]= GGA_Data[1];
		 Tra_Data[2]= GGA_Data[2];
		 Tra_Data[3]= GGA_Data[3];
		 Tra_Data[4]= GGA_Data[4];
		}
		else                                 //此处不合理，-102.2==>-102 所以应该是动态....
		{
		 Tra_Data[0]= '-';
		 Tra_Data[1]= GGA_Data[0];
		 Tra_Data[2]= GGA_Data[1];
		 Tra_Data[3]= GGA_Data[2];
		 Tra_Data[4]= GGA_Data[3];
		}
	//----------9-----6------------------
	//ddmmyy 由ProcessGPRMC处理
	//----------------------------------
		ProcessITOA(GPRMCData.Day);
		Dat_Data[0]  = GGA_Data[0];  //0
		Dat_Data[1]  = GGA_Data[1];  //1

		ProcessITOA(GPRMCData.Month);
		Dat_Data[2]  = GGA_Data[0];	//2
		Dat_Data[3] = GGA_Data[1];	//3
	 
		ProcessITOA(GPRMCData.Year-2000);
		Dat_Data[4] = GGA_Data[0];	//4
		Dat_Data[5] = GGA_Data[1];	//5
	//---------10-----3------------------
	// 磁场变化值			   x.x
	//---------------------------------
		if(GPRMCData.MagVarValid)
		{
		 ProcessSFTOA(GPRMCData.MagVar,3); 
		 Mag_Data[0]= GGA_Data[0];
		 Mag_Data[1]= GGA_Data[1];
		 Mag_Data[2]= GGA_Data[2];
		 Mag_Data[3]= GGA_Data[3];
		}
		else
		{
		 Mag_Data[0]= '0';
		 Mag_Data[1]= '.';
		 Mag_Data[2]= '0';
		 Mag_Data[3]= '0';
		}
	//---------11-----3------------------
	//Magnetic West East 由ProcessGGA处理
	//-----------------------------------
	 //------------------------------------
	 //  BAngle_Data
	 //------------------------------------ 

		

		ZF_Flag=ProcessSFTOA(0,4);
		
		if(ZF_Flag==1)
		{
		 BAngle_Data[0] = GGA_Data[0];
		 BAngle_Data[1] = GGA_Data[1];
		 BAngle_Data[2] = GGA_Data[2];
		 BAngle_Data[3] = GGA_Data[3];
		 BAngle_Data[4] = GGA_Data[4];  
		}
		else
		{
		 BAngle_Data[0] = '-';
		 BAngle_Data[1] = GGA_Data[0];
		 BAngle_Data[2] = GGA_Data[1];
		 BAngle_Data[3] = GGA_Data[2];
		 BAngle_Data[4] = GGA_Data[3];   
		}
	//----------12----1------------------
	//ModeIndictor   
	//----------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
		/* if(GINavResult.GpsStatus&&GINavResult.Gpsvalid)
		 {
			Mod_Data[0] = GPRMCData.ModeIn;
		 }
		 else
		 {
			Mod_Data[0] = 'N';
		 }*/
		 
			Mod_Data[0] = GPRMCData.ModeIn;
	 }
	 else
	 {
			Mod_Data[0] ='A'; 
	 }
	//----------13----1------------------
	//*  				 由ProcessGGA处理
	//----------------------------------
		TEN_Data[0]='*';
	//----------14----1------------------
	//CheckSum
	//----------------------------------

	//----------15----1------------------
	//回车换行			 由ProcessGGA处理
	//----------------------------------
		
		
		
		
	 //----------0--------6---------------
	 //$GPRMC			 0-5,6
	 //-----------------------------------
	 
	 
		for(i=0;i<6;i++)		          
		RMC_TX_DATA[i] = Hea_Data[i];

		RMC_TX_DATA[6] = ',';
	 
	 //----------1--------9---------------
	 //UTC hhmmss.ss    7-15,16
	 //-----------------------------------
		for(i=0;i<9;i++)		          
		RMC_TX_DATA[7+i] = UTC_Data[i];

		RMC_TX_DATA[16] = ',';

	 //----------2--------1---------------
	 //DataValid         17,18
	 //-----------------------------------    
		RMC_TX_DATA[17] = Vad_Data[0];
		RMC_TX_DATA[18] = ',';

	 //----------3--------10---------------
	 //维度 ddmm.mmmmm    19-30,31
	 //-----------------------------------
		for(i=0;i<10;i++)		          
		RMC_TX_DATA[19+i] = Lat_Data[i];

		RMC_TX_DATA[29] = ',';

		
	 //----------4--------1---------------
	 //维度 N S           30,31
	 //-----------------------------------
		RMC_TX_DATA[30] = SNth_Data[0];
		RMC_TX_DATA[31] = ',';


	 //----------5--------11---------------
	 //经度dddmm.mmmmmmmmm     34-46,47
	 //-----------------------------------
		for(i=0;i<11;i++)		          
		RMC_TX_DATA[32+i] = Lon_Data[i];

		RMC_TX_DATA[43] = ',';
	 
	 //----------6-----------1---------------
	 //东西 W E            48,49
	 //-----------------------------------
		RMC_TX_DATA[44] = WEst_Data[0];
		RMC_TX_DATA[45] = ',';


	 //----------7-----------5---------------
	 //Speed               50-53,54
	 //-----------------------------------
		for(i=0;i<5;i++)		          
		RMC_TX_DATA[46+i] = Spd_Data[i];
	 
		RMC_TX_DATA[50] = ',';

	 //----------8-----------5---------------
	 //Track_Mode          55-59,60
	 //-----------------------------------
		for(i=0;i<5;i++)		          
		RMC_TX_DATA[51+i] = Tra_Data[i];
		

	 //-----------------------------------------
		RMC_TX_DATA[56] = ',';
	 //----------9-----------6---------------
	 //Data                61-66,67
	 //-----------------------------------
		for(i=0;i<6;i++)		          
		RMC_TX_DATA[57+i] = Dat_Data[i];
	 
		RMC_TX_DATA[63] = ',';

	 //----------10-----------3---------------
	 //Mag_Data             68-70,71
	 //-----------------------------------

		for(i=0;i<5;i++)		          
		RMC_TX_DATA[64+i] = BAngle_Data[i];

	 
		RMC_TX_DATA[69] = ',';

	 //----------11-----------1---------------
	 //东西 W E            72,73
	 //-----------------------------------
		if(GPRMCData.MagVarValid)
		{
		 RMC_TX_DATA[70] = GPRMCData.MagWEst;
		}
		else
		{
		 RMC_TX_DATA[70] = '0';
		}
		
		RMC_TX_DATA[71] = ',';

	 //----------12---------1-----------
	 //ModeIndictor   		74,75,76,77
	 //----------------------------------
		RMC_TX_DATA[72] = Mod_Data[0];
		RMC_TX_DATA[73] = TEN_Data[0];         
		
		CheckNumData    = ProcessCheckResult(0,73);
		ProcessCheckToA(CheckNumData);
		RMC_TX_DATA[74] = GGA_Data[0];	
		RMC_TX_DATA[75] = GGA_Data[1];	          		
						
	 //----------15-----------1---------------
	 //回车换行			   74 75
	 //-------------------------------------- 
		RMC_TX_DATA[76] = 0x0D;	          
		RMC_TX_DATA[77] = 0x0A;	          	
			
	 //---------------------------------------
	 //启动发送
	 //---------------------------------------

		RMC_TX_Max = 78;   	 
	 printf("%s",RMC_TX_DATA);
	//保存GPS数据
	 sd_savegps("0:SaveData/GpsData.txt",RMC_TX_DATA);
//		GGA_UP_Num = RMC_TX_Max;
//		DIS_UP_Num = RMC_TX_Max;
}

void  GetLGPGGA(GPGGA_DATA GPGGAData)
{
	uint8_t i=0;
	uint8_t ZF_Flag=0;
	uint8_t  CheckNumData;
		
		
	uint8_t  Hea_Data[6];         //$GPGGA    = 6              ??
	uint8_t  TEN_Data[1];         //*         = 1               *

		
	//-----------------GGA------------------------------------------
	uint8_t  UTC_Data[9];          //hhmmss.ss  =  9            UTC??
	uint8_t  Lat_Data[15];         //ddmm.mmmmmmm = 12          ??
	uint8_t  SNth_Data[1];		     //N S
	uint8_t  Lon_Data[15];         //dddmm.mmmmmmm= 13          ??
	uint8_t  WEst_Data[1];         //W E
	uint8_t  GPSQ_Data[1];         //1,2,3,4                    GPSQuality
	uint8_t  NumS_Data[2];         //08                         NumOfSatsInUse
	uint8_t  HDOP_Data[4];         //1321/0132/13.2/1.32/0.13/  HDOP	    ?????? ???4?;
	uint8_t  Alti_Data[8];         //1020/102./10.2/0.10/0.01/  Altitude	?????? ???4?;
	uint8_t  Unit_Data[1];         //M                          Unites;
	uint8_t  Geoi_Data[5];         //980/098/9.8/0.9/           Geoidal		?????? ???3?;
	uint8_t  DTim_Data[4];         //                           DTime 		?????? ???1?;
	uint8_t  DFID_Data[4];         //0000                       DGPS ID					 ???4?;
		
	//-------------------------------
	//          GPGGA
	//-------------------------------

	//--------字段----数-------------

	//----------0-----6---------------
	//帧头
	//--------------------------------

//		if(Select_Data.GNGASelect)
//		{
		 Hea_Data[0] = '$';
		 Hea_Data[1] = 'G';
		 Hea_Data[2] = 'N';
		 Hea_Data[3] = 'G';
		 Hea_Data[4] = 'G';
		 Hea_Data[5]=  'A';
//	 }
//	 else
//	 {
//		 Hea_Data[0] = '$';
//		 Hea_Data[1] = 'G';
//		 Hea_Data[2] = 'P';
//		 Hea_Data[3] = 'G';
//		 Hea_Data[4] = 'G';
//		 Hea_Data[5]=  'A';
//	 }


	 
	//----------1-----9----------------
	//UTC时间
	//--------------------------------

		//ProcessITOA(GINavResult.UtcTime.Hour);
		ProcessITOA(GPGGAData.Hour);
		UTC_Data[0]  = GGA_Data[0];    //0
		UTC_Data[1]  = GGA_Data[1];    //1

		//ProcessITOA(GINavResult.UtcTime.Minute);
		ProcessITOA(GPGGAData.Minute);
		UTC_Data[2]  = GGA_Data[0];	//2
		UTC_Data[3] = GGA_Data[1];	//3
	 
		//ProcessITOA(GINavResult.UtcTime.Second); 
		ProcessITOA(GPGGAData.Second);

		UTC_Data[4] = GGA_Data[0];	//4
		UTC_Data[5] = GGA_Data[1];	//5

		UTC_Data[6] = '.';	        //6

		//ProcessITOA(GINavResult.UtcTime.MillSecond/10);
		if(GPGGAData.MSecond>=100)
		{
			GPGGAData.MSecond=GPGGAData.MSecond/10;
		}
		
		ProcessITOA(GPGGAData.MSecond);
		UTC_Data[7] = GGA_Data[0];	//7
		UTC_Data[8] = GGA_Data[1];	//8

	 

	//----------2------10----------------
	//维度
	//--------------------------------
	 //----------2------10----------------
	//维度
	//--------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
			ProcessLatToA(GPGGAData.Latitude);   //10-19;
	 }
	 else
	 { 
			ProcessLatToA(GINavResult.Position.Lat*RAD2DEG);     //10-19;	 		
	 }

		for(i=0;i<10;i++)
		{
		 Lat_Data[i] = GGA_Data[i];  
		}
		
	//----------3------1----------------	
	//南北
	//--------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
		{		
			if((GPGGAData.SNth=='N')||(GPGGAData.SNth=='S'))
			{
			 SNth_Data[0] = GPGGAData.SNth;	         //21; < 0 = South, > 0 = North
			}
			else
			{				
		
			}	
		}
		else
		{
			if((GPGGAData.SNth=='N')||(GPGGAData.SNth=='S'))
			{
			 SNth_Data[0] = GPGGAData.SNth;	         //21; < 0 = South, > 0 = North
			}
				 
		}
	 


	 
	 //--------------------------------
	 //如果车库启动,且没有见过GPS
	 //--------------------------------
		if((GPGGAData.SNth=='N')||(GPGGAData.SNth=='S'))
		{
			 SNth_Data[0] = GPGGAData.SNth;	         //21; < 0 = South, > 0 = North
			
			
		}
		else
		{
		 
		}
	//----------4-------11----------------	
	//经度
	//-------------------------------- 
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
		 ProcessLongToA(GPGGAData.Longitude);   //10-19;	
	 }
	 else
	 {
		 ProcessLongToA(GINavResult.Position.Lon*RAD2DEG);     //10-19;	 			
	 } 
	 
		for(i=0;i<11;i++)
		{
		 Lon_Data[i] = GGA_Data[i];  
		}
		
		

		
	//----------5-------1----------------	
	//东西
	//--------------------------------

		
		if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
		{	 
			WEst_Data[0] = GPGGAData.WEst;	           //35;	< 0 = West, > 0 = East
		}
		else
		{
			if((GPGGAData.WEst=='W')||(GPGGAData.WEst=='E'))
			{
			 WEst_Data[0] = GPGGAData.WEst;	           //35;	< 0 = West, > 0 = East
			}		
		}
		
	 

	 

	 //--------------------------------
	 //如果车库启动,且没有见过GPS
	 //--------------------------------
		if((GPGGAData.WEst=='W')||(GPGGAData.WEst=='E'))
		{
			 WEst_Data[0] = GPGGAData.WEst;	           //35;	< 0 = West, > 0 = East
			

		}
		else
		{

		}

	//----------6-------6-----------------
	//标志
	//--------------------------------
		/*if(GPGGAData.GPSQuality==0x04) 
		{
		 GPSQ_Data[0] = GPGGAData.GPSQuality+0x30;  //37;    
		}
		 else
		{
		 GPSQ_Data[0] = 0x36;                       //37;    
		}
	 */

	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
		 /*if(GINavResult.GpsStatus&&GINavResult.Gpsvalid)
		 {
			GPSQ_Data[0] = GPGGAData.GPSQuality+0x30;  //37;
		 }
		 else
		 {
			GPSQ_Data[0] = 0x30;  //37;
		 }*/
		 
		 GPSQ_Data[0] = GPGGAData.GPSQuality+0x30;  //37;
	 }
	 else
	 {
			GPSQ_Data[0] = GPGGAData.GPSQuality+0x30;  //37;
	 }

			
	 if ((IS_INS_ALIGNED(g_GINavInfo.INSState))) 
		{
			if((GPGGAData.GPSQuality==0)||(g_GINavInfo.Jugde==0))
			//if(GPGGAData.GPSQuality==0)
			{
			 GPSQ_Data[0] =0x36;  //37;		  
			}
		}  
	//-----------7------2----------------- 
	//星数
	//--------------------------------


			
		 ProcessITOA(GPGGAData.NumOfSatsInUse);	 
		 NumS_Data[0]  = GGA_Data[0];	          //39;
		 NumS_Data[1]  = GGA_Data[1];            //40; 

	//-----------8------4----------------
	//精度
	//--------------------------------
			
		ProcessSFTOA(0,3); 	
		HDOP_Data[0] = GGA_Data[0];	           //42;
		HDOP_Data[1] = GGA_Data[1];              //43; 
		HDOP_Data[2] = GGA_Data[2];              //44; 
		HDOP_Data[3] = GGA_Data[3];              //45; 
		
	//-----------9------4-----------	
	//高度
	//--------------------------------
	 if ((!IS_INS_ALIGNED(g_GINavInfo.INSState))) 
	 {
		ZF_Flag=ProcessSFTOA(GPGGAData.Altitude,4);  	 
	 }
	 else
	 {	
		ZF_Flag=ProcessSFTOA(GINavResult.Position.Alt,4);  
	}  
	 
		 

		if(ZF_Flag==1)
		{
		 Alti_Data[0] = GGA_Data[0];	            //47;
		 Alti_Data[1] = GGA_Data[1];              //48; 
		 Alti_Data[2] = GGA_Data[2];              //49; 
		 Alti_Data[3] = GGA_Data[3];              //50; 
		 Alti_Data[4] = GGA_Data[4];              //50;   
		}
		else
		{
		 Alti_Data[0] = '-';	                    //47;
		 Alti_Data[1] = GGA_Data[0];              //48; 
		 Alti_Data[2] = GGA_Data[1];              //49; 
		 Alti_Data[3] = GGA_Data[2];              //50; 
		 Alti_Data[4] = GGA_Data[3];              //50;     
		}
		

	 
	//-----------10----1------------	
	//单位
	//--------------------------------

		Unit_Data[0] = 'M';	                  //52;  
	 
	 

	//-----------11----3------------
	//水平高度
	//--------------------------------
//	 if(Select_Data.DebSelect)
//	 {
	 ZF_Flag=ProcessSFTOA(GINavResult.Attitude.Pitch,4);
//	 }
//	 else
//	 {
//	 ZF_Flag=ProcessSFTOA(Gps_raw.gga_data.Geoidal,4);
//	 } 
	 
		if(ZF_Flag)
		{
		Geoi_Data[0] = GGA_Data[0];	           //54;
		Geoi_Data[1] = GGA_Data[1];              //55; 
		Geoi_Data[2] = GGA_Data[2];              //56; 
		Geoi_Data[3] = GGA_Data[3];              //56; 
		Geoi_Data[4] = GGA_Data[4];              //56;  
		}
		else
		{
		Geoi_Data[0] = '-';	           //54;
		Geoi_Data[1] = GGA_Data[0];              //55; 
		Geoi_Data[2] = GGA_Data[1];              //56; 
		Geoi_Data[3] = GGA_Data[2];              //56; 
		Geoi_Data[4] = GGA_Data[3];              //56;   
		}
	 
	//------------12---1-------------	
	//单位
	//--------------------------------


	//------------13---1-------------	
	//DPS Time		GGA处理
	//--------------------------------


			ZF_Flag = ProcessSFTOA(GPGGAData.DTime,3);

		


	 DTim_Data[0] = GGA_Data[0];	           //42;
	 DTim_Data[1] = GGA_Data[1];             //43; 
	 DTim_Data[2] = GGA_Data[2];             //44; 
	 DTim_Data[3] = GGA_Data[3];             //45;  		    


	 
			ProcessIITOA(0);  

		 DFID_Data[0]=GGA_Data[0];
		 DFID_Data[1]=GGA_Data[1];
		 DFID_Data[2]=GGA_Data[2];
		 DFID_Data[3]=GGA_Data[3];		
		 
	//------------15---1-------------	
	//结尾
	//--------------------------------
		TEN_Data[0]  = '*';                	  //64; 
	//------------15---2-------------
	//CheckNum
	//--------------------------------


//	if(GGA_UP_Num>=100)
//		GGA_UP_Num=100;

	//--------------------------------------------------


			
	 //----------0--------6---------------
	 //$GPGGA			 0-5,6
	 //-----------------------------------

		for(i=0;i<6;i++)		          
		GGA_TX_DATA[i] = Hea_Data[i];

		GGA_TX_DATA[6] = ',';
	 
	 //----------1--------9---------------
	 //UTC hhmmss.ss    7-15,16
	 //-----------------------------------
		for(i=0;i<9;i++)		          
		GGA_TX_DATA[7+i] = UTC_Data[i];

		GGA_TX_DATA[16] = ',';

	 //----------2--------10---------------
	 //维度 ddmm.mmmmm    17-38,29
	 //-----------------------------------
		for(i=0;i<10;i++)		          
		GGA_TX_DATA[17+i] = Lat_Data[i];
		
		GGA_TX_DATA[27] = ',';
	 //----------3--------1---------------
	 //维度 N S           30,31
	 //-----------------------------------
		GGA_TX_DATA[28] = SNth_Data[0];
		GGA_TX_DATA[29] = ',';
	 

	 //----------4--------11---------------
	 //经度dddmm.mmmmm     32-44,45
	 //-----------------------------------
		for(i=0;i<11;i++)		          
		GGA_TX_DATA[30+i] = Lon_Data[i];

		GGA_TX_DATA[41] = ',';


	 //----------5-----------1---------------
	 //东西 W E            46,47
	 //-----------------------------------
		GGA_TX_DATA[42] = WEst_Data[0];
		GGA_TX_DATA[43] = ',';

	 //----------6-----------1---------------
	 //GPSQuality          48,49
	 //-----------------------------------
		GGA_TX_DATA[44] = GPSQ_Data[0];
		GGA_TX_DATA[45] = ',';
	 //----------7-----------2---------------
	 //NumOfStar          50-51,52
	 //-----------------------------------
		GGA_TX_DATA[46] = NumS_Data[0];
		GGA_TX_DATA[47] = NumS_Data[1];
		GGA_TX_DATA[48] = ',';
	 //----------8-----------4---------------
	 //HDOP               53-56,57
	 //-----------------------------------
		for(i=0;i<4;i++)		          
		GGA_TX_DATA[49+i] = HDOP_Data[i];
	 
		GGA_TX_DATA[53] = ',';

	 //----------9-----------4---------------
	 //Alititude          58-62,63
	 //-----------------------------------
		for(i=0;i<5;i++)		          
		GGA_TX_DATA[54+i] = Alti_Data[i];
	 
		GGA_TX_DATA[59] = ',';
	 //----------10-----------1------------
	 //M                   64,65
	 //-----------------------------------
		GGA_TX_DATA[60] = Unit_Data[0];
		GGA_TX_DATA[61] = ',';

	 //----------11-----------3---------------
	 //Geoidal            66-70,71
	 //--------------------------------------
		for(i=0;i<5;i++)		          
		GGA_TX_DATA[62+i]= Geoi_Data[i];
		
		GGA_TX_DATA[67] = ',';

	 //----------12-----------1------------
	 //M                   72,73
	 //-----------------------------------
		GGA_TX_DATA[68] = Unit_Data[0];
		GGA_TX_DATA[69] = ',';

	 //----------12-----------1------------
	 //DTime                74,75
	 //-----------------------------------
		GGA_TX_DATA[70] = DTim_Data[0];
		GGA_TX_DATA[71] = DTim_Data[1];
		GGA_TX_DATA[72] = DTim_Data[2];
		GGA_TX_DATA[73] = DTim_Data[3];
		GGA_TX_DATA[74] = ',';

	 //----------13-----------4---------------
	 //ID                 76-79,80
	 //--------------------------------------

		for(i=0;i<4;i++)		          
		GGA_TX_DATA[75+i]= DFID_Data[i];
	 //----------14-----------1---------------
	 //CheckSum           80,81-82
	 //-------------------------------------- 
		GGA_TX_DATA[79] = TEN_Data[0];         
		
		CheckNumData    = ProcessCheckResult(84,79);
		ProcessCheckToA(CheckNumData);
		GGA_TX_DATA[80] = GGA_Data[0];	
		GGA_TX_DATA[81] = GGA_Data[1];	          		
						
	 //----------15-----------1---------------
	 //回车换行			   83 84
	 //-------------------------------------- 
		GGA_TX_DATA[82] = 0x0D;	          
		GGA_TX_DATA[83] = 0x0A;	    
	 

	 //---------------------------------------
	 //启动发送
	 //---------------------------------------
		GGA_TX_Max     = 84;   	
		
		printf("%s",GGA_TX_DATA);
		//保存GPS数据
		sd_savegps("0:SaveData/GpsData.txt",GGA_TX_DATA);
		
	 
		//DIS_UP_Num     = GGA_UP_Num + GGA_TX_Max;
}




