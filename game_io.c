#include <stdio.h>
#include "game.h"
#include "game_io.h"
#include <stdlib.h>

game game_load(char *filename) {
if(filename == NULL)
{
  fprintf(stderr,"Probleme pointeur fonction game load");
  exit(EXIT_FAILURE);
}
FILE* f = fopen(filename,"r");

if (f!=NULL) {
uint width, height, nb_coups_max;
char wrap;
bool wrapping;

/*fscanf (f, "%u\n%u\n%u\n%c\n", &width,&height,&nb_coups_max,wrap);*/

fscanf (f, "%u\n", &width);
fscanf (f, "%u\n", &height);
fscanf (f, "%u\n", &nb_coups_max);
fscanf (f, "%c\n", &wrap);
//on convertit le char lu pour wrap en bool et on l'enregistre dans une variable wrapping
if(wrap == 'N') {
  wrapping = false;
}
else {
  wrapping = true;
}


//on alloue un tableau qui stockera le jeu correspondant au fichier lu
color *tableau_lu =malloc ((width * height) * sizeof(color));

if(tableau_lu == NULL) {
  printf("Mauvaise allocation du nouveau tableau \n");
  fclose(f);
  exit(1);
}

unsigned int indice = 0;
for(unsigned int i = 0; (i < height && !(feof(f))); i++) { //parcours de toutes les lignes du fichiers
  for(unsigned int j = 0 ; j<width ; j++)  { //parcours caractère par caractère au sain de la ligne en cours
      uint val; //variable qui stocke temporairement la valeur courante lue dans le fichier f
      fscanf(f, "%u ", &val);
      tableau_lu[indice]=val;
      indice++;
      if(ferror(f)) {
          free(tableau_lu);
          exit(EXIT_FAILURE);
      }

  }
}

fclose(f);
game g= game_new_ext(width, height, tableau_lu, nb_coups_max,wrapping);

free(tableau_lu);

return g;
  }
return NULL;
}




void game_save(cgame g, char *filename)
{
if(filename == NULL)
{
  fprintf(stderr,"Probleme pointeur fonction game save");
  exit(EXIT_FAILURE);
}
FILE* f = fopen(filename,"w");
  fprintf(f, "%u ", game_width(g));
  fprintf(f, "%u ", game_height(g));
  fprintf(f, "%u ",game_nb_moves_max(g));
  bool test_wrapping = game_is_wrapping(g);
  if(test_wrapping == true)
  {
    fprintf(f, "S\n");
  }
  else
  {
    fprintf(f, "N\n");
  }
  for(unsigned int i = 0; i < game_height(g); i++)
  {
    for(unsigned int j = 0 ; j < game_width(g) ; j++)
    {
        if(j!=game_width(g)-1) {
          fprintf(f, "%u ", game_cell_current_color(g, j, i));
        }
        else {
          fprintf(f, "%u", game_cell_current_color(g, j, i));
        }
      }

      fprintf(f, "\n");
  }

  fclose(f);

}
