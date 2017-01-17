#include <iostream>
#include <cfloat> // for FLT_MAX
#include <chrono>
#include <thread>
#include <cstdlib>
#include <vector>
#include <thread>
#include <string>
#include "m1.h"
#include "m2.h"
#include "m3.h"
#include "easygl/graphics.h"
#include "StreetData.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#include "Dijkstra.h"

int main() {
    // Load Toronto immediately
    load_map("/cad2/ece297s/public/maps/toronto.streets.bin");
    StreetData::getObjectPtr()->loadPressed = false;
    draw_map();
    bool loadPressed = StreetData::getObjectPtr()->loadPressed; // Save loadPressed before we can't access StreetData anymore
    close_map();
    
    // If the top right quit button is pressed the program will "proceed" out of the even loop.
    // If loadPressed wasn't set to true we end the program.
    // This loop exists so that maps can be loaded endlessly until the program is ended.
    while (loadPressed) {
        const char* fileNameFilter[1];
        fileNameFilter[0] = "*.streets.bin";
        const char * fileName = tinyfd_openFileDialog ( "Choose a map file" , "/cad2/ece297s/public/maps/" , 1 , fileNameFilter , "Map file" , 0 ) ;
        load_map(fileName);
        StreetData::getObjectPtr()->loadPressed = false;
        draw_map();
        loadPressed = StreetData::getObjectPtr()->loadPressed; // Save loadPressed before we can't access StreetData anymore
        close_map();
    }
}
