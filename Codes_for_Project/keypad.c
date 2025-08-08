#include <LPC21xx.h>

/* Keypad pin connections */
//rows are connected to P1.16 to P1.19
#define row0 16
#define row1 17
#define row2 18
#define row3 19

//cols are connected to P1.20 to P1.23
#define col0 20
#define col1 21
#define col2 22
#define col3 23


//keypad lookup table
unsigned char keypad_lut[4][4] = {1,2,3,4,
								  5,6,7,8,
								  9,10,11,12,
								  13,14,15,16};

//column scan function defination																	
unsigned int colscan(void)
{
	unsigned int c0,c1,c2,c3,retval;
	
	c0 = (IOPIN1>>col0) & 1;	//reading column 0 status
	c1 = (IOPIN1>>col1) & 1;  //reading column 1 status
	c2 = (IOPIN1>>col2) & 1;  //reading column 2 status
	c3 = (IOPIN1>>col3) & 1;  //reading column 3 status
	
	retval = (c0&c1&c2&c3);
	
	return retval;
}
//rows initialization function defination
void initrows(void)
{
	//configuring the rows port pins as output port pins
	IODIR1 = (1<<row0 | 1<<row1 | 1<<row2 | 1<<row3);
	//initially making all rows to zero 
	IOCLR1 = (1<<row0 | 1<<row1 | 1<<row2 | 1<<row3);
}
//columns initialization function defination
void initcols(void)
{
	//initially making all columns to one 
	IOSET1 = (1<<col0 | 1<<col1 | 1<<col2 | 1<<col3);
}

/* keyscan function is used to find out the which switch 
it is pressed from user and return the equivalent value from 
keypad lookup table*/
//keyscan function defination
unsigned char keyscan(void)
{
	//variables declaration
	unsigned char row,col;
	
	initrows();
	initcols();
	
	//waiting for switch press
	while(colscan() == 1);
	
  //row identification process
	//row0 identification
	IOCLR1 = 1<<row0;
	IOSET1 = (1<<row1 | 1<<row2 | 1<<row3);
	if(colscan() == 0)
	{
		row = 0;
		goto colcheck;
	}
	
	//row1 identification
	IOCLR1 = 1<<row1;
	IOSET1 = (1<<row0 | 1<<row2 | 1<<row3);
	if(colscan() == 0)
	{
		row = 1;
		goto colcheck;
	}
	
	//row2 identification
	IOCLR1 = 1<<row2;
	IOSET1 = (1<<row0 | 1<<row1 | 1<<row3);
	if(colscan() == 0)
	{
		row = 2;
		goto colcheck;
	}
	
	//row3 identification
	IOCLR1 = 1<<row3;
	IOSET1 = (1<<row0 | 1<<row1 | 1<<row2);
	if(colscan() == 0)
	{
		row = 3;
		goto colcheck;
	}
	//columns identification process
	colcheck:
	if(((IOPIN1>>col0)&1) == 0)	//col0 identification
		col = 0;
	else if(((IOPIN1>>col1)&1) == 0)	//col1 identification
		col = 1;
	else if(((IOPIN1>>col2)&1) == 0)	//col2 identification
		col = 2;
	else if(((IOPIN1>>col3)&1) == 0)	//col3 identification
		col = 3;
	
	//waiting for switch release
	while(colscan() == 0);
	
	//return the switch equivalent value from defined keypad lookup table
	return keypad_lut[row][col];	
}
