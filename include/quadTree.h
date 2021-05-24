#ifndef QUADTREE_H
#define QUADTREE_H

#include "geometry.h"

typedef struct Node
{    // Les points sont les angles de la surface sur laquelle la node est définie 
    Square nodeSquare;

    // Les nodes sont les enfants du noeud dans lequel on se trouve 
    Node* nordOuest;
    Node* nordEst; 
    Node* sudEst;
    Node* sudOuest;
} QuadTree, Node;

void addNode(Node* newNode, int l1, int l2, int h1, int h2);

#endif