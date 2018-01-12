// ply_element_sep.cpp - derived class for handling PLY elements where the
//                       properties are defined on separate lines
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

#include "ply_element_sep.hpp"
#include "util.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>


using namespace std;

// ===========================================================================

struct returnResult plyElementSep::importDataFromFile( ifstream& inputFile, const string format )
{
  struct returnResult res = { true, "" };
  string inputLine;

  try
  {
    if( format == "ascii" )
    {
      for (int i = 0; i < count; i++)
      {
        // Read a line
        getline( inputFile, inputLine );
        // Split into data elements
        vector<string> params = split( inputLine, ' ' );
        vector<UINT64> data;
        // Check that the number of items match the property list
        if( params.size() != properties.size() )
        {
          throw invalid_argument( "Incorrect number of data parameters" );
        }
        // Convert each one in turn
        for( unsigned int j=0; j<params.size(); j++ )
        {
          data.push_back( packAscii( params.at(j), properties.at(j).type ) );
        }
        // And store the data
        elementData.push_back( data );
      }
    }
    else
    {
      for (int i = 0; i < count; i++)
      {
        vector<UINT64> data;
        vector<unsigned char> value;
        char dataByte;
        // Binary format so read correct number of bytes for each line of data
        for ( auto p : properties )
        {
          int b = getNumberOfBytes( p.type );
          // Read a byte at a time
          value.clear();
          for( int j=0; j<b; j++ )
          {
            inputFile.read( &dataByte, 1 );
            value.push_back( static_cast<unsigned char>( dataByte ) );
          }
          // Convert
          data.push_back( packBinary( value, p.type, format ) );
        }
        // And store the data
        elementData.push_back( data );
      }
    }
  }
  catch (const ifstream::failure& e)
  {
    // File opening failures
    res.result = false;
    res.reason = "Error while reading file\n" + string( e.what() );
  }
  catch( const std::invalid_argument& e )
  {
    res.result = false;
    res.reason = "Error parsing line\n" + string( e.what() );
  }
  catch(...)
  {
    // Report any parsing failures here
    res.result = false;
    res.reason = "General error reading element data";
  }

  return res;
}

// ===========================================================================

void plyElementSep::writeDataToFile( ofstream& outputFile, const string format )
{
  for( auto data : elementData )
  {
    for( unsigned int j=0; j<data.size(); j++ )
    {
      if( format == "ascii" )
      {
        outputFile << unpackAscii( data.at(j), properties.at(j).type ) << " ";
      }
      else
      {
        vector<unsigned char> bytes = unpackBinary( data.at(j), properties.at(j).type, format );
        for( auto b : bytes )
        {
          outputFile << b;
        }
      }
    }
    if( format == "ascii" )
    {
      outputFile << endl;
    }
  }
}

// ===========================================================================

string plyElementSep::getHeader( void )
{
  std::stringstream buffer;
  buffer << ELEMENT << " " << name << " " << elementData.size() << endl;
  for ( auto p : properties )
  {
    buffer << PROPERTY << " " << p.type << " " << p.name << endl;
  }

  return buffer.str();
}

// ===========================================================================

unsigned int plyElementSep::size( void )
{
    return elementData.size();
}

// ===========================================================================

struct returnResult plyElementSep::addProperty( const string name, const string type )
{
  struct returnResult res = { true, "" };
  // Check that the name doesn't already exist
  for ( auto p : properties )
  {
    if( p.name == name )
    {
      res.result = false;
      res.reason = "Duplicate property name found: " + name;
      break;
    }
  }

  if( res.result == true )
  {
    // No duplicate so add property to list
    struct elementProperty p = { name, type };
    properties.push_back( p );
    // Check if there are any existing data entries, if so then append
    // a zero value to each data vector to account for this new property
    for( unsigned int i=0; i<elementData.size(); i++ )
    {
      elementData[i].push_back( packAscii( "0", type ) );
    }
  }

  return res;
}

// ===========================================================================

string plyElementSep::getData( unsigned int index, const string name )
{
  // Data is converted to ascii so that the calling routine can
  // do what it wants with it

  // Check the data index
  if( index >= elementData.size() )
  {
    throw invalid_argument( "Data index out of range");
  }

  // Check that the name exist
  bool found = false;
  unsigned int i = 0;
  while( ( found == false ) && ( i < properties.size() ) )
  {
    if( properties.at(i).name == name )
    {
      found = true;
    }
    i++;
  }
  // Throw an exception if it wasn't found
  if( found == false )
  {
    throw invalid_argument( "getData - Parameter name: " + name + " not found");
  }

  // Extract the data value
  i--;
  return unpackAscii( elementData.at( index ).at(i), properties.at(i).type );
}

// ===========================================================================

void plyElementSep::setData( unsigned int index, const string name, const string value )
{
  // Check the data index
  if( index >= elementData.size() )
  {
    throw invalid_argument( "Data index out of range");
  }

  // Check that the name exist
  bool found = false;
  unsigned int i = 0;
  while( ( found == false ) && ( i < properties.size() ) )
  {
    if( properties.at(i).name == name )
    {
      found = true;
    }
    i++;
  }
  // Throw an exception if it wasn't found
  if( found == false )
  {
    throw invalid_argument( "setData - Parameter name: \"" + name + "\" not found");
  }

  // Set the data value
  i--;
  elementData.at( index ).at(i) = packAscii( value, properties.at(i).type );

}

// ===========================================================================

unsigned int plyElementSep::dupVertex( unsigned int index )
{
  if( index < elementData.size() )
  {
    elementData.push_back( elementData.at( index ) );
  }
  else
  {
    throw invalid_argument( "dupvertex - invalid index: " + index );
  }

  return elementData.size() - 1;
}

// ===========================================================================

unsigned int plyElementSep::addVertex( vector<UINT64> values )
{
  // Note: the only checking done is that the number of values match
  // the number of properties. No type checking is done.

  if( values.size() == properties.size() )
  {
    elementData.push_back( values );
  }
  else
  {
    throw invalid_argument( "addvertex - wrong number of values: " + values.size() );
  }

  return elementData.size() - 1;
}
