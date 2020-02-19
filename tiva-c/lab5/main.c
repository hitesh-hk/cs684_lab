/*Program to display simple text on Graphics LCD*/

#include<intrins.h>
#include "at89c5131.h"
#include "stdio.h"
#define dport P2
#define LED P0

sbit rs=P0^0;       // Register select. used to select between command and data
sbit rw=P0^1;      // Read write select. used to select if data is read or written to LCD controller
sbit en=P0^2;       // enable pin. used to create a data transfer initiation on high to low transition.
sbit cs1=P3^0;     // chip select 1. selects controller 1
sbit cs2=P3^1;		// chip select 2, selects controller 2


unsigned char c,z=0;


void delay(unsigned int value)
{
	unsigned int temp=0;
	  while(temp<value)
		{
			temp++;
		}
}

void ctrloff()
{
	rs=0;
	rw=0;
	en=0;
	cs1=0;
	cs2=0;
}

//Display on function			 
void displayon() // step 1
{ 
	ctrloff();
	dport=0x3f;  // on display
	cs1=1;cs2=1; // enable bot halves
	rw=0;rs=0;   //instruction mode , write operation
	en=1;        // LCD enable
	delay(1); 
	en=0;
}


void setpage(unsigned char x) // step 2 
{
	ctrloff();
	dport= 0xb8|x;	   //0xb8 represents Page 0 // 1011xxx
	cs1=1;
	cs2=1;
	rs=0;               //instruction mode , write operation
	rw=0;
	en=1;
	delay(1); 
	en=0;
}

void setcolumn(unsigned char y) //step 3 - setting of column address
{
	if(y<64)
	{
		ctrloff();
		c=y;
		dport=0x40|(y&63);	  //0x40 represents Column 0 //01xxxxxx
		cs1=1;cs2=0;          //page 1 on
		rs=0;
		rw=0;
		en=1;
		delay(1); 
		en=0;
		
		
	}
	else
	{ 
		c=y;
		dport=0x40|((y-64)&63);	  //0x40 represents Column 0
		cs2=1;cs1=0;    // page 2 on
		rs=0;
		rw=0;
		en=1;
		delay(1); 
		en=0;
	}
}


void set_loc(unsigned char x,unsigned char y) 
{
	setpage(x);
	setcolumn(y);
}


// This function is always called after setcoloumn function is called. c stores the value of coloumn to which data has to be written
void lcddata(unsigned char *value,unsigned int limit) // writing the data in perticular column
{
	unsigned int i;
	for(i=0;i<limit;i++)
	{
		if(c<64)
		{
			dport=value[i];
			cs1=1;cs2=0;
			rs=1;
			rw=0;
			en=1;
			delay(1); 
			en=0;
			c++;
		}

		else
		{
			setcolumn(c); 
			dport=value[i];
			cs2=1;cs1=0;
			rs=1;
			rw=0;
			en=1;
			delay(1); 
			en=0;
			c++;
		}
	if(c>127)
	return;
	}
}

// It must be note down that the LCD doesnt have a single command to clear the entire screen.
// Hence the screen is clearedd by writing '0' data to the screen
// This function cleares the entire screen
void clrlcd()
{
    unsigned char i,j;
    for (i=0;i < 8;i++)
    {
	  setpage(i);
	  setcolumn(0);
	  for (j= 0 ;j < 128; j++)
            lcddata(&z,1);
    }
}

// This function is used to read from the LCD. This function is useful if the data 
// has to be written to a pixel group in which some of pixels are already ON which shouldnt be modified.
// Hence the pixels are read XOR'ed and written back.
// A pixel group refers to a set of 8 pixels that are written simultaneously.
char read_pixel(void)
{
	  char temp=0;
	  P2=0xFF;             // Make P2 input port to read data
		rw=1;
		rs=1;
	
		en=1;
		_nop_();
	  _nop_();
	  _nop_();
	  _nop_();
	  _nop_();
	  _nop_();
	  en=0;
	  _nop_();
	  _nop_();
	  _nop_();        // By trail and error, it was found that the data in only read when en pin is made high to low 2 times
	  _nop_();
	  _nop_();
	  _nop_();
	  en=1;
	  _nop_();
	  _nop_();
	  _nop_();
	  _nop_();
	  _nop_();
	  _nop_();	  
	  temp=P2;
		en=0;
	  P2&=~0xFF;             // make P2 output port
	  return(temp);
}

// This function writes to a single pixel without modifing the other pixels in the same group
void write_pixel(unsigned char x,unsigned char y,char pixel_data)
{
	  char temp=0;
	  x=x-1;
				set_loc(x/8,y);
	      cs1=(y<64);
	      cs2=~cs1;
				temp=read_pixel();
				temp&=~(1<<(x%8));
				pixel_data=(pixel_data&0x01)<<(x%8);
	  temp = temp+pixel_data;
    set_loc(x/8,y);
		lcddata(&temp,1);  
}

void draw_boundary(void)
{
	  char byte=0xFF,index=0;
	  for(index=0;index<8;index++)
	  {
      set_loc(index,99);
		  lcddata(&byte,1);
		}			
}

void draw_grid_lines(void)
{
	  char byte=0x11,index=0;
	  for(index=1;index<10;index++)
	  {
      set_loc(0,10*index);
		  lcddata(&byte,1);
      set_loc(1,10*index);
		  lcddata(&byte,1);
      set_loc(2,10*index);
		  lcddata(&byte,1);
      set_loc(3,10*index);
		  lcddata(&byte,1);
      set_loc(4,10*index);
		  lcddata(&byte,1);
      set_loc(5,10*index);
		  lcddata(&byte,1);
      set_loc(6,10*index);
		  lcddata(&byte,1);
      set_loc(7,10*index);
		  lcddata(&byte,1);
		}			
}


void main()
{ 
	  unsigned char temp=0x88;
	  clrlcd();
		displayon();
	  LED&=~0x80;
	  P1|=0x0F;
	  while(1)
	  {
	      draw_boundary();  
	      draw_grid_lines();  
			  while((P1&0x01)==0x01);
			  delay(10000);     
	      clrlcd();           
	  }
}
 

