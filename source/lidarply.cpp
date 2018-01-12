// lidarply.cpp - Derived class of PLY to make creating PLY files
//                programmatically a bit easier
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

#include "lidarply.hpp"
#include "plylib.hpp"
#include "ply_element_sep.hpp"
#include "ply_element_list.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdexcept>

using namespace std;

// ====================================================================
// Constructor

lidarply::lidarply( void )
{
  // Set up header
  format = "ascii";
  version = "1.0";
  comments.push_back( "comment created using \"basicply\" library");

  struct returnResult r;
  plyElement* newElement;
  // Create empty vertex element with fixed properties
  newElement = new plyElementSep( "vertex" );
  if( newElement == NULL )
  {
    throw logic_error( "Failed to create vertex element" );
  }
  newElement->setCount( 0 );
  // Add the properties
  // x,y,z coordinates
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "x", "float" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "y", "float" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "z", "float" );
  // Colours
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "red", "uchar" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "green", "uchar" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "blue", "uchar" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  // Vertex normals.
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "nx", "float" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "ny", "float" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to add property: Reason: " + r.reason );
  }
  r = dynamic_cast<plyElementSep *>( newElement )->addProperty( "nz", "float" );
  elements.push_back( newElement );
  vertexElementIndex = 0;

  // Create empty face element
  newElement = new plyElementList( "face" );
  if( newElement == NULL )
  {
    throw logic_error( "Failed to create face element" );
  }
  newElement->setCount( 0 );
  // Set up the properties
  r = dynamic_cast<plyElementList *>( newElement )->setProperty( "int", "vertex_index", "int" );
  if( r.result == false )
  {
    throw invalid_argument( "Failed to set up list element Reason: " + r.reason );
  }
  elements.push_back( newElement );
  faceElementIndex = 1;
}

// ====================================================================
// Data manipulation

unsigned int lidarply::addVertex( float x, float y, float z, unsigned int red, unsigned int green, unsigned int blue )
{
  // Build up list of data items
  // Ascii conversion makes the conversion independent of local machine endianism
  stringstream buffer;
  vector<UINT64> data;
  // x,y,z coordinates
  buffer << x;
  data.push_back( packAscii( buffer.str(), "float" ) );
  buffer.str("");
  buffer << y;
  data.push_back( packAscii( buffer.str(), "float" ) );
  buffer.str("");
  buffer << z;
  data.push_back( packAscii( buffer.str(), "float" ) );
  buffer.str("");
  // Colours
  buffer << red;
  data.push_back( packAscii( buffer.str(), "uchar" ) );
  buffer.str("");
  buffer << green;
  data.push_back( packAscii( buffer.str(), "uchar" ) );
  buffer.str("");
  buffer << blue;
  data.push_back( packAscii( buffer.str(), "uchar" ) );
  buffer.str("");
  // Vertex normals, note that these are set to point upwards
  data.push_back( packAscii( "0.0", "float" ) ); // x
  data.push_back( packAscii( "0.0", "float" ) ); // y
  data.push_back( packAscii( "1.0", "float" ) ); // z

  // And store the data
  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );
  return vertElement->addVertex( data );

}
