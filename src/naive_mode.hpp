#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <chibi/eval.h> 

#include "transwarp.h"
namespace tw = transwarp;

#include "mode.hpp"
#include "schemes.hpp"

using std::make_unique, std::unique_ptr;
using std::make_shared, std::shared_ptr;
using std::vector,  std::pair, std::string, std::to_string;
using std::function, std::bind, std::ref;
using namespace std::placeholders;

using sf::Keyboard, sf::Event, sf::Window, sf::RenderWindow, sf::Text;

/////// Input Events

bool key_p(Event& event, Keyboard::Key key);
void dostring(sexp ctx, const string& dome);
void listen_close(Event& event, RenderWindow& window);
void process_backspaces(string& str);
void process_enter(string& str, sexp ctx);
void listen_typing(Event& event, string* buf, sexp ctx);

/////// The Mode Declared

// the atomic physical unit, some data and a transform
template <typename Data>
struct Physical {
  Data data;
  function<Data(Data)> behavior;
};

using ipair = pair<int, int>;
using phys2d = Physical<ipair>;

// template <typename Data>
// unique_ptr<vector<tw::task>> runBehaviors(vector<Physical<Data>>&);

//using naive = Physical<vector<ipair>>;
//using naive = int;

struct Naive {
  Text inputline;
  Text framecounter;
  string* input;
};

Mode<Naive> make_naive_mode(sexp ctx, RenderWindow& window);
