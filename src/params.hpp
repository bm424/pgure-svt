/***************************************************************************

    Copyright (C) 2015-16 Tom Furnival

    This file is part of PGURE-SVT.

    PGURE-SVT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PGURE-SVT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PGURE-SVT. If not, see <http://www.gnu.org/licenses/>.

***************************************************************************/

#ifndef PARAMS_H
#define PARAMS_H

#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

void ParseParameters(std::istream &cfgfile, 
                     std::map<std::string,
                     std::string>& options) {
    for (std::string line; std::getline(cfgfile, line);) {
        std::istringstream iss(line);
        std::string id, eq, val, temp;

        if (!(iss >> id)) {
            continue;    // Ignore empty lines
        } else if (id[0] == '#') {
            continue;    // Ignore comment lines
        } else if (!(iss >> eq ) || eq != ":" || iss.get() != EOF) {
            while( iss >> temp ) {
                if( iss >> std::ws) {
                    val += temp;
                }
                else {
                    val += temp + " ";
                }
            }
        }
        // Set the parameter
        options[id] = val;
    }
    return;
}

#endif
