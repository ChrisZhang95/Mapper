#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "StreetData.h"
#include "draw_feature.h"
#include "LatLon.h"
#include "OSMDatabaseAPI.h"      
#include <iostream>
#include <list>
#include "StreetsDatabaseAPI.h"
#include "Dijkstra.h"
#include <sstream> 
#include <vector>
#include <math.h>
//#include "graphics.h" // just added this to make the arrows rsponsive onn screen

#define PI 3.14159265
#define DETAIL_LEVEL_1 13
#define DETAIL_LEVEL_2 22
#define DETAIL_LEVEL_3 36
#define DETAIL_LEVEL_4 60
#define LON_TO_DIS 45.015 //longtitude to actual distance factor


// Constant for conversion from km/h to m/s
#define KPH_TO_MPS  (5/((double)(18)))



class waveElem {
public:
    IntersectionNode node;
    unsigned streetSeg;

    waveElem(IntersectionNode n, unsigned seg) {
        node = n;
        streetSeg = seg;
    }
};






bool bfsPath(IntersectionNode &sourceNode, unsigned dest) {
    std::list<waveElem> wavefront; // BFS is like a wave that contains wave elements
    wavefront.push_back(waveElem(sourceNode, NO_EDGE)); // Initialize the wave front with initial data

    setcolor(RED); // Draw street segments you're examining through BFS as red

    while (wavefront.size() != 0) {
        unsigned currentIntersection = wavefront.front().node.intersectionID;
        unsigned reachingEdge = wavefront.front().streetSeg;
        unsigned prevIntersection;
        double newTravelTime;
        wavefront.pop_front();
        StreetData::getObjectPtr()->intersectionNodeByIntersection[currentIntersection].reachingEdge = reachingEdge;
        StreetData::getObjectPtr()->intersectionNodeByIntersection[currentIntersection].visited = true; // Mark node as visited when reached
        if ((int) reachingEdge != NO_EDGE) {
            if (currentIntersection == getStreetSegmentInfo(reachingEdge).from) {
                prevIntersection = getStreetSegmentInfo(reachingEdge).to;
            } else {
                prevIntersection = getStreetSegmentInfo(reachingEdge).from;
            }
            newTravelTime = StreetData::getObjectPtr()->intersectionNodeByIntersection[prevIntersection].travelTime + compute_path_travel_time({reachingEdge});
            if (getStreetName(getStreetSegmentInfo(prevIntersection).streetID) != getStreetName(getStreetSegmentInfo(currentIntersection).streetID)) {
                newTravelTime += 0.25;
            }
            if (StreetData::getObjectPtr()->intersectionNodeByIntersection[currentIntersection].visited && StreetData::getObjectPtr()->intersectionNodeByIntersection[currentIntersection].travelTime >= newTravelTime) {
                // Don't examine this element
                continue;
            } else {
                StreetData::getObjectPtr()->intersectionNodeByIntersection[currentIntersection].travelTime = newTravelTime;
            }
        }
        if (currentIntersection == dest) {
            return true;
        }

        for (unsigned i = 0; i < StreetData::getObjectPtr()->streetSegsByIntersectionID[currentIntersection].size(); i++) {
            StreetSegmentInfo streetSeg = getStreetSegmentInfo(StreetData::getObjectPtr()->streetSegsByIntersectionID[currentIntersection][i]);
            if (streetSeg.from == currentIntersection && StreetData::getObjectPtr()->intersectionNodeByIntersection[streetSeg.to].visited == false) {
                IntersectionNode& toNode = StreetData::getObjectPtr()->intersectionNodeByIntersection[streetSeg.to];
                wavefront.push_back(waveElem(toNode, StreetData::getObjectPtr()->streetSegsByIntersectionID[currentIntersection][i]));
                draw_street_segment(StreetData::getObjectPtr()->streetSegsByIntersectionID[currentIntersection][i]);
            } else if (streetSeg.oneWay == false && StreetData::getObjectPtr()->intersectionNodeByIntersection[streetSeg.from].visited == false) {
                IntersectionNode& toNode = StreetData::getObjectPtr()->intersectionNodeByIntersection[streetSeg.from];
                wavefront.push_back(waveElem(toNode, StreetData::getObjectPtr()->streetSegsByIntersectionID[currentIntersection][i]));
                draw_street_segment(StreetData::getObjectPtr()->streetSegsByIntersectionID[currentIntersection][i]);
            }
        }
    }
    return false;
}






std::list<unsigned> bfsTraceback(unsigned dest) {
    std::list<unsigned> path;
    IntersectionNode currentNode = StreetData::getObjectPtr()->intersectionNodeByIntersection[dest];
    int prevEdge = currentNode.reachingEdge;

    while (prevEdge != NO_EDGE) {
        path.push_front(prevEdge);
        StreetSegmentInfo streetSeg = getStreetSegmentInfo(prevEdge);
        if (currentNode.intersectionID == streetSeg.from) {
            currentNode = StreetData::getObjectPtr()->intersectionNodeByIntersection[streetSeg.to];
        } else {
            currentNode = StreetData::getObjectPtr()->intersectionNodeByIntersection[streetSeg.from];
        }
        prevEdge = currentNode.reachingEdge;
    }
    return path;
}






std::vector<unsigned> find_path_between_intersections(unsigned intersect_id_start, unsigned intersect_id_end) {

    std::cout << "Starting: " << intersect_id_start << "->" << intersect_id_end << std::endl;
    dijkstra(intersect_id_start, intersect_id_end);
    std::vector<unsigned> returnVector = traceback(intersect_id_end);;
    std::cout << "Ending: " << intersect_id_start << "->" << intersect_id_end << std::endl;
    return returnVector;
}








// Returns the time required to travel along the path specified. The path
// is passed in as a vector of street segment ids, and this function can 
// assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus 15 seconds per turn implied by the path. A turn occurs
// when two consecutive street segments have different street names.

double compute_path_travel_time(const std::vector<unsigned>& path) {
    double travelTime = 0;
    for (unsigned i = 0; i < path.size(); i++) {
        double distance = find_street_segment_distance(path[i]);
        travelTime += distance / (getStreetSegmentInfo(path[i]).speedLimit * KPH_TO_MPS);
        if (i != 0 && getStreetName(getStreetSegmentInfo(path[i]).streetID) != getStreetName(getStreetSegmentInfo(path[i - 1]).streetID)) { // Comparisons between street names might get really expensive, you might be able to improve this in the future.
            travelTime += 15; // Turn was made
        }
    }
    return travelTime / 60; // Travel time result is in minutes
}






double find_street_segment_distance(unsigned streetSeg) {
    // This entire function should only be run when the map is being loaded and then this function should just look up the travel time for a street seg
    LatLon from = getIntersectionPosition(getStreetSegmentInfo(streetSeg).from);
    LatLon to = getIntersectionPosition(getStreetSegmentInfo(streetSeg).to);
    if (getStreetSegmentInfo(streetSeg).curvePointCount == 0) {
        return find_distance_between_two_points(from, to);
    } else {
        double distance = find_distance_between_two_points(from, getStreetSegmentCurvePoint(streetSeg, 0));
        for (unsigned i = 1; i < getStreetSegmentInfo(streetSeg).curvePointCount; i++) {
            distance += find_distance_between_two_points(getStreetSegmentCurvePoint(streetSeg, i - 1), getStreetSegmentCurvePoint(streetSeg, i));
        }
        distance += find_distance_between_two_points(getStreetSegmentCurvePoint(streetSeg, getStreetSegmentInfo(streetSeg).curvePointCount - 1), to);
        return distance;
    }
}










// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// If no such path exists, returns an empty (size == 0) vector.
std::vector<unsigned> find_path_to_point_of_interest(unsigned intersect_id_start, std::string point_of_interest_name) {
    std::clock_t start;
    double duration;

    start = std::clock();
    std::vector<unsigned> destIDs;
    for (unsigned i = 0; i < StreetData::getObjectPtr()->POIByName[point_of_interest_name].size(); i++) {
        destIDs.push_back(find_closest_intersection(getPointOfInterestPosition(StreetData::getObjectPtr()->POIByName[point_of_interest_name][i])));
    }
    if (destIDs.size() == 1) {
        dijkstra(intersect_id_start, destIDs[0]);
    } else {
        dijkstraPOI(intersect_id_start, destIDs);
    }
    duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;

    std::cout << "Time taken: " << duration << std::endl;

    double minTravelTime = 99999999;
    int minID = -1;
    for (unsigned i = 0; i < StreetData::getObjectPtr()->POIByName[point_of_interest_name].size(); i++) {
        double travelTime = compute_path_travel_time(traceback(find_closest_intersection(getPointOfInterestPosition(StreetData::getObjectPtr()->POIByName[point_of_interest_name][i]))));
        if (travelTime < minTravelTime && travelTime != 0) {
            minTravelTime = travelTime;
            minID = i;
        }
    }
    std::cout << "Start Intersection: " << intersect_id_start << std::endl;
    std::cout << "Point of Interest Name: " << point_of_interest_name << std::endl;

    if (minID != -1) {
        std::vector<unsigned> result = traceback(find_closest_intersection(getPointOfInterestPosition(StreetData::getObjectPtr()->POIByName[point_of_interest_name][minID])));
        return result;
    } else {
        std::vector<unsigned> emptyResult;
        return emptyResult;
    }
}


//print out all partially correct street names
void print_matched_street_names(std::vector<std::string> partial_matched_street_names) {
    for (auto iter = partial_matched_street_names.begin();
            iter != partial_matched_street_names.end();
            iter++) {
        std::cout << *iter << std::endl;
    }
}






std::vector<LatLon> calculate_points_for_turn(unsigned streetSeg1, unsigned streetSeg2) {
    //getStreetSegmentCurvePoint
    // This function breaks up the problem of finding a direction instruction from the
    // complex problem considering from and to intersections for each street segment to
    // just considering the points involved in a turn.
    LatLon Pos1from = getIntersectionPosition(getStreetSegmentInfo(streetSeg1).from);
    LatLon Pos1next = getIntersectionPosition(getStreetSegmentInfo(streetSeg1).to);
    LatLon Pos2from = getIntersectionPosition(getStreetSegmentInfo(streetSeg2).from);
    LatLon Pos2next = getIntersectionPosition(getStreetSegmentInfo(streetSeg2).to);


    bool streetSeg1HasCurvePoints = getStreetSegmentInfo(streetSeg1).curvePointCount != 0;
    bool streetSeg2HasCurvePoints = getStreetSegmentInfo(streetSeg2).curvePointCount != 0;

    std::vector<LatLon> returnVector;
    // The turns will share either a common position for their "to" or "from"
    if(Pos1from == Pos2from){ // Shared turning intersection
        if(!streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints){
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos2next);
        } else if (!streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos1from);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, 0));
        } else if (streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, 0));
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos2next);
        } else if (streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, 0));
            returnVector.push_back(Pos1from);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, 0));
        }

    }
    else if(Pos1next == Pos2next){ // Shared turning intersection
        if(!streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints){
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos2from);
        } else if (!streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos1next);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, getStreetSegmentInfo(streetSeg2).curvePointCount - 1));
        } else if (streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, getStreetSegmentInfo(streetSeg1).curvePointCount - 1));
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos2from);
        } else if (streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, getStreetSegmentInfo(streetSeg1).curvePointCount - 1));
            returnVector.push_back(Pos1next);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, getStreetSegmentInfo(streetSeg2).curvePointCount - 1));
        }
    }
    else if(Pos1from == Pos2next){ // Shared turning intersection
        if(!streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints){
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos2from);
        } else if (!streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos1from);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, getStreetSegmentInfo(streetSeg2).curvePointCount - 1));
        } else if (streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, 0));
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos2from);
        } else if (streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, 0));
            returnVector.push_back(Pos1from);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, getStreetSegmentInfo(streetSeg2).curvePointCount - 1));
        }
    }
    else if(Pos1next == Pos2from){ // Shared turning intersection
        if(!streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints){
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos2next);
        } else if (!streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(Pos1from);
            returnVector.push_back(Pos1next);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, 0));
        } else if (streetSeg1HasCurvePoints && !streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, getStreetSegmentInfo(streetSeg1).curvePointCount - 1));
            returnVector.push_back(Pos1next);
            returnVector.push_back(Pos2next);
        } else if (streetSeg1HasCurvePoints && streetSeg2HasCurvePoints) {
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg1, getStreetSegmentInfo(streetSeg1).curvePointCount - 1));
            returnVector.push_back(Pos1next);
            returnVector.push_back(getStreetSegmentCurvePoint(streetSeg2, 0));
        }
    }
    return returnVector;
}








// DEBUGGING FUNCTIONS
void draw_circle_red(LatLon pos) {
    double lat_avg = (StreetData::getObjectPtr()->latIntersectionMin + StreetData::getObjectPtr()->latIntersectionMax) / 2; // Get the average latitude.
    double scalingFactor = cos(lat_avg * DEG_TO_RAD); // Find the scaling factor.

    LatLon closestIntersectionPos = pos;
    setcolor(RED);

    fillarc(closestIntersectionPos.lon*scalingFactor, closestIntersectionPos.lat, 0.000013 * StreetData::getObjectPtr()->zoomLevel, 0, 360);
}






void draw_circle_yellow(LatLon pos) {
    double lat_avg = (StreetData::getObjectPtr()->latIntersectionMin + StreetData::getObjectPtr()->latIntersectionMax) / 2; // Get the average latitude.
    double scalingFactor = cos(lat_avg * DEG_TO_RAD); // Find the scaling factor.

    LatLon closestIntersectionPos = pos;
    setcolor(YELLOW);

    fillarc(closestIntersectionPos.lon*scalingFactor, closestIntersectionPos.lat, 0.000013 * StreetData::getObjectPtr()->zoomLevel, 0, 360);
}






void draw_circle_blue(LatLon pos) {
    double lat_avg = (StreetData::getObjectPtr()->latIntersectionMin + StreetData::getObjectPtr()->latIntersectionMax) / 2; // Get the average latitude.
    double scalingFactor = cos(lat_avg * DEG_TO_RAD); // Find the scaling factor.

    LatLon closestIntersectionPos = pos;
    setcolor(BLUE);

    fillarc(closestIntersectionPos.lon*scalingFactor, closestIntersectionPos.lat, 0.000013 * StreetData::getObjectPtr()->zoomLevel, 0, 360);
}







double calculate_turning_angle(std::vector<LatLon> pointsForTurn) {
    // Scale X's first so we can calculate a screen angle properly
    double lat_avg = (StreetData::getObjectPtr()->latIntersectionMin + StreetData::getObjectPtr()->latIntersectionMax) / 2; // Get the average latitude.
    double scalingFactor = cos(lat_avg * DEG_TO_RAD); // Find the scaling factor.
    pointsForTurn[0].lon = pointsForTurn[0].lon*scalingFactor;
    pointsForTurn[1].lon = pointsForTurn[1].lon*scalingFactor;
    pointsForTurn[2].lon = pointsForTurn[2].lon*scalingFactor;

    double angle1 = atan2(pointsForTurn[1].lat - pointsForTurn[0].lat, pointsForTurn[1].lon - pointsForTurn[0].lon);
    if (angle1 < 0) {
        angle1 = 2*PI + angle1; // Make sure you have an angle in the standard form
    }
    double angle2 = atan2(pointsForTurn[2].lat - pointsForTurn[1].lat, pointsForTurn[2].lon - pointsForTurn[1].lon);
    if (angle2 < 0) {
        angle2 = 2*PI + angle2; // Make sure you have an angle in the standard form
    }

    if (angle1 - angle2 < -PI) {
        return ((angle1-angle2)+2*PI);
    }
    else {
        return angle1 - angle2;
    }
}






//PURPOSELY STORED THE DIRECTIONS SINCE CLICKING ON THE MAP WILL CLOSE DIRECTIONS WHEN YOU ARE DRAGGING AROUND
//BUT WHEN CLICK DIRECTIONS AGAIN IT HAS SAVED YOUR LAST RESULT
std::vector<std::string> get_directions(std::vector<unsigned> streetSegments) {

    std::vector<std::string> returnVector;

    if (giveDirectionsButtonPressed() && !streetSegments.empty()) {

        std::string sentence = "Go straight on " + getStreetName(getStreetSegmentInfo(streetSegments[0]).streetID);
        while (sentence.size() != 0) {
            returnVector.push_back(sentence.substr(0, 70));
            if (sentence.size() >= 70) {
                sentence = sentence.substr(70, sentence.size() - 70);
            } else {
                sentence = "";
            }
        }


        for (unsigned i = 1; i < streetSegments.size(); i++) { // Must go from i=1 to i < streetSegments.size()

            std::vector<LatLon> pointsForTurn = calculate_points_for_turn(streetSegments[i - 1], streetSegments[i]);
            double turningAngle = calculate_turning_angle(pointsForTurn);
            double angle = turningAngle / DEG_TO_RAD;

            std::string street1Name = getStreetName(getStreetSegmentInfo(streetSegments[i - 1]).streetID);
            std::string street2Name = getStreetName(getStreetSegmentInfo(streetSegments[i]).streetID);

            if (street1Name != street2Name) {
                if (angle > -45 && angle <-10) {
                    sentence = "From " + street1Name + " make a slight left on " + street2Name;
                } else if (angle < 45 && angle > 10) {
                    sentence = "From " + street1Name + " make a slight right on " + street2Name;
                } else if (angle < 10 && angle > -10) {
                    sentence = "From " + street1Name + " go straight on " + street2Name;
                } else if (angle > 150 || angle < -150) {
                    sentence = "From " + street1Name + " make a U-turn on " + street2Name; // maybe same street...
                } else if (angle < -45 && angle > -150) {
                    sentence = "From " + street1Name + " make a sharp left on " + street2Name;
                } else if (angle > 45 && angle < 150) {
                    sentence = "From " + street1Name + " make a sharp right on " + street2Name;
                }
                while (sentence.size() != 0) {
                    returnVector.push_back(sentence.substr(0, 70));
                    if (sentence.size() >= 70) {
                        sentence = sentence.substr(70, sentence.size() - 70);
                    } else {
                        sentence = "";
                    }
                }
            }

        }
    }

    return returnVector;
}






void give_directions(std::vector<std::string> directions, unsigned pageNumber) {
    // For drawing directions we'll only display 6 directions per page

    settextrotation(0);
    setlinestyle(SOLID);
    //only if the button is pressed give directions
    if (giveDirectionsButtonPressed() == true) {
        // first check if there is any input to the vector
        if (directions.empty() == true) {
            if (giveDirectionsErrorMessage() == true)
                no_directions();

        }            //if there are valid ids give the directions!! 
        else {
            setDirectionsErrorMessage(false);
            int spacing = 25; // need to rest this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! when reach 6 so that new page starts writing at the top

            //size of the vertical direction bar
            double sinfo_xleft = obtainTranscoord().top_width / 20 - 70;
            double sinfo_xright = obtainTranscoord().top_width / 20 + 350;
            double sinfo_ytop = obtainTranscoord().top_height / 4 - 250;
            double sinfo_ybot = obtainTranscoord().top_height / 4 - 100;
            setcolor(WHITE);
            fillrect(xscrn_to_world(sinfo_xleft), yscrn_to_world(sinfo_ytop), xscrn_to_world(sinfo_xright), yscrn_to_world(sinfo_ybot));

            draw_arrows(); // draw the arrows if valid input

            // Print first direction 
            setcolor(BLACK);
            setfontsize(8);
            setlinewidth(3);
            for (unsigned i = 0; i < 6; i++) { // Must go from i=1 to i < streetSegments.size()
                if (6 * pageNumber + i < directions.size()) {
                    drawtext(xscrn_to_world(sinfo_xleft + 210), yscrn_to_world(sinfo_ytop + spacing), directions[6 * pageNumber + i], 1, 1);
                }
                spacing = spacing + 20;
            }
            setcolor(LIGHTGREY);
            std::string pageString = "Page : " + std::to_string(pageNumber + 1) + "/" + std::to_string((unsigned) ceil((double) directions.size() / (double) 6));
            drawtext(xscrn_to_world(sinfo_xleft + 380), yscrn_to_world(sinfo_ytop + 145), pageString, 1, 1);
        }
    }
}







void draw_arrows() {

    settextrotation(0);
    setfontsize(20);
    setlinestyle(SOLID);
    setlinewidth(25);



    // left button
    double sinfo_xleft = obtainTranscoord().top_width / 20 - 10;
    double sinfo_xright = obtainTranscoord().top_width / 20 + 30;
    double sinfo_ytop = obtainTranscoord().top_height / 4 - 85;
    double sinfo_ybot = obtainTranscoord().top_height / 4 - 45;
    setcolor(WHITE);
    fillrect(xscrn_to_world(sinfo_xleft), yscrn_to_world(sinfo_ytop), xscrn_to_world(sinfo_xright), yscrn_to_world(sinfo_ybot));
    setcolor(BLACK);
    drawtext(xscrn_to_world(sinfo_xleft + 20), yscrn_to_world(sinfo_ytop + 20), "<", 1, 1);


    //// right button
    setcolor(WHITE);
    fillrect(xscrn_to_world(sinfo_xleft + 250), yscrn_to_world(sinfo_ytop), xscrn_to_world(sinfo_xright + 250), yscrn_to_world(sinfo_ybot));
    setcolor(BLACK);
    drawtext(xscrn_to_world(sinfo_xleft + 270), yscrn_to_world(sinfo_ytop + 20), ">", 1, 1);
}







void no_directions() {

    settextrotation(0);
    setfontsize(14);
    setlinestyle(SOLID);
    setlinewidth(3);
    setcolor(WHITE);

    //size of error message
    double sinfo_xleft = obtainTranscoord().top_width / 2 - 170;
    double sinfo_ytop = obtainTranscoord().top_height / 10 + 25;
    double sinfo_xright = obtainTranscoord().top_width / 2 + 170;
    double sinfo_ybot = obtainTranscoord().top_height / 10 + 70;
    fillrect(xscrn_to_world(sinfo_xleft), yscrn_to_world(sinfo_ytop), xscrn_to_world(sinfo_xright), yscrn_to_world(sinfo_ybot));
    setcolor(BLACK);

    drawtext(xscrn_to_world(obtainTranscoord().top_width / 2), yscrn_to_world(sinfo_ytop + 25), "No path given!!", 1, 1);
}
