/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   draw_feature.h
 * Author: woloszyn
 *
 * Created on February 21, 2016, 9:36 PM
 */
#pragma once

#ifndef DRAW_FEATURE_H
#define DRAW_FEATURE_H

void draw_river(unsigned featureID,double scalingFactor);
void draw_stream(unsigned featureID,double scalingFactor);
void draw_lake(unsigned featureID,double scalingFactor);
void draw_park(unsigned featureID,double scalingFactor);
void draw_beach(unsigned featureID,double scalingFactor);
void draw_shoreline(unsigned featureID,double scalingFactor);
void draw_greenspace(unsigned featureID,double scalingFactor);
void draw_golfcourse(unsigned featureID,double scalingFactor);
void draw_island(unsigned featureID,double scalingFactor);
void draw_building(unsigned featureID,double scalingFactor);
void draw_slanted_text(std::string text,double startX,double startY,double endX,double endY);

#endif /* DRAW_FEATURE_H */

