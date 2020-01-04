#ifndef STATE_H
#define STATE_H

class State
{
public:

   State() = default;
   virtual ~State() = default;

   State(const State&) = delete;
   State& operator=(const State&) = delete;

   State(State&&) = delete;
   State& operator=(State&&) = delete;

   virtual void enter() = 0;
   virtual void execute(float deltaTime) = 0;
   virtual void exit() = 0;
};

#endif
