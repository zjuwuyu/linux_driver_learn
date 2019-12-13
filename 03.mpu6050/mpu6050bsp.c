#include"mpu6050bsp.h"

u8 mpu_Date[14];

#define mpu6050_SIC_H()     	*(volatile unsigned int *)REG_GPIOE_DATAOUT|=1<<11
#define mpu6050_SIC_L()     	*(volatile unsigned int *)REG_GPIOE_DATAOUT&=~(1<<11)

#define mpu6050_SID_H()     	*(volatile unsigned int *)REG_GPIOE_DATAOUT|= 1<<10
#define mpu6050_SID_L()     	*(volatile unsigned int *)REG_GPIOE_DATAOUT&= ~(1<<10)

#define mpu6050_DATA_IN      	*(volatile unsigned int *)REG_GPIOE_DIR    &=~(1<<10)
#define mpu6050_DATA_OUT     	*(volatile unsigned int *)REG_GPIOE_DIR    |=  1<<10
#define mpu6050_SID_STATE	*(volatile unsigned int *)REG_GPIOE_DATAIN &  (1<<10)

void delayus(u8 a)
{
udelay(2);
}
void mpu6050_Init(void)
{

	*(volatile unsigned int *)REG_GPIOE_DIR|=1<<10;
	*(volatile unsigned int *)REG_GPIOE_DIR|=1<<11;



	mpu6050_SID_H();	
	mpu6050_SIC_H();

}


void mpu6050_Start(void)
{
    mpu6050_SID_H();
    delayus(2);
    mpu6050_SIC_H();
    delayus(2);
    mpu6050_SID_L();
    delayus(2);
    mpu6050_SIC_L();
    delayus(2);
}


void mpu6050_Stop(void)
{
    mpu6050_SID_L();
    delayus(2);
    mpu6050_SIC_H();	
    delayus(2);  
    mpu6050_SID_H();	
    delayus(2);  
}

void noAck(void)
{	
	mpu6050_SID_H();	
	delayus(2);	
	mpu6050_SIC_H();	
	delayus(2);	
	mpu6050_SIC_L();	
	delayus(2);	
	mpu6050_SID_L();	
	delayus(2);
}
void Ack(void)
{	
	mpu6050_SID_L();	
	delayus(2);	
	mpu6050_SIC_H();	
	delayus(2);	
	mpu6050_SIC_L();	
	delayus(2);	
	mpu6050_SID_L();	
	delayus(2);
}
u8 mpu6050_Write(u8 m_data)
{
	u8 j,tem;

	for(j=0;j<8;j++)
	{
		if((m_data<<j)&0x80)mpu6050_SID_H();
		else mpu6050_SID_L();
		delayus(2);
		mpu6050_SIC_H();	
		delayus(2);
		mpu6050_SIC_L();	
		delayus(2);
	}
	delayus(2);
	mpu6050_DATA_IN;
	delayus(2);
	mpu6050_SIC_H();	
	delayus(2);
	if(mpu6050_SID_STATE)tem=0;
	else tem=1;
	mpu6050_SIC_L();	
	delayus(2);	
    mpu6050_DATA_OUT;
	return tem;  
}

u8 mpu6050_Read(void)
{
	u8 read,j;
	read=0x00;
	
	mpu6050_DATA_IN;
	delayus(2);
	for(j=8;j>0;j--)
	{		     
		delayus(2);
		mpu6050_SIC_H();
		delayus(2);
		read=read<<1;
		if(mpu6050_SID_STATE)read=read+1;  
		mpu6050_SIC_L();
		delayus(2);
	}	
    mpu6050_DATA_OUT;
	return read;
}

u8 WriteReg(u8 a,u8 regID, u8 regDat)
{
	mpu6050_Start();
	if(mpu6050_Write(a)==0)
	{
		mpu6050_Stop();
		return 1;
	}
	delayus(2);
  	if(mpu6050_Write(regID)==0)
	{
		mpu6050_Stop();
		return 2;
	}

	
	delayus(2);
  	if(mpu6050_Write(regDat)==0)
	{
		mpu6050_Stop();
		return 3;
	}
  	mpu6050_Stop();
  	return 0;
}


u8 ReadReg(u8 a,u8 regID)
{
	u8 regDat;
	
	mpu6050_Start();
	if(mpu6050_Write(a&0xfe)==0)
	{
		mpu6050_Stop();
		return 1;
	}
	delayus(2);
  	if(mpu6050_Write(regID)==0)
	{
		mpu6050_Stop();
		return 2;
	}
	delayus(2);

	mpu6050_Stop();
	delayus(2);	
	
	mpu6050_Start();
	
	if(mpu6050_Write(a+1)==0)
	{
		mpu6050_Stop();
		return 3;
	}

  	regDat=mpu6050_Read();
	noAck();
  	mpu6050_Stop();
  	return regDat;
}
u8 InitMPU6050()
{
	 mpu6050_Init();
	 if(WriteReg(SlaveAddress,0x6b, 1)) return 1;
	 WriteReg(SlaveAddress,0x19, 4);
	 WriteReg(SlaveAddress,0x1a, 2);
	 WriteReg(SlaveAddress,0x1b, 3<<3);
	 WriteReg(SlaveAddress,0x1c, 2<<3);
	 WriteReg(SlaveAddress,0x37, 0x32);
	 WriteReg(SlaveAddress,0x38, 1);
	 WriteReg(SlaveAddress,0x6a, 0);
	return 0;
}
void mpu_get_date()
{
	mpu_Date[1]=ReadReg(SlaveAddress,0x3B);
	mpu_Date[0]=ReadReg(SlaveAddress,0x3C);
	mpu_Date[3]=ReadReg(SlaveAddress,0x3D);
	mpu_Date[2]=ReadReg(SlaveAddress,0x3E);
	mpu_Date[5]=ReadReg(SlaveAddress,0x3F);
	mpu_Date[4]=ReadReg(SlaveAddress,0x40);
	mpu_Date[7]=ReadReg(SlaveAddress,0x41);
	mpu_Date[6]=ReadReg(SlaveAddress,0x42);
	mpu_Date[9]=ReadReg(SlaveAddress,0x43);
	mpu_Date[8]=ReadReg(SlaveAddress,0x44);
	mpu_Date[11]=ReadReg(SlaveAddress,0x45);
	mpu_Date[10]=ReadReg(SlaveAddress,0x46);
	mpu_Date[13]=ReadReg(SlaveAddress,0x47);
	mpu_Date[12]=ReadReg(SlaveAddress,0x48);

}

