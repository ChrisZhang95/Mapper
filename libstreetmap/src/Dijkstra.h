/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Dijkstra.h
 * Author: woloszyn
 *
 * Created on March 10, 2016, 2:03 PM
 */
#include <vector>
#include "Node.h"
#include "StreetData.h"
#include "m3.h"
#pragma once

class Dijkstra {
public:
    static std::vector< DijkstraNode > nodes;
    static std::vector< std::vector<unsigned> > edges;
    static std::vector<double> weights;
};

void dijkstra(unsigned sourceID,unsigned destID);
std::vector<unsigned> traceback(unsigned destID);
void dijkstraBackwards(unsigned sourceID,unsigned destID);
std::vector<unsigned> tracebackBackwards(unsigned destID);
void dijkstraPOI(unsigned sourceID,std::vector<unsigned> destIDs);
void initializeNodes(std::vector<std::vector<unsigned> > streetSegsByIntersectionID);