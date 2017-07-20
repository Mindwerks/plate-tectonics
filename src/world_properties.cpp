/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   world_properties.cpp
 * Author: ro-lip
 * 
 * Created on 19. Juli 2017, 16:37
 */

#include "world_properties.h"

world_properties::world_properties() {
}

world_properties& world_properties::instance() {
   static world_properties instance;
   return instance;
}
