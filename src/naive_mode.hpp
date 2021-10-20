#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <chibi/eval.h> 

#include "mode.hpp"
#include "schemes.hpp"

using std::vector,  std::pair, std::string, std::to_string;
using std::function, std::bind, std::ref;
using namespace std::placeholders;

using sf::Keyboard, sf::Event, sf::Window, sf::RenderWindow, sf::Text;

// the atomic physical unit, some data and a transform
template <typename Data>
struct Physical {
  Data data;
  function<Data(Data)>& behavior;
};

/////// Input Events

bool key_p(Event& event, Keyboard::Key key);
void dostring(sexp ctx, const string& dome);
void listen_close(Event& event, RenderWindow& window);
void process_backspaces(string& str);
void process_enter(string& str, sexp ctx);
void listen_typing(Event& event, string* buf, sexp ctx);

//using naive = Physical<vector<ipair>>;
//using ipair = pair<int, int>;
//using naive = int;

struct Naive {
  Text inputline;
  Text framecounter;
  string* input;
};

Mode<Naive> make_naive_mode(sexp& ctx, RenderWindow& window);
