/*Auteurs : LE DIREACH Gurvan <gledireach> / NADAL Louis <lnadal> / ARNAUD Brice
 * <brarnaud> / MORI Coraline <cmori> */

#include "game.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game_io.h"

// Game structure
struct game_s {
  color* tab;       // tab with the colors of the current game
  color* init_tab;  // tab with the initial colors
  uint move_max;
  uint move_cur;
  uint width;
  uint height;
  bool wrapping;
};

// Error detection

void static test_pointer(void* p) {
  if (!p) {
    fprintf(stderr, "Unvalid pointer !\n");
    exit(EXIT_FAILURE);
  }
}

void static test_game(cgame game) {
  if (game == NULL || !game->tab || !game->init_tab) {
    game_delete((struct game_s*)game);
    fprintf(stderr, "unvalid game !\n");
    exit(EXIT_FAILURE);
  }
}

void static test_coordinates(uint x, uint y, cgame g) {
  if (x >= game_width(g) || y >= game_height(g)) {
    game_delete((struct game_s*)g);
    fprintf(stderr, "Unvalid coordinates !\n");
    exit(EXIT_FAILURE);
  }
}

// Game structure initialization
/**
 * @brief set perameters in game structure
 * @param g the game
 * @param tab game color tab
 * @param init_tab game color tab to restart game
 * @param move_max maximal movement at game
 * @param move_cur current movement in game
 * @param width width
 * @param height height
 * @param wrapping boolean for is wrapping or not
 * @pre @p g is a valid pointer toward a game structure
 **/
void set_in_struct(game g, color* tab, color* init_tab, uint move_max,
                   uint move_cur, uint width, uint height, bool wrapping) {
  g->tab = tab;
  g->init_tab = init_tab;
  g->move_max = move_max;
  g->move_cur = move_cur;
  g->width = width;
  g->height = height;
  g->wrapping = wrapping;
}

game game_new_empty_ext(uint width, uint height, bool wrapping) {
  // we allocate the memory for the game, the 1st and 2nd tabs
  game new_empty_ext = (game)malloc(sizeof(struct game_s));
  if (!new_empty_ext) {
    fprintf(stderr, "Not enough memory for struct game_s !\n");
    exit(EXIT_FAILURE);
  }
  color* t = (color*)malloc(height * width * sizeof(color));
  if (!t) {
    free(new_empty_ext);
    fprintf(stderr, "Not enough memory for tab!\n");
    exit(EXIT_FAILURE);
  }
  color* t_init = (color*)malloc(height * width * sizeof(color));
  if (!t_init) {
    free(t);
    free(new_empty_ext);
    fprintf(stderr, "Not enough memory for init_tab!\n");
    exit(EXIT_FAILURE);
  }
  // we fill both tabs with the color "0"
  for (uint i = 0; i < height * width; i++) {
    t[i] = 0;
    t_init[i] = 0;
  }
  set_in_struct(new_empty_ext, t, t_init, 0, 0, width, height, wrapping);
  return new_empty_ext;
}

game game_new_ext(uint width, uint height, color* cells, uint nb_moves_max,
                  bool wrapping) {
  test_pointer(cells);
  // we allocate the memory for the game
  game new_ext = (game)malloc(sizeof(struct game_s));
  if (!new_ext) {
    fprintf(stderr, "Not enough memory for struct game_s !\n");
    exit(EXIT_FAILURE);
  }
  // we allocate the memory for the first tab
  color* t = (color*)malloc(width * height * sizeof(color));
  if (!t) {
    free(new_ext);
    fprintf(stderr, "Not enough memory for tab!\n");
    exit(EXIT_FAILURE);
  }
  // we allocate the memory for the second tab
  color* t_init = (color*)malloc(width * height * sizeof(color));
  if (!t_init) {
    free(t);
    free(new_ext);
    fprintf(stderr, "Not enough memory for init_tab!\n");
    exit(EXIT_FAILURE);
  }

  // we fill both tabs with the given colors
  for (uint i = 0; i < width * height; i++) {
    t[i] = cells[i];
    t_init[i] = cells[i];
  }
  // we set the other parameters
  set_in_struct(new_ext, t, t_init, nb_moves_max, 0, width, height, wrapping);
  // we return the game
  return new_ext;
}

game game_new(color* cells, uint nb_moves_max) {
  return game_new_ext(SIZE, SIZE, cells, nb_moves_max, false);
}

game game_new_empty() { return game_new_empty_ext(SIZE, SIZE, false); }

// Data recovery

uint game_height(cgame game) {
  test_game(game);
  uint h = game->height;
  return h;
}

uint game_width(cgame game) {
  test_game(game);
  return game->width;
}

bool game_is_wrapping(cgame game) {
  test_game(game);
  return game->wrapping;
}

uint game_nb_moves_max(cgame g) {
  test_game(g);
  uint n = g->move_max;
  return n;
}

uint game_nb_moves_cur(cgame g) {
  test_game(g);
  uint n = g->move_cur;
  return n;
}

color game_cell_current_color(cgame g, uint x, uint y) {
  test_game(g);
  test_coordinates(x, y, g);
  // (x,y) <=> y*game_width(g)+x
  return g->tab[y * game_width(g) + x];
}

// Set game parameters

void game_set_cell_init(game g, uint x, uint y, color c) {
  test_game(g);
  test_coordinates(x, y, g);
  // we set the case (x,y) to given color in both the initial and the current
  // game tabs
  g->init_tab[y * game_width(g) + x] = c;
  g->tab[y * game_width(g) + x] = c;
  return;
}

void game_set_max_moves(game g, uint nb_max_moves) {
  test_game(g);
  if (nb_max_moves < 1) {
    game_delete(g);
    fprintf(stderr, "Unvalid number of moves !\n");
    exit(EXIT_FAILURE);
  }
  g->move_max = nb_max_moves;
  return;
}

// Apply game

/**
 * @brief Colors all the neighbours of the case (x,y) if they have
 * @param g the game
 * @param c the color
 * @param x the first coordinate of the cell
 * @param y the second coordinate of the cell
 * @pre @p g is a valid pointer toward a game structure
 * @pre 0 <= @p c_old < 16
 * @pre 0 <= @p c_new < 16
 * @pre @p c_old != @p c_new
 * @pre @p x < game_width(g)
 * @pre @p y < game_height(g)
 **/
void static game_color_neighbors(game g, color c_old, color c_new, uint x,
                                 uint y) {
  test_game(g);
  test_coordinates(x, y, g);
  if (c_old == c_new) {
    game_delete(g);
    fprintf(stderr, "Unvalid color !\n");
    exit(EXIT_FAILURE);
  }

  // upper cell : (x,y-1)
  // if the coordinates of the upper neighbor are valid and it is the same color
  if (y >= 1 && game_cell_current_color(g, x, y - 1) == c_old) {
    // color the cell and call function on its neighbors
    g->tab[(y - 1) * game_width(g) + x] = c_new;
    game_color_neighbors(g, c_old, c_new, x, y - 1);
    // no need to worry about repeated neighbors because the old color is always
    // different from the new one
  }
  // lower cell : (x,y+1)
  if (y + 1 < game_height(g) && game_cell_current_color(g, x, y + 1) == c_old) {
    g->tab[(y + 1) * game_width(g) + x] = c_new;
    game_color_neighbors(g, c_old, c_new, x, y + 1);
  }
  // left cell : (x-1,y)
  if (x >= 1 && game_cell_current_color(g, x - 1, y) == c_old) {
    g->tab[y * game_width(g) + (x - 1)] = c_new;
    game_color_neighbors(g, c_old, c_new, x - 1, y);
  }
  // right cell : (x+1,y)
  if (x + 1 < game_width(g) && game_cell_current_color(g, x + 1, y) == c_old) {
    g->tab[y * game_width(g) + (x + 1)] = c_new;
    game_color_neighbors(g, c_old, c_new, x + 1, y);
  }

  // if the game is wrapping, we check the other neighbors too
  if (game_is_wrapping(g)) {
    // (x,y) <=> y*game_width(g)+x
    // upper cell : (x,game_height(g)-1) <=> (game_height(g)-1)*game_width(g)+x
    if (y == 0 && game_cell_current_color(g, x, game_height(g) - 1) == c_old) {
      // color the cell and call function on its neighbors
      g->tab[(game_height(g) - 1) * game_width(g) + x] = c_new;
      game_color_neighbors(g, c_old, c_new, x, game_height(g) - 1);
    }
    // lower cell : (x,0) <=> x
    if (y == game_height(g) - 1 && game_cell_current_color(g, x, 0) == c_old) {
      g->tab[x] = c_new;
      game_color_neighbors(g, c_old, c_new, x, 0);
    }
    // left cell : (game_width(g)-1,y) <=> y*game_width(g)+game_width(g)-1
    if (x == 0 && game_cell_current_color(g, game_width(g) - 1, y) == c_old) {
      g->tab[y * game_width(g) + game_width(g) - 1] = c_new;
      game_color_neighbors(g, c_old, c_new, game_width(g) - 1, y);
    }
    // right cell : (0,y) <=> y*game_width(g)
    if (x == game_width(g) - 1 && game_cell_current_color(g, 0, y) == c_old) {
      g->tab[y * game_width(g)] = c_new;
      game_color_neighbors(g, c_old, c_new, 0, y);
    }
  }
  return;
}

void game_play_one_move(game g, color c) {
  test_game(g);
  // we get the old color of the first cell
  color old_color = g->tab[0];
  // if the old and the new color are the same, we don't do anything
  if (old_color != c) {
    // we color the first cell (0,0) and its neighbors
    g->tab[0] = c;
    game_color_neighbors(g, old_color, c, 0, 0);
  }
  g->move_cur++;
  return;
}

// Tools

game game_copy(cgame g) {
  test_game(g);

  game copy =
      game_new_empty_ext(game_width(g), game_height(g), game_is_wrapping(g));
  // we copy both tabs and the other parameters
  for (uint i = 0; i < game_width(g) * game_height(g); i++) {
    copy->tab[i] = g->tab[i];
    copy->init_tab[i] = g->init_tab[i];
  }
  copy->move_max = g->move_max;
  copy->move_cur = g->move_cur;
  copy->width = g->width;
  copy->height = g->height;
  copy->wrapping = g->wrapping;

  return copy;
}

void game_delete(game g) {
  if (g) {
    if (g->tab) {
      free(g->tab);
    }
    if (g->init_tab) {
      free(g->init_tab);
    }
    free(g);
  }
}

bool game_is_over(cgame g) {
  test_game(g);
  // we compare all of the cells colors with the color of the cell (0,0)
  color c = g->tab[0];
  for (uint i = 1; i < game_width(g) * game_height(g); i++) {
    // if one of the cells is different from the first one, the game is not over
    if (g->tab[i] != c) return false;
  }

  // if the current number of moves is different from the maximum, the game is
  // not over
  if (g->move_cur > g->move_max) return false;

  // else the game is over
  return true;
}

void game_restart(game g) {
  test_game(g);
  // we set all the cells of the game to the initial colors
  for (uint i = 0; i < game_width(g) * game_height(g); i++) {
    g->tab[i] = g->init_tab[i];
  }
  // we reset the current number of moves
  g->move_cur = 0;
}
