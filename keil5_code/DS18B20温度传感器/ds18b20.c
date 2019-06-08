#include<reg52.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit ds   = P2^2;
sbit dula = P2^6;
sbit wela = P2^7;
sbit beep = P2^3;
bit flag;
uint temp;
uchar code table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,
0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};  

uchar code table1[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,
0x87,0xff,0xef}; 

void TempDelay(uchar us)
{
	while(us--); //6.51us
}
void delay(int z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=115;y>0;y--);
}
void display(uint temp)
{
	uchar bai,shi,ge;
	bai=temp/100;
	shi=temp%100/10;
	ge=temp%100%10;

	dula=1;
	P0=table[bai];
	dula=0;
	P0=0xff;
	wela=1;
	P0=0xfe;
	wela=0;
	delay(3);

	dula=1;
	P0=table1[shi];
	dula=0;
	P0=0xff;
	wela=1;
	P0=0xfd;
	wela=0;
	delay(3);

	dula=1;
	P0=table[ge];
	dula=0;
	P0=0xff;
	wela=1;
	P0=0xfb;
	wela=0;
	delay(3);
}
void ds_reset()
{
	ds=1;
	_nop_();
	ds=0;
	TempDelay(80);//520us
	ds=1;
	TempDelay(5);
	if(ds==0)
		flag=1;
	else
		flag=0;
	TempDelay(20);
	ds=1;
}
bit ds_read_bit()
{
	bit dat;
	
	// 读数开始标志 拉低电平   1us < T（INT） < 15us
	ds=0;
	_nop_();
	_nop_();
	// 15us内释放引脚  等待ds18b20发送逻辑电平
	ds=1;
	_nop_();
	dat=ds;
	TempDelay(12);
	return dat;
}
uchar ds_read_byte()
{
	uchar i,j,value;
	for(i=0;i<8;i++)
	{
		j=ds_read_bit();
		value=(j<<7)|(value>>1);
	}
	return value;
}

/** 
	写入数据 也需要开始标志  
	无论发送 0或1 都是以 低电平开始发送
	其中隐含 低电平开始标识
*/
void ds_write_byte(uchar dat)
{
	uchar i,onebit;
	for(i=0;i<8;i++)
	{
		onebit=dat&0x01;
		if(onebit)	   //д1
		{
			ds=0;
			TempDelay(1);
			ds=1;
			_nop_();
		}
		else  //д0
		{
			ds=0;
			TempDelay(11);
			ds=1;
			_nop_();
		}
		dat>>=1;
	}
}
void ds_change()
{
	ds_reset();
	ds_write_byte(0xcc);
	ds_write_byte(0x44);
}

uint get_temperature()
{
	uchar a,b;
	float temperature;
	ds_reset();
	ds_write_byte(0xcc);
	ds_write_byte(0xbe);
	a=ds_read_byte();
	b=ds_read_byte();
	temp=b;
	temp<<=8;
	temp|=a;
	temperature=temp*0.0625;
	temp=temperature*10+0.5;
	return temp;
} 

// 写和读操作都需要拉低电平表示 开始发送或者接收数据
void main()
{
	ds_change();
	delay(1000);
	while(1)
	{		

		/**
		
		1.转换温度格式
			1.0 复位
			1.1 输入 0xcc 0x44
			
		2.读取温度信息
			2.0 复位
			2.1 输入 0xcc 0xbe
			2.2 读出温度信息
		*/
		display(get_temperature());
		ds_change();
		if(get_temperature()>350)//35�ȷ���������
			beep=0;
		else
			beep=1;
	}
}