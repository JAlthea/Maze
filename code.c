#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>

#define MAX_STACK_SIZE 100000

/*~ Maze creating & searching ~*/

typedef int* intptr;
typedef intptr* intptr2;
typedef char* charptr;
typedef charptr* charptr2;

typedef struct indextype indextype;
typedef indextype* indexptr;
typedef struct indextype {
	int r;
	int c;
} indextype;

typedef struct stacktype stacktype;
typedef stacktype* stackptr;
typedef struct stacktype {
	indextype data[MAX_STACK_SIZE];
	int top;
} stacktype;

void init(stackptr s)
{
	s->top = -1;
}

int is_empty(stackptr s)
{
	return (s->top == -1);
}

int is_full(stackptr s)
{
	return (s->top == (MAX_STACK_SIZE - 1));
}

void push(stackptr s, indextype item)
{
	if (is_full(s))
	{
		fprintf(stderr, "Stack full error!\n");
		return;
	}

	s->data[++(s->top)] = item;
}

indextype pop(stackptr s)
{
	if (is_empty(s))
	{
		fprintf(stderr, "Stack empty error!\n");
		exit(1);
	}

	return s->data[(s->top)--];
}

indextype peek(stackptr s)
{
	if (is_empty(s))
	{
		fprintf(stderr, "Stack empty error!\n");
		exit(1);
	}

	return s->data[s->top];
}

void push_loc(stackptr s, intptr2 maze, int r, int c)
{
	if (r < 2 || c < 2)
		return;

	if ((maze[r][c] != 1) && (maze[r][c] != '.'))
	{
		indextype tmp;
		tmp.r = r;
		tmp.c = c;
		push(s, tmp);
	}
}

void push_loc2(stackptr s, intptr2 maze, int r, int c)
{
	if (r < 2 || c < 2)
		return;

	if ((maze[r][c] != 1) && (maze[r][c] != '*'))
	{
		indextype tmp;
		tmp.r = r;
		tmp.c = c;
		push(s, tmp);
	}
}

intptr2 create_maze(indexptr start, indexptr end)
{
	intptr2 maze;
	int width, height;
	int x, y, r;
	int start_x, start_y, end_x, end_y;

	//Input the size of the maze
	printf("Please decide the size of the maze. Input only odd number : (example : 51 51)\n");
	int numbering = scanf("%d %d", &width, &height);
	if ((numbering < 2) || (width < 4) || (height < 4) || (width % 2 == 0) || (height % 2 == 0))
	{
		printf("Please enter the correct number.\n");
		exit(1);
	}

	//Create memory space to create a maze
	maze = (intptr2)malloc(sizeof(intptr) * (width + 5));
	for (int i = 0; i < width + 5; i++)
	{
		maze[i] = (intptr)malloc(sizeof(int) * (height + 5));
		for (int j = 0; j < height + 5; j++)
			maze[i][j] = 0;
	}

	/* #1. Randomly create inside a maze */
	x = 2, y = 2;
	maze[x][y] = 1;

	srand(time(NULL));
	for (int i = 0; i < width * height * 10; i++)
	{
		r = rand() % 4 + 1;

		if (r == 1)
		{
			if (y == 2)
				continue;

			if (maze[x][y - 2] == 0)
			{
				maze[x][--y] = 1;
				maze[x][--y] = 1;
			}
			else
				y -= 2;
		}

		if (r == 2)
		{
			if (x == width + 1)
				continue;

			if (maze[x + 2][y] == 0)
			{
				maze[++x][y] = 1;
				maze[++x][y] = 1;
			}
			else
				x += 2;
		}

		if (r == 3)
		{
			if (y == height + 1)
				continue;

			if (maze[x][y + 2] == 0)
			{
				maze[x][++y] = 1;
				maze[x][++y] = 1;
			}
			else
				y += 2;
		}

		if (r == 4)
		{
			if (x == 2)
				continue;

			if (maze[x - 2][y] == 0)
			{
				maze[--x][y] = 1;
				maze[--x][y] = 1;
			}
			else
				x -= 2;
		}
	}

	/* #2. Remove the interior to create a maze */

	//Temporarily block between inner and outer walls
	for (int i = 1; i < width + 3; i++)
	{
		for (int j = 1; j < height + 3; j++)
		{
			if ((i == 1) || (i == width + 2) || (j == 1) || (j == height + 2))
				maze[i][j] = 1;
		}
	}

	//Remove the inside of a maze
	for (int i = 2; i < width + 2; i++)
	{
		for (int j = 2; j < height + 2; j++)
		{
			if (((i == 2) || (i == width + 1) || (j == 2) || (j == height + 1)) && (maze[i][j] == 0))
			{
				stacktype s;
				init(&s);

				int r, c;

				indextype here = { i, j };
				while ((here.r != width + 3) && (here.c != height + 3))
				{
					r = here.r;
					c = here.c;
					maze[r][c] = '.';

					push_loc(&s, maze, r - 1, c);
					push_loc(&s, maze, r + 1, c);
					push_loc(&s, maze, r, c - 1);
					push_loc(&s, maze, r, c + 1);

					if (is_empty(&s))
					{
						maze[r - 1][c] = 0;
						maze[r + 1][c] = 0;
						maze[r][c - 1] = 0;
						maze[r][c + 1] = 0;

						break;
					}
					else
						here = pop(&s);
				}
			}
		}
	}

	//Piercing between inner and outer walls
	for (int i = 1; i < width + 3; i++)
	{
		for (int j = 1; j < height + 3; j++)
		{
			if ((i == 1) || (i == width + 2) || (j == 1) || (j == height + 2))
				maze[i][j] = 0;
		}
	}

	/* #3. Create the outer wall of a maze */
	for (int i = 2; i < width + 2; i++)
	{
		for (int j = 2; j < height + 2; j++)
		{
			if ((i == 2) || (i == width + 1) || (j == 2) || (j == height + 1))
				maze[i][j] = 1;
		}
	}

	/* #4. Create the entrance and exit of a maze */
	while (1)
	{
		start_x = rand() % (width - 3) + 3;
		start_y = 2;

		end_x = rand() % (width - 3) + 3;
		end_y = height + 1;

		if ((start_x == end_x) && (start_y == end_y))
			continue;
		else if ((maze[start_x][start_y + 1] == 1) || (maze[end_x][end_y - 1] == 1))
			continue;
		else
		{
			start->r = start_x;
			start->c = start_y;
			end->r = end_x;
			end->c = end_y;
			maze[start_x][start_y] = 0;
			maze[end_x][end_y] = 'x';
			break;
		}
	}

	return maze;
}

void print_maze(intptr2 maze)
{
	int w = _msize(maze) / sizeof(int);
	int h = _msize(maze[0]) / sizeof(int);

	for (int i = 0; i < w - 1; i++)
	{
		for (int j = 0; j < h - 1; j++)
		{
			if (maze[i][j] == 1)
				printf("%s", "■");
			else
				printf("%s", "  ");
		}
		printf("\n");
	}
}

void remove_maze(intptr2 maze)
{
	int len = _msize(maze) / sizeof(int);

	for (int i = 0; i < len; i++)
		free(maze[i]);
	free(maze);
}

void gotoxy(int x, int y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void search_exit(intptr2 maze, indexptr start, indexptr end)
{
	int h = _msize(maze[0]) / sizeof(int);

	stacktype s;
	init(&s);

	int r, c;
	indextype here = { start->r, start->c };
	indextype entry = { start->r, start->c };

	_getch();
	while (maze[here.r][here.c] != 'x')
	{
		r = here.r;
		c = here.c;
		maze[r][c] = '*';

		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		gotoxy(2 * c, r + 2);
		printf("%s", "★");
		Sleep(100);
		//_getch();	//User checking one by one

		push_loc2(&s, maze, r - 1, c);		//North
		push_loc2(&s, maze, r + 1, c);		//South
		push_loc2(&s, maze, r, c - 1);		//West
		push_loc2(&s, maze, r, c + 1);		//East

		push_loc2(&s, maze, r - 1, c - 1);	//Northwest
		push_loc2(&s, maze, r + 1, c - 1);	//Southwest
		push_loc2(&s, maze, r - 1, c + 1);	//Northeast
		push_loc2(&s, maze, r + 1, c + 1);	//Southeast

		if (is_empty(&s))
		{
			gotoxy(0, h);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
			printf("Escape status : Failure! \n\n");
			return;
		}
		else
			here = pop(&s);
	}
	gotoxy(0, h);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	printf("Escape status : Success! \n\n");
}

int main()
{
	system("title Althea Maze");

	indextype start, end;
	intptr2 maze = create_maze(&start, &end);
	print_maze(maze);
	search_exit(maze, &start, &end);
	remove_maze(maze);
	
	return 0;
}
