#ifndef __TETRIS_H__
#define __TETRIS_H__

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <graphics.h>

#define Vertical_boxs	50
#define Horizontal_boxs	15
#define MAX_BOX 19

//key defines
#define VK_LEFT 	0x4b00	//**why these value
#define VK_RIGHT 	0x4d00
#define VK_DOWN		0x5000
#define VK_UP		0x4800
#define VK_ESC		0x011b
#define TIMER		0x1c	//interrupt number

#define MAX_BOX		19		//shpes of tetris
#define BSIZE		20		//pixel of edge of square
#define SYS_X		160		//x axis of up-left
#define SYS_Y		25		//y axis of up-left
#define HORIZON_BOX	10		//boxes in horizontal direction
#define VERTICAL_BOX 20		//boxes in vertical direction
#define BEGIN_BOX_X	HORIZON_BOX + 2		// first box in horizon direction

#define FGCOLOR		3		//foreground color
#define BGCOLOR		0		//background color

#define LEFTWIN_X	SYS_X + HORIZON_BOX * BSIZE + 46

#define false	0
#define true	1

#define MoveLeft	1
#define MoveRight	2
#define MoveDown	3
#define MoveRoll	4

struct BOARD	//game board of each point
{
	int var;	//state of current point, 1 for used and 0 for free
	int color;	//color of current point
}Table_board[Vertical_boxs][Horizontal_boxs];

struct SHAPE	
{
	//shape of tetris, echo 4 bit represent one row of shap
	/*
		for example, 0x88, 0xC0 represent tetris of shap L
		1000
		1000
		1100
		0000
	*/

	char box[2];
	int color;
	int next;
};

struct SHAPE shapes[MAX_BOX]
{
	/*
		口     口口口  口口       口
		口     口        口   口口口
		口口             口
	*/
	{0x88, 0xC0, CYAN, 1},
	{0xE8, 0x00, CYAN, 2},
	{0xC4, 0x40, CYAN, 3},
	{0x2E, 0x00, CYAN, 0},

	/*
	       口           口口   口口口
	       口  口       口         口
	     口口  口口口   口
	*/
	{0x44, 0xC0, MAGENTA, 5},
	{0X8E, 0x00, MAGENTA, 6},
	{0xC8, 0x80, MAGENTA, 7},
	{0xE2, 0x00, MAGENTA, 4},

	/*
		口         
		口口      口口
		  口    口口
	*/
	{0x8C, 0x40, YELLOW, 9},
	{0x6C, 0x00, YELLOW, 8},

	/*
		  口       口口
		口口         口口
		口
	*/
	{0x4C, 0x80, BROWN, 11},
	{0xC6, 0x00, BROWN, 10},

	/*
		  口     口              口
		口口口   口口   口口口  口口
                 口     口       口
	*/
	{0x4E, 0x00, WHITE, 13},
	{0x8C, 0x80, WHITE, 14},
	{0xE4, 0x00, WHITE, 15},
	{0x4C, 0x40, WHITE, 12},

	/*
		口
		口
		口   口口口口
		口
	*/
	{0x88, 0x88, RED, 17},
	{0xF0, 0x00, RED, 16},

	/*
		口口
		口口
	*/
	{0xCC, 0x00, BLUE, 18}

}

void	interrupt newTimer(void);
void	SetTimer(void interrupt(*IntProc)(void));
void	KillTimer(void);
void	Initialize(int x, int y, int m, int n);
int		DelFullRow(int y);
void	SetFullRow(int y);
int		MkNewBox(int boxNbr);
int		EraseBox(int x, int y, int boxNbr);
int		ShowBox(int x, int y, int boxNbr, int color);
int		MoveAble(intx, inty, int boxNbr, int color);
void	Interrupt(*oldtimer)(void);

#endif