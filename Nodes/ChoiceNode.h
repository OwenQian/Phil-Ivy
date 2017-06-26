#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include <memory>  //smart pointer

#include "Node.h"
#include "../Action.h"
#include "../GameObject.h"

class ChoiceNode: public Node {
  private:
    virtual Decision makeDecision(std::vector<int> deck);

  public:
    // Constructor
    ChoiceNode();
    ChoiceNode(
        Stage             state,
        double            pot,
        std::vector<int>  boardCards,
        Player            botPlayer,
        Player            oppPlayer,
        int               playerTurn,
        Node*             const parent);
    ChoiceNode(const ChoiceNode&);
    ChoiceNode(const Node&);
    ChoiceNode(const NodeParamObject params) : Node(params) { };
};

#endif //ChoiceNode.h
