/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IntersectionNode.h
 * Author: woloszyn
 *
 * Created on March 9, 2016, 11:20 AM
 */
#pragma once
#define NO_EDGE -1

class IntersectionNode {
public:
    unsigned intersectionID;
    unsigned reachingEdge;
    bool visited;
    double travelTime;
    IntersectionNode(unsigned intersectionID) {
        this->intersectionID = intersectionID;
    }
    IntersectionNode() {
        this->intersectionID = -1;
        this->reachingEdge = NO_EDGE;
        this->visited = false;
        this->travelTime = 0;
    }
};
