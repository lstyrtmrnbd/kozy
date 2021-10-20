#pragma once

#include <functional>
#include <vector>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

using std::vector, std::function;

using sf::Time, sf::Clock;
using sf::Window, sf::Event;

/**
 * Mode
 *
 * - a declaration of physics, rendering, and event handlers
 * - a method of execution (do_mode)
 * - Time system and Event system could be factored out
 */

template <typename State>
struct Mode {
  State state;
  function<void(State&)> physics;
  function<void(State&)> rendition;
  vector<function<void(Event&)>> handlers;
};

template <typename State>
void do_mode(Mode<State>& mode, Window& window) {

  // length of each physics step
  Time dt = sf::milliseconds(16);
  
  Clock frame_clock = Clock();

  Time frame_time, // how long each render + event cycle took
    accumulator;   // remainder of time to be simulated
  
  Event event;
  
  while(window.isOpen()) {
    
    while(window.pollEvent(event)) {
          
      for(auto& handler : mode.handlers) {
        handler(event); // event pump prods state
      }
    }

    frame_time = frame_clock.restart();
    accumulator += frame_time;
    auto steps = accumulator.asMilliseconds() / dt.asMilliseconds();
    
    for(auto i = 0; i < steps; ++i) {
      // state gets updated by physics but
      // assigning to a data member each step: not very cool
      mode.physics(mode.state);  
    }

    accumulator %= dt;
    
    mode.rendition(mode.state); // state gets rendered
  }
}
