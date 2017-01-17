#include "Dijkstra.h"
#include "m4.h"
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

std::vector<unsigned> traveling_courier(const std::vector<DeliveryInfo>& deliveries, const std::vector<unsigned>& depots) {
    std::cout << "Starting the traveling courier function" << std::endl;
    std::vector<unsigned> grandPath;
    for (unsigned i = 0;i<depots.size() && grandPath.size() == 0;i++) {
        dijkstra(depots[i],deliveries[0].pickUp); // First find the path from just the random first depot to the first pickup locations
        grandPath = traceback(deliveries[0].pickUp);
    }
    for (unsigned i = 1;i < deliveries.size();i++) { // Go to all the pickup locations first
        std::cout << "Going from " << i-1 << "th pickup location to " << i << "th pickup location." << std::endl;
        dijkstra(deliveries[i-1].pickUp,deliveries[i].pickUp); // Find a path from 0 pickup -> 1 pickup, 1 pickup -> 2 pickup, ...
        std::vector<unsigned> intermediatePath = traceback(deliveries[i].pickUp); // Store that result in a vector
        grandPath.insert(grandPath.end(), intermediatePath.begin(), intermediatePath.end()); // Add the path in-between the two pickup locations to grandPath
    }
    std::cout << "Going from final pickup location to the first dropOff location." << std::endl;
    // This is for finding the path between the last pickUp to the first dropOff
    dijkstra(deliveries[deliveries.size()-1].pickUp,deliveries[0].dropOff); // Find a path from 0 pickup -> 1 pickup, 1 pickup -> 2 pickup, ...
    std::vector<unsigned> intermediatePath = traceback(deliveries[0].dropOff);
    grandPath.insert(grandPath.end(), intermediatePath.begin(), intermediatePath.end()); // Add the path in-between the two last pickup location and the first dropoff
    for (unsigned i = 1;i < deliveries.size();i++) { // Go to all the dropOff locations now
        std::cout << "Going from " << i-1 << "th dropOff location to " << i << "th dropOff location." << std::endl;
        dijkstra(deliveries[i-1].dropOff,deliveries[i].dropOff); // Find a path from 0 dropOff -> 1 dropOff, 1 dropOff -> 2 dropOff, ...
        std::vector<unsigned> intermediatePath = traceback(deliveries[i].dropOff); // Store that result in a vector
        grandPath.insert(grandPath.end(), intermediatePath.begin(), intermediatePath.end()); // Add the path in-between the two dropoff locations to grandPath
    }
    std::cout << "Going from last dropOff location to 0th depot pickup location." << std::endl;
    // This is for finding the path between the last dropOff back to the first depot
    intermediatePath.clear();
    for (unsigned i = 0;i < depots.size() && intermediatePath.size() == 0;i++) {
        dijkstra(deliveries[deliveries.size()-1].dropOff,depots[i]); // Find a path from 0 pickup -> 1 pickup, 1 pickup -> 2 pickup, ...
        intermediatePath = traceback(depots[i]);
    }
    grandPath.insert(grandPath.end(), intermediatePath.begin(), intermediatePath.end()); // Add the path in-between the last dropoff location to the first depot
    std::cout << "Done" << std::endl;
    return grandPath;
}