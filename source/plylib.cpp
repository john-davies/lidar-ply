// plylib.cpp - code file for plylib
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
// File IO

struct returnResult ply::readFromFile( const string fileName )
{
  struct returnResult res = { true, "" };

  // Open file for reading. Has to be binary mode because we don't
  // know the format of the PLY file at this stage
  ifstream inputFile;
  string inputLine;
  inputFile.exceptions( ifstream::failbit | ifstream::badbit | ifstream::eofbit );
  try
  {
    inputFile.open( fileName.c_str(), ios::in | ios::binary );
    // First line should be "ply"
    getline ( inputFile, inputLine );
    if( inputLine != PLY )
    {
      throw invalid_argument( "First line should be \"ply\"" );
    }
    // Then the format line, e.g. format ascii 1.0
    getline ( inputFile, inputLine );
    vector<string> params;
    params = split( inputLine, ' ' );
    if( params.at(0) != FORMAT )
    {
      throw invalid_argument( "Second line should start with \"format\"" );
    }
    // Check for an acceptable format
    if ( find( formatOptions.begin(), formatOptions.end(), params.at(1)) == formatOptions.end() )
    {
      throw invalid_argument( "Invalid format specification" );
    }
    format = params.at(1);
    version = params.at(2);

    // Now look for "comment" and "element" lines
    // Stop when "end_header" is found
    string element_name = "";
    int element_count = 0;
    plyElement* newElement;
    bool needElementCreating = false;
    bool isList = false;
    getline ( inputFile, inputLine );
    while( inputLine != END_HEADER )
    {
      vector<string> params = split( inputLine, ' ' );
      if( params.at(0) == COMMENT )
      {
        // Add on to the comments list
        comments.push_back( inputLine );
      }
      else if( params.at(0) == ELEMENT )
      {
        // Set up the data to create a new element. Type
        // of element will depend on the next line in the file
        element_name = params.at(1);
        element_count = stoi( params.at(2), nullptr );
        needElementCreating = true;
      }
      else
      {
        // Properties of an element.
        // If this is the first time through for this element
        // then create the element using the data stored above.
        if( element_name == "" )
        {
          // No element data set up so something funny has happened
          throw invalid_argument( "Property definition before an element definition" );
        }
        if( needElementCreating == true )
        {
          // Need to parse the property line here to decide what sort of element
          vector<string> params = split( inputLine, ' ' );
          if( params.at(1) == LIST )
          {
            // List of property definitions
            newElement = new plyElementList( element_name );
            if( newElement == NULL )
            {
              throw logic_error( "Failed to create a new list element: " + element_name );
            }
            newElement->setCount( element_count );
            elements.push_back( newElement );
            // Check if this is a quick access element
            if( element_name == "vertex" )
            {
              vertexElementIndex = elements.size()-1;
            }
            else if( element_name == "face" )
            {
              faceElementIndex = elements.size()-1;
            }
            needElementCreating = false;
            isList = true;
          }
          else
          {
            // Separate property definitions
            newElement = new plyElementSep( element_name );
            if( newElement == NULL )
            {
              throw logic_error( "Failed to create a new element: " + element_name );
            }
            newElement->setCount( element_count );
            elements.push_back( newElement );
            // Check if this is a quick access element
            if( element_name == "vertex" )
            {
              vertexElementIndex = elements.size()-1;
            }
            else if( element_name == "face" )
            {
              faceElementIndex = elements.size()-1;
            }
            needElementCreating = false;
            isList = false;
          }
        }
        // Now add the property depending on the element type
        if( isList )
        {
          // List element
          // Check that type definition of both the list and variables is acceptable
          if( ( find(typeOptions.begin(), typeOptions.end(), params.at(2)) != typeOptions.end() ) &&
              ( find(typeOptions.begin(), typeOptions.end(), params.at(3)) != typeOptions.end() ) )
          {
            // Add the property to the element
            struct returnResult r = dynamic_cast<plyElementList *>( newElement )->
                                          setProperty( params.at(2), params.at(4), params.at(3) );
            if( r.result == false )
            {
              throw invalid_argument( "Failed to add property: " + params.at(2)
                                        + " Reason: " + r.reason );
            }
          }
          else
          {
            // Unknown type definition
            throw invalid_argument( "Unknown type defintion: " + params.at(1) );
          }
        }
        else
        {
          // Separate element
          // Check that type definition is acceptable
          if( find(typeOptions.begin(), typeOptions.end(), params.at(1)) != typeOptions.end() )
          {
            // Add the property to the element
            struct returnResult r = dynamic_cast<plyElementSep *>( newElement )->
                                          addProperty( params.at(2), params.at(1) );
            if( r.result == false )
            {
              throw invalid_argument( "Failed to add property: " + params.at(2)
                                        + " Reason: " + r.reason );
            }
          }
          else
          {
            // Unknown type definition
            throw invalid_argument( "Unknown type defintion: " + params.at(1) );
          }
        }
      }
      // And get the next line
      getline ( inputFile, inputLine );
    }

    // Now read the data
    for( auto & elem : elements )
    {
      struct returnResult r = elem->importDataFromFile( inputFile, format );
      if( r.result == false )
      {
        throw invalid_argument( "Error reading data for: " + elem->getName() + "\n"
                                  + r.reason );
      }
    }
  }
  catch (const ifstream::failure& e)
  {
    // File opening failures
    res.result = false;
    res.reason = "Error processing file: " + fileName + "\n" + e.what();
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = "Error parsing line: " + inputLine + "\n" + e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "Error parsing line: " + inputLine;
  }

  return res;
}

// --------------------------------------------------------------------

struct returnResult ply::writeToFile( const string fileName )
{
  struct returnResult res = { true, "" };

  // Open file for reading. Has to be binary mode because we don't
  // know the format of the PLY file at this stage
  ofstream outputFile;
  outputFile.exceptions( ofstream::failbit | ofstream::badbit );
  try
  {
    outputFile.open( fileName.c_str(), ios::out | ios::binary );
    // Write the header
    outputFile << printHeader();

    // Then the data
    for ( auto e : elements )
    {
      plyElementList *le = dynamic_cast<plyElementList *>( e );
      plyElementSep *se = dynamic_cast<plyElementSep *>( e );
      if( le )
      {
        // Check if there's any data available
        if( le->size() > 0 )
        {
          le->writeDataToFile( outputFile, format );
        }
      }
      else if( se )
      {
        // Check if there's any data available
        if( se->size() > 0 )
        {
          se->writeDataToFile( outputFile, format );
        }
      }
    }

    // Close the file
    outputFile.close();
  }
  catch (const ofstream::failure& e)
  {
    // File opening failures
    res.result = false;
    res.reason = "Error processing file: " + fileName + "\n" + e.what();
  }
  /*catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = "Error parsing line: " + inputLine + "\n" + e.what();
  }*/
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "Error writing file";
  }

  return res;
}

// ====================================================================
// Data manipulation

string ply::getFormat( void )
{
  return format;
}

struct returnResult ply::setFormat( string newFormat )
{
  struct returnResult res = { true, "" };

  // Check that the new format is valid
  if( find( formatOptions.begin(), formatOptions.end(), newFormat ) != formatOptions.end() )
  {
    format = newFormat;
  }
  else
  {
    res.result = false;
    res.reason = "Invalid format";
  }

  return res;
}

// --------------------------------------------------------------------

struct returnResult  ply::scaleModel( string xScale, string yScale, string zScale )
{
  struct returnResult res = { true, "" };

  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );
  int size = vertElement->size();

  // Loop through getting the x,y,z coordinates of each vertex and
  // multiply accordingly
  try
  {
    // Check the rest of the data
    for( int i=0; i<size; i++ )
    {
      vertElement->setData( i, "x", to_string( stod( xScale ) * stod( vertElement->getData( i, "x" ) ) ) );
      vertElement->setData( i, "y", to_string( stod( yScale ) * stod( vertElement->getData( i, "y" ) ) ) );
      vertElement->setData( i, "z", to_string( stod( zScale ) * stod( vertElement->getData( i, "z" ) ) ) );
    }
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "scaleModel - Unknown error";
  }

  return res;
}

// --------------------------------------------------------------------

struct returnResult ply::changeAllVertexColours( string red, string green, string blue )
{
  struct returnResult res = { true, "" };

  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );

  // Check if the colour entries exist
  try
  {
    vertElement->getData( 0, "red" );
  }
  catch( const std::invalid_argument& e )
  {
    vertElement->addProperty( "red", "uchar" );
  }
  try
  {
    vertElement->getData( 0, "green" );
  }
  catch( const std::invalid_argument& e )
  {
    vertElement->addProperty( "green", "uchar" );
  }
  try
  {
    vertElement->getData( 0, "blue" );
  }
  catch( const std::invalid_argument& e )
  {
    vertElement->addProperty( "blue", "uchar" );
  }

  // Loop through setting the colours
  try
  {
    for( unsigned int i=0; i<vertElement->size(); i++ )
    {
      vertElement->setData( i, "red", red );
      vertElement->setData( i, "green", green );
      vertElement->setData( i, "blue", blue );
    }
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "changeAllVertexColours - Unknown error";
  }

  return res;
}

// --------------------------------------------------------------------

struct returnResult ply::changeVertexColours( unsigned int vertex, string red, string green, string blue )
{
  struct returnResult res = { true, "" };
  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );

  try
  {
    vertElement->setData( vertex, "red", red );
    vertElement->setData( vertex, "green", green );
    vertElement->setData( vertex, "blue", blue );
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "changeVertexColours - Unknown error";
  }

  return res;

}

// --------------------------------------------------------------------

struct returnResult ply::dupVertex( unsigned int index, unsigned int& newIndex )
{
  struct returnResult res = { true, "" };
  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );

  // Create a new vertex based on the data of an existing one. This
  // action would normally be followed by an editVertex command. It's
  // done this way to ensure that all properties are set correctly

  try
  {
    newIndex = vertElement->dupVertex( index );
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "dupvertex - Unknown error";
  }

  return res;
}

// --------------------------------------------------------------------

struct returnResult ply::changeVertexCoords( unsigned int vertex, string x, string y, string z )
{
  struct returnResult res = { true, "" };
  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );

  // Create a new vertex based on the data of an existing one. This
  // action would normally be followed by an editVertex command. It's
  // done this way to ensure that all properties are set correctly

  try
  {
    vertElement->setData( vertex, "x", x );
    vertElement->setData( vertex, "y", y );
    vertElement->setData( vertex, "z", z );
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "changeVertexCoords - Unknown error";
  }

  return res;
}

// --------------------------------------------------------------------

struct returnResult ply::addFace( vector<string> faceData )
{
  struct returnResult res = { true, "" };
  plyElementList *faceElement = dynamic_cast<plyElementList *>( elements.at( faceElementIndex ) );

  try
  {
    faceElement->setData( faceData );
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "changeVertexCoords - Unknown error";
  }

  return res;
}


// ====================================================================
// Information

string ply::printHeader( void )
{
  std::stringstream buffer;

  buffer << PLY << endl;
  buffer << FORMAT << " " << format << " " << version << endl;
  for ( auto c : comments )
  {
        buffer << c << endl;
  }
  // First the header
  for ( auto e : elements )
  {
    plyElementList *le = dynamic_cast<plyElementList *>( e );
    plyElementSep *se = dynamic_cast<plyElementSep *>( e );
    if( le )
    {
      // Check if there's any data available
      if( le->size() > 0 )
      {
        buffer << le->getHeader();
      }
    }
    else if( se )
    {
      // Check if there's any data available
      if( se->size() > 0 )
      {
        buffer << se->getHeader();
      }
    }
  }
  buffer << END_HEADER << endl;

  return buffer.str();
}

// --------------------------------------------------------------------

unsigned int ply::getVertexCount( void )
{
  return elements.at( vertexElementIndex )->size();
}

// --------------------------------------------------------------------

unsigned int ply::getFaceCount( void )
{
  return elements.at( faceElementIndex )->size();
}

// --------------------------------------------------------------------

vector<string> ply::getFaceData( unsigned int index )
{
  return dynamic_cast<plyElementList *>( elements.at( faceElementIndex ) )->getData( index );
}

// --------------------------------------------------------------------

struct returnResult ply::getBoundingBox( struct boundingBox& box )
{
  struct returnResult res = { true, "" };

  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );
  int size = vertElement->size();
  double x,y,z;

  // Loop through getting the x,y,z coordinates of each vertex and
  // set max/min accordingly
  try
  {
    // Initialise with the first data
    box.minX = stod( vertElement->getData( 0, "x" ) );
    box.maxX = box.minX;
    box.minY = stod( vertElement->getData( 0, "y" ) );
    box.maxY = box.minY;
    box.minZ = stod( vertElement->getData( 0, "z" ) );
    box.maxZ = box.minZ;

    // Check the rest of the data
    for( int i=1; i<size; i++ )
    {
      x = stod( vertElement->getData( i, "x" ) );
      y = stod( vertElement->getData( i, "y" ) );
      z = stod( vertElement->getData( i, "z" ) );

      // Update if necessary
      if( x > box.maxX )
      {
        box.maxX = x;
      }
      else if( x < box.minX )
      {
        box.minX = x;
      }
      if( y > box.maxY )
      {
        box.maxY = y;
      }
      else if( y < box.minY )
      {
        box.minY = y;
      }
      if( z > box.maxZ )
      {
        box.maxZ = z;
      }
      else if( z < box.minZ )
      {
        box.minZ = z;
      }
    }
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "Unknown error";
  }

  return res;
}

// --------------------------------------------------------------------

struct returnResult ply::getCoordinates( unsigned int index, struct vertexCoordinates& vertexCoord )
{
  struct returnResult res = { true, "" };
  plyElementSep *vertElement = dynamic_cast<plyElementSep *>( elements.at( vertexElementIndex ) );

  try
  {
    vertexCoord.x = stod( vertElement->getData( index, "x" ) );
    vertexCoord.y = stod( vertElement->getData( index, "y" ) );
    vertexCoord.z = stod( vertElement->getData( index, "z" ) );
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = e.what();
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "Unknown error";
  }

  return res;
}
