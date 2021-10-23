#include "naive_mode.hpp"

void dostring(sexp ctx, const string& dome) {
  sexp_eval_string(ctx, dome.c_str(), -1, NULL);
}

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

Mode<Naive> make_naive_mode(sexp ctx, Window& window) {

  /// Scheme parameter initialization--------------------------
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

  /// Asset initialization-------------------------------------
  auto font = new sf::Font;
  if (!font->loadFromFile("Ricty-Bold.ttf")) {
    std::cout << "Failed to load ricty...\n";
  }
  
  function<Text(sf::Color)> default_text =
    [&](sf::Color color){
      Text text;
      text.setFont(*font);
      text.setCharacterSize(24);
      text.setFillColor(color);
      return text;
    };
  
  Text framecounter = default_text(sf::Color::White);

  string* input = new string{"(display \"hi\\n\")"};
  Text inputline = default_text(sf::Color::Cyan);
  inputline.setString(*input);
  inputline.setPosition(0.0f, 32.0f);

  /// Visual resources-----------------------------------------
  
  auto gremlin = new sf::Texture{};
  if (!gremlin->loadFromFile("gremlin.png")) {
    std::cout << "Failed to load gremlin.png...\n";
  }

  auto sprite = new sf::Sprite{};
  sprite->setTexture(*gremlin);

  //// Initialize physical processes---------------------------

  auto total_phys = 256;
  vector<phys2d>* physicals = new vector<phys2d>{total_phys};
  
  auto inbounds =
    [](const Window& window, const ipair& pos) {
      const int x = pos.first;
      const int y = pos.second;
      const int winx = window.getSize().x;
      const int winy = window.getSize().y;
      
      return pair<bool, bool> {
        (0 < x) && (x < winx),
        (0 < y) && (y < winy)
      };
    }; 

  auto reverse_if_false = [](bool p, int num) {
                            return p ? num : -1 * num;
                          };
  
  auto bounce_behavior =
    [&window, reverse_if_false, inbounds](pos_speed data) {
      const auto bounds = inbounds(window, data.first);

      const auto newspeed =
        ipair{reverse_if_false(bounds.first, data.second.first),
              reverse_if_false(bounds.second, data.second.second)};
                             
      return pos_speed{
        ipair{data.first.first + newspeed.first,
                data.first.second + newspeed.second},
          newspeed};
    };
  
  for(int i = 0; i < total_phys; ++i) {
    physicals->push_back(phys2d{{{i,i},{i,i%2}}, bounce_behavior});
  }

  // transwarp executor
  auto exec = make_shared<tw::parallel>(8);  

  //// the Mode: state, physics lambda, render lambda
  //// state holds parameters common to physics & render
  //// they capture their individual resource dependencies
  return Mode<Naive> {
    {// initial State
      inputline,
      framecounter,
      input,
      physicals
    },  
    [
     ctx, env, greeting, greeting_sym, exec
    ]
    (Naive& state) {
        
      sexp getme = sexp_env_ref(ctx, env, greeting_sym, greeting);
      string setme = unbox<string>(getme);
      state.framecounter.setString(setme);
      state.inputline.setString(*state.input);
      
      auto physical_behaviors = tw::for_each(*exec,
                                             state.physicals->begin(),
                                             state.physicals->end(),
                                             [](phys2d& phys){
                                               phys.data = phys.behavior(phys.data);
                                             });
      
      // force compiler to spit out type :)
      //decltype(behavior_task)::dummy = 1;
      physical_behaviors->wait();
      
      // acts on and mutates state
    },
    [
     gremlin, sprite
    ]
    (Naive& state, RenderWindow& window){
      // wants drawing info(?)
        
      window.clear(sf::Color::Black);

      auto colorme =
        [](RenderWindow& window, const ipair& pos){
          auto g = (pos.first / window.getSize().x)%255;
          auto b = (pos.second / window.getSize().y)%255;
          return sf::Color(128, g, b, 255);
        };
      
      for(const phys2d& phys : *state.physicals) {
        sprite->setPosition(phys.data.first.first,
                            phys.data.first.second);
        sprite->setColor(colorme(window, phys.data.first));
        window.draw(*sprite);
      }
      
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
