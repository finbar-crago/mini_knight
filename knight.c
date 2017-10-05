#include <stdio.h>
#include <stdlib.h>

#define SIZE 4

#define GET_Y(b,ptr)   (((unsigned long)ptr-(unsigned long)&b)/sizeof(b[0][0])%(sizeof(b[0])/sizeof(b[0][0])))
#define GET_X(b,ptr,y) (((((unsigned long)ptr-(unsigned long)&b)/sizeof(b[0][0])-y))/(sizeof(b[0])/sizeof(b[0][0])))

struct {int x,y;} M[8] = {{ 1, 2}, { 2, 1}, { 2,-1}, { 1,-2}, {-1,-2}, {-2,-1}, {-2, 1}, {-1, 2}};
#define SAFE(n, x, y) ( (x+M[n].x>=0) && (x+M[n].x<SIZE) && (y+M[n].y>=0) && (y+M[n].y<SIZE) )

struct mov {
	int n;
	struct mov *from;
};

struct node {
	struct mov *mov;
	struct node *nxt;
};

struct mov board[SIZE][SIZE];
struct mov *stack[SIZE*SIZE*SIZE*SIZE*SIZE*SIZE*SIZE*SIZE];

int main(int argc, char **argv){
	struct mov *move;
	int sptr = 1;

	stack[0] = &board[0][0];
	stack[0]->from = stack[0];
	stack[0]->n = 0;

	printf("\x1b[2J");

	do {
		int i, x, y;
		move = stack[--sptr];
		if(!move)
			break;

		move->n = move->from->n + 1;

		y = GET_Y(board, move);
		x = GET_X(board, move, y);

		for(i=0; i < 8; i++){
			if( SAFE(i, x, y) && !board[x+M[i].x][y+M[i].y].n ){
				stack[++sptr] = &board[x+M[i].x][y+M[i].y];
				stack[sptr]->from = move;
			}
		}

 		/*printf("\x1b[H");*/
		printf("[MOV:%2d; S:%2d] (%d, %d)\n", move->n, sptr, x, y);

		for(x=0;x<SIZE;x++){
			printf("|");
			for(y=0;y<SIZE;y++){
				printf("%s%2d\x1b[0m|", (&board[x][y] == move)?"\x1b[1;33m":"", board[x][y].n);
			}
			printf("\n");
		}
		printf("\n");

		if(stack[sptr] == move){
			printf("^^^ %d -> %d\n", move->n, stack[sptr-1]->n);

			while(move->from != stack[sptr-1]->from){
				move->n = 0;
				move = move->from;
			}
		}

		/*stack[sptr--] = NULL;*/

	} while (sptr && move->n != (SIZE*SIZE));

	return 0;
}
