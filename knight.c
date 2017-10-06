#include <stdio.h>
#include <stdlib.h>

#define SIZE 8

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
	int sptr, best;

	sptr = 1;
	stack[sptr].mov  = &board[0][0];
	stack[sptr].from = &board[0][0];
	stack[sptr].mov->n = 0;

	best = 0;

	do {
		int i, x, y;
		move = stack[sptr--].mov;

		if(!move)
			break;

		move->n    = stack[sptr+1].from->n + 1;
		move->from = stack[sptr+1].from;

		y = GET_Y(board, move);
		x = GET_X(board, move, y);

		for(i=0; i < 8; i++){
			if( SAFE(i, x, y) && !board[x+M[i].x][y+M[i].y].n ){
				sptr++;
				stack[sptr].mov  = &board[x+M[i].x][y+M[i].y];
				stack[sptr].from = move;
			}
		}
		if(move == &board[0][0])
			sptr--; /* rm first branch */

		if(move->n > best){
			best = move->n;

			printf("[MOV:%2d; S:%2d] (%d, %d)\n", move->n, sptr, x, y);

			for(x=0;x<SIZE;x++){
				printf("|");
				for(y=0;y<SIZE;y++){
					printf("%s%s%s%3d\x1b[0m|",
						   (&board[x][y] == move)?"\x1b[1;32m":"",
						   (&board[x][y] == move->from)?"\x1b[36m":"",
						   (!board[x][y].n)?"\x1b[31;41m":"",
						   board[x][y].n);
				}
				printf("\n");
			}
			printf("\n");
		}

		if(stack[sptr+1].mov == move){
			while (move != stack[sptr].from) {
				if(move == &board[0][0])
					break;

				y = GET_Y(board, move);
				x = GET_X(board, move, y);

				move->n = 0;
				move = move->from;
			}
		}

	} while (best < (SIZE*SIZE) && sptr > 0);

	return 0;
}
