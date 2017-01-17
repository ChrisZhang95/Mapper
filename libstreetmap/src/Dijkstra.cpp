/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <vector>
#include <limits>
#include <queue>
#include <iostream>
#include "Dijkstra.h"
#include "m2.h"
#include <ctime>
#define NO_EDGE -1
#define FROM 0
#define TO 1
#define STREETID 2
std::vector< DijkstraNode > Dijkstra::nodes;

std::vector< double > Dijkstra::weights; // = {7,9,10,15,11,2,14,9,6};
std::vector< std::vector<unsigned> > Dijkstra::edges; //= {{0,1,0},{0,2,0},{2,1,0},{1,3,0},{2,3,0},{5,2,0},{0,5,0},{5,4,0},{4,3,0}}; // Each edge has a "from" and "to" node and a "streetid" entry

class waveElem {
public:
    unsigned intID;
    int edgeID;
    double travelTime;
    waveElem(unsigned intid,int edge,double ttime) {intID = intid;edgeID = edge;travelTime=ttime;}
    waveElem() {
        
    }
};

struct CompareOperator {
    bool operator()(const waveElem &lhs,const waveElem &rhs) {
        if (lhs.travelTime > rhs.travelTime) {
            return true;
        }
        else {
            return false;
        }
    }
};

//std::queue<waveElem> waveFront;
std::priority_queue< waveElem , std::vector<waveElem>, CompareOperator > waveFront;

void resetNodes(unsigned sourceID) {
    //waveFront = std::priority_queue< unsigned , std::vector<unsigned>, CompareOperator >(); // Reset the waveFront
    Dijkstra::nodes[sourceID].distance = 0;
    waveFront = std::priority_queue< waveElem , std::vector<waveElem>, CompareOperator >();
    for (unsigned i=0;i<Dijkstra::nodes.size();i++) {
        if (i != sourceID) {
            Dijkstra::nodes[i].distance = std::numeric_limits<double>::max(); // Set each node's distance from source to infinity "Unknown so far"
        }
        Dijkstra::nodes[i].previousEdge = NO_EDGE; // Make the edge of the previous node undefined
    }
    
}

double length(unsigned currentNodeID,std::vector<unsigned> edge,double weight) {

    if (Dijkstra::nodes[currentNodeID].previousEdge != NO_EDGE && Dijkstra::edges[Dijkstra::nodes[currentNodeID].previousEdge][STREETID] != edge[STREETID]) {
        return 0.25 + weight; // Turn was made
    }
    else {
        return weight; // No turn was made
    }
}

unsigned findNeighbour(unsigned currentNodeID,std::vector<unsigned> edge) {
    if (edge[FROM] == currentNodeID) {
        return edge[TO];
    }
    else {
        return edge[FROM];
    }
}



void dijkstra(unsigned sourceID,unsigned destID) {
    //double duration = 0;
    resetNodes(sourceID);
    //std::clock_t startTotal;
    //double durationTotal;
    //startTotal = std::clock();
    //setcolor(BLACK);
    double bestPathLen = std::numeric_limits<double>::max();
    
    for (unsigned i = 0;i<Dijkstra::nodes[sourceID].edgeIDs.size();i++) {
        //std::clock_t start;
        //start = std::clock();

        //draw_street_segment(Dijkstra::nodes[sourceID].edgeIDs[i]);
        //duration += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
         // Finds the neighbour of the current node (minimalIntID) along the edge
        unsigned neighbourNode = findNeighbour(sourceID,Dijkstra::edges[Dijkstra::nodes[sourceID].edgeIDs[i]]);
        unsigned currentEdge = Dijkstra::nodes[sourceID].edgeIDs[i];
        double &weight = Dijkstra::weights[currentEdge];
        double alternativeDistance = Dijkstra::nodes[sourceID].distance + length(sourceID,Dijkstra::edges[currentEdge],weight);// Potentially better distance to get to neighbour node from current node via edge
        if (getStreetSegmentInfo(Dijkstra::nodes[sourceID].edgeIDs[i]).to != sourceID || !getStreetSegmentInfo(Dijkstra::nodes[sourceID].edgeIDs[i]).oneWay) {
            waveFront.push(waveElem(neighbourNode,currentEdge,alternativeDistance)); // Reinsert back into the wavefront
        }
    }
    
    while (waveFront.size() != 0 && waveFront.top().travelTime < bestPathLen){//&& Dijkstra::nodes[waveFront.top()].distance < Dijkstra::nodes[destID].distance ) { // If you just do waveFront.size()!= 0 it just works
        
        waveElem waveElemm = waveFront.top(); // Intersection ID with lowest travel times
        waveFront.pop(); // Remove the element from the waveFront
        
        //std::vector<unsigned> &edge = Dijkstra::edges[Dijkstra::nodes[minimalIntID].edgeIDs[i]];
        unsigned waveElemID = waveElemm.intID;
        if (waveElemm.travelTime < Dijkstra::nodes[waveElemID].distance) {  //&& waveElemm.travelTime < bestPathLen) {
            Dijkstra::nodes[waveElemID].distance = waveElemm.travelTime;
            Dijkstra::nodes[waveElemID].previousEdge = waveElemm.edgeID;
            
            if (waveElemID == destID) {
                bestPathLen = waveElemm.travelTime;
            }
            
            for (unsigned i = 0;i<Dijkstra::nodes[waveElemID].edgeIDs.size();i++) {
                //std::clock_t start;
                //start = std::clock();

                //draw_street_segment(Dijkstra::nodes[waveElemm.intID].edgeIDs[i]);
                //duration += ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
                 // Finds the neighbour of the current node (minimalIntID) along the edge
                unsigned neighbourNode = findNeighbour(waveElemID,Dijkstra::edges[Dijkstra::nodes[waveElemID].edgeIDs[i]]);
                unsigned currentEdge = Dijkstra::nodes[waveElemID].edgeIDs[i];
                double &weight = Dijkstra::weights[currentEdge];
                double alternativeDistance = Dijkstra::nodes[waveElemID].distance + length(waveElemID,Dijkstra::edges[currentEdge],weight);// Potentially better distance to get to neighbour node from current node via edge
                if (getStreetSegmentInfo(currentEdge).to != waveElemID || !getStreetSegmentInfo(currentEdge).oneWay) { // Account for one way streets
                    waveFront.push(waveElem(neighbourNode,currentEdge,alternativeDistance)); // Reinsert back into the wavefront
                }
            }
        }  
    }   
}

void dijkstraPOI(unsigned sourceID,std::vector<unsigned> destIDs) {
    resetNodes(sourceID);
    double bestPathLen = std::numeric_limits<double>::max();
    
    for (unsigned i = 0;i<Dijkstra::nodes[sourceID].edgeIDs.size();i++) {
         // Finds the neighbour of the current node (minimalIntID) along the edge
        unsigned neighbourNode = findNeighbour(sourceID,Dijkstra::edges[Dijkstra::nodes[sourceID].edgeIDs[i]]);
        unsigned currentEdge = Dijkstra::nodes[sourceID].edgeIDs[i];
        double &weight = Dijkstra::weights[currentEdge];
        double alternativeDistance = Dijkstra::nodes[sourceID].distance + length(sourceID,Dijkstra::edges[currentEdge],weight);// Potentially better distance to get to neighbour node from current node via edge
        if (getStreetSegmentInfo(Dijkstra::nodes[sourceID].edgeIDs[i]).to != sourceID || !getStreetSegmentInfo(Dijkstra::nodes[sourceID].edgeIDs[i]).oneWay) {
            waveFront.push(waveElem(neighbourNode,currentEdge,alternativeDistance)); // Reinsert back into the wavefront
        }
    }
    int countForCalculation = 0;
    while (waveFront.size() != 0 && waveFront.top().travelTime < bestPathLen){//&& Dijkstra::nodes[waveFront.top()].distance < Dijkstra::nodes[destID].distance ) { // If you just do waveFront.size()!= 0 it just works
        
        waveElem waveElemm = waveFront.top(); // Intersection ID with lowest travel times
        waveFront.pop(); // Remove the element from the waveFront
        
        countForCalculation++;
        if (countForCalculation == 200) {
            // This calculation would be expensive to run on every iteration of
            // the while loop if you have a lot of POIs of a certain name so we'll
            // run it once every 200 iterations (200 was picked arbitrarily).
            for (unsigned i=0;i<destIDs.size();i++) {
                if (Dijkstra::nodes[destIDs[i]].distance < bestPathLen) {
                    bestPathLen = Dijkstra::nodes[destIDs[i]].distance;
                }
            }
            countForCalculation = 0;
        }
        if (waveElemm.travelTime < Dijkstra::nodes[waveElemm.intID].distance) {
            Dijkstra::nodes[waveElemm.intID].distance = waveElemm.travelTime;
            Dijkstra::nodes[waveElemm.intID].previousEdge = waveElemm.edgeID;
            
            
            for (unsigned i = 0;i<Dijkstra::nodes[waveElemm.intID].edgeIDs.size();i++) {
                 // Finds the neighbour of the current node (minimalIntID) along the edge
                unsigned neighbourNode = findNeighbour(waveElemm.intID,Dijkstra::edges[Dijkstra::nodes[waveElemm.intID].edgeIDs[i]]);
                unsigned currentEdge = Dijkstra::nodes[waveElemm.intID].edgeIDs[i];
                double &weight = Dijkstra::weights[currentEdge];
                double alternativeDistance = Dijkstra::nodes[waveElemm.intID].distance + length(waveElemm.intID,Dijkstra::edges[currentEdge],weight);// Potentially better distance to get to neighbour node from current node via edge
                if (getStreetSegmentInfo(Dijkstra::nodes[waveElemm.intID].edgeIDs[i]).to != waveElemm.intID || !getStreetSegmentInfo(Dijkstra::nodes[waveElemm.intID].edgeIDs[i]).oneWay) { // Account for one way streets
                    waveFront.push(waveElem(neighbourNode,currentEdge,alternativeDistance)); // Reinsert back into the wavefront
                }
            }
        }   
    }    
}

std::vector<unsigned> traceback(unsigned destID) {
    // Traceback the optimal path for any destinationID assuming that you ran dijkstra(sourceID) before this
    std::vector<unsigned> returnVector;
    unsigned currentNodeID = destID;
    while (Dijkstra::nodes[currentNodeID].previousEdge != NO_EDGE) {
        returnVector.push_back(Dijkstra::nodes[currentNodeID].previousEdge);
        currentNodeID = findNeighbour(currentNodeID,Dijkstra::edges[Dijkstra::nodes[currentNodeID].previousEdge]);
    }
    std::reverse(returnVector.begin(),returnVector.end());
    return returnVector;
}

void initializeNodes(std::vector<std::vector<unsigned> > streetSegsByIntersectionID) {
    // Clear if data is left over for some reason
    Dijkstra::nodes.clear();
    Dijkstra::edges.clear();
    Dijkstra::weights.clear();
    
    // Initialize Nodes here (intersections)
    for (unsigned i = 0;i < streetSegsByIntersectionID.size();i++) {
        Dijkstra::nodes.push_back(streetSegsByIntersectionID[i]);
    }
    
    // Initialize edges here (street segments)
    for (unsigned i = 0;i<getNumberOfStreetSegments();i++) {
        std::vector<unsigned> streetSeg;
        streetSeg.push_back(getStreetSegmentInfo(i).from); // FROM
        streetSeg.push_back(getStreetSegmentInfo(i).to); // TO
        streetSeg.push_back(getStreetSegmentInfo(i).streetID); // STREETID
        Dijkstra::edges.push_back(streetSeg);
        Dijkstra::weights.push_back(compute_path_travel_time({i}));
    }
}