#include <stdio.h>

#ifndef SIZE
#define SIZE 8
#endif

struct mov { int n;	struct mov *from; };

struct mov board[SIZE][SIZE];
struct {struct mov *mov; struct mov *from;} stack[SIZE*SIZE*SIZE];

struct {int row,col;} M[8] = {{ 1, 2}, { 2, 1}, { 2,-1}, { 1,-2}, {-1,-2}, {-2,-1}, {-2, 1}, {-1, 2}};

#define SAFE(n, row, col) ( (row+M[n].row>=0) && (row+M[n].row<SIZE) && (col+M[n].col>=0) && (col+M[n].col<SIZE) )
#define GET_COL(b,ptr) (((unsigned long)ptr-(unsigned long)&b)/sizeof(b[0][0])%(sizeof(b[0])/sizeof(b[0][0])))
#define GET_ROW(b,ptr,col) (((((unsigned long)ptr-(unsigned long)&b)/sizeof(b[0][0])-col))/(sizeof(b[0])/sizeof(b[0][0])))

int main(int argc, char **argv){
	struct mov *move;
	int sptr, best;

	sptr = 1;
	stack[sptr].mov  = &board[0][0];
	stack[sptr].from = &board[0][0];
	stack[sptr].mov->n = 0;

	best = 0;

	do {
		int i, row, col;

		move = stack[sptr--].mov;
		move->n    = stack[sptr+1].from->n + 1;
		move->from = stack[sptr+1].from;

		col = GET_COL(board, move);
		row = GET_ROW(board, move, col);

		for(i=0; i < 8; i++){
			if( SAFE(i, row, col) && !board[row+M[i].row][col+M[i].col].n ){
				sptr++;
				stack[sptr].mov  = &board[row+M[i].row][col+M[i].col];
				stack[sptr].from = move;
			}
		}

		/* tree should be symmetrical so we can discard the first branch */
		if(move == &board[0][0])
			sptr--;

		if(move->n > best){
			best = move->n;

			printf("[MOV:%2d; S:%2d] (%d, %d)\n", move->n, sptr, row, col);

			for(row=0;row<SIZE;row++){
				printf("|");
				for(col=0;col<SIZE;col++){
					printf("%s%s%s%3d\x1b[0m|",
						   (&board[row][col] == move)?"\x1b[1;32m":"",
						   (&board[row][col] == move->from)?"\x1b[36m":"",
						   (!board[row][col].n)?"\x1b[31;41m":"",
						   board[row][col].n);
				}
				printf("\n");
			}
			printf("\n");
		}

		if(stack[sptr+1].mov == move){
			while(move != stack[sptr].from){
				if(move == &board[0][0])
					break;

				col = GET_COL(board, move);
				row = GET_ROW(board, move, col);

				move->n = 0;
				move = move->from;
			}
		}
	} while (best < (SIZE*SIZE) && sptr > 0);

	return 0;
}
