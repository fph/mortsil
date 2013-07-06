#ifndef INCLUDED_MORT_H
#define INCLUDED_MORT_H

/*
 * p[i] contains the probability of rolling a total of i, i>=0
 */

struct odds
{
  int length;
  double *p;
};

typedef struct odds odds;

/* functions to manage odds */
/* all returned results have to be freed with delete_odds*/
odds* make_odds(); /*return an "empty throw" of 0d0*/
void add_throw(odds* o, int n, int sides);
void add_const(odds* o, int n);
void kill_odds(odds*);







#endif /*INCLUDED_MORT_H*/
