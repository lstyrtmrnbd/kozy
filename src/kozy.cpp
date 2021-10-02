#include <iostream>
#include <memory>
#include <string>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <chibi/eval.h> 

using std::string, std::to_string, std::u32string;

using sf::Keyboard, sf::Event, sf::Window, sf::Text;

void dostring(sexp& ctx, string& dome) {
  sexp_eval_string(ctx, dome.c_str(), -1, NULL);
}

bool key_p(Event& event, Keyboard::Key key) {
  return event.type == Event::KeyPressed && event.key.code == key;
}

Event& listen_close(Event& event, Window& window) {

  if(key_p(event, Keyboard::Escape) || event.type == Event::Closed) {
    window.close();
  }
  
  return event;
}

char keyToChar(Event::TextEvent text) {

  return text.unicode < 128 ? static_cast<char>(text.unicode) : '\0';
}

void processBackspaces(string& str) {

  size_t pos = str.find('\b');

  if(pos != string::npos) {
    if(str.length() <= 1) str.erase(pos,1);
    if(str.length() > 1) str.erase(pos - 1, 2);
  }
}

void processEnter(string& str, sexp& ctx) {

  size_t pos = str.find('\r');

  if(pos != string::npos) {
    dostring(ctx, str);
    str.clear();
    std::cout.flush();
  }
}

Event& listen_typing(Event& event, string& buf, sexp& ctx) {

  const char value = event.type == Event::TextEntered ?
    keyToChar(event.text) : '\0';

  if(value != '\0') buf.push_back(value);

  processBackspaces(buf);
  processEnter(buf, ctx);
  
  return event;
}

void pump_events(Window& window, string& inputBuf, sexp& ctx) {
  Event event;
  while (window.pollEvent(event)) {

    listen_close(event, window);
    listen_typing(event, inputBuf, ctx);
  }
}

int main() {


  sexp ctx;
  sexp_scheme_init();
  ctx = sexp_make_eval_context(NULL, NULL, NULL, 0, 0);
  sexp_load_standard_env(ctx, NULL, SEXP_SEVEN);
  sexp_load_standard_ports(ctx, NULL, stdin, stdout, stderr, 1);
  
  string dome = "(display \"hi\")";
  dostring(ctx, dome);
  std::cout << "\n";
  
  sf::Vector2i screenSize(640, 480);
  sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "kozy");

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
  Text inputLine;
  inputLine.setFont(font);
  inputLine.setString(input);
  inputLine.setCharacterSize(24);
  inputLine.setFillColor(sf::Color::Cyan);
  inputLine.setPosition(0.0f, 32.0f);
  
  sf::Clock frameClock = sf::Clock();
  
  while(window.isOpen()) {
    
    pump_events(window, input, ctx);

    sf::Time frameTime = frameClock.restart();
    
    // physics
    framecounter.setString(frameout + to_string(frameTime.asMicroseconds()));
    inputLine.setString(input);
    
    // rendition
    window.clear(sf::Color::Black);
    window.draw(framecounter);
    window.draw(inputLine);
    window.display();
  }

  sexp_destroy_context(ctx);
  
  return 0;
}


