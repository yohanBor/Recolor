// SDL2 Demo by aurelien.esnard@u-bordeaux.fr

#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>
#include "game_io.h"
#include "model.h"
#include "game.h"
#include "game_rand.h"

typedef unsigned int uint;



/* **************************************************************** */

//constantes pour l'affichage
#define BACKGROUND "fond_blanc.png"
#define FONT "police.TTF" //fichier contenant la police d'écriture
#define FONTSIZE 30 //taille police cpt coups ou win ou OVER
#define FONTSIZE2 20 //taille message pour l'utilisateur



/* ***************************************************************** */

//structure qui contient les variables du programme (ex: textures)
struct Env_t{
  SDL_Texture * background; //fond d'écran du jeu
  SDL_Texture * evolution_coups_or_win_over; //pour l'affichage de l'évolution du nombre de nombres_coups
  SDL_Texture * message_for_user; //message d'information pour l'utilisateur
  game g;
  uint largeur_grille;
  uint hauteur_grille;
  uint size_color_table;
  uint **color_tablee;
};

/* **************************************************************** */

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

/* ********************************************************************** */
// initialisation de la structure (Env)
Env * init(SDL_Window* win, SDL_Renderer* ren, int argc, char* argv[])
{
  Env * env = malloc(sizeof(struct Env_t));

  /* obtenir la taille de l'écran actuel */
  int largeur_fenetre, hauteur_fenetre;
  SDL_GetWindowSize(win, &largeur_fenetre, &hauteur_fenetre);

  //initialisation des paramètres
  if(argv[1] == NULL)
  {
    env -> g = game_load("default_game.rec");
  }
  if(argc == 2)
  {
    env -> g = game_load(argv[1]);
  }
  if(argc == 4)
  {
    game jeu = game_random_ext(atoi(argv[1]), atoi(argv[2]), false, 4, atoi(argv[3]));
    game_save(jeu, "game_tempo.rec");
    env -> g = game_load("game_tempo.rec");

  }
  if(argc == 5)
  {
    game jeu = game_random_ext(atoi(argv[1]), atoi(argv[2]), false, atoi(argv[4]), atoi(argv[3]));
    game_save(jeu, "game_tempo.rec");
    env -> g = game_load("game_tempo.rec");
  }
  if(argc == 6)
  {
    bool wrapping = false;
    if(strcmp(argv[5],"S") == 0)
    {
      wrapping = true;
    }

    game jeu = game_random_ext(atoi(argv[1]), atoi(argv[2]), wrapping, atoi(argv[4]), atoi(argv[3]));
    game_save(jeu, "game_tempo.rec");
    env -> g = game_load("game_tempo.rec");
  }

  uint taille= 1;
  uint *nb_color = &taille;
  unsigned int *table = nb_different_colors(env ->g ,nb_color);
  unsigned int **color_table = malloc(*nb_color * sizeof(uint*));
  srand( time( NULL ) );
  for(unsigned int i = 0; i < *nb_color; i ++)
  {
    color_table[i] = malloc(4 * sizeof(uint));
    for(unsigned int p = 1; p < 4; p++)
    {
      color_table[i][p]= (uint)rand() % 255;
    }
    color_table[i][0]= table[i];

  }
  env -> color_tablee = color_table;
  env -> size_color_table = *nb_color;

  env -> largeur_grille = game_width(env->g);
  env -> hauteur_grille = game_height(env->g);
  env->background = IMG_LoadTexture(ren, BACKGROUND);

  if(!env->background)
  {
    ERROR("IMG_LoadTexture: %s\n", BACKGROUND);
  }


  //initialise l'affichage du compteur de coups
  SDL_Color color = { 167, 56, 56, 255}; //couleur rouge-orange
  TTF_Font * font = TTF_OpenFont(FONT, FONTSIZE);
  if(!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD); // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
  char str[15];
  sprintf(str, "%u / %u",  game_nb_moves_cur(env->g) ,game_nb_moves_max(env->g));
  SDL_Surface * text = TTF_RenderText_Blended(font, str, color); // blended rendering for ultra nice text
  env->evolution_coups_or_win_over = SDL_CreateTextureFromSurface(ren, text);
  SDL_FreeSurface(text);

  //initialise l'affichage pour l'UTILISATEUR
  SDL_Color color2 = { 42, 34, 221 , 255}; //couleur bleu foncé
  TTF_Font *font2 = TTF_OpenFont(FONT, FONTSIZE2);
  SDL_Surface *msg = TTF_RenderText_Blended(font2, "R: restart / Q: quit / S: save", color2);
  env->message_for_user = SDL_CreateTextureFromSurface(ren, msg);
  SDL_FreeSurface(msg);
  TTF_CloseFont(font);

  //nom de la fenêtre
  SDL_SetWindowTitle(win, "Recolor Text");

  /*SDL_Surface *icon;
  icon = SDL_LOADBMP("icon.png");
  SDL_WM_SetIcon(icon, NULL);
  SDL_FreeSurface(icon);*/
  return env;
}

/* **************************************************************** */

//effectue le rendu graphique des différents élements
void render(SDL_Window* win, SDL_Renderer* ren, Env * env)
{

  SDL_Rect rect; //pour l'affichage du texte (évolution nombre coups)
  uint largeur_grille = env -> largeur_grille;
  uint hauteur_grille = env -> hauteur_grille;

  /* obtenir la taille de l'écran actuel */
  int largeur_fenetre, hauteur_fenetre;
  SDL_GetWindowSize(win, &largeur_fenetre, &hauteur_fenetre);

  //afin de laisser de la place dans la partie inférieure de la fenetre (pour l'affichage des paramètres du jeu)
  hauteur_fenetre -= 50;

  /* render background texture */
  SDL_RenderCopy(ren, env->background, NULL, NULL);

  //initialisation et remplissage des carrés (ou rectangle)
  SDL_Rect rectangle;
  rectangle. x = 0; //coordonnée x de départ
  rectangle. y = 0;  //coordonnée y de départ
  rectangle. w = largeur_fenetre / largeur_grille ; //largeur rectangle
  rectangle. h = hauteur_fenetre / hauteur_grille ; // hauteur rectangle

  for (uint i = 0; i< hauteur_grille ; i++ )
  {
    for (uint j = 0; j< largeur_grille ; j++)
    {
      //on remplit les lignes

      // ROUGE
      for(unsigned int s = 0; s < env -> size_color_table; s ++ )
      {
        if(game_cell_current_color(env -> g, j, i) == env -> color_tablee[s][0])
        {
            SDL_SetRenderDrawColor(ren, env -> color_tablee[s][1], env -> color_tablee[s][2],env -> color_tablee[s][3], SDL_ALPHA_OPAQUE);
        }
      }



      SDL_RenderFillRects(ren, &rectangle,1);
      rectangle . x += rectangle.w;
    }
    //puis les colonnes
    rectangle.x = 0;
    rectangle. y += rectangle.h;
  }


  // Affichage graphique du compteur de coups
  SDL_QueryTexture(env->evolution_coups_or_win_over, NULL, NULL, &rect.w, &rect.h);
  rect.x = largeur_fenetre/2 - rect.w/2;
  rect.y = hauteur_fenetre;
  SDL_RenderCopy(ren, env->evolution_coups_or_win_over, NULL, &rect);

  // Affichage graphique message pour l'utilisateur
  SDL_QueryTexture(env->message_for_user, NULL, NULL, &rect.w, &rect.h);
  rect.x = largeur_fenetre/1.75;
  rect.y = (hauteur_fenetre+50)/1.05;
  SDL_RenderCopy(ren, env->message_for_user, NULL, &rect);


}


/* **************************************************************** */

//fonction qui traite les événements (interaction utilisateur - machine) via la souris/clavier
bool process(SDL_Window* win, SDL_Renderer* ren, Env * env, SDL_Event * e)
{
  // DEFINI POLICE ECRITURE
  TTF_Font * font = TTF_OpenFont(FONT, FONTSIZE);
  if(!font)
  {
    ERROR("TTF_OpenFont: %s\n", FONT);
  }

  /* obtenir la taille de l'écran actuel */
  int largeur_fenetre, hauteur_fenetre;
  SDL_GetWindowSize(win, &largeur_fenetre, &hauteur_fenetre);

  //afin de laisser de la place dans la partie inférieure de la fenetre (pour l'affichage des paramètres du jeu)
  hauteur_fenetre -= 50;
  uint largeur_rectangle = largeur_fenetre / env -> largeur_grille ;
  uint hauteur_rectangle = hauteur_fenetre / env -> hauteur_grille ;

  if (e->type == SDL_QUIT)
  {
    return true;
  }

  /* Android events */
#ifdef __ANDROID__

  else if (e->type == SDL_FINGERDOWN) {
    int x = e->tfinger.x;
    int y = e->tfinger.y;
    if (y < hauteur_fenetre-10) {
      color current = game_cell_current_color(env -> g, (x/largeur_rectangle), (y/hauteur_rectangle));
      game_play_one_move(env -> g, current);
    }


  }



#else
  //recuperer les coordonnées (x,y) sur lesquelles l'utilisateur a cliqué
  else if (e->type == SDL_MOUSEBUTTONDOWN)
  {
    int x;
    int y;

    SDL_GetMouseState(&x, &y);
    if(y < hauteur_fenetre - 10 )
    {
      //fprintf(stderr, "%u\n", y);
      //fprintf(stderr, "%u\n", hauteur_fenetre);
      color current = game_cell_current_color(env -> g, (x/largeur_rectangle), (y/hauteur_rectangle));
      game_play_one_move(env -> g, current);
    }

}
    // GAME OVER
    if( game_nb_moves_cur(env->g) > game_nb_moves_max(env->g))
    {
      SDL_Color color = { 255, 0, 0, 255}; //couleur rouge
      SDL_Surface * text = TTF_RenderText_Blended(font, "GAME OVER", color); // blended rendering for ultra nice text
      env->evolution_coups_or_win_over = SDL_CreateTextureFromSurface(ren, text);
      SDL_FreeSurface(text);
      //TTF_CloseFont(font);
    }
    //GAME WIN
    else if (game_is_over(env -> g)) {
      SDL_Color color_win_over = { 84, 167, 56, 255}; //couleur verte
      TTF_SetFontStyle(font, TTF_STYLE_BOLD); // TTF_STYLE_ITALIC | TTF_STYLE_NORMAL
      SDL_Surface * surf = TTF_RenderText_Blended(font, "YOU WIN !", color_win_over); // blended rendering for ultra nice text
      env->evolution_coups_or_win_over = SDL_CreateTextureFromSurface(ren, surf);
      SDL_FreeSurface(surf);
      TTF_CloseFont(font);
    }
    // ON CONTINUE DE JOUER
    else
    {
      SDL_Color color = { 167, 56, 56, 255};
      char str[15];
      sprintf(str, "%u / %u",  game_nb_moves_cur(env->g) ,game_nb_moves_max(env->g));
      SDL_Surface * text = TTF_RenderText_Blended(font, str, color); // blended rendering for ultra nice text
      env->evolution_coups_or_win_over = SDL_CreateTextureFromSurface(ren, text);
      SDL_FreeSurface(text);
      TTF_CloseFont(font);
    }

  // SDL_Event events;

  // RECUPERE LA LETTRE SAISI PAR L'UTILISATEUR
  if (e->type == SDL_KEYDOWN)
  {
    switch(e->key.keysym.sym)
    {
      //GAME RESTART
      case SDLK_r:
        game_restart(env -> g);
      break;

      case SDLK_s:
        game_save(env -> g, "mon_jeu.rec");
        break;


      //GAME QUIT
      case SDLK_q:
      if("game_tempo.rec" != NULL)
      {
        remove ("game_tempo.rec");
      }

        return true;
        break;


    }
  }
#endif
  return false;
}

/* **************************************************************** */

//termine le programme en s'assurant d'avoir libéré les allocations mémoires (faites dans la fonction init)
void clean(SDL_Window* win, SDL_Renderer* ren, Env * env)
{
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->evolution_coups_or_win_over);
  SDL_DestroyTexture(env->message_for_user);
  //SDL_DestroyTexture(env->color_tablee);
  remove ("game_tempo.rec");
  game_delete(env->g);
  free(env);
}

/* **************************************************************** */
