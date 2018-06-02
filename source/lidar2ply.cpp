// lidar2ply.cpp - basic LiDAR to PLY converter
// Copyright (C) 2018 John Davies
//
// Usage:
// lidar2ply -h : shows help message
//
// lidar2ply.lua -f <input file> [ options]
//          <input file> : LiDAR file
// Options: -i <image file> : specify image overlay
//          -x <value> : add X axis offset to PLY model
//          -y <value> : add Y axis offset to PLY model
//          -z <value> : add Z axis offset to PLY model
//
// lidar2ply -l <list file>
//          <list file> : text file containing a list of LiDAR/image files
//
// General options:  -a : auto fill NODATA values
//                   -m : create an output mesh

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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.#include <cstdlib>

#include <unistd.h>
#include <iostream>
#include <map>
#include <climits>
#include "lidarlib.hpp"
#include "util.hpp"
#include "lidarply.hpp"
#include "lidarimage.hpp"

using namespace std;

// Command line arguments
bool helpOpt = false;
bool inputFileOpt = false;
char *inputFileName = NULL;
bool imageFileOpt = false;
char *imageFileName = NULL;
bool xOffsetOpt = false;
float xOffset = 0.0;
bool yOffsetOpt = false;
float yOffset = 0.0;
bool zOffsetOpt = false;
float zOffset = 0.0;
bool listFileOpt = false;
char *listFileName = NULL;
bool autofillOpt = false;
bool meshOpt = false;
bool parseCheck = true;

// ------------------------------------------------------------------------

void printHelp( void )
{
  cout << "Usage:" << endl;
  cout << "lidar2ply -h : shows help message" << endl;
  cout << endl;
  cout << "lidar2ply -f <input file> [ options]" << endl;
  cout << "             <input file> : LiDAR file" << endl;
  cout << "Options: -i <image file> : specify image overlay" << endl;
  cout << "         ( image must be in Imagemagick text format )" << endl;
  cout << "         -x <value> : add X axis offset to PLY model" << endl;
  cout << "         -y <value> : add Y axis offset to PLY model" << endl;
  cout << "         -z <value> : add Z axis offset to PLY model" << endl;
  cout << endl;
  cout << "lidar2ply -l <list file>" << endl;
  cout << "             <list file> : text file containing a list of LiDAR/image files" << endl;
  cout << endl;
  cout << "General options: -m : create an output mesh" << endl;
}

// ------------------------------------------------------------------------

void processFiles( vector< vector<string> > files, string modelName )
{
  lidarply model;
  model.setFormat( "binary_little_endian" );

  struct returnResult ret;
  unsigned int xllMin = UINT_MAX;
  unsigned int yllMin = UINT_MAX;

  // First pass to calculate positions
  for ( auto f : files )
  {
    lidar lidarFile;
    ret = lidarFile.readFromFile( f.at(0) );
    if( ret.result == false )
    {
      cout << "Could not open file: " << inputFileName << " " << endl << ret.reason << endl;
      break;
    }

    // Loop through the points and copy to PLY file
    unsigned int xll = lidarFile.getXllcorner();
    unsigned int yll = lidarFile.getYllcorner();

    if( xll < xllMin )
    {
      xllMin = xll;
    }
    if( yll < yllMin )
    {
      yllMin = yll;
    }
  }
  cout << "xllcorner min = " << xllMin << " yllcorner min = " << yllMin << endl;

  // Second pass to process the files
  for ( auto f : files )
  {

    lidar lidarFile;
    lidarImage* image;
    bool imageOverlay = false;

    cout << "Processing: " << f.at(0) << " ( ";
    if( f.size() == 2 )
    {
      cout << f.at(1) << " )" << endl;
      imageOverlay = true;
    }
    else
    {
      cout << " No overlay )" << endl;
    }

    // Read the LiDAR file
    cout << "  Reading LiDAR file" << endl;
    ret = lidarFile.readFromFile( f.at(0) );
    if( ret.result == false )
    {
      cout << "Could not open file: " << inputFileName << " " << endl << ret.reason << endl;
      break;
    }

    // Loop through the points and copy to PLY file
    unsigned int r = lidarFile.getNoRows();
    unsigned int c = lidarFile.getNoColumns();
    float noData = lidarFile.getNODATA_value();
    float cellsize = lidarFile.getCellsize();

    // Work out offset for tiling
    unsigned int xOff = lidarFile.getXllcorner() - xllMin;
    unsigned int yOff = lidarFile.getYllcorner() - yllMin;

    // Check if an image overlay is needed
    if( imageOverlay == true )
    {
      cout << "  Opening image file" << endl;
      image = new lidarImage( r, c, 255 );
      ret = image->readFromFile( f.at(1) );
      if( ret.result == false )
      {
        cout << "Could not open image file: " << f.at(1) << endl << ret.reason << endl;
        break;
      }
    }

    // Write the data
    cout << "  Creating PLY file" << endl;
    float v;
    unsigned char red = 128;
    unsigned char green = 128;
    unsigned char blue = 128;
    bool res;
    // Create an array for storing the vertex ids in case a mesh needs
    // to be created
    int** ids;
    ids = new int*[ r ];
    if( ids == NULL )
    {
      cout << "Failed to create \"ids\" array" << endl;
      return;
    }
    for( unsigned int i=0; i<r; i++ )
    {
      ids[i] = new int[ c ];
      if( ids[i] == NULL )
      {
        cout << "Failed to create row array: "<< i << endl;
        return;
      }
    }
    // Copy data to model
    for( unsigned int y=0; y<r; y++ )
    {
      for( unsigned int x=0; x<c; x++ )
      {
        res = lidarFile.getValue( x, y, v );
        if( ( res == true ) && ( v != noData ) )
        {
          if( imageOverlay == true )
          {
            ret = image->getPixel( x, y, red, green, blue );
            if( ret.result == false )
            {
              cout << "Error getting pixel: " << ret.reason << endl;
              break;
            }
          }
          ids[y][x] = model.addVertex( ( (float)x * cellsize ) + xOff,
                           ( (float)y * cellsize ) + yOff,
                           ( zOffset + v ),
                           red, green, blue );
        }
        else
        {
          // Mark data as invalid
          ids[y][x] = -1;
        }
      }
    }

    // Check for mesh creation required
    if( meshOpt == true )
    {
      for( unsigned int row=0; row<r-1; row++ )
      {
        for( unsigned int col=0; col<c-1; col++ )
        {
          int a = ids[row][col];
          int b = ids[row][col+1];
          int c = ids[row+1][col];
          int d = ids[row+1][col+1];
          // Add faces
          if( ( a != -1 ) && ( d != -1 ) && ( b != -1 ) )
          {
            vector<string> f;
            f.push_back( to_string( b ) );
            f.push_back( to_string( d ) );
            f.push_back( to_string( a ) );
            model.addFace( f );
          }
          if( ( a != -1 ) && ( c != -1 ) && ( d != -1 ) )
          {
            vector<string> f;
            f.push_back( to_string( d ) );
            f.push_back( to_string( c ) );
            f.push_back( to_string( a ) );
            model.addFace( f );
          }
        }
      }
    }
  } // of auto

  // Write model
  model.writeToFile( modelName );

}

// ------------------------------------------------------------------------

void singleFile( void )
{
  lidar lidarFile;

  cout << "Processing single LiDAR file: " << inputFileName << endl;
  struct returnResult r = lidarFile.readFromFile( inputFileName );
  if( r.result == false )
  {
    cout << "Could not open file: " << inputFileName << " " << endl << r.reason << endl;
  }
  else
  {
    lidarply model;
    model.setFormat( "binary_little_endian" );

    lidarImage* image;
    struct returnResult ret;

    // Loop through the points and copy to PLY file
    unsigned int r = lidarFile.getNoRows();
    unsigned int c = lidarFile.getNoColumns();
    float noData = lidarFile.getNODATA_value();
    float cellsize = lidarFile.getCellsize();

    // Check if an image overlay is needed
    if( imageFileOpt == true )
    {
      cout << "Processing image file: " << imageFileName << endl;
      image = new lidarImage( r, c, 255 );
      ret = image->readFromFile( string( imageFileName ) );
      if( ret.result == false )
      {
        cout << "Could not open image file: " << imageFileName << endl << ret.reason << endl;
        return;
      }
    }

    // Write the data
    cout << "Creating PLY file" << endl;
    float v;
    unsigned char red = 128;
    unsigned char green = 128;
    unsigned char blue = 128;
    bool res;
    // Create an array for storing the vertex ids in case a mesh needs
    // to be created
    int** ids;
    ids = new int*[ r ];
    if( ids == NULL )
    {
      cout << "Failed to create \"ids\" array" << endl;
      return;
    }
    for( unsigned int i=0; i<r; i++ )
    {
      ids[i] = new int[ c ];
      if( ids[i] == NULL )
      {
        cout << "Failed to create row array: "<< i << endl;
        return;
      }
    }
    // Copy data to model
    for( unsigned int y=0; y<r; y++ )
    {
      for( unsigned int x=0; x<c; x++ )
      {
        res = lidarFile.getValue( x, y, v );
        if( ( res == true ) && ( v != noData ) )
        {
          if( imageFileOpt == true )
          {
            ret = image->getPixel( x, y, red, green, blue );
            if( ret.result == false )
            {
              cout << "Error getting pixel: " << ret.reason << endl;
              return;
            }
          }
          ids[y][x] = model.addVertex( ( (float)x * cellsize ) + xOffset,
                           ( (float)y * cellsize ) + yOffset,
                           ( zOffset + v ),
                           red, green, blue );
        }
        else
        {
          // Mark data as invalid
          ids[y][x] = -1;
        }
      }
    }

    // Check for mesh creation required
    if( meshOpt == true )
    {
      for( unsigned int row=0; row<r-1; row++ )
      {
        for( unsigned int col=0; col<c-1; col++ )
        {
          int a = ids[row][col];
          int b = ids[row][col+1];
          int c = ids[row+1][col];
          int d = ids[row+1][col+1];
          // Add faces
          if( ( a != -1 ) && ( d != -1 ) && ( b != -1 ) )
          {
            vector<string> f;
            f.push_back( to_string( b ) );
            f.push_back( to_string( d ) );
            f.push_back( to_string( a ) );
            model.addFace( f );
          }
          if( ( a != -1 ) && ( c != -1 ) && ( d != -1 ) )
          {
            vector<string> f;
            f.push_back( to_string( d ) );
            f.push_back( to_string( c ) );
            f.push_back( to_string( a ) );
            model.addFace( f );
          }
        }
      }
    }

    // Write out the converted file
    cout << "Writing PLY file: " << inputFileName << ".ply" << endl;
    model.writeToFile( string( inputFileName ) + ".ply" );
  }

}

//=========================================================================

int main( int argc, char *argv[] )
{
  cout << "LiDAR to PLY converter, V1.1" << endl;

  int c;
  opterr = 0;

  // Process the command line
  while( (c = getopt( argc, argv, "hf:i:x:y:z:l:am" ) ) != -1 )
  {
    switch( c )
      {
        case 'h':
          helpOpt = true;
          break;

        case 'f':
          inputFileOpt = true;
          inputFileName = optarg;
          break;

        case 'i':
          imageFileOpt = true;
          imageFileName = optarg;
          break;

        case 'x':
          xOffsetOpt = true;
          xOffset = stof( optarg );
          break;

        case 'y':
          yOffsetOpt = true;
          yOffset = stof( optarg );
          break;

        case 'z':
          zOffsetOpt = true;
          zOffset = stof( optarg );
          break;

        case 'l':
          listFileOpt = true;
          listFileName = optarg;
          break;

        case 'a':
          autofillOpt = true;
          break;

        case 'm':
          meshOpt = true;
          break;

        case '?':
          if( optopt == 'f' || optopt == 'i' || optopt == 'x'
           || optopt == 'y'  || optopt == 'z'  || optopt == 'l' )
          {
            cout << "Option -" << static_cast<char>(optopt) << " requires an argument." << endl;
          }
          else
          {
            cout << "Unknown option: -" << static_cast<char>(optopt) << endl;
          }
          parseCheck = false;
          break;

        default:
          abort ();
      }
    }

  if( parseCheck == false )
  {
    return EXIT_FAILURE;
  }

  if( helpOpt == true )
  {
    // Help file
    printHelp();
  }
  else if( listFileOpt == true )
  {
    // List file processing
    cout << "Processing multi LiDAR file: " << string( listFileName ) << endl;
    ifstream listFile;
    string inputLine;

    // Build list of files & images ( if necessary )
    vector< vector<string> > fileList;
    listFile.open( listFileName, ios::in );
    if( !listFile )
    {
      cout << "Could not open file: " << listFileName << endl;
    }
    while( getline ( listFile, inputLine ) )
    {
      // Ignore comment or blank lines
      if( !inputLine.empty() && inputLine.at(0) != '#' )
      {
        // Get file names
        vector<string> params = split( inputLine, ' ' );
        // And add to list
        fileList.push_back( params );
      }
    }

    // And process
    processFiles( fileList, string( listFileName ) + ".ply" );
  }
  else if( inputFileOpt == true )
  {
    // Single file processing
    singleFile();
  }
  else
  {
    // Nothing specified so ignore
    cout << "No actions specified" << endl;
  }

  return EXIT_SUCCESS;
}
