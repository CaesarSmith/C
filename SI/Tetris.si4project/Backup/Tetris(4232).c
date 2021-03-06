#include "stdafx.h"
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
	SetColor(FGCOLOR);
	Randomize();
	SetTimer(newTimer);
	Initialize();
	nextBox = MkNewBox(-1);

	ShowBox(current_box_x, current_box_y, current_box_nbr, shapes[current_box_nbr].color);
	ShowBox(current_box_x, current_box_y + 200, current_box_nbr, shapes[nextBox].color);
	ShowIntro(SYS_X, current_box_y + 320);
	getch();

	while (1)
	{
		flag_newbox = false;
		if (bioskey(1))
		{
			key = bioskey(0);
		}
		else
		{
			switch(key)
			{
				case VK_LEFT:
					if (MoveAble(current_box_x, current_box_y, current_box_nbr, MoveLeft))
					{
						EraseBox(current_box_x, current_box_y, current_box_nbr);
						current_box_x -= BSIZE;
						currentAcation = MoveLeft;
					}
					break;
				case VK_RIGHT:
					if (MoveAble(current_box_x, current_box_y, current_box_nbr, MoveRight))
					{
						EraseBox(current_box_x, current_box_y, current_box_nbr);
						current_box_x += BSIZE;
						currentAcation = MoveRight;
					}
					break;
				case VK_DOWN:
					if (MoveAble(current_box_x, current_box_y, current_box_nbr, MoveDown))
					{
						EraseBox(current_box_x, current_box_y, current_box_nbr);
						current_box_y += BSIZE;
						currentAcation = MoveDown;
					}
					else flag_newbox = true;
					break;
				case VK_UP:
					if (MoveAble(current_box_x, current_box_y, shapes[current_box_nbr].next, MoveRoll))
					{
						EraseBox(current_box_x, current_box_y, current_box_nbr);
						current_box_nbr = shape[current_box_nbr].next;
						currentAcation = MoveRoll;
					}
					break;
				case VK_ESC:
					gameOver = 1;
					break;
				default:
					break;
			}

			if (currentAcation)
			{
				ShowBox(current_box_x, current_box_y, current_box_nbr, shapes[current_box_nbr].color);
				currentAcation = 0;
			}
		}	
	}
}
