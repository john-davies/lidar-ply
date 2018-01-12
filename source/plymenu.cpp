// plymenu.cpp - basic ply manipulation program
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
// along with this program.  If not, see <http://www.gnu.org/licenses/>.#include <cstdlib>

#include <iostream>
#include <map>
#include "plylib.hpp"
#include "util.hpp"

using namespace std;

// Model file being edited
ply modelFile;

// Hole data
vector< vector<unsigned int> >holeList;

// Menu item
struct menuItem {
  void (*function)(void);
  string text;
};
//=========================================================================

int domenu( vector<menuItem> options, bool repeat, string header )
{
  int selection = -1;
  bool quit = false;

  // Print the menu options
  cout << header << endl;
  for( unsigned int i=0; i<options.size(); i++ )
  {
    cout << i+1 << " - " << options.at(i).text << endl;
  }
  cout << "q - quit" << endl;

  while( quit == false )
  {
    // Get the user command
    string inputLine;
    cout << ">";
    getline( cin, inputLine );
    // Process the option
    if( inputLine == "" )
    {
      //Blank line so just ignore and reprint the prompt
    }
    else if( inputLine == "q" )
    {
      // Exit
      quit = true;
    }
    else
    {
      // It's an integer and within range
      try
      {
        unsigned int s = stoi( inputLine );
        if( ( s > 0 ) && ( s <= options.size() ) )
        {
          if( repeat == true )
          {
            // If it's a repeat then call the functions
            options.at(s-1).function();
            // Note which function was called
            selection = s - 1;
            // Reprint the menu items
            cout << header << endl;
            for( unsigned int i=0; i<options.size(); i++ )
            {
              cout << i+1 << " - " << options.at(i).text << endl;
            }
            cout << "q - quit" << endl;
          }
          else
          {
            // Otherwise just return the selection value and quit
            selection = s - 1;
            quit = true;
          }
        }
      }
      catch( const std::invalid_argument& e )
      {
        // Couldn't convert to an integer so ignore
      }
    }
  }

  return selection;
}

// Edit menu functions ====================================================

void changeFormat( void )
{
  cout << "Current format is: " << modelFile.getFormat() << endl;

  vector<menuItem> chooseFormat = {
    { NULL, "ascii" },
    { NULL, "binary_big_endian" },
    { NULL, "binary_little_endian" }
  };

  int newFormat = domenu( chooseFormat, false, "-----\nChoose new format" );

  switch( newFormat )
  {
    case 0:
      modelFile.setFormat( "ascii" );
      break;

    case 1:
      modelFile.setFormat( "binary_big_endian" );
      break;

    case 2:
      modelFile.setFormat( "binary_little_endian" );
      break;

    default:
      break;
  }
}

// ------------------------------------------------------------------------

void changeAllVertexColours( void )
{
  string rgbValue;
  cout << "Enter new colour ( r,g,b ): ";
  getline( cin, rgbValue );
  vector<string> values = split( rgbValue, ',' );
  if( values.size() != 3 )
  {
    cout << "Error: incorrect number of values" << endl;
  }
  else
  {
    int r = stoi( values.at(0) );
    int g = stoi( values.at(1) );
    int b = stoi( values.at(2) );
    if( ( r > 255 ) || ( g > 255 ) || ( b > 255 ) )
    {
      cout << "Error: value out of range" << endl;
    }
    else
    {
      struct returnResult res = modelFile.changeAllVertexColours(
                values.at(0), values.at(1), values.at(2) );
      if( res.result == false )
      {
        cout << "Error: " + res.reason << endl;
      }
    }
  }
}

// ------------------------------------------------------------------------

void changeScale( void )
{
  struct boundingBox box;
  struct returnResult res = modelFile.getBoundingBox( box );
  string scaleFactor;

  if( res.result == false )
  {
    cout << "Error: " + res.reason << endl;;
  }
  else
  {
    cout << "Current bounding box:" << endl;
    cout << "X: min: " << box.minX << " max: " << box.maxX << endl;
    cout << "Y: min: " << box.minY << " max: " << box.maxY << endl;
    cout << "Z: min: " << box.minZ << " max: " << box.maxZ << endl;
    cout << "Enter scale factor ( or blank to quit ): ";
    getline( cin, scaleFactor );
    if( scaleFactor != "" )
    {
      res = modelFile.scaleModel( scaleFactor, scaleFactor, scaleFactor );
      if( res.result == false )
      {
        cout << "Error: " + res.reason << endl;
      }
    }
  }
}

// Hole Menu functions ====================================================

void clearHoleData( void )
{
  holeList.clear();
}

// ------------------------------------------------------------------------

void findHoles( void )
{
  map<UINT64, int> edgeMap;
  UINT64 mapEntry;
  unsigned int faceCount = modelFile.getFaceCount();
  for( unsigned int i=0; i<faceCount; i++ )
  {
    // Loop though each face and extract the edges
    vector<string> data = modelFile.getFaceData( i );
    if( !data.empty() )
    {
      for( unsigned int j=0; j<data.size()-1; j++ )
      {
        // Convert to integers
        unsigned int a = stoi( data.at(j) );
        unsigned int b = stoi( data.at(j+1) );
        // Order in ascending value of verticies and create map entry
        if( a < b )
        {
          mapEntry = ( (UINT64) a << 32 ) + b;
        }
        else
        {
          mapEntry = ( (UINT64) b << 32 ) + a;
        }
        // Add or erase from map
        map<UINT64,int>::iterator m = edgeMap.find( mapEntry );
        if( m == edgeMap.end() )
        {
          // Entry doesn't exist so add it
          edgeMap.insert( std::pair<UINT64,int>( mapEntry, 0 ) );
        }
        else
        {
          // Already exists so clear it
          edgeMap.erase( m );
        }
      }
      // Wrap round from last vertex to first
      unsigned int a = stoi( data.at(0) );
      unsigned int b = stoi( data.back() );
      // Order in ascending value of verticies
      if( a < b )
      {
        mapEntry = ( (UINT64) a << 32 ) + b;
      }
      else
      {
        mapEntry = ( (UINT64) b << 32 ) + a;
      }
      // Add or erase from map
      map<UINT64,int>::iterator m = edgeMap.find( mapEntry );
      if( m == edgeMap.end() )
      {
        // Entry doesn't exist so add it
        edgeMap.insert( std::pair<UINT64,int>( mapEntry, 0 ) );
      }
      else
      {
        // Already exists so clear it
        edgeMap.erase( m );
      }
    }
  }

  // Link the edges together to define the holes
  bool findStart = true;
  bool endLoop = false;
  unsigned int nextVertex = 0;
  vector <unsigned int> thisHole;
  map<UINT64,int>::iterator m = edgeMap.begin();

  // Check for an empty edge map
  if( edgeMap.empty() )
  {
    endLoop = true;
  }

  while( endLoop == false )
  {
    if( findStart == true )
    {
      // Find the start of the hole
      m = edgeMap.begin();
      thisHole.clear();
      // Extract the two vertex numbers
      UINT64 key = m->first;
      unsigned int a = key >> 32;
      unsigned int b = key & 0xFFFFFFFF;
      // Clear this entry from the edge list
      edgeMap.erase( m++ );
      // Start the hole list
      thisHole.push_back( a );
      nextVertex = b;
      // Find the rest of this hole
      findStart = false;
    }
    else
    {
      // Extract the two vertex numbers
      UINT64 key = m->first;
      unsigned int a = key >> 32;
      unsigned int b = key & 0xFFFFFFFF;
      // Check for a match
      if( a == nextVertex )
      {
        // Check if the hole has been closed
        if( b == thisHole.at(0) )
        {
          // Close hole and start scanning again
          thisHole.push_back( a );
          holeList.push_back( thisHole );
          findStart = true;
        }
        else
        {
          // Not closed so log this value
          thisHole.push_back( a );
          nextVertex = b;
        }
        edgeMap.erase( m++ );
      }
      else if( b == nextVertex )
      {
        // Check if the hole has been closed
        if( a == thisHole.at(0) )
        {
          // Close hole and start scanning again
          thisHole.push_back( b );
          holeList.push_back( thisHole );
          findStart = true;
        }
        else
        {
          // Not closed so log this value
          thisHole.push_back( b );
          nextVertex = a;
        }
        edgeMap.erase( m++ );
      }
      else
      {
        // No match so move on
        m++;
      }
    }
    // Loop round the iterator if necessary
    // If all the edges have been processed then exit
    if( edgeMap.empty() == true )
    {
      endLoop = true;
    }
    else if( m == edgeMap.end() )
    {
      m = edgeMap.begin();
    }
  }
}

// ------------------------------------------------------------------------

void listHoles( void )
{
  cout << "Number of holes: " << holeList.size() << endl;
  for( unsigned int i=0; i<holeList.size(); i++ )
  {
    cout << "#" << i+1 << ": " << holeList.at(i).size() << " verticies" << endl;
  }
}

// ------------------------------------------------------------------------

void highlightHoles( void )
{
  struct returnResult res;

  // Get the hole id
  unsigned int holeNo;
  string holeID;
  cout << "Select hole number: ";
  getline( cin, holeID );
  holeNo = stoi( holeID );
  if( holeNo > holeList.size() || holeNo == 0 )
  {
    cout << "Error - invalid hole number" << endl;
  }
  else
  {
    holeNo--;
    for( unsigned int i=0; i<holeList.at(holeNo).size(); i++ )
    {
      // Change colour of vertex to red
      res = modelFile.changeVertexColours( holeList.at(holeNo).at(i), "255", "0", "0" );

      if( res.result == false )
      {
        cout << "Error - " << res.reason << endl;
        break;
      }
    }
  }
}

// ------------------------------------------------------------------------

void fillSingleHole( vector<unsigned int> singleHole )
{
  struct returnResult res;
  double x = 0;
  double y = 0;
  double z = 0;

  // Sanity check for hole array
  if( singleHole.empty() )
  {
    cout << "Error - empty hole array" << endl;
    return;
  }
  // Calculate centre point - loop through each vertex
  for( unsigned int i=0; i<singleHole.size(); i++ )
  {
    struct vertexCoordinates v;
    res = modelFile.getCoordinates( singleHole.at(i), v );
    if( res.result == true )
    {
      x += v.x;
      y += v.y;
      z += v.z;
    }
    else
    {
      cout << "Error - " << res.reason << endl;
      return;
    }
  }
  // Centre point = average of each axis
  x /= singleHole.size();
  y /= singleHole.size();
  z /= singleHole.size();

  // Add centre point
  unsigned int newIndex;
  res = modelFile.dupVertex( singleHole.at(0), newIndex );
  if( res.result == false )
  {
    cout << "Error - " << res.reason << endl;
    return;
  }
  res = modelFile.changeVertexCoords( newIndex, to_string( x ), to_string( y ), to_string( z ) );
  if( res.result == false )
  {
    cout << "Error - " << res.reason << endl;
    return;
  }

  // Create faces
  vector<string> face;
  for( unsigned int i=0; i<singleHole.size() - 1; i++ )
  {
    face.clear();
    face.push_back( to_string( singleHole.at(i) ) );
    face.push_back( to_string( singleHole.at(i+1) ) );
    face.push_back( to_string( newIndex ) );
    modelFile.addFace( face );
  }
  face.clear();
  face.push_back( to_string( singleHole.front() ) );
  face.push_back( to_string( singleHole.back() ) );
  face.push_back( to_string( newIndex ) );
  modelFile.addFace( face );

}

// ------------------------------------------------------------------------

void fillHoles( void )
{
  // Get the hole id
  unsigned int holeNo;
  string input;

  cout << "Select hole number: ";
  getline( cin, input );
  holeNo = stoi( input );
  if( holeNo > holeList.size() || holeNo == 0 )
  {
    cout << "Error - invalid hole number" << endl;
  }
  else
  {
    fillSingleHole( holeList.at( holeNo - 1 ) );
  }

  cout << "Run Meshlab: Filters > Normals, Curvatures and Orientation > Re-Orient All Faces Coherently" << endl;

}

// ------------------------------------------------------------------------

void fillHolesWithBase( void )
{
  struct returnResult res;
  unsigned int holeNo;
  float height;
  string input;

  // Get the hole id
  cout << "Select hole number: ";
  getline( cin, input );
  holeNo = stoi( input );
  if( holeNo > holeList.size() || holeNo == 0 )
  {
    cout << "Error - invalid hole number" << endl;
    return;
  }
  holeNo--;

  // Get base height
  cout << "Enter base height as a %age of overall height: ";
  getline( cin, input );
  height = stof( input );
  if( height < 0 )
  {
    cout << "Error - base specification" << endl;
    return;
  }

  // Calculate the lowest Z value of the hole verticies
  struct vertexCoordinates v;
  double zmin;
  res = modelFile.getCoordinates(holeList.at(holeNo).front(), v );
  if( res.result == false )
  {
    cout << res.reason << endl;
    return;
  }
  zmin = v.z;
  for( unsigned int i=1; i<holeList.at(holeNo).size(); i++ )
  {
    res = modelFile.getCoordinates(holeList.at(holeNo).at(i), v );
    if( res.result == false )
    {
      cout << res.reason << endl;
      return;
    }
    if( v.z < zmin )
    {
      zmin = v.z;
    }
  }

  // Calculate the z displacement for base
  struct boundingBox bBox;
  res = modelFile.getBoundingBox( bBox );
  if( res.result == false )
  {
    cout << res.reason << endl;
    return;
  }
  zmin = zmin - ( ( bBox.maxZ - bBox.minZ ) * ( height / 100 ) );

  // Make a copy of the hole verticies for the base
  vector<unsigned int> base;
  for( unsigned int i=0; i<holeList.at(holeNo).size(); i++ )
  {
    unsigned int newIndex;
    res = modelFile.dupVertex( holeList.at(holeNo).at(i), newIndex );
    if( res.result == false )
    {
      cout << res.reason << endl;
      return;
    }
    base.push_back( newIndex );
    // Get the original x,y coordinates
    res = modelFile.getCoordinates(holeList.at(holeNo).at(i), v );
    if( res.result == false )
    {
      cout << res.reason << endl;
      return;
    }

    res = modelFile.changeVertexCoords( newIndex, to_string( v.x ), to_string( v.y ), to_string( zmin ) );
    if( res.result == false )
    {
      cout << res.reason << endl;
      return;
    }
  }

  // Create the faces for the base
  vector<string> face;
  for( unsigned int i=0; i<holeList.at(holeNo).size()-1; i++ )
  {
    face.push_back( to_string( holeList.at(holeNo).at(i) ) );
    face.push_back( to_string( holeList.at(holeNo).at(i+1) ) );
    face.push_back( to_string( base.at(i+1) ) );
    face.push_back( to_string( base.at(i) ) );
    modelFile.addFace( face );
    face.clear();
  }
  face.push_back( to_string( holeList.at(holeNo).back() ) );
  face.push_back( to_string( holeList.at(holeNo).front() ) );
  face.push_back( to_string( base.front() ) );
  face.push_back( to_string( base.back() ) );
  modelFile.addFace( face );

  // And fill the hole in the new base
  fillSingleHole( base );

  cout << "Run Meshlab: Filters > Normals, Curvatures and Orientation > Re-Orient All Faces Coherently" << endl;

}

// Top Level functions ====================================================

void openFile( void )
{
  // Get the file name
  string inputFile;
  cout << "Enter input file name: ";
  getline( cin, inputFile );

  struct returnResult r = modelFile.readFromFile( inputFile );
  if( r.result == false )
  {
    cout << "Could not open file: " << inputFile << " " << endl << r.reason << endl;
  }
}

// ------------------------------------------------------------------------

void writeFile( void )
{
  // Get the file name
  string outputFile;
  cout << "Enter output file name: ";
  getline( cin, outputFile );

  struct returnResult r = modelFile.writeToFile( outputFile );
  if( r.result == false )
  {
    cout << "Could not write to file: " << outputFile << " " << endl << r.reason << endl;
  }
}

// ------------------------------------------------------------------------

void showHeader( void )
{
  cout << modelFile.printHeader();
}

// ------------------------------------------------------------------------

void edit( void )
{
 // Show edit menu
 vector<menuItem> editMenu = {
   { &changeFormat, "Change format type" },
   { &changeAllVertexColours, "Change all vertex colours"},
   { &changeScale, "Change model scale" }
 };

 domenu( editMenu, true, "-----\nEdit functions" );
}

// ------------------------------------------------------------------------

void holes( void )
{
  vector<menuItem> holeMenu = {
                 { &clearHoleData, "Clear hole data" },
                 { &findHoles, "Find holes" },
                 { &listHoles, "List holes" },
                 { &highlightHoles, "Highlight holes" },
                 { &fillHoles, "Fill holes" },
                 { &fillHolesWithBase, "Fill holes and add a base" }
  };

   domenu( holeMenu, true, "-----\nHole options" );
}

//=========================================================================

int main( int argc, char *argv[] )
{
  cout << "PLY file editor" << endl;
  cout << "===============" << endl;

  if( argc == 2 )
  {
    struct returnResult res;
	  cout << "Reading file: " << argv[1] << endl;
    res = modelFile.readFromFile( argv[1] );
    if( res.result == false )
    {
      cout << res.reason << endl;
    }
  }

  // Now run menu
  vector<menuItem> options = {
    { &openFile, "Open file" },
    { &writeFile, "Write file"},
    { &showHeader, "Show header" },
    { &edit, "Edit" },
    { &holes, "Holes" }
  };

  domenu( options, true, "-----\nSelect option:" );

  return EXIT_SUCCESS;
}
