#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include <string.h>
#include <assert.h>

/* *************** TEST  ******************** */

bool test_game_restart()
{
    game g = game_new_empty();
    if(g == NULL)
    {
        fprintf(stderr,"Error: variable g nulle");
        return false;
    }
    // On joue une fois
    game_play_one_move(g, GREEN);
    game_restart(g);
    // On verifie que les mouvements actuels se sont bien remis à 0 apres le restart
    if(game_nb_moves_cur(g) != 0)
    {
      fprintf(stderr, "Error ! game_restart doesn't restart the number of moves to 0 ");
      game_delete(g);
      return false;
    }
    // On verifie que le jeu a retrouvé sa grille de depart
    for(unsigned int i = 0; i < SIZE ; i++)
    {
      for(unsigned int j = 0; j < SIZE ; j++)
      {
        if(game_cell_current_color(g,i,j) != RED)
        {
          fprintf(stderr, "Error ! game_restart doesn't restart the colors of the game to RED");
          game_delete(g);
          return false ;
        }
      }
    }
    game_delete(g);
    return true;
}

/* *************** TEST*************************** */
bool test_game_is_over()
{
  game g = game_new_empty();
  if(g==NULL)
  {
    fprintf(stderr,"Error: g null.");
    return false;
  }
  if(game_is_over(g))
  {
    // On verifie que le joueur n'a pas depassé le nombre de mouvement maximum
    if(game_nb_moves_cur(g) > game_nb_moves_max(g))
    {
      fprintf(stderr,"Limit of moves max reached");
      return false;
    }
    for(unsigned int i = 0; i < SIZE; i++)
    {
      for(unsigned int j = 0 ; j < SIZE;  j++)
      {
        //On verifie que toutes les cellules ont bien la même couleur
         if(game_cell_current_color(g, 0, 0) != game_cell_current_color(g, i, j))
         {
            game_delete(g);
            return false;
          }
      }
    }
  }
  game_delete(g);
  return true;
}
/* ************** TEST ***************** */
game del(game gm , game c)
{   game_delete(gm);
    game_delete(c);
  return false;
}
bool test_game_copy()
{
  color tab[SIZE*SIZE]={0,0,0,2,0,2,1,0,1,0,3,0,0,3,3,1,1,1,1,3,2,0,1,0,1,0,1,2,3,2,3,2,0,3,3,2,2,3,1,0,3,2,1,1,1,2,2,0,2,1,2,3,3,3,3,2,0,1,0,0,0,3,3,0,1,1,2,3,3,2,1,3,1,1,2,2,2,0,0,1,3,1,1,2,1,3,1,3,1,0,1,0,1,3,3,3,0,3,0,1,0,0,2,1,1,1,3,0,1,3,1,0,0,0,3,2,3,1,0,0,1,3,3,1,1,2,2,3,2,0,0,2,2,0,2,3,0,1,1,1,2,3,0,1};
  game g = game_new(tab, SIZE);
  if(g == NULL)
  {
    fprintf(stderr,"Error: variable g nulle");
    return false;
  }
  // On copie "g" dans "t"
  game t = game_copy(g);
  if(t == NULL)
  {
    fprintf(stderr,"Error: t null ");
    game_delete(g);
    return false;
  }
  // On verifie que les deux jeux ont le même nombre de mouvement max
  if(game_nb_moves_max(t) != game_nb_moves_max(g))
  {
    fprintf(stderr, "Not then same numpber of moves\n");
    del(g,t);
  }
  // On verifie que les deux jeux ont le même nombre de mouvement actuel
  if(game_nb_moves_cur(t) != game_nb_moves_cur(g))
  {
    fprintf(stderr, "Not the same number of current move\n");
    del(g,t);
  }
  // On verifie si les deux jeux sont toriques ou pas
  if(game_is_wrapping(t) != game_is_wrapping(g))
  {
    fprintf(stderr, "Problem on wrapping function for game copy\n");
    del(g,t);
  }
  // On verifie qu'ils ont la même hauteur
  if(game_height(t) != game_height(g))
  {
    fprintf(stderr, "Not the same height on game copy\n");
    del(g,t);
  }
  // On verifie qu'ils ont la même largeur
  if(game_width(t) != game_width(g))
  {
    fprintf(stderr, "Not the same height on game copy\n");
    del(g,t);
  }
  // On verifie que les deux jeux "t" et "g" ont une grille identique
  for(unsigned int a = 0 ; a < SIZE ; a++)
  {
    for (unsigned int b = 0 ; b < SIZE ; b++)
    {
      if (game_cell_current_color(g,b,a) != game_cell_current_color(t,b,a))
      {
        fprintf(stderr, "Error: the color doesn't corresponds to the initialised tab");
        del(g,t);
      }
    }
  }

game_delete(g);
game_delete(t);
return true;
}


/* ********** USAGE ********** */

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********** MAIN ROUTINE ********** */
int main(int argc, char *argv[]) {
    if (argc == 1) {
        usage(argc, argv);
    }

    fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
    bool ok = false;
/* test game new */

    if (strcmp("restart", argv[1]) == 0) {
       ok=test_game_restart();
    }
    else if (strcmp("over", argv[1]) == 0) {
         ok=test_game_is_over();
      }
      else if (strcmp("copy", argv[1]) == 0){
                    ok=test_game_copy();
             }
            else{
          fprintf(stderr, "Error: test \"%s\" not found!\n", argv[1]);
          exit(EXIT_FAILURE);
           }



// print test result
  if (ok) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }

}

