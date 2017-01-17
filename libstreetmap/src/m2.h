/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m2.h
 * Author: woloszyn
 *
 * Created on February 9, 2016, 10:15 AM
 */
#pragma once

#include "m1.h"
#include "easygl/graphics.h"

#ifndef M2_H
#define M2_H


void initialize_street__highway_types(); // Initialize data structure to lookup information about street types.
void initialize_building_types(); // Initialize data structure to lookup information about building types.
void init_features(); // Initialize data structure to lookup information about feature types.
void clear_map_data(); // Clear any map data we stored after we're done looking at the current map.
void draw_streets(); // Draw all the streets on the map. This accounts for the appropriate level of detail.
void draw_street_segment(unsigned streetID); // Draw an individual street segment
void draw_buttons(); // Draw the custom GUI buttons.
void draw_scale(); // Draw the dynamic scale to measure distances on screen.
void draw_searchbox(bool); // Draw the search box.

void draw_street_names(); // Draw streetnames.
void draw_info_button(bool findClosestIntersectionMode); // Draw info button as white if we aren't looking for the closest intersection, green if we are.
void draw_features(); // Draw all map features
void draw_info_highlight(); // Highlights the found intersection for info
void draw_info_highlight(unsigned intersectionID); // Sets the intersectionID to highlight and highlights it.
void draw_found_intersections(); // Draws the found intersections for two streets in search bar or using "Find" button.
void draw_found_intersections(std::vector<unsigned> findResult); // Sets the found intersections and draws them
void draw_quit(); // Draw quit button.
void draw_map(); // Draw the entire map.
void act_on_mousebutton(float x, float y, t_event_buttonPressed button_info); // Handle a mouse button event
void act_on_keypress(char c, int keysym); // Handle a keypress event
void drawscreen(); // Calls all the smaller draw functions
void intersection_highlight(float x,float y); // Gets a mouseclick position and highlights nearest intersection and displays info
void find(); // Draws the intersections common to two streets
void draw_error_message(std::string);
void draw_points_of_interest(); // Draw points of interest
void zoom_fit(void (*drawscreen), float, float); //Zoom fit for intersection


//MILESTONE 3
void draw_guide_button();
void draw_clear_button();
void draw_direction_button();
void draw_path_button(bool);
void guide_textbox();
void click_two_intersections(float x, float y);
bool giveDirectionsButtonPressed();
bool giveDirectionsErrorMessage();
void setDirectionsErrorMessage(bool x);
std::string partialStreetComplete(std::string filterString);
std::string partialPOIComplete(std::string filterString);



void draw_intersection_one_highlight();
void draw_intersection_one_highlight(unsigned intersectionID);


void draw_intersection_two_highlight();
void draw_intersection_two_highlight(unsigned intersectionID);



#endif /* M2_H */

