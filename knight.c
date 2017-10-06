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
struct mov *stack[SIZE*SIZE*SIZE*SIZE*SIZE*SIZE*SIZE*SIZE];

int main(int argc, char **argv){
	struct mov *move;
	int sptr = 1;

	stack[0] = &board[0][0];
	stack[0]->from = &board[0][0];
	stack[0]->n = 0;

	stack[1] = &board[0][0];
	stack[1]->from = &board[0][0];
	stack[1]->n = 0;

	printf("\x1b[2J");

	do {
		int i, x, y;

		if(sptr)
			move = stack[sptr--];

		if(!move)
			break;

		move->n = move->from->n + 1;

		y = GET_Y(board, move);
		x = GET_X(board, move, y);

		printf("\n>>> (%d, %d)<%p>\n", x, y, stack[sptr+1]->from );

		for(i=0; i < 8; i++){
			if( SAFE(i, x, y) && !board[x+M[i].x][y+M[i].y].n ){
				sptr++;
				stack[sptr] = &board[x+M[i].x][y+M[i].y];
				stack[sptr]->from = move;
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

		if(stack[sptr+1] == move){
			printf("^^^ %d -> %d\n", move->n, stack[sptr]->from->n);
			do {
				if(move == &board[0][0])
					break;
				move->n = 0;
				move = move->from;
				y = GET_Y(board, move);
				x = GET_X(board, move, y);
				printf("%p <- %p (%d, %d) \n", move, move->from, x, y);
			} while (move->from->n && move->from != stack[sptr]->from);
		}

		printf("STACK[%2d] %p\n", sptr+1, move);
		for(i=0;i<=sptr;i++){
			y = GET_Y(board, stack[sptr-i]);
			x = GET_X(board, stack[sptr-i], y);
			printf(" (%d, %d) -> %2d <%p>\n", x, y, stack[sptr-i]->from->n, stack[sptr-i]->from );
		}

	} while (sptr > 0 && move->n < (SIZE*SIZE));

	return 0;
}
