#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>

#include "../GameObject.h"
#include "../Player.h"
#include "../Action.h"
#include "../GameUtilities/Decision.h"

class Node {
    protected:
        Node* parent; 

        GameObject game;
        int visitCount;
        double expectedValue;

        double currentRaise;
        bool isFolded;
        bool isAllIn;
        bool firstAction;

    public:
        std::unique_ptr<Node> foldChild;
        std::unique_ptr<Node> callChild;
        std::unique_ptr<Node> raiseChild;
        Node();
        Node(int, double, std::vector<int>, Player, Player, int, Node*);
        Node(const Node& obj);
        Node& operator= (const Node& rhs);
        virtual ~Node();

        // Monte Carlo
        Action monteCarlo(int);
        void runSelection();
        void runSimulation();
        void backprop(double, double);
        void naiveUCT(std::vector<double>&);

        // Action functions
        virtual void call() = 0;
        virtual void raise(double) = 0;
        virtual void fold() = 0;
		
		//Game functions
		static void playGame();
		static void playRound(Player& BotPlayer, Player& OppPlayer);
        int playTurn();
        virtual Decision makeDecision() = 0;

        // Getters
        GameObject & getGame() { return game; }
        GameObject const & getGame() const { return game; }
        Node* getParent() const { return parent; }
        bool getIsFolded() const { return isFolded; }
        double const & getCurrentRaise() const { return currentRaise; }
        bool getIsAllIn() const { return isAllIn; }
        bool getIsFirst() const { return firstAction; }
        int & getVisitCount() { return visitCount; }
        int const & getVisitCount() const { return visitCount; }
        double & getExpectedValue() { return expectedValue; }
        double const & getExpectedValue() const { return expectedValue; }

        // Setters
        void setCurrentRaise(double amount) { currentRaise = amount; }
        void setIsFolded(bool v) { isFolded = v; }
        void setIsAllIn(bool status) { isAllIn = status; }
        void setIsFirst(bool a) { firstAction = a; }
        void incrementVisitCount() { ++visitCount; }
        void setVisitCount(int c) { visitCount = c; }
        void setExpectedValue(double EV) { expectedValue = EV; }
        void setParent(Node* newParent) { parent = newParent; }
		
};

#endif	//Node.h
