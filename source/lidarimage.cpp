// lidarimage.cpp - Reads an Imagemagick ".txt" file
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

#include "lidarimage.hpp"
#include "util.hpp"
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

lidarImage::lidarImage( unsigned int x, unsigned int y, unsigned int colourDepth )
{
  xSize = x;
  ySize = y;
  maxColourDepth = colourDepth;

  // Create the array to store the data
  values = new struct rgb*[ ySize ];
  if( values == NULL )
  {
    throw logic_error( "Failed to create \"values\" array" );
  }
  for( unsigned int i=0; i<ySize; i++ )
  {
    values[i] = new struct rgb[ xSize ];
    if( values[i] == NULL )
    {
      throw logic_error( "Failed to create row array: " + i );
    }
  }
}

// ====================================================================
// Load image

struct returnResult lidarImage::readFromFile( string fileName )
{
  struct returnResult res = { true, "" };

  ifstream inputFile;
  string inputLine;
  //inputFile.exceptions( ifstream::failbit | ifstream::badbit | ifstream::eofbit );
  try
  {
    inputFile.open( fileName.c_str(), ios::in );
    // Get the first line to check the format
    // Should be something like
    // # ImageMagick pixel enumeration: 20,20,255,srgb
    getline ( inputFile, inputLine );
    if( inputLine.find( "ImageMagick pixel enumeration" ) == string::npos )
    {
      // Wrong type of file
      res.result = false;
      res.reason = "Wrong file type";
    }
    else
    {
      // Read the file size
      vector<string> p1 = split( inputLine, ':' );
      vector<string> p2 = split( p1.at(1), ',' );
      unsigned int x = stoi( p2.at(0) );
      unsigned int y = stoi( p2.at(1) );
      unsigned int c = stoi( p2.at(2) );

      if( ( x > xSize ) || ( y > ySize ) || ( c > maxColourDepth ) )
      {
        res.result = false;
        res.reason = "File size mismatch";
      }

      // Read the data, line at a time
      // Should be something like:
      // 0,0: (32,31,225)  #201FE1  srgb(32,31,225)
      while( getline ( inputFile, inputLine ) )
      {
        // Ignore blank lines
        if( inputLine.empty() )
        {
          continue;
        }
        p1 = split( inputLine, ':' );
        // Row & column
        p2 = split( p1.at(0), ',' );
        unsigned int x = stoi( p2.at(0) );
        unsigned int y = stoi( p2.at(1) );
        // Colours
        p1 = split( inputLine, ' ' );
        // Get rid of the brackets
        p1.at(1).erase( p1.at(1).begin() );
        p1.at(1).erase( p1.at(1).end()-1 );
        p2 = split( p1.at(1), ',' );
        unsigned int red = stoi( p2.at(0) );
        unsigned int green = stoi( p2.at(1) );
        unsigned int blue = stoi( p2.at(2) );
        // And write to array, north = up
        values[x][ySize-1-y].red = red;
        values[x][ySize-1-y].green = green;
        values[x][ySize-1-y].blue = blue;
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

// ====================================================================
// Get data

struct returnResult  lidarImage::getPixel( unsigned int x, unsigned int y,
               unsigned char& red, unsigned char& green, unsigned char& blue )
{
 struct returnResult res = { true, "" };

 if( ( x >= xSize ) || ( y >= ySize ) )
 {
   res.result = false;
   res.reason = "Image coordinates out of range";
 }
 else
 {
   red = values[x][y].red;
   green = values[x][y].green;
   blue = values[x][y].blue;
 }

 return res;
}
