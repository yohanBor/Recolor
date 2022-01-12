#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include <string.h>
#include <assert.h>


/*******************test is_wrapping*********************/

bool test_is_wrapping(void)
{
    // On initialise un jeu torique
    game g = game_new_empty_ext(SIZE, SIZE, true);
    // On verifie que le jeu initialisé est bien torique
    if(game_is_wrapping(g) != true)
    {
        game_delete(g);
         return false;
    }
 game_delete(g);
 return true;
}

/*******************test wrapping***************************/

bool test_wrapping(void)
{
 color tab[SIZE*SIZE]={0,0,0,2,0,2,1,0,1,0,3,0,0,3,3,1,1,1,1,3,2,0,1,0,1,0,1,2,3,2,3,2,0,3,3,2,2,3,1,0,3,2,1,1,1,2,2,0,2,1,2,3,3,3,3,2,0,1,0,0,0,3,3,0,1,1,2,3,3,2,1,3,1,1,2,2,2,0,0,1,3,1,1,2,1,3,1,3,1,0,1,0,1,3,3,3,0,3,0,1,0,0,2,1,1,1,3,0,1,3,1,0,0,0,3,2,3,1,0,0,1,3,3,1,1,2,2,3,2,0,0,2,2,0,2,3,0,1,1,1,2,3,0,1};
 // On initialise un jeu torique
 game g = game_new_ext(SIZE, SIZE, tab, 20,  true);
 if(g==NULL)
{
     fprintf(stderr,"Error: variable g nulle");
     game_delete(g);
     return false;
 }
 // On applique un mouvement avec la couleur verte
 game_play_one_move (g, 1);
 // On verfie que le jeu est bien mis a jour (verification sur 2 cellules) en fonction du mouvement joué précédement et du fait qu'il soit torique
 if(game_cell_current_color(g, 1, 0) != game_cell_current_color(g, 1, 11))
 {
   game_delete(g);
    return false;
 }
 game_delete(g);
 return true;
}

/* *************** TEST GAME PLAY ONE MOVE ******************* */

bool test_game_play_one_move(void)
{
    // On crée un jeu vide (toutes les cellules ont la même couleur)
    game g = game_new_empty_ext(SIZE, SIZE, false);
    if(g==NULL)
    {
        fprintf(stderr,"Error: variable g nulle");
        return false;
    }
    if(game_is_wrapping(g) != false) {
      game_delete(g);
      return false;
    }
    // On stock le nombre de mouvement avant de jouer
    uint nb_moves_current = game_nb_moves_cur(g);
    // On applique un mouvement avec la couleur 5
    game_play_one_move(g, 5);
    // On regarde que le nombre de mouvement actuel a bien augmenté de 1
    if(game_nb_moves_cur(g) != nb_moves_current + 1)
    {
      game_delete(g);
      return false;
    }
    for (unsigned int a = 0 ; a < game_width(g) ; a++)
    {
        for (unsigned int b = 0 ; b < game_height(g); b++)
        {
            // On regarde que toutes les cellules ont la couleur 5
            if (game_cell_current_color(g,b,a) != 5  ) 
            {
                fprintf(stderr,"The game_play_one_move fonction does not works");
                game_delete(g);
                return false;
                }
            }
  }
  game_delete(g);
  return true;

}

/* *************** TEST GAME NEW ***************** */

bool test_game_new(void)
{
    color tab[SIZE*SIZE]={0,0,0,2,0,2,1,0,1,0,3,0,0,3,3,1,1,1,1,3,2,0,1,0,1,0,1,2,3,2,3,2,0,3,3,2,2,3,1,0,3,2,1,1,1,2,2,0,2,1,2,3,3,3,3,2,0,1,0,0,0,3,3,0,1,1,2,3,3,2,1,3,1,1,2,2,2,0,0,1,3,1,1,2,1,3,1,3,1,0,1,0,1,3,3,3,0,3,0,1,0,0,2,1,1,1,3,0,1,3,1,0,0,0,3,2,3,1,0,0,1,3,3,1,1,2,2,3,2,0,0,2,2,0,2,3,0,1,1,1,2,3,0,1};
    // On crée un jeu avec 12 mouvements max
    game g = game_new(tab, SIZE);
    if(g == NULL)
    {
        fprintf(stderr,"Error: variable g nulle");
        return false;
    }
    // On verifie que le jeu a bien SIZE mouvements max
    if(game_nb_moves_max(g) !=SIZE)
    {
        fprintf(stderr,"Error: bad initialisation of nb_max_moves");
        game_delete(g);
        return false;
    }
    unsigned int j = 0 ;
    for (unsigned int a = 0 ; a < game_height(g) ; a++)
    {
        for (unsigned int b = 0 ; b < game_width(g) ; b++)
        {
            // On verifie que "tab" marche bien
            if(game_cell_current_color(g,b,a) != tab[j])
            {
                fprintf(stderr, "Error: the color doesn't corresponds to the initialised tab");
                game_delete(g) ;
                return false ;
            }
            j++;
            }
        }

    game_delete(g);
    return true;
}

/******************TEST GAME EXT**********************/

bool test_game_new_ext(void){

    color tab[SIZE*SIZE]={0,0,0,2,0,2,1,0,1,0,3,0,0,3,3,1,1,1,1,3,2,0,1,0,1,0,1,2,3,2,3,2,0,3,3,2,2,3,1,0,3,2,1,1,1,2,2,0,2,1,2,3,3,3,3,2,0,1,0,0,0,3,3,0,1,1,2,3,3,2,1,3,1,1,2,2,2,0,0,1,3,1,1,2,1,3,1,3,1,0,1,0,1,3,3,3,0,3,0,1,0,0,2,1,1,1,3,0,1,3,1,0,0,0,3,2,3,1,0,0,1,3,3,1,1,2,2,3,2,0,0,2,2,0,2,3,0,1,1,1,2,3,0,1};
    // On crée un jeu torique de longueur SIZE et de largeur SIZE avec un nombre de mouvement maximum de 12
    game g = game_new_ext(SIZE, SIZE, tab, 12, true);
    if(g==NULL) {
        fprintf(stderr,"Error: variable g nulle");
        return false;
    }
    // On verifie la longeur et la largeur du jeu 
    if( game_width(g) != SIZE || game_height(g) != SIZE)
    {
        fprintf(stderr, "Error width and height\n");
        game_delete(g);
        return false;
    }
    // On verifie qu'il est bien torique
     if(game_is_wrapping(g) == false)
    {
        game_delete(g);
        return false;
    }
    // On verfie que son nombre de mouvement maximum est bien 12
    if(game_nb_moves_max(g) !=12)
    {
        fprintf(stderr,"Error: bad initialisation of nb_max_moves");
        game_delete(g);
        return false;
    }
    unsigned int j = 0 ;
    for (unsigned int a = 0 ; a < game_height(g) ; a++)
    {
        for (unsigned int b = 0 ; b < game_width(g) ; b++)
        {
            // On verifie que "tab" est bien initialisé
            if(game_cell_current_color(g,b,a) != tab[game_width(g)*a+b])
             {
                fprintf(stderr, "Error: the color doesn't corresponds to the initialised tab");
                game_delete(g) ;
                return false ;
            }
            j++;
        }
    }
    game_delete(g);
    return true;
}

/* *************** TEST NEW EMPTY ******************/

bool test_new_empty(void){
    game g = game_new_empty();
    if(g==NULL)
    {
        fprintf(stderr,"Error: variable g nulle");
        return false;
    }
    // On verifie que le nombre de mouvement est superieur à 0
    if(game_nb_moves_max(g) !=0 )
    {
    	game_delete(g);
        return false;
    }
    for (unsigned int a = 0 ; a < game_height(g) ; a++)
    {
        for (unsigned int b = 0 ; b< game_width(g) ; b++)
        {
            // On verifie que toutes les cellules du jeu son initalisé sur RED
            if (game_cell_current_color(g,b,a) != RED)
            {
                fprintf(stderr, "Error: the color doesn't corresponds to the initialised tab");
                game_delete(g) ;
                return false ;
            }

        }
    }
    game_delete(g);
    return true;
}

/******************* TEST NEW EMPTY EXT *********************/

bool test_new_empty_ext(void) {
    // On crée un jeu torique de longueur et largeur SIZE
    game g = game_new_empty_ext( SIZE, SIZE, true);
    if(g == NULL)
    {
        fprintf(stderr,"Error: variable g nulle");
        return false;
    }
    // On verifie que le jeu est bien torique
    if(game_is_wrapping(g) == false)
    {
        game_delete(g);
        return false;
    }
    // On verifie que son nombre de mouvement maximum est bien superieur à 0
    if(game_nb_moves_max(g) !=0)
    {
    	game_delete(g);
        return false;
    }
    // On verifie que la largeur et la longueur du jeu sont bien initialisées
    if(game_width(g) <= 0 || game_height(g) <= 0 || game_width(g) != SIZE || game_height(g) != SIZE)
    {
        game_delete(g);
        return false;
    }
    for (unsigned int a = 0 ; a < game_height(g) ; a++)
    {
        for (unsigned int b = 0 ; b < game_width(g) ; b++)
        {
            // On verifie que toutes les cellules du jeu sont bien initialisées sur la couleur RED
            if (game_cell_current_color(g,b,a) != 0)
            {
                    fprintf(stderr, "Error: the color doesn't corresponds to the initialised tab");
                    game_delete(g) ;
                    return false ;
            }

        }
    }
    game_delete(g);
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
/* test game_new */

    if (strcmp("game_new", argv[1]) == 0) {
        ok = test_game_new();
    }

/* test new_empty */

    else if(strcmp("new_empty", argv[1]) == 0) {
        ok = test_new_empty();
    }
    else if(strcmp("one_move", argv[1]) == 0)
        ok =test_game_play_one_move();
    else if(strcmp("game_new_ext", argv[1]) == 0)
        ok = test_game_new_ext();
    else if(strcmp("game_new_empty_ext", argv[1]) == 0)
        ok = test_new_empty_ext();
    /* test is_wrapping*/

     else if (strcmp("is_wrapping", argv[1]) == 0){
     ok = test_is_wrapping();
     }
     else if (strcmp("wrapping", argv[1]) == 0){
     ok = test_wrapping();
     }
    else {
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
