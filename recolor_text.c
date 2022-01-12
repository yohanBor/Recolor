#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include <string.h>
#include <assert.h>
#include "game_io.h"
#include "game_rand.h"


/******************************** TRADUCTION HEXADECIMAL -> DECIMAL***********************/

void translate(int *v) {

  if(v==NULL) {
    fprintf(stderr,"Couleur v indalide \n");
    exit(1);
  }

//si la valeur est inférieure ou égal à 9 pas besoin de la traiter
 if(*v<=9){
   return ;
 }

//sinon, on part de cpt = 10 (car A=10 en héxadécimal)
int cpt = 10;
//sur la table ASCII, A=65 donc si v = 65, le pointeur v prend la valeur 10
 if(*v==65){
   *v = cpt;

 }

 //sinon on calcule la différence qu'il y a entre la valeur saisie par l'utilisateur et la valeur de A (65) et on l'incrémente au compteur
 // exemple: B = 66 (sur la table ASCII) donc diff = 1 et cpt + 1 = 11, ce qui correspond bien à la valeur de B
else {

int diff = *v-65;
*v=diff+cpt;
}


}

game game_default(){

    //tableau de couleurs par défauts
    color tab[SIZE*SIZE] =  {   0,0,0,2,0,2,1,0,1,0,3,0,
                                0,3,3,1,1,1,1,3,2,0,1,0,
                                1,0,1,2,3,2,3,2,0,3,3,2,
                                2,3,1,0,3,2,1,1,1,2,2,0,
                                2,1,2,3,3,3,3,2,0,1,0,0,
                                0,3,3,0,1,1,2,3,3,2,1,3,
                                1,1,2,2,2,0,0,1,3,1,1,2,
                                1,3,1,3,1,0,1,0,1,3,3,3,
                                0,3,0,1,0,0,2,1,1,1,3,0,
                                1,3,1,0,0,0,3,2,3,1,0,0,
                                1,3,3,1,1,2,2,3,2,0,0,2,
                                2,0,2,3,0,1,1,1,2,3,0,1
                            };

    //initialise par défaut un nouveau jeu ayant comme nombre de coups maximum 12
    game g = game_new(tab,12);

return g;
}


void game_display(cgame g)
{
unsigned int x = 0, y=0, nbm;
nbm = game_nb_moves_cur(g);

printf("nb coups joués : %u ; nb coups max : %u \n",nbm, game_nb_moves_max(g));
 for(x=0;x< game_height(g); x++)
{
    for(y=0;y< game_width(g);y++)
    {
    color v=game_cell_current_color(g,y,x);
    //gère les cas où la valeur saisie par l'utilisateur n'est pas un chiffre mais A,B,C,D,E ou F
    if(v==10) {
      printf("A");
    }
    else if(v==11) {
      printf("B");
    }
    else if(v==12) {
      printf("C");
    }
    else if(v==13) {
      printf("D");
    }
    else if(v==14) {
      printf("E");
    }
    else if(v==15) {
      printf("F");
    }
    //par défaut, affiche la valeur numérique saisie par l'utilisateur
      else {
          printf("%u",v);
        }
    }
printf("\n");
}
printf("Jouer un coup: (0,1,2,3,r ou q ;  r pour redémarrer ou q pour quitter\n");

}


game play(char * src, char * sol)
{
    game g = game_load(src);
    FILE* f = fopen(sol,"r");
    char test = fgetc(f);
    if(test == EOF)
    {
        printf("Attention, votre solution est vide !\n");
        fclose(f);
        exit(1);
    }
    fseek(f, 0, SEEK_SET); // repositionne le pointeur du fichier (qui a été deplacé par le fgetc) sur 0
    char val_to_play;
    uint i = game_nb_moves_max(g);
    while(!feof(f ) && i != 0)
    {
        fscanf(f, "%c ", &val_to_play);
        game_play_one_move(g,val_to_play - 48);
        game_display(g);
        i--;
    }
    fclose(f);
    return g;
}
int main(int argc, char **argv)
{
    //s'il n'y a qu'un seul argument, c'est le nom du programme donc on chare le jeu par défaut
    if(argc ==1)
    {
    //on crée un nouveau jeu qui recoit les paramètres par défaut (taille 12*12 et !wrapping)
    game g= game_default();
    game_display(g);
    //tant que le jeu n'est pas fini
    while(game_is_over(g)==false)
    {
        //récupère ce que saisit l'utilisateur
        int c=getchar();
        if (c == 'r')
        {
            game_restart(g);
            game_display(g);
        }
        if (c == 'q')
        {
          printf("DOMMAGE \n");
          game_save(g, "ecriture.rec");
          game_delete(g);
          return EXIT_SUCCESS;
        }
        //vérifie si la valeur, en hexadecimale est comprise entre 0 et 22 (pour '0'-'0' et 'F'-'0')
        if((c-'0') >=0 && c-'0' <=22)
        {
            //vérifie si le caractère saisi par l'utilisateur est supérieur ou égal à 'A' (en héxadécimal 'A'-'0' ==17)
            if((c-'0') >= 17)
            {
                //si c'est le cas, on crée un pointeur b vers le caractère puis on l'apelle avec la fonction translate (qui traduit une valeur d'hexadecimal en décimal)
                int *b=&c;
                if(b == NULL)
                {
                    exit(1);
                }
                translate(b);
                game_play_one_move(g,*b);
                game_display(g);
            }
            //sinon on ne modifie pas la valeur saisie et on joue le coup
            else
            {
                game_play_one_move(g,c-'0');
                game_display(g);
            }
            //si le jeu est fini
            if(game_is_over(g)==true)
            {
                //game_display(g);
                printf("BRAVO\n");
                game_delete(g);
                return EXIT_SUCCESS;
            }
        }
    }
    game_delete(g);
    }

  //sinon, on veut lire le fichier de sauvegarde
    if(argc == 2)
    {
        //on ouvre le fichier correspondant à l'argument passé en paramètre
        game g = game_load(argv[1]);
        game_display(g);
        //tant que le jeu n'est pas fini
        while(game_is_over(g)==false)
        {

            //récupère ce que saisit l'utilisateur
            int c=getchar();
            if (c == 'r')
            {
              game_restart(g);
              game_display(g);
            }
            if (c == 'q')
            {
                printf("DOMMAGE \n");
                //game_save(g, "ecriture.rec");
                game_delete(g);
                return EXIT_SUCCESS;
            }

            //vérifie si la valeur, en hexadecimale est comprise entre 0 et 22 (pour '0'-'0' et 'F'-'0')
            if((c-'0') >=0 && c-'0' <=22)
            {
                //vérifie si le caractère saisi par l'utilisateur est supérieur ou égal à 'A' (en héxadécimal 'A'-'0' ==17)
                if((c-'0') >= 17)
                {
                    //si c'est le cas, on crée un pointeur b vers le caractère puis on l'apelle avec la fonction translate (qui traduit une valeur d'hexadecimal en décimal)
                    int *b=&c;
                    if(b == NULL)
                    {
                        exit(1);
                    }
                    translate(b);
                    game_play_one_move(g,*b);
                    game_display(g);
                }
                //sinon on ne modifie pas la valeur saisie et on joue le coup
                else
                {
                    game_play_one_move(g,c-'0');
                    game_display(g);
                }
                //si le jeu est fini
                if(game_is_over(g) == true)
                {
                    //game_display(g);
                    printf("BRAVO\n");
                    game_delete(g);
                    return EXIT_SUCCESS;
                }

            }
        }
        game_delete(g);
    }
    if(argc == 4)
    {
        game g = game_random_ext(atoi(argv[1]), atoi(argv[2]), false, 4, atoi(argv[3]));
        game_display(g);

        while(game_is_over(g)==false)
        {
            int c=getchar();
            if (c == 'r')
            {
              game_restart(g);
              game_display(g);
            }
            if (c == 'q')
            {
                printf("DOMMAGE \n");
                game_delete(g);
                return EXIT_SUCCESS;
            }
            if((c-'0') >=0 && c-'0' <=22)
            {
                if((c-'0') >= 17)
                {
                    int *b=&c;
                    if(b == NULL)
                    {
                        exit(1);
                    }
                    translate(b);
                    game_play_one_move(g,*b);
                    game_display(g);
                }
                else
                {
                    game_play_one_move(g,c-'0');
                    game_display(g);
                }
                if(game_is_over(g) == true)
                {
                    printf("BRAVO\n");
                    game_delete(g);
                    return EXIT_SUCCESS;
                }

            }
        }
        game_delete(g);
    }
    if(argc == 5)
    {
        game g = game_random_ext(atoi(argv[1]), atoi(argv[2]), false, atoi(argv[4]), atoi(argv[3]));

        game_display(g);

        while(game_is_over(g)==false)
        {
            int c=getchar();
            if (c == 'r')
            {

                game_restart(g);
                game_display(g);
            }
            if (c == 'q')
            {
                printf("DOMMAGE \n");
                game_delete(g);
                return EXIT_SUCCESS;
            }
            if((c-'0') >=0 && c-'0' <=22)
            {
                if((c-'0') >= 17)
                {
                    int *b=&c;
                    if(b == NULL)
                    {
                        exit(1);
                    }
                    translate(b);
                    game_play_one_move(g,*b);
                    game_display(g);
                }
                else
                {
                    game_play_one_move(g,c-'0');
                    game_display(g);
                }
                if(game_is_over(g) == true)
                {
                    printf("BRAVO\n");
                    game_delete(g);
                    return EXIT_SUCCESS;
                }

            }
        }
        game_delete(g);
    }
    if(argc == 6)
    {
        bool wrapping = true;
        if(strcmp(argv[5],"N") == 0)
        {
            wrapping = false;
        }
        game g = game_random_ext(atoi(argv[1]), atoi(argv[2]), wrapping, atoi(argv[4]), atoi(argv[3]));
        //game test = game_copy(g);
        game_display(g);


        while(game_is_over(g)==false)
        {
            int c=getchar();
            if (c == 'r')
            {

                game_restart(g);
                game_display(g);
            }
            if (c == 'q')
            {
                printf("DOMMAGE \n");
                game_delete(g);
                //game_delete(test);
                return EXIT_SUCCESS;
            }
            if((c-'0') >=0 && c-'0' <=22)
            {
                if((c-'0') >= 17)
                {
                    int *b=&c;
                    if(b == NULL)
                    {
                        exit(1);
                    }
                    translate(b);
                    game_play_one_move(g,*b);
                    game_display(g);
                }
                else
                {
                    game_play_one_move(g,c-'0');
                    game_display(g);
                }
                if(game_is_over(g) == true)
                {
                    printf("BRAVO\n");
                    game_delete(g);
                    //game_delete(test);
                    //game_delete(test);
                    return EXIT_SUCCESS;
                }

            }
        }
        game_delete(g);

    }
    // if(argc == 4)
    // {
    //     if(strcmp(argv[1], "PLAY_SOL") == 0)
    //     {
    //         game g = play(argv[2],argv[3]);
    //         if(game_is_over(g) == true)
    //         {
    //             printf("BRAVO\n");
    //             game_delete(g);
    //             return EXIT_SUCCESS;
    //         }
    //         else
    //         {
    //             printf("La solution n'est pas bonne !\n");
    //             game_delete(g);
    //             return EXIT_SUCCESS;
    //         }
    //     }

    // }
    return EXIT_SUCCESS;
}
