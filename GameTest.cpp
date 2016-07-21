#include "GameObject.h"
#include "Player.h"

#include <vector>

int main() {
	std::vector<Player> p;
	std::vector<int> v{1, 2};
	GameObject g(0, 1000.0, v, p, 2);
}
