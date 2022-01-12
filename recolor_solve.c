#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include <string.h>
#include <assert.h>
#include "game_io.h"
#include <time.h> 
uint * nb_different_colors(game g,uint *size)
{
  //on initialise un tableau de taille max = nombre d'élements dans la grille!!!!!
  uint *tab_colors=malloc(game_nb_moves_max(g) *sizeof(uint));
  //indice qui sert de curseur dans le tableau tab_colors
  uint indice=0;

  //récupère la couleur (0,0)
  uint color_up_left = game_cell_current_color(g,0,0);
  tab_colors[0] = color_up_left;

  bool mark=true;
  for(unsigned int i = 0; i < game_height(g); i++)
  {
    for(unsigned int j = 0 ; j < game_width(g) ; j++)
    {
      //stocke temporairement la valeur qu'on lit dans la grille
       uint current_color = game_cell_current_color(g,j,i);
       //on se déplace dans tab_colors pour vérifier si la couleur actuelle est déjà présente
      for(uint x = 0; x <= indice ; x++ )
      {
        if( tab_colors[x] == current_color) 
        {
            mark = false;
        }
      }
      //si mark == true, la couleur actuelle n'a pas été encore rencontrée,
      //on l'insère donc dans le tableau et on incrémente l'indice
      if (mark)
      {
          *size = *size +1;
          indice ++;
          tab_colors[indice]=current_color;
      }

      mark=true;
    }
  }

  return tab_colors;
}


unsigned int apply_seq(game g, uint * seq, color * tab)
{
  uint nb_max_move = game_nb_moves_max(g);
  uint* sans_zero = malloc(nb_max_move*sizeof(uint));
  bool zero = false;
  bool ok = true;
  uint cpt = 0;
  for(uint i = 0; i < nb_max_move; i++)
  {
    if(seq[i] != 0)
    {
      zero = true;
    }
    if(zero)
    {
      sans_zero[cpt] = seq[i];
      cpt++;
    }
  }
  if(cpt > 1)
  {
    for(uint i = cpt - 1 ; i >  0 ; i--)
      {
        if(sans_zero[i] == sans_zero[i-1])
        {
          ok = false;
        }
      }
  }

  if(ok && zero && (game_cell_current_color(g,0,0) != tab[sans_zero[0]]))
  {
    for(uint i = 0; i < cpt; i++)
    {
      game_play_one_move(g, tab[sans_zero[i]]);
      //printf("test:%u ",tab[sans_zero[i]]);
    }
    //printf("\n");
  }
  free(sans_zero);
  return cpt;
}


void incrementeur(uint * seq, uint nb_color, uint size){
  seq[size-1] = (seq[size-1]+1) % nb_color;

    while(seq[size-1] == 0 ){
      size --;

      seq[size-1] = (seq[size-1] + 1) % nb_color;
    }
}

uint color_max( uint * tab, uint nb_color)
{
  unsigned int max = tab[0];
  for(unsigned int i = 0; i < nb_color; i++ )
  {
    if(tab[i] > max)
    {
      max = tab[i];
    }
  }
  return max;
}

bool fini(uint* seq, uint fin, uint s){
  for(uint i = 0; i < s; i++){
    if(seq[i] != fin){
      return false;
    }
  }
  return true;
}

void affichage(uint* seq, uint size){
  printf("PLAYING SEQUENCE --> ");
  bool write = false;
  for(uint i = 0; i < size; i++){
    if(seq[i] != 0)
    {
      write = true;
    }
    if(write)
    {
      printf("%u ",seq[i]);
    }
  }
  printf("\n");
}

void write_on_save(FILE *f, uint *tab, uint * seq, unsigned int size, game g)
{
	printf("Sequence found --> ");
	unsigned int i = game_nb_moves_max(g) - size;
	for(uint j = i; j < game_nb_moves_max(g); j++)
	{
		printf("%u ",tab[seq[j]]);
		fprintf(f,"%u ",tab[seq[j]]);
    }
    printf("\n");
    fprintf(f, "\n");
}


void bruteforce(game g, uint p, uint * tab, uint nb_color, uint * seq, char* ecris){
 
  	FILE* f = fopen(ecris ,"w");
  	uint max = game_nb_moves_max(g);
  	uint taille_sans_zero = 0;
  	unsigned int color_min = color_max(tab, nb_color);
  	if(p == 0)
  	{
    	while(!game_is_over(g) && !fini(seq,color_min , max))
    	{
      		game_restart(g);
      		//affichage(seq, max);
      		taille_sans_zero = apply_seq(g, seq, tab);
      		if(game_is_over(g))
      		{
        		write_on_save(f, tab, seq, taille_sans_zero, g);
      		} 
      		incrementeur(seq, nb_color, max);
      	}
    	if(fini(seq,color_min , max) && !game_is_over(g))
    	{
        	printf("No solution found\n");
        	fprintf(f, "NO SOLUTION\n" );
    	}
  	}
	 if(p == 1)
	 {
	    uint cpt = 0;
	    // uint taille_prev = 0;
	    while(!fini(seq,color_min , max))
	    {
	      game_restart(g);
	      
	      taille_sans_zero = apply_seq(g, seq, tab);
	      //affichage(seq, max);
	      if(game_is_over(g))
	      {
	        // if(taille_prev == 0)
	        // {
	        // 	taille_prev = taille_sans_zero;
	        // }
	        // if(taille_prev < taille_sans_zero)
	        // {
	        // 	printf("Toutes les solutions minimales ont déjà été trouvées !\n");
	        // 	break;
	        // }
	        // write_on_save(f, tab, seq,  taille_sans_zero,g);
	        cpt++;
	        // taille_prev = taille_sans_zero;
	        
	      }
	      incrementeur(seq, nb_color, max);
	    }
	    fprintf(f,"NB_SOL = %u\n",cpt);
	  }
	  fclose(f);
}


void nb_sol(char *jeu_source, char *sol) {
  uint taille = 1;
  uint *nb_color = &taille;
  game g = game_load(jeu_source);
  uint * colortab = nb_different_colors(g, nb_color);
  uint maxi = color_max( colortab, *nb_color);
  uint* seq = (uint*) calloc(game_nb_moves_max(g),sizeof(uint));
  for(unsigned int i = 0; i < *nb_color; i ++)
  {
    seq[game_nb_moves_max(g) - i - 1] = maxi;
  }

  bruteforce(g, 1, colortab, *nb_color, seq, sol);
  game_delete(g);
  free(seq);
  free(colortab);
}


void findOne(char * src, char * sol){
  uint taille= 1;
  uint *nb_color = &taille;
  game g = game_load(src);
  uint * colortab = nb_different_colors(g, nb_color);
  uint* seq = (uint*) calloc(game_nb_moves_max(g),sizeof(uint));
  uint maxi = color_max( colortab, *nb_color);
  for(unsigned int i = 0; i < game_nb_moves_max(g) - 1; i ++)
  {
    seq[game_nb_moves_max(g) - i - 1] = maxi;
  }
  bruteforce(g, 0, colortab, *nb_color, seq, sol);
  game_delete(g);
  free(seq);
  free(colortab);
}

void findMin(char * src, char * sol){
  uint taille= 1;
  uint *nb_color = &taille;
  game g = game_load(src);
  uint * colortab = nb_different_colors(g, nb_color);
  uint* seq = (uint*) calloc(game_nb_moves_max(g),sizeof(uint));
  uint maxi = color_max( colortab, *nb_color);
  for(unsigned int i = 0; i < *nb_color; i ++)
  {
    seq[game_nb_moves_max(g) - i - 1] = maxi;
  }
  bruteforce(g, 0, colortab, *nb_color, seq, sol);
  game_delete(g);
  free(seq);
  free(colortab);
}

void usage(int argc, char *argv[]) {
  fprintf(stderr, "Usage: %s <solve> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}
char *concat(const char *s1, const char *s2) {
    char *res = malloc(strlen(s1) + strlen(s2) + 1);
    if (res) {
        strcpy(res, s1);
        strcat(res, s2);
    }
    return res;
}

int main(int argc , char ** argv){
  if (argc!=4){
    usage(argc, argv);
  }
  if(strcmp(argv[1], "FIND_ONE") == 0){
    clock_t t = clock();

    //convertir prefix en prefix.sol
    char * extension = ".sol";
    //unsigned int lenght = strlen(argv[3]);
    extension = concat(argv[3], extension);

    findOne(argv[2],extension);
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    uint minute = 0;
        if(time_taken >= 60)
        {
          while(time_taken >= 60)
          {
            minute ++;
            time_taken = time_taken - 60;
          }
        }
        if(minute > 0)
        {
          printf("FIND_ONE took %u minutes and %f seconds to execute \n", minute, time_taken);
        }
        else
        {
          printf("FIND_ONE took %f seconds to execute \n", time_taken);
        }
        free(extension);
    }
  else{
    if(strcmp(argv[1], "NB_SOL") == 0){
    	clock_t t = clock();

    	//convertir prefix en prefix.sol
    	char * extension = ".nbsol";
    	//unsigned int lenght = strlen(argv[3]);
    	extension = concat(argv[3], extension);
    	

      nb_sol(argv[2],extension);
      t = clock() - t;
      double time_taken = ((double)t)/CLOCKS_PER_SEC;
      uint minute = 0;
        if(time_taken >= 60)
        {
          while(time_taken >= 60)
          {
            minute ++;
            time_taken = time_taken - 60;
          }
        }
        if(minute > 0)
        {
          printf("NB_SOL took %u minutes and %f seconds to execute \n", minute, time_taken);
        }
        else
        {
          printf("NB_SOL took %f seconds to execute \n", time_taken);
        }
        free(extension);

    } 
    else{
    if(strcmp(argv[1], "FIND_MIN") == 0){
        clock_t t = clock();

        //convertir prefix en prefix.sol
    	char * extension = ".sol";
    	//unsigned int lenght = strlen(argv[3]);
    	extension = concat(argv[3],  extension);
    	

        findMin(argv[2],extension);
        t = clock() - t;
        double time_taken = ((double)t)/CLOCKS_PER_SEC;
        uint minute = 0;
        if(time_taken >= 60)
        {
          while(time_taken >= 60)
          {
            minute ++;
            time_taken = time_taken - 60;
          }
        }
        if(minute > 0)
        {
          printf("FIND_MIN took %u minutes and %f seconds to execute \n", minute, time_taken);
        }
        else
        {
          printf("FIND_MIN took %f seconds to execute \n", time_taken);
        }
        free(extension);
        
      }
    }
  }

  return EXIT_SUCCESS;
}
