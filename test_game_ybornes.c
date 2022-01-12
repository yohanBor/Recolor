#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include <string.h>
#include <assert.h>
#include "game_io.h"

//test fonctions sauvegardes

bool test_game_load()
{
	game g = game_load("default_game.rec");
	uint val; //variable qui stocke temporairement la valeur courante lue dans le fichier f
	char test_wrapping;
	FILE* f = fopen("default_game.rec","r");
	fscanf(f, "%u ", &val);
	if(val != game_width(g))
	{
		fprintf(stderr, "Le width ne correspond pas\n");
		game_delete(g);
		fclose(f);
		return false;
	}
	fscanf(f, "%u ", &val);
	if(val != game_height(g))
	{
		fprintf(stderr, "Le height ne correspond pas\n");
		game_delete(g);
		fclose(f);
		return false;
	}
	fscanf(f, "%u ", &val);
	if(val != game_nb_moves_max(g))
	{
		fprintf(stderr, "Le nombre de move max ne correspond pas\n");
		game_delete(g);
		fclose(f);
		return false;
	}
	fscanf(f, "%c ", &test_wrapping);
	if(game_is_wrapping(g) == true)
	{
		if(test_wrapping != 'S')
		{
			fprintf(stderr, "Le parametre wrapping ne correspond pas\n");
			game_delete(g);
			fclose(f);
			return false;
		}
	}
	if(game_is_wrapping(g) == false)
	{
		if(test_wrapping != 'N')
		{
			fprintf(stderr, "Le parametre wrapping ne correspond pas\n");
			game_delete(g);
			fclose(f);
			return false;
		}
	}
	if(f==NULL)
	{
       fprintf(stderr,"bad pointor");
       exit(EXIT_FAILURE);
   	}
   	// test la grille
	for(unsigned int i = 0; i < game_height(g); i++)
  	{
    	for(unsigned int j = 0 ; j < game_width(g) ; j++)
    	{

     		fscanf(f, "%u ", &val);
     		if(val != game_cell_current_color(g, j, i))
     		{
     			fprintf(stderr, "Couleur ne correspond pas\n");
     			game_delete(g);
					fclose(f);
     			return false;
     		}
    	}
    }
	game_delete(g);
	fclose(f);
	return true;
}

bool test_game_save()
{
	game g = game_load("default_game.rec");
    if(g == NULL)
    {
        fprintf(stderr,"Error: variable g nulle");
        return false;
    }
    if(game_width(g) != 12)
    {
    	fprintf(stderr, "La largeur ne correspond pas\n");
     	game_delete(g);
     	return false;
    }
    if(game_height(g) != 12)
    {
    	fprintf(stderr, "La hauteur ne correspond pas\n");
     	game_delete(g);
     	return false;
    }
    if(game_nb_moves_max(g) != 12)
    {
    	fprintf(stderr, "Le nombre de move max ne correspond pas\n");
     	game_delete(g);
     	return false;
    }
    if(game_is_wrapping(g)!= false)
    {
    	fprintf(stderr, "Le wrapping ne correspond pas\n");
     	game_delete(g);
     	return false;
    }
    game_play_one_move(g, 1);
    if(game_cell_current_color(g,0,0) != 1 && game_cell_current_color(g,0,1) != 1 && game_cell_current_color(g,0,2) != 1 && game_cell_current_color(g,1,0) != 1)
    {
    	fprintf(stderr, "La grille ne correspond pas\n");
     	game_delete(g);
     	return false;
    }
    return true;
}

//test fonctions v2:



bool test_game_height()
{
	// On initialise un jeu avec une longueur de 6
	game g = game_new_empty_ext(SIZE, SIZE, false);
	// On regarde si la fonction nous retourne bien une longueur de 6
	if(SIZE != game_height(g))
	{
    game_delete(g);
		return false;
	}
  game_delete(g);
	return true;
}

bool test_game_width()
{
	// On initialise un jeu avec une largeur de 6
	game g = game_new_empty_ext(6, 6, false);
	// on regarde si la fonction nous retourne bien une largeur de 6
	if(6 != game_width(g))
	{
    game_delete(g);
    return false;
	}
  game_delete(g);
	return true;
}


//test des fonctions "game_set_cell_init" "game_set_max_moves" "game_cell_current_color"

bool test_game_set_cell_init(void)
{
	game g = game_new_empty();
	if(g == NULL)
	{
		fprintf(stderr,"The game points toward an empty cell\n");
		exit(1);
	}
	// On initialise des cellules avec des couleurs arbitraires
	game_set_cell_init(g, 2, 1, 3);
	game_set_cell_init(g, 0, 0, 2);
	game_set_cell_init(g, 1, 2, 1);
	game_set_cell_init(g, 1, 1, 0);

	for (unsigned int i = 0 ; i < SIZE ; i++)
	{
		for (unsigned int j = 0 ; j < SIZE ; j++)
		{
			// On regarde que le jeu ne comprends que des cellules avec des couleurs valides
			if( game_cell_current_color(g, i, j) <0 )
			{
				fprintf(stderr,"bad color\n");
				game_delete(g);
				exit(1);
			}
		}
	}
	// On verifie que les couleurs des cellules correspondent bien a celle que l'on a definit auparavant
	if(game_cell_current_color(g, 0, 0) != 2 || game_cell_current_color(g, 1, 1) !=0 || game_cell_current_color(g, 1, 2) !=1 || game_cell_current_color(g, 2, 1) !=3)
	{
		game_delete(g);
		fprintf(stderr, "The initialised color is not the wanted one\n");
		return false;
	}
	game_delete(g);
	return true;
}


bool test_game_set_max_moves(void)
{
	game g = game_new_empty();
	if(g == NULL)
	{
		fprintf(stderr,"The game points toward an empty cell\n");
		return false;
	}
	// On initialise un jeu avec un maximum de 12 deplacements
	game_set_max_moves(g, 12);
	// On verifie que le jeu a bien un nombre de deplacement maximal de 12
	if(game_nb_moves_max(g) <=0 || game_nb_moves_max(g) !=12)
	{
		game_delete(g);
		fprintf(stderr,"The initialised max number of moves is not the good one\n");
		return false;
	}
	game_delete(g);
	color tab[SIZE*SIZE]={0,0,0,2,0,2,1,0,1,0,3,0,0,3,3,1,1,1,1,3,2,0,1,0,1,0,1,2,3,2,3,2,0,3,3,2,2,3,1,0,3,2,1,1,1,2,2,0,2,1,2,3,3,3,3,2,0,1,0,0,0,3,3,0,1,1,2,3,3,2,1,3,1,1,2,2,2,0,0,1,3,1,1,2,1,3,1,3,1,0,1,0,1,3,3,3,0,3,0,1,0,0,2,1,1,1,3,0,1,3,1,0,0,0,3,2,3,1,0,0,1,3,3,1,1,2,2,3,2,0,0,2,2,0,2,3,0,1,1,1,2,3,0,1};
    // On initialie un nombre de move max sur 10 depuis la fonction "game_new"
    game t = game_new(tab, 10);
	if(t == NULL)
	{
		fprintf(stderr,"The game points toward an empty cell\n");
		return false;
	}
	// On verifie que le jeu à bien 10 moves max
	if(game_nb_moves_max(t) != 10)
	{
		fprintf(stderr,"Function set_max_moves doesn't work");
		game_delete(t);
		return false;
	}
	game_delete(t);
	return true;
}

bool test_game_cell_current_color(void)
{
	// On initialise un jeu avec des couleurs arbitraires
	color tab[SIZE*SIZE]={0,0,0,2,0,2,1,0,1,0,3,0,0,3,3,1,1,1,1,3,2,0,1,0,1,0,1,2,3,2,3,2,0,3,3,2,2,3,1,0,3,2,1,1,1,2,2,0,2,1,2,3,3,3,3,2,0,1,0,0,0,3,3,0,1,1,2,3,3,2,1,3,1,1,2,2,2,0,0,1,3,1,1,2,1,3,1,3,1,0,1,0,1,3,3,3,0,3,0,1,0,0,2,1,1,1,3,0,1,3,1,0,0,0,3,2,3,1,0,0,1,3,3,1,1,2,2,3,2,0,0,2,2,0,2,3,0,1,1,1,2,3,0,1};
    game g = game_new(tab, SIZE);
	if(g == NULL)
	{
		fprintf(stderr,"The game points toward an empty cell\n");
		exit(1);
	}
	for (unsigned int i = 0 ; i < SIZE ; i++)
	{
            for (unsigned int j = 0 ; j < SIZE ; j++)
            {
            	// On verifie que le jeu ne contienne que des couleurs valides
				if(game_cell_current_color(g, i, j) >NB_COLORS || game_cell_current_color(g, i, j) <0 )
				{
					fprintf(stderr,"bad color\n");
					game_delete(g);
					exit(1);
				}
			}
	}
	// On verifie que les couleurs choisies precedement correspondent bien a celles retournées par "game_cell_current_color"
	if(game_cell_current_color(g, 0, 0) != 0 ||game_cell_current_color(g, 3, 0) != 2  || game_cell_current_color(g, 6, 0) != 1 || game_cell_current_color(g, 10, 0) != 3 ){
		game_delete(g);
		fprintf(stderr,"game_cell_current_color doesn't return the wanted color\n");
		exit(1);
	}
	game_delete(g);
	return true;
}

bool test_game_nb_moves_max(void)
{
    game g = game_new_empty();
    if (g==NULL)
    {
    fprintf(stderr,"pointer is NULL");
    return false;
  	}
  	// On donne un nombre de 12 mouvements maximum a notre jeu "g"
    game_set_max_moves(g, 12);
    // On verfie que le jeu comprends bien 12 mouvements maximums
    if((game_nb_moves_max(g)) != 12)
    {
        fprintf (stderr,"wrong max number of moves ");
        game_delete(g);
        return false;
    }
    game_delete(g);
    return true;
}

bool test_game_nb_moves_cur()
{
	game g = game_new_empty();
	int a = game_nb_moves_cur(g);
  	if (g==NULL)
  	{
    fprintf(stderr,"pointer is NULL");
    return false;
  	}
  	// On joue une fois
  	game_play_one_move(g, GREEN);
  	// On verifie que le nombre de mouvement actuel a augmenter de 1
  	if(game_nb_moves_cur(g) != a + 1)
  	{
	    fprintf (stderr,"The fonction game_nb_moves_cur does not return the right number of move");
	    game_delete(g);
	    return false;
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
    if (argc <= 1) {
        usage(argc, argv);
    }

    fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
    bool ok = false;
/* test_game_set_cell_init */

    if (strcmp("game_set_cell_init", argv[1]) == 0) {
        ok = test_game_set_cell_init();
    }
    else if(strcmp("game_set_max_moves", argv[1]) == 0) {
        ok = test_game_set_max_moves();
    }
    else if(strcmp("game_cell_current_color", argv[1]) == 0) {
        ok = test_game_cell_current_color();
    }
    else if(strcmp("game_nb_moves_max", argv[1]) == 0) {
        ok = test_game_nb_moves_max();
    }
    else if(strcmp("game_nb_moves_cur", argv[1]) == 0) {
        ok = test_game_nb_moves_cur();
    }
    else if(strcmp("game_width", argv[1]) == 0) {
        ok = test_game_width();
    }
    else if(strcmp("game_height", argv[1]) == 0) {
        ok = test_game_height();
    }
    else if(strcmp("game_load", argv[1]) == 0) {
        ok = test_game_load();
    }
    else if(strcmp("game_save", argv[1]) == 0) {
        ok = test_game_save();
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
