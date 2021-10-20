#include <algorithm>
#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <chibi/eval.h> 

#include "mode.hpp"
#include "schemes.hpp"
#include "naive_mode.hpp"

/////// KOZY
///////
/////// yeah, ya finna

int main() {

  /// Scheme Initialization
  sexp ctx;
  sexp_scheme_init();
  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);

  /// Window Initialization
  sf::Vector2i screenSize(640, 480);
  sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "kozy");

  auto mode = make_naive_mode(ctx, window);

  do_mode(mode, window);
  
  sexp_destroy_context(ctx);
  return 0;
}


