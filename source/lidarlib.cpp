// lidarlib.cpp - Read and process LiDAR files from
//                http://lle.gov.wales/catalogue/item/LidarCompositeDataset/?lang=en
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

#include "lidarlib.hpp"
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

struct returnResult lidar::readFromFile( const string fileName )
{
  struct returnResult res = { true, "" };

  // Open file for reading.
  ifstream inputFile;
  string inputLine;
  inputFile.exceptions( ifstream::failbit | ifstream::badbit | ifstream::eofbit );
  try
  {
    inputFile.open( fileName.c_str(), ios::in );

    // Read the first 6 lines to get the header information

    bool ncols_found = false;
    bool nrows_found = false;
    bool xllcorner_found = false;
    bool yllcorner_found = false;
    bool cellsize_found = false;
    bool NODATA_value_found = false;

    for( int i=0; i<6; i++ )
    {
      getline( inputFile, inputLine );
      vector<string> params = split( inputLine, ' ' );
      if( params.at(0) == "ncols" )
      {
        if( ncols_found == true )
        {
          throw invalid_argument( "Duplicate \"ncols\" header line" );
        }
        else
        {
          ncols = stoi( params.at(1) );
          ncols_found = true;
        }
      }
      else if( params.at(0) == "nrows" )
      {
        if( nrows_found == true )
        {
          throw invalid_argument( "Duplicate \"nrows\" header line" );
        }
        else
        {
          nrows = stoi( params.at(1) );
          nrows_found = true;
        }
      }
      else if( params.at(0) == "xllcorner" )
      {
        if( xllcorner_found == true )
        {
          throw invalid_argument( "Duplicate \"xllcorner\" header line" );
        }
        else
        {
          xllcorner = stoi( params.at(1) );
          xllcorner_found = true;
        }
      }
      else if( params.at(0) == "yllcorner" )
      {
        if( yllcorner_found == true )
        {
          throw invalid_argument( "Duplicate \"yllcorner\" header line" );
        }
        else
        {
          yllcorner = stoi( params.at(1) );
          yllcorner_found = true;
        }
      }
      else if( params.at(0) == "cellsize" )
      {
        if( cellsize_found == true )
        {
          throw invalid_argument( "Duplicate \"cellsize\" header line" );
        }
        else
        {
          cellsize = stof( params.at(1) );
          cellsize_found = true;
        }
      }
      else if( params.at(0) == "NODATA_value" )
      {
        if( NODATA_value_found == true )
        {
          throw invalid_argument( "Duplicate \"NODATA_value\" header line" );
        }
        else
        {
          NODATA_value = stof( params.at(1) );
          NODATA_value_found = true;
        }
      }
      else
      {
        throw invalid_argument( "Unknown header type: " + params.at(0) );
      }
    }

    // Now read the rest of the data
    // Dynamically allocate the memory for the array
    values = new float*[ nrows ];
    if( values == NULL )
    {
      throw logic_error( "Failed to create \"values\" array" );
    }
    for( unsigned int i=0; i<nrows; i++ )
    {
      values[i] = new float[ ncols ];
      if( values[i] == NULL )
      {
        throw logic_error( "Failed to create row array: " + i );
      }
    }

    // Read data a row at a time
    vector<string> p;
    unsigned int count = 0;
    // Arrange so that up = north
    for( int r=nrows-1; r>=0; r-- )
    //for( unsigned int r=0; r<nrows; r++ )
    {
      for( unsigned int c=0; c<ncols; c++ )
      {
        // Not sure if we can assume that each row of data is on one line
        // If no data available then get another line
        if( p.empty() )
        {
          getline ( inputFile, inputLine );
          p = split( inputLine, ' ' );
        }
        values[r][c] = stof( p.front() );
        p.erase( p.begin() );
        count++;
      }
    }

    /* Debug to print values read
    for( unsigned int r=nrows-1; r!=0; r-- )
    {
      for( unsigned int c=0; c<ncols; c++ )
      {
        cout << values[r][c] << " ";
      }
      cout << endl;
    }*/

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
// Information

string lidar::printHeader( void )
{
  std::stringstream buffer;

  buffer << "ncols " << ncols << endl;
  buffer << "nrows " << nrows << endl;
  buffer << "xllcorner " << xllcorner << endl;
  buffer << "yllcorner " << yllcorner << endl;
  buffer << "cellsize " << cellsize << endl;
  buffer << "NODATA_value " << NODATA_value << endl;

  return buffer.str();
}

// ====================================================================
// Get/set data

unsigned int lidar::getNoColumns( void )
{
  return ncols;
}

// --------------------------------------------------------------------

unsigned int lidar::getNoRows( void )
{
  return nrows;
}

// --------------------------------------------------------------------

unsigned int lidar::getXllcorner( void )
{
  return xllcorner;
}

// --------------------------------------------------------------------

unsigned int lidar::getYllcorner( void )
{
  return yllcorner;
}

// --------------------------------------------------------------------

float lidar::getCellsize( void )
{
  return cellsize;
}

// --------------------------------------------------------------------

float lidar::getNODATA_value( void )
{
  return NODATA_value;
}

// --------------------------------------------------------------------

bool lidar::getValue( unsigned int col, unsigned int row, float &value )
{
  bool res = true;

  if( ( col < ncols ) && ( row < nrows ) )
  {
    value = values[row][col];
  }
  else
  {
    res = false;
  }

  return res;
}

// --------------------------------------------------------------------

bool lidar::setValue( unsigned int col, unsigned int row, float value )
{
  bool res = true;

  if( ( col < ncols ) && ( row < nrows ) )
  {
    values[row][col] = value;
  }
  else
  {
    res = false;
  }

  return res;
}
