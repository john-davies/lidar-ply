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

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include "lidarimage.hpp"
#include "util.hpp"
#include "lodepng.h"
using namespace std;
using namespace lodepng;

// ====================================================================
// Constructor

lidarImage::lidarImage( unsigned int x, unsigned int y, unsigned int colourDepth )
{
  xSize = x;
  ySize = y;
  maxColourDepth = colourDepth;

  // Create the array to store the data
  values = new struct rgb*[ xSize ];
  if( values == NULL )
  {
    throw logic_error( "Failed to create \"values\" array" );
  }
  for( unsigned int i=0; i<xSize; i++ )
  {
    values[i] = new struct rgb[ ySize ];
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

  // Uses https://github.com/lvandeve/lodepng
  std::vector<unsigned char> image; //the raw pixels
  unsigned width, height;

  //decode
  unsigned error = lodepng::decode( image, width, height, fileName.c_str() );

   //if there's an error then exit
   if( error )
   {
     res.result = false;
     res.reason = lodepng_error_text( error );
   }
   else if( ( width != xSize ) || ( height != ySize ) )
   {
     // Image size mismatch
     res.result = false;
     res.reason = "File size mismatch";
   }
   else
   {
     // File OK so read into memory
     //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA
     for( unsigned int y = 0; y < ySize; y++ )
     {
       for( unsigned int x = 0; x < xSize; x++ )
       {
         unsigned int pixel = ( ( ( ySize - y - 1 ) * xSize ) + x ) * 4;
         values[x][y].red = image[ pixel ];
         values[x][y].green = image[ pixel + 1 ];
         values[x][y].blue = image[ pixel + 2 ];
       }
     }
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
