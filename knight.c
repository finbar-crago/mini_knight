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

struct mov board[SIZE][SIZE];
struct {struct mov *mov; struct mov *from;} stack[SIZE*SIZE*SIZE];

int main(int argc, char **argv){
	struct mov *move;

	int sptr = 1;
	stack[1].mov  = &board[0][0];
	stack[1].from = &board[0][0];
	stack[1].mov->n = 0;

	printf("\x1b[2J");

	do {
		int i, x, y;

		move = stack[sptr--].mov;

		if(!move)
			break;

		move->n    = stack[sptr+1].from->n + 1;
		move->from = stack[sptr+1].from;

		y = GET_Y(board, move);
		x = GET_X(board, move, y);

		printf("\n>>> (%d, %d) <%p>\n", x, y, stack[sptr+1].from );

		for(i=0; i < 8; i++){
			if( SAFE(i, x, y) && !board[x+M[i].x][y+M[i].y].n ){
				sptr++;
				stack[sptr].mov  = &board[x+M[i].x][y+M[i].y];
				stack[sptr].from = move;
			}
		}

        /* printf("\x1b[H");*/
		printf("[MOV:%2d; S:%2d] (%d, %d)\n", move->n, sptr, x, y);

		for(x=0;x<SIZE;x++){
			printf("|");
			for(y=0;y<SIZE;y++){
				printf("%s%s%2d\x1b[0m|",
					   (&board[x][y] == move)?"\x1b[1;33m":"",
					   (&board[x][y] == move->from)?"\x1b[44m":"",
					   board[x][y].n);
			}
			printf("\n");
		}
		printf("\n");

		if(stack[sptr+1].mov == move){
			printf("^^^ %d -> %d\n", move->n, stack[sptr+1].from->n);

			while (move != stack[sptr].from) {
				if(move == &board[0][0])
					break;

				y = GET_Y(board, move);
				x = GET_X(board, move, y);
				printf("%p <- %p (%d, %d) - %d\n", move->from, stack[sptr].from, x, y, move->n);

				move->n = 0;
				move = move->from;

			}

		}

		printf("STACK[%2d] %p\n", sptr, move);
		for(i=0;i<sptr;i++){
			y = GET_Y(board, stack[sptr-i].mov);
			x = GET_X(board, stack[sptr-i].mov, y);
			printf(" (%d, %d) -> %2d <%p>\n", x, y, stack[sptr-i].from->n, stack[sptr-i].from );
		}

	} while (sptr > 0 && move->n < (SIZE*SIZE));

	return 0;
}
