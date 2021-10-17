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
#include "naive_mode.hpp"

using std::pair;

using std::string, std::to_string;
using std::vector, std::function;
using std::bind, std::for_each;
using namespace std::placeholders;
using std::ref;

using sf::Keyboard, sf::Event;
using sf::Window, sf::Text;

/////// Scheme

void dostring(sexp& ctx, string& dome) {
  sexp_eval_string(ctx, dome.c_str(), -1, NULL);
}

/////// Input Events

bool key_p(Event& event, Keyboard::Key key) {
  return event.type == Event::KeyPressed && event.key.code == key;
}

void listen_close(Event& event, Window& window) {

  if(key_p(event, Keyboard::Escape) || event.type == Event::Closed) {
    window.close();
  }
}

void process_backspaces(string& str) {

  size_t pos = str.find('\b');

  if(pos != string::npos) {
    if(str.length() <= 1) str.erase(pos,1);
    if(str.length() > 1) str.erase(pos - 1, 2);
  }
}

void process_enter(string& str, sexp& ctx) {

  size_t pos = str.find('\r');

  if(pos != string::npos) {
    dostring(ctx, str);
    str.clear();
    std::cout.flush();
  }
}

void listen_typing(Event& event, string& buf, sexp& ctx) {

  if(event.type == Event::TextEntered) buf.push_back(event.text.unicode);
  
  process_backspaces(buf);
  process_enter(buf, ctx);
}

/////// KOZY
///////
///////

int main() {

  /// Scheme Init
  sexp ctx;
  sexp_scheme_init();
  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);

  pair<int, int> testpair{4,20};
  
  sexp env = sexp_context_env(ctx);
  sexp a = sexp_make_fixnum(testpair.first);
  sexp b = sexp_make_fixnum(testpair.second);
  sexp param = sexp_cons(ctx, a, b);

  sexp_env_define(ctx, env, sexp_string_to_symbol(ctx, sexp_c_string(ctx, "param", -1)), param);

  string greetz = "greetz";
  sexp greeting = box(ctx, greetz);
  sexp greeting_sym = sexp_string_to_symbol(ctx, sexp_c_string(ctx, "greeting", -1));
  sexp_env_define(ctx, env, greeting_sym, greeting);
  
  string dome = "(display param)";
  dostring(ctx, dome);
  std::cout << "\n";

  /// Window Init
  sf::Vector2i screenSize(640, 480);
  sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "kozy");

  /// Asset Init
  sf::Font font;
  if (!font.loadFromFile("Ricty-Bold.ttf")) {
    std::cout << "Failed to load ricty...\n";
  }

  function<Text(sf::Color)> default_text =
    [&](sf::Color color){
      Text text;
      text.setFont(font);
      text.setCharacterSize(24);
      text.setFillColor(color);
      return text;
    };
  
  string frameout = "Current frame time: ";
  Text framecounter = default_text(sf::Color::White);

  string input = "";
  Text inputline = default_text(sf::Color::Cyan);
  inputline.setPosition(0.0f, 32.0f);

  //// Constructing a Mode
  Mode<int> prime =
    {
     1, // initial state
     
     [&](int state){
       // wants old state entities and associated behavior
       sexp getme = sexp_env_ref(ctx, env, greeting_sym, greeting);
       string setme = unbox<string>(getme);
       framecounter.setString(setme);
       inputline.setString(input);

       // produces new state
       return 1;
     },
     
     [&](int state){
       // wants entity state and drawing info(?)
      
       window.clear(sf::Color::Black);
       window.draw(framecounter);
       window.draw(inputline);
       window.display();

       // produces side effect ;)
     },
     // event handlers
     {
      bind(listen_close, _1, ref(window)),
      bind(listen_typing, _1, ref(input), ref(ctx))
     }
    };

  do_mode(prime, window);
  
  sexp_destroy_context(ctx);
  return 0;
}


