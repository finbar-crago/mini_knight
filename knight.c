#include <stdio.h>
#include <stdlib.h>

#define SIZE_X 8
#define SIZE_Y 8

#define START_X 0
#define START_Y 0

#define M 8

#define LOCAL_XY_EQUAL(base) (base->x == x && base->y == y)

struct {int x,y;} MOVES[M] = {{ 1, 2}, { 2, 1}, { 2,-1}, { 1,-2},
                              {-1,-2}, {-2,-1}, {-2, 1}, {-1, 2}};

struct game;
struct pos;
struct stack;
struct stack_node;

/* ---- stack stuff ---- */

struct stack_n {
  struct pos *pos;
  struct stack_n *next;
};

struct stack {
  struct stack_n *top, *anchor;
  int size;
};

struct stack * init_stack(struct pos *anchor){
  struct stack *s = malloc(sizeof(struct stack));

  s->anchor = malloc(sizeof(struct stack_n));
  s->anchor->pos = anchor;
  s->top = s->anchor;

  s->size = 0;

  return s;
}

void push(struct stack *s, struct pos *pos){
  struct stack_n *tmp = s->top;
  s->top = malloc(sizeof(struct stack_n));
  s->top->pos = pos;
  s->top->next = tmp;

  s->size++;
}

struct pos * pop(struct stack *s){
  struct stack_n *tmp;
  struct pos *pos;

  if(s->top == s->anchor)
    return s->top->pos;

  tmp = s->top;
  pos = s->top->pos;
  s->top = s->top->next;

  s->size--;
  free(tmp);

  return pos;
}

struct pos * peak(struct stack *s){
  return s->top->pos;
}

/* ---- end stack ---- */


/* ---- game stuff ---- */

struct pos {
  int move, x, y;
  struct pos **next[M];
  struct pos *pos, *last;
};

struct game {
  struct pos *board[SIZE_X][SIZE_Y];
  struct pos *start, *invalid;
  struct stack *work;
  int moves, win;
};

struct game * init_game(){
  struct game *g = malloc(sizeof(struct game));
  int x, y, n;

  g->win = (SIZE_X * SIZE_Y);
  g->moves = 0;
  g->invalid = malloc(sizeof(struct pos));
  g->invalid->move = -1;
  g->invalid->x = -1;
  g->invalid->y = -1;

  for(x=0;x<SIZE_X;x++){
    for(y=0;y<SIZE_Y;y++){
      g->board[x][y] = malloc(sizeof(struct pos));
      g->board[x][y]->move = -1;
      g->board[x][y]->pos = g->board[x][y];
      g->board[x][y]->x = x;
      g->board[x][y]->y = y;
      for(n=0;n<M;n++){
        if(x + MOVES[n].x >= 0 && x + MOVES[n].x < SIZE_X &&
           y + MOVES[n].y >= 0 && y + MOVES[n].y < SIZE_Y){
          g->board[x][y]->next[n] = &(g->board[ x + MOVES[n].x ][ y + MOVES[n].y ]);
        } else { g->board[x][y]->next[n] = &g->invalid; }
      }
    }
  }

  g->start = g->board[START_X][START_Y];
  g->start->move = 0;
  g->start->last = g->invalid;

  g->work = init_stack(g->start->pos);

  return g;
}



#define is_valid(g,x) (x != g->invalid && (x)->move == -1)


/* ---- end game ---- */

void print_board(struct game *game, struct stack *c, struct pos *pos){
  const char clean[] = "\x1b[0m";
  const char here[]  = "\x1b[1;33m";
  const char move[]  = "\x1b[1;42m";

  int x, y, n;
  char p[3], nxt[15], *fmt;

  for(x=0;x<SIZE_X;x++){
    for(y=0;y<SIZE_Y;y++){

      fmt = (char *) clean;

      for(n=0;n<M;n++){
        if( (*pos->next[n])->move == -1 && LOCAL_XY_EQUAL((*pos->next[n])) )
            fmt = (char *) move;
      }

      if(game->board[x][y] == pos){
        fmt = (char *) here;
      }

      nxt[0] = '\0';
      if(LOCAL_XY_EQUAL(game->work->top->pos))
        sprintf(nxt, "\x1b[0;1;32;5mXX");

      sprintf(p, "%2d", game->board[x][y]->move);
      printf("%s%2s\x1b[0m|", fmt, (nxt[0]!='\0')?nxt:((*p!='-')?p:"  "));
    }
    printf("\n");
  }
}

int main(int argc, char **argv){
  struct game  *game;
  struct stack *c, *bt, *sol_n;
  struct pos *pos;

  game = init_game();

  c  = init_stack(game->start->pos);
  bt = init_stack(game->start->pos);
  sol_n = c;

  do {
    int i, new;
    if(game->moves == game->win)
      break;

    printf("\n<%d> WRK:%d C:%d MOV:%d\n", game->win, game->work->size, c->size, game->moves);

    pos = pop(game->work);
    push(bt, peak(game->work));

    pos->move = game->moves++;
    push(c, pos);

    new = 0;

    for(i=0; i < M; i++){
      if( is_valid( game, *pos->next[i] )){
        push( game->work, *pos->next[i] );
        new++;
      }
    }

    if(!new){
      printf("\x1b[1;31mDEAD END!\x1b[0m\n");
      do {
        printf("\x1b[1;31mGO BACK...\x1b[0m\n");
        pos->move = -1;
        pop(c);
        game->moves--;
      } while((pos = pop(bt), pos) != peak(game->work));
    }

    print_board(game, c, pos);

  } while(game->work->size);

  printf("\nDONE!\n");

  return 0;
}
