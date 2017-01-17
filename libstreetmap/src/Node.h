/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Node.h
 * Author: woloszyn
 *
 * Created on March 10, 2016, 4:43 PM
 */

#pragma once


class DijkstraNode {
public:
    unsigned id;
    double distance; // The distance from source. (this will only be positive)
    int previousEdge; // The edge used to arrive at this node
    std::vector<unsigned> edgeIDs; // The edges that this node contains
    DijkstraNode(std::vector<unsigned> edgs) {
        edgeIDs = edgs;
    }
    DijkstraNode(std::vector<unsigned> edgs,double dist,int prevEdge) {
        edgeIDs = edgs;
        distance = dist;
        previousEdge = prevEdge;
    }
};
