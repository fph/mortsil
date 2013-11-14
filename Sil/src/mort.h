#ifndef INCLUDED_MORT_H
#define INCLUDED_MORT_H

#include "angband.h"
//#include "defines.h"
//#include "externs.h"

/*
 * structure to keep probabilities of drawing nonnegative numbers (pdf)
 * p[i] contains the probability of rolling a total of i, i>=0
 */

struct odds
{
  int length;
  double *p;
};

typedef struct odds odds;

/* functions to manage odds (i.e., discrete pdf's over [0,length-1]) */
/* all returned results have to be freed with kill_odds*/
odds* make_zero_odds(); /*return the pdf of constant 0*/
odds* make_empty_odds(int length); /*return an empty pdf (zero vector of given length) */
double* make_p(int length); /*allocates a double[length] and sets it to 0.0*/ 
void set_odds(odds* o, double *p, int length); /*set the content of o to the given pdf and length p. Will take ownership of the pointer p*/

void add_scaled_odds(odds *o1, double constant, odds* o2); /*adds a multiple of o2 to o1. If o1==NULL, simply return a scaled version of o2*/
void add_const(odds* o, int n);
void kill_odds(odds*); /*frees memory*/
void min_twice(odds*); /*set odds to the pdf of min(a,b), for (a,b) independent and distributed as odds*/

void compress_odds(odds* o); /*removes trailing zeros from an odds structure*/

void add_throw(odds* o, int n, int sides); /*adds to the current odds n dice with a given number of sides*/
odds* make_variable_throw(odds *n, int sides); /*create pdf of throwing n dice with a given number of sides, where n varies according to another distribution*/

void odds_multiply_percent(odds* o, int percent); /*multiply x -> (x*percent)/100 (integer division) */
void odds_multiply(odds* o, int n); /*multiplies by n*/
void odds_divide(odds* o, int d); /*divides by d*/
void odds_chance(odds* o, odds* chance); /*simulates a "hit roll": multiplies a pdf (pointwise) by a 0/1 one*/
void collapse_to_01(odds* o); /*changes pdf to the one of min(o,1) */

void check_sum(odds* o); //debug-like function that throws an error if the odds of something do not sum to 1

void print_odds(odds*);

odds* odds_difference_capped(odds* o1, odds* o2); /*returns pdf of MAX(0,o1-o2) */

void negate(odds* o); /*swaps 0 and 1 for a boolean (length-2) pdf*/

/*simulates the result of hit_roll. Returns 0 for any result <0*/
/*the logic must match hit_roll(...) in cmd1.c, in case it gets changed */
odds* hit_roll_odds(int att, int evn, const monster_type *m_ptr1, const monster_type *m_ptr2); 

/*simulates the result of dam_roll and return odds of possible answers.*/
/*logic must match make_attack_normal in cmd1.c and project_p in spells1.c */
/*passes all the parameters that we need. The r_ptr is the one passed to crit_bonus */
/*there is a switch in make_attack_normal so that if !effect then we always hit - not sure if this is actually used anywhere */
odds* damroll_odds(odds* hit_result, int dd, int ds, int weight, const monster_race *r_ptr, int elem_bonus_dice, int effect, int no_crit);

odds* protection_roll_odds(int typ, bool melee);

void breath_damage_odds(int dd, int ds, int typ, int resistance); /*simulates *_dam_pure in spells1.c */

odds* check_hit_odds(int power); /*simulates check_hit in cmd1.c, returns odds of 0 or 1 */

odds* skill_check_odds(monster_type *m_ptr1, int skill, int difficulty, monster_type *m_ptr2); /*simulates skill_check in cmd1.c, returning 0 if it goes <=0 */

odds* saving_throw_odds(monster_type *m_ptr, bool resistance); /*simulates saving_throw in xtra2.c*/

odds* allow_player_aux_odds(monster_type *m_ptr, int player_flag); /*simulates allow_player_aux in xtra2.c*/

/*places where damage is dealt: 
		- make_attack_normal in melee1.c (melee) DONE
		- project_p in spells1.c (arrows, boulders) DONE
		- fire_dam_pure, cold_dam_pure, dark_dam_pure in spells1.c (breaths) DONE
		- hit_trap in cmd1.c (dungeon traps) DONE
		- chest_trap in cmd2.c (chest traps) DONE
		- play_instrument in use-obj.c (aiming a trumpet of blasting up) DONE
		- falling_damage in xtra2.c (falling damage) TODO
		- process_player in dungeon.c (poison, starvation, cuts) <- we can safely ignore this since it's flat damage, with no random component: either you die or not
		- earthquake, genocides in spells2.c <- I believe Sil does not use these
*/

#endif /*INCLUDED_MORT_H*/
