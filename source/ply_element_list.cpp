// ply_element_list.cpp - derived class for handling PLY elements where the
//                        properties are defined as lists on one line
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

#include "ply_element_list.hpp"
#include "util.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>


using namespace std;

// ===========================================================================

struct returnResult plyElementList::importDataFromFile( ifstream& inputFile, const string format )
{
  // For the base class just read and discard the input if format = ascii
  // Otherwise an exception is thrown
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
        vector<string> params = split( inputLine, ' ' );
        // Get number of list items
        vector<UINT64> data;
        // Get and convert the items depending on data type
        for( unsigned int i = 1; i < params.size(); i++ )
        {
          data.push_back( packAscii( params.at(i), property.type ) );
        }
        // And store
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
        // Binary format so first get the number of values
        int lb = getNumberOfBytes( property.listType );
        // Read a byte at a time
        value.clear();
        for( int j=0; j<lb; j++ )
        {
          inputFile.read( &dataByte, 1 );
          value.push_back( static_cast<unsigned char>( dataByte ) );
        }
        // Convert - note that at this point we assume that the type of the
        // length of the list variable is a uchar. If not then this section
        // will probably fail if the code is run on a big endian machine
        // because the internal representation of the data in the UINT64
        // is little endian ( i.e. Intel/AMD PC format )
        UINT64 listNumber = packBinary( value, property.listType, format );

        // Now read the rest of the data for this entry
        int b = getNumberOfBytes( property.type );
        for( UINT64 j=0; j<listNumber; j++ )
        {
          // Read a byte at a time
          value.clear();
          for( int k=0; k<b; k++ )
          {
            inputFile.read( &dataByte, 1 );
            value.push_back( static_cast<unsigned char>( dataByte ) );
          }
          // Convert
          data.push_back( packBinary( value, property.type, format ) );
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

struct returnResult plyElementList::setProperty( const string listType, const string name, const string type )
{
  struct returnResult res = { true, "" };

  property.listType = listType;
  property.name = name;
  property.type = type;

  return res;
}

// ===========================================================================

vector<string> plyElementList::getData( const unsigned int index )
{
  vector<string> data;

  // Check that the index is within range
  if( index < elementData.size() )
  {
    // Extract the data as ascii
    vector<UINT64> element = elementData.at( index );
    for( unsigned int i=0; i<element.size(); i++ )
    {
      data.push_back( unpackAscii( element.at(i), property.type ) );
    }
  }

  return data;
}

// ===========================================================================

string plyElementList::getHeader( void )
{
  std::stringstream buffer;
  buffer << ELEMENT << " " << name << " " << elementData.size() << endl;
  buffer << PROPERTY << " " << LIST << " " << property.listType << " "
                                  << property.type << " " << property.name << endl;

  return buffer.str();
}

// ===========================================================================

unsigned int plyElementList::size( void )
{
    return elementData.size();
}

// ===========================================================================

void plyElementList::writeDataToFile( ofstream& outputFile, const string format )
{
  for( auto data : elementData )
  {
    // Size
    if( format == "ascii" )
    {
      outputFile << unpackAscii( (UINT64) data.size(), property.listType ) << " ";
    }
    else
    {
      vector<unsigned char> bytes = unpackBinary( (UINT64) data.size(), property.listType, format );
      for( auto b : bytes )
      {
        outputFile << b;
      }
    }
    for( auto value : data )
    {
      // Then the actual data
      if( format == "ascii" )
      {
        outputFile << unpackAscii( value, property.type ) << " ";
      }
      else
      {
        vector<unsigned char> bytes = unpackBinary( value, property.type, format );
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

unsigned int plyElementList::setData( const vector<string> data )
{
  vector<UINT64> convertedData;

  // Convert from ascii
  for( unsigned int i=0; i<data.size(); i ++ )
  {
    convertedData.push_back( packAscii( data.at(i), property.type ) );
  }
  elementData.push_back( convertedData );

  return elementData.size() - 1;

}
