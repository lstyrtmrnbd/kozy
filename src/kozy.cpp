#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <chibi/eval.h> 

using std::string, std::to_string, std::u32string;
using std::vector, std::function;

using sf::Keyboard, sf::Event, sf::Window, sf::Text;
using sf::Time, sf::Clock;

/////// Scheme

void dostring(sexp& ctx, string& dome) {
  sexp_eval_string(ctx, dome.c_str(), -1, NULL);
}

/////// Input Events

bool key_p(Event& event, Keyboard::Key key) {
  return event.type == Event::KeyPressed && event.key.code == key;
}

Event& listen_close(Event& event, Window& window) {

  if(key_p(event, Keyboard::Escape) || event.type == Event::Closed) {
    window.close();
  }
  
  return event;
}

char key_to_char(Event::TextEvent text) {

  return text.unicode < 128 ? static_cast<char>(text.unicode) : '\0';
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

Event& listen_typing(Event& event, string& buf, sexp& ctx) {

  const char value = event.type == Event::TextEntered ?
    key_to_char(event.text) : '\0';

  if(value != '\0') buf.push_back(value);

  process_backspaces(buf);
  process_enter(buf, ctx);
  
  return event;
}

void pump_events(Window& window, string& inputBuf, sexp& ctx) {
  Event event;
  while (window.pollEvent(event)) {

    listen_close(event, window);
    listen_typing(event, inputBuf, ctx);
  }
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
  
  string dome = "(display \"hi\")";
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

  string frameout = "Current frame time: ";
  Text framecounter;
  framecounter.setFont(font);
  framecounter.setString(frameout);
  framecounter.setCharacterSize(24); // in pixels, not points!
  framecounter.setFillColor(sf::Color::White);

  string input = "";
  Text inputline;
  inputline.setFont(font);
  inputline.setString(input);
  inputline.setCharacterSize(24);
  inputline.setFillColor(sf::Color::Cyan);
  inputline.setPosition(0.0f, 32.0f);

  function<void(int)> physics =
    [&](int dt_micros){

      framecounter.setString(frameout + to_string(dt_micros));
      inputline.setString(input);
    };

  function<void(int)> rendition =
    [&](int dt_micros){
      
      window.clear(sf::Color::Black);
      window.draw(framecounter);
      window.draw(inputline);
      window.display();     
    };

  function<void(function<void(int)>, function<void(int)>)> dowork =
    [&](function<void(int)> physics, function<void(int)> rendition) {

      Clock frame_clock = Clock();
  
      while(window.isOpen()) {
    
        pump_events(window, input, ctx); // the shadow of time?

        Time frame_time = frame_clock.restart();
    
        physics(frame_time.asMicroseconds());
        rendition(frame_time.asMicroseconds());
      }

      sexp_destroy_context(ctx);
    };

  dowork(physics, rendition);
    
  return 0;
}


