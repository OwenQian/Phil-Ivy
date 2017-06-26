#include "Stage.h"

Stage operator++(Stage& s) {
  s = static_cast<Stage>(static_cast<int>(s) + 1);
  return s;
}

Stage operator++(Stage& s, int) {
  Stage temp = s;
  s = static_cast<Stage>(static_cast<int>(s) + 1);
  return temp;
}

Stage operator+(const Stage& s, int num) {
  return static_cast<Stage>(static_cast<int>(s) + num);
}

Stage operator-(const Stage& s, int num) {
  return static_cast<Stage>(static_cast<int>(s) - num);
}
