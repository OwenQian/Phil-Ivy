#ifndef STAGES_H
#define STAGES_H
enum class Stage {
  HOLECARDS=0,// Deal stage ONLY 
  PREFLOP,    // Game stage, not deal stage
  FLOP,       // Deal and game stage
  TURN,       // Deal and game stage
  RIVER,      // Deal and game stage
  SHOWDOWN    // Game stage ONLY
};

Stage operator++(Stage& s);
Stage operator++(Stage& s, int);
Stage operator+(const Stage& s, int num);
Stage operator-(const Stage& s, int num);
#endif
