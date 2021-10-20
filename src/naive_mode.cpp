#include "naive_mode.hpp"

void dostring(sexp ctx, const string& dome) {
  sexp_eval_string(ctx, dome.c_str(), -1, NULL);
}

bool key_p(Event& event, Keyboard::Key key) {
  return event.type == Event::KeyPressed && event.key.code == key;
}

void listen_close(Event& event, RenderWindow& window) {

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

void process_enter(string& str, sexp ctx) {

  size_t pos = str.find('\r');

  if(pos != string::npos) {
    dostring(ctx, str);
    str.clear();
    std::cout.flush();
  }
}

void listen_typing(Event& event, string* buf, sexp ctx) {

  if(event.type == Event::TextEntered)
    buf->push_back(event.text.unicode);
  
  process_backspaces(*buf);
  process_enter(*buf, ctx);
}

Mode<Naive> make_naive_mode(sexp ctx, RenderWindow& window) {

  /// Scheme parameter initialization
  pair<int, int> testpair{4,20};

  sexp env = sexp_context_env(ctx);
  sexp a = sexp_make_fixnum(testpair.first);
  sexp b = sexp_make_fixnum(testpair.second);
  sexp param = sexp_cons(ctx, a, b);
  
  sexp_env_define(ctx, env, sexp_string_to_symbol(ctx, sexp_c_string(ctx, "param", -1)), param);

  // greeting line
  string greetz = "greetz";
  sexp greeting = box(ctx, greetz);
  sexp greeting_sym = sexp_string_to_symbol(ctx, sexp_c_string(ctx, "greeting", -1));
  sexp_env_define(ctx, env, greeting_sym, greeting);

  //cute
  string dome = "(display param)";
  dostring(ctx, dome);
  std::cout << "\n";

  /// Asset initialization
  auto font = new sf::Font;
  if (!font->loadFromFile("Ricty-Bold.ttf")) {
    std::cout << "Failed to load ricty...\n";
  }

  std::cout << "font init\n";
  
  function<Text(sf::Color)> default_text =
    [&](sf::Color color){
      Text text;
      text.setFont(*font);
      text.setCharacterSize(24);
      text.setFillColor(color);
      return text;
    };

  std::cout << "fn def\n";
  
  Text framecounter = default_text(sf::Color::White);

  string* input = new string{""};
  Text inputline = default_text(sf::Color::Cyan);
  inputline.setString(*input);
  inputline.setPosition(0.0f, 32.0f);

  std::cout << "finner return\n";
  
  return Mode<Naive> {
    {// initial State
      inputline, framecounter, input
    },
     
    [ctx, env, greeting, greeting_sym](Naive& state) {
        
      sexp getme = sexp_env_ref(ctx, env, greeting_sym, greeting);
      string setme = unbox<string>(getme);
      state.framecounter.setString(setme);
      state.inputline.setString(*state.input);
      
      // acts on and mutates state
    },
      
    [&window](Naive& state){
      // wants drawing info(?)
        
      window.clear(sf::Color::Black);
      window.draw(state.framecounter);
      window.draw(state.inputline);
      window.display();
      
      // produces side effect ;)
    },
    // event handlers
    {
      bind(listen_close, _1, ref(window)),
      bind(listen_typing, _1, input, ctx)
    }
  };
}
