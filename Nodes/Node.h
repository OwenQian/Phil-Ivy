#ifndef NODE_H
#define NODE_H

#include "../GameObject.h"
#include "../Player.h"

#include <vector>
#include <memory>


class Node {
    protected:
        Node* parent; 
        std::unique_ptr<Node> foldChild;
        std::unique_ptr<Node> callChild;
        std::unique_ptr<Node> raiseChild;

        GameObject game;
        int visitCount;
        double expectedValue;

        double currentRaise;
        bool isFolded;
        bool isAllIn;
        bool firstAction;

        // Monte Carlo functions
        void naiveUCT(std::vector<double>& selectionScores, double exploreConst);

    public:
        Node();
        Node(int, double, std::vector<int>, Player, Player, int, Node*);
        Node(const Node& obj);
        Node& operator= (const Node& rhs);
        virtual ~Node();

        // Action functions
        virtual std::unique_ptr<Node>& call() = 0;
        virtual std::unique_ptr<Node>& raise(double) = 0;
        virtual std::unique_ptr<Node>& fold() = 0;
		
		//Game functions
		static void playGame();
		static void playRound(Player& BotPlayer, Player& OppPlayer);

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
