// ply_element.cpp - base class for handling PLY elements
// Copyright (C) 2018 John Davies
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "ply_element.hpp"
#include "util.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>


using namespace std;

// Constructor
plyElement::plyElement( const string elementName )
{
  name = elementName;
}

string plyElement::getName( void )
{
  return name;
}

void plyElement::setCount( const int value )
{
  count = value;
}

int plyElement::getCount( void )
{
  return count;
}
