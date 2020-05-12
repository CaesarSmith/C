#include "stdafx.h
#include "tetris.h"

int current_box_nbr;	
int current_box_x = SYS_X + BEGIN_BOX_X * BSIZE, current_box_y = SYS_Y;
int flag_newbox = false;
int speed = 0;
int score = 0;
int speed_step = 100;

int _tmain()
{
	int gameOver = 0;
	int key, nextBox;
	int currentAcation = 0;
	int gd = VGA, gm = VGAHI, errorcode;

	InitGrouph(&gd, $gm, "");
	errorcode = GraphResult();

	if (errorcode != grOk)
	{
		printf("\nNotice:Graphics error:%s\n", GraphErrorMsg(errorcode));
		printf("Press any key to quit!");
		getch();
		exit(1);
	}

	SetBkColor(BGCOLOR);
	SetColor(FG);
