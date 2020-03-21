#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include<conio.h>
#include<string.h>
#include<time.h>
#include<limits.h>
#include<float.h>
//N is recommended to be odd (5 is the best for to see if its learning or not)
#define N 5
#define alpha 0.1
#define gamma 1

typedef struct cell{
	double *directions;
}cell;

int size;
char **matrix;
int posi=1,posj=1;
cell **rValues;
cell **qValues;

double getRand()//returns random [0-1] double
{
	return (double)rand()/(double)RAND_MAX;
}

void printMatrix()
{
	int i,j;
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			printf("%2c",matrix[i][j]);
		}
		printf("\n");
	}
}

void initMatrix()//adds walls to matrix and initializes q-reward values
{
	//initialize mazish matrix
	size=N*2+1;
	matrix=(char**)calloc(size,sizeof(char*));
	int i,j;
	for(i=0;i<size;i++)
		matrix[i]=(char*)calloc(size,sizeof(char));
	for(i=0;i<size;i++)
	{
		for(j=0;j<size;j++)
		{
			if(i%2==0)
			{
				matrix[i][j]='-';	
			}
			else if(j%2==0)
			{
				matrix[i][j]='|';
			}
			else
			{
				matrix[i][j]=' ';
			}
		}	
	}
	matrix[posi][posj]='*';
	matrix[size-2][size-2]='T';
	//initialize q and reward matrix
	rValues=(cell**)calloc(N,sizeof(cell*));
	qValues=(cell**)calloc(N,sizeof(cell*));
	int k;
	for(i=0;i<N;i++)
	{
			rValues[i]=(cell*)calloc(N,sizeof(cell));
			qValues[i]=(cell*)calloc(N,sizeof(cell));
			for(j=0;j<N;j++)
			{
				rValues[i][j].directions=(double*)calloc(4,sizeof(double));
				for(k=0;k<4;k++)
					rValues[i][j].directions[k]=-0.01;
				qValues[i][j].directions=(double*)calloc(4,sizeof(double));
			}
	}
}

void breakWalls()//randomly break some walls in each cell with probability X
{
	int tempi=posi,tempj=posj;
	int r;
	//left top to right down
	/*do
	{
		r=(getRand()<0.5)?1:0;
		if(r)
		{
			if(tempi!=size-2)
			{
				matrix[tempi+1][tempj]=' ';
				tempi+=2;
			}
			else if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
				tempj+=2;
			}
			else
				break;
		}
		else
		{
			if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
				tempj+=2;
			}
			else if(tempi!=size-2)
			{
				matrix[tempi+1][tempj]=' ';
				tempi+=2;
			}
			else
				break;
		}
	}while(1);*/
	//break random walls in each cell
	for(tempi=1;tempi<size-1;tempi+=2)
	{
		for(tempj=1;tempj<size-1;tempj+=2)
		{
			if(getRand()<0.5)
			{
				r=(getRand()<0.5)?1:0;
				if(r)
				{
					if(tempi!=size-2)
						matrix[tempi+1][tempj]=' ';
					else if(tempj!=size-2)
						matrix[tempi][tempj+1]=' ';
					else
						break;
				}
				else
				{
					if(tempj!=size-2)
						matrix[tempi][tempj+1]=' ';
					else if(tempi!=size-2)
						matrix[tempi+1][tempj]=' ';
					else
						break;
				}
			}
		}
	}
	
}

void breakWallsLine(int n)//break nth row and nth column by line
{
	int r;
	//left to right
	int tempi=n,tempj=posj;
	do
	{
		r=(getRand()<0.5)?1:0;
		if(r)
		{
			matrix[tempi-1][tempj]=' ';
			if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
			}
			else
				matrix[tempi+1][tempj]=' ';
		}
		else
		{
			matrix[tempi+1][tempj]=' ';
			if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
			}
			else
				matrix[tempi-1][tempj]=' ';
		}
		tempj+=2;
	}while(tempj!=size);
	//top to down
	tempi=posi;tempj=n;
	do
	{
		r=(getRand()<0.5)?1:0;
		if(r)
		{
			matrix[tempi][tempj-1]=' ';
			if(tempi!=size-2)
			{
				matrix[tempi+1][tempj]=' ';
			}
			else
				matrix[tempi][tempj+1]=' ';
		}
		else
		{
			matrix[tempi][tempj+1]=' ';
			if(tempi!=size-2)
			{
				matrix[tempi+1][tempj]=' ';
			}
			else
				matrix[tempi][tempj-1]=' ';
		}
		tempi+=2;
	}while(tempi!=size);
}

void breakWalls2()//create route from left top to right down(main route to finish) and from left bottom to top right
{
	int tempi=posi,tempj=posj;
	int r;
	//left top to right down
	do
	{
		r=(getRand()<0.5)?1:0;
		if(r)
		{
			if(tempi!=size-2)
			{
				matrix[tempi+1][tempj]=' ';
				tempi+=2;
			}
			else if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
				tempj+=2;
			}
			else
				break;
		}
		else
		{
			if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
				tempj+=2;
			}
			else if(tempi!=size-2)
			{
				matrix[tempi+1][tempj]=' ';
				tempi+=2;
			}
			else
				break;
		}
	}while(1);
	//left bottom to right top
	tempi=size-2;tempj=posj;
	do
	{
		r=(getRand()<0.5)?1:0;
		if(r)
		{
			if(tempi!=1)
			{
				matrix[tempi-1][tempj]=' ';
				tempi-=2;
			}
			else if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
				tempj+=2;
			}
			else
				break;
		}
		else
		{
			if(tempj!=size-2)
			{
				matrix[tempi][tempj+1]=' ';
				tempj+=2;
			}
			else if(tempi!=1)
			{
				matrix[tempi-1][tempj]=' ';
				tempi-=2;
			}
			else
				break;
		}
	}while(1);
}

void move(int direction)//move to the cell at given direction
{
	char temp;
	switch(direction)
	{
		case 0://North
			if(matrix[posi-1][posj]==' ')
			{
				matrix[posi-2][posj]=matrix[posi][posj];
				matrix[posi][posj]=' ';
				posi-=2;
			}
			break;
		case 1://East
			if(matrix[posi][posj+1]==' ')
			{
				matrix[posi][posj+2]=matrix[posi][posj];
				matrix[posi][posj]=' ';
				posj+=2;
			}
			break;
		case 2://South
			if(matrix[posi+1][posj]==' ')
			{
				matrix[posi+2][posj]=matrix[posi][posj];
				matrix[posi][posj]=' ';
				posi+=2;
			}
			break;
		case 3://West
			if(matrix[posi][posj-1]==' ')
			{
				matrix[posi][posj-2]=matrix[posi][posj];
				matrix[posi][posj]=' ';
				posj-=2;
			}
			break;
	}
}

int bestDirection(int *direction)//find bestDirection by using q values
{
	int i,count=0,tempDirs[4];
	double max=-10000;
	for(i=0;i<4;i++)
	{
		if(qValues[posi/2][posj/2].directions[i]>max)
		{
			if(i==0 && matrix[posi-1][posj]==' ')//North
				{max=qValues[posi/2][posj/2].directions[i];tempDirs[0]=i;count=1;}
			else if(i==1 && matrix[posi][posj+1]==' ')//East
				{max=qValues[posi/2][posj/2].directions[i];tempDirs[0]=i;count=1;}
			else if(i==2 && matrix[posi+1][posj]==' ')//South
				{max=qValues[posi/2][posj/2].directions[i];tempDirs[0]=i;count=1;}
			else if(i==3 && matrix[posi][posj-1]==' ')//West
				{max=qValues[posi/2][posj/2].directions[i];tempDirs[0]=i;count=1;}
		}
		else if(qValues[posi/2][posj/2].directions[i]==max)
		{
			tempDirs[count]=i;
			count++;
		}
	}
	//if there is more than 1 direction at same value, choose random
	*direction=tempDirs[rand()%count];
	return max;
}

void restart()
{
	posi=posj=1;
	matrix[posi][posj]='*';
	matrix[size-2][size-2]='T';
}

void printSleepClear(int sleepMS)
{
	printMatrix();
	usleep(1000*sleepMS);
	system("cls");
}

void qLearning()//do algorithm
{
	int i,direction,tempI,tempJ,tempDir,tempCount;
	double max;
	//rewards to the finish
	if(matrix[size-2][size-3]==' ')
		rValues[N-1][N-2].directions[1]=100000;
	if(matrix[size-3][size-2]==' ')
		rValues[N-2][N-1].directions[2]=100000;
	int count=0,countTotal=0,limit=4*N;
	printSleepClear(500);
	do
	{
		max=bestDirection(&direction);
		tempI=posi;tempJ=posj;tempDir=direction;
		move(direction);
		max=bestDirection(&direction);
		qValues[tempI/2][tempJ/2].directions[tempDir]+=(alpha*(rValues[tempI/2][tempJ/2].directions[tempDir]+gamma*(max-qValues[tempI/2][tempJ/2].directions[tempDir])));
		printSleepClear(100);
		count++;
		if(posi==size-2 && posj==size-2)
		{
			printf("#%d Move:%d\n",countTotal+1,count);
			printSleepClear(999);
			tempCount=count;
			count=0;
			countTotal++;
			if(countTotal!=limit)
				restart();
		}
	}while(countTotal<limit);//while contidion = max restart time
	printf("#%d Move:%d\n",countTotal,tempCount);
	printMatrix();
}

int main()
{
	int i;
	srand(time(NULL));
	initMatrix();
	//breakWallsLine(n-(size-n)/2);
	//breakWallsLine(n+(size-n)/2);
	breakWalls2();
	breakWalls();
	breakWallsLine(N);
	qLearning();
	exit(0);
}
