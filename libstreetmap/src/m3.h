#pragma once
#include <vector>
#include <string>
#include <list>
#include "IntersectionNode.h"

#ifndef M3_H
#define M3_H

// Returns a path (route) between the start intersection and the end 
// intersection, if one exists. If no path exists, this routine returns 
// an empty (size == 0) vector. If more than one path exists, the path 
// with the shortest travel time is returned. The path is returned as a vector 
// of street segment ids; traversing these street segments, in the given order,
// would take one from the start to the end intersection.
std::vector<unsigned> find_path_between_intersections(unsigned 
                   intersect_id_start, unsigned intersect_id_end);


// Returns the time required to travel along the path specified. The path
// is passed in as a vector of street segment ids, and this function can 
// assume the vector either forms a legal path or has size == 0.
// The travel time is the sum of the length/speed-limit of each street 
// segment, plus 15 seconds per turn implied by the path. A turn occurs
// when two consecutive street segments have different street names.
double compute_path_travel_time(const std::vector<unsigned>& path);


// Returns the shortest travel time path (vector of street segments) from 
// the start intersection to a point of interest with the specified name.
// If no such path exists, returns an empty (size == 0) vector.
std::vector<unsigned> find_path_to_point_of_interest (unsigned 
                   intersect_id_start, std::string point_of_interest_name);

double find_street_segment_distance(unsigned streetSeg);


bool bfsPath(IntersectionNode &sourceNode,unsigned dest);
std::list<unsigned> bfsTraceback(unsigned dest);



std::vector<std::string> find_partial_matched_street_names (std::string userinput);
std::string find_partial_matched_street_name (std::string userinput);
void print_matched_street_names (std::vector<std::string>);




 std::vector<std::string> get_directions(std::vector<unsigned>);
 void give_directions(std::vector<std::string>,unsigned);
 void no_directions();
 void draw_arrows();

 
 #endif /* M3_H */
