// util.cpp - General utilities
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

#include "util.hpp"
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

// String splitter
// Based on https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
vector<string> split( const string inputString, char delimiter )
{
  vector<string> params;
  stringstream inputStream;
  inputStream.str( inputString );
  std::string item;
  while( getline( inputStream, item, delimiter ) )
  {
    // Note: exclude \r because files may come from Windows systems
    // and will create an extra parameter if not filtered out
    if( item != "" && item != "\r" )
    {
      params.push_back( item );
    }
  }

  return params;
}

// Utility routines for packing & unpacking data
// Data stored intenally as little endian

// PLY data types: http://paulbourke.net/dataformats/ply/
// name       type        number of bytes
// ---------------------------------------
// char       character                 1
// uchar      unsigned character        1
// short      short integer             2
// ushort     unsigned short integer    2
// int        integer                   4
// uint       unsigned integer          4
// float      single-precision float    4
// double     double-precision float    8

// Endian - https://www.cs.umd.edu/class/sum2003/cmsc311/Notes/Data/endian.html
// For 90AB12CD hex at address 1000
// Big Endian:    1000 = 90, 1001 = AB. 1002 = 12, 1003 = CD ( binary_big_endian )
// Little Endian: 1000 = CD, 1001 =	12, 1002 = AB, 1003 = 90 ( binary_little_endian )

// -----------------------------------------------------------------------

int getNumberOfBytes( string type )
{
  int noOfBytes = 0;

  if( type == "char" || type == "uchar" )
  {
      noOfBytes = 1;
  }
  else if( type == "short" || type == "ushort" )
  {
    noOfBytes = 2;
  }
  else if( type == "int" || type == "uint")
  {
    noOfBytes = 4;
  }
  else if( type == "float" )
  {
    noOfBytes = 4;
  }
  else if( type == "double" )
  {
    noOfBytes = 8;
  }
  else
  {
    // Should never get here but just in case
    throw invalid_argument( "Unknown type when calculating data size" );
  }

  return noOfBytes;

}

// Generic pack/unpack routines

UINT64 packAscii( string value, string type )
{
  UINT64 result;

  if( type == "char" || type == "uchar" )
  {
      result = packCharAscii( value );
  }
  else if( type == "short" || type == "ushort" )
  {
    result = packShortAscii( value );
  }
  else if( type == "int" || type == "uint")
  {
    result = packIntAscii( value );
  }
  else if( type == "float" )
  {
    result = packFloatAscii( value );
  }
  else if( type == "double" )
  {
    result = packDoubleAscii( value );
  }
  else
  {
    // Should never get here but just in case
    throw invalid_argument( "Unknown type when parsing data" );
  }

  return result;
}

string unpackAscii( UINT64 value, string type )
{
  string result;

  if( type == "char" || type == "uchar" )
  {
    result = unpackCharAscii( value );
  }
  else if( type == "short" || type == "ushort" )
  {
    result = unpackShortAscii( value );
  }
  else if( type == "int" || type == "uint")
  {
    result = unpackIntAscii( value );
  }
  else if( type == "float" )
  {
    result = unpackFloatAscii( value );
  }
  else if( type == "double" )
  {
    result = unpackDoubleAscii( value );
  }
  else
  {
    // Should never get here but just in case
    throw invalid_argument( "Unknown type when parsing data" );
  }

    return result;
}

UINT64 packBinary( vector<unsigned char> value, string type, string endian )
{
  UINT64 result;

  if( type == "char" || type == "uchar" )
  {
    if( endian == "binary_big_endian" )
    {
      result = packCharBigEndian( value );
    }
    else
    {
      result = packCharLittleEndian( value );
    }
  }
  else if( type == "short" || type == "ushort" )
  {
    if( endian == "binary_big_endian" )
    {
      result = packShortBigEndian( value );
    }
    else
    {
      result = packShortLittleEndian( value );
    }
  }
  else if( type == "int" || type == "uint")
  {
    if( endian == "binary_big_endian" )
    {
      result = packIntBigEndian( value );
    }
    else
    {
      result = packIntLittleEndian( value );
    }
  }
  else if( type == "float" )
  {
    if( endian == "binary_big_endian" )
    {
      result = packFloatBigEndian( value );
    }
    else
    {
      result = packFloatLittleEndian( value );
    }
  }
  else if( type == "double" )
  {
    if( endian == "binary_big_endian" )
    {
      result = packDoubleBigEndian( value );
    }
    else
    {
      result = packDoubleLittleEndian( value );
    }
  }
  else
  {
    // Should never get here but just in case
    throw invalid_argument( "Unknown type when parsing data" );
  }

  return result;
}

vector<unsigned char> unpackBinary( UINT64 value, string type, string endian )
{
  vector<unsigned char> result;

  if( type == "char" || type == "uchar" )
  {
    if( endian == "binary_big_endian" )
    {
      result = unpackCharBigEndian( value );
    }
    else
    {
      result = unpackCharLittleEndian( value );
    }
  }
  else if( type == "short" || type == "ushort" )
  {
    if( endian == "binary_big_endian" )
    {
      result = unpackShortBigEndian( value );
    }
    else
    {
      result = unpackShortLittleEndian( value );
    }
  }
  else if( type == "int" || type == "uint")
  {
    if( endian == "binary_big_endian" )
    {
      result = unpackIntBigEndian( value );
    }
    else
    {
      result = unpackIntLittleEndian( value );
    }
  }
  else if( type == "float" )
  {
    if( endian == "binary_big_endian" )
    {
      result = unpackFloatBigEndian( value );
    }
    else
    {
      result = unpackFloatLittleEndian( value );
    }
  }
  else if( type == "double" )
  {
    if( endian == "binary_big_endian" )
    {
      result = unpackDoubleBigEndian( value );
    }
    else
    {
      result = unpackDoubleLittleEndian( value );
    }
  }
  else
  {
    // Should never get here but just in case
    throw invalid_argument( "Unknown type when parsing data" );
  }

  return result;
}

// -----------------------------------------------------------------------
// Pack 8 bit value

UINT64 packCharAscii( string value )
{
  UINT64 i;
  i = stol( value );
  // Check that it's in range
  if( i < 256 )
  {
    return i;
  }
  else
  {
    throw out_of_range( "Value too large" );
  }
}

UINT64 packCharBigEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = value.at(0);
  return i;
}

UINT64 packCharLittleEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = value.at(0);
  return i;
}

// Unpack 8 bit value

string unpackCharAscii( UINT64 value )
{
  return to_string( value );
}

vector<unsigned char> unpackCharBigEndian( UINT64 value )
{
  unsigned char b0 = (unsigned char)( value & 0x000000FF );
  return vector<unsigned char> { b0 };
}

vector<unsigned char> unpackCharLittleEndian( UINT64 value )
{
  unsigned char b0 = (unsigned char)( value & 0x000000FF );
  return vector<unsigned char> { b0 };
}

// -----------------------------------------------------------------------
// Pack 16 bit value

UINT64 packShortAscii( string value )
{
  UINT64 i;
  i = stol( value );
  // Check that it's in range
  if( i < 0x10000 )
  {
    return i;
  }
  else
  {
    throw out_of_range( "Value too large" );
  }
}

UINT64 packShortBigEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( value.at(0) << 8 ) + value.at(1);
  return i;
}

UINT64 packShortLittleEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( value.at(1) << 8 ) + value.at(0);
  return i;
}

// Unpack 16 bit value

string unpackShortAscii( UINT64 value )
{
  return to_string( value );
}

vector<unsigned char> unpackShortBigEndian( UINT64 value )
{
  unsigned char b0 = (unsigned char)( value & 0x000000FF );
  unsigned char b1 = (unsigned char)( ( value & 0x0000FF00 ) >> 8 );
  return vector<unsigned char> { b1, b0 };
}

vector<unsigned char> unpackShortLittleEndian( UINT64 value )
{
  unsigned char b0 = (unsigned char)( value & 0x000000FF );
  unsigned char b1 = (unsigned char)( ( value & 0x0000FF00 ) >> 8 );
  return vector<unsigned char> { b0, b1 };
}

// -----------------------------------------------------------------------
// Pack 32 bit value

UINT64 packIntAscii( string value )
{
  UINT64 i;
  i = stol( value );
  // Check that it's in range
  if( i < 0x100000000 )
  {
    return i;
  }
  else
  {
    throw out_of_range( "Value too large" );
  }
}

UINT64 packIntBigEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( value.at(0) << 24 ) + ( value.at(1) << 16 ) + ( value.at(2) << 8 ) + value.at(3);
  return i;
}

UINT64 packIntLittleEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( value.at(3) << 24 ) + ( value.at(2) << 16 ) + ( value.at(1) << 8 ) + value.at(0);
  return i;
}

// Unpack 32 bit value

string unpackIntAscii( UINT64 value )
{
  return to_string( value );
}

vector<unsigned char> unpackIntBigEndian( UINT64 value )
{
  unsigned char b0 = (unsigned char)( value & 0x000000FF );
  unsigned char b1 = (unsigned char)( ( value & 0x0000FF00 ) >> 8 );
  unsigned char b2 = (unsigned char)( ( value & 0x00FF0000 ) >> 16 );
  unsigned char b3 = (unsigned char)( ( value & 0xFF000000 ) >> 24 );
  return vector<unsigned char> { b3, b2, b1, b0 };
}

vector<unsigned char> unpackIntLittleEndian( UINT64 value )
{
  unsigned char b0 = (unsigned char)( value & 0x000000FF );
  unsigned char b1 = (unsigned char)( ( value & 0x0000FF00 ) >> 8 );
  unsigned char b2 = (unsigned char)( ( value & 0x00FF0000 ) >> 16 );
  unsigned char b3 = (unsigned char)( ( value & 0xFF000000 ) >> 24 );
  return vector<unsigned char> { b0, b1, b2, b3 };
}

// -----------------------------------------------------------------------
// Pack Float value

UINT64 packFloatAscii( string value )
{
  float f;
  f = stof( value );
  unsigned char *bytes = reinterpret_cast<unsigned char*>( &f );
  return ( bytes[3] << 24 ) + ( bytes[2] << 16 ) + ( bytes[1] << 8 ) + bytes[0];
}

UINT64 packFloatBigEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( value.at(0) << 24 ) + ( value.at(1) << 16 ) + ( value.at(2) << 8 ) + value.at(3);
  return i;
}

UINT64 packFloatLittleEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( value.at(3) << 24 ) + ( value.at(2) << 16 ) + ( value.at(1) << 8 ) + value.at(0);
  return i;
}

// Unpack Float value

string unpackFloatAscii( UINT64 value )
{
  float f;
  string out;
  int offset = 1;

  unsigned char *bytes = reinterpret_cast<unsigned char*>( &f );
  bytes[0] = (unsigned char)( value & 0x000000FF );
  bytes[1] = (unsigned char)( ( value & 0x0000FF00 ) >> 8 );
  bytes[2] = (unsigned char)( ( value & 0x00FF0000 ) >> 16 );
  bytes[3] = (unsigned char)( ( value & 0xFF000000 ) >> 24 );

  // Delete any trailing zeros
  out = to_string( f );
  if( out.find_last_not_of('0') == out.find('.') )
  {
    offset = 0;
  }
  out.erase( out.find_last_not_of('0') + offset, string::npos);

  return out;
}

vector<unsigned char> unpackFloatBigEndian( UINT64 value )
{
  float f;
  unsigned char *bytes = reinterpret_cast<unsigned char*>( &f );
  bytes[0] = (unsigned char)( value & 0x000000FF );
  bytes[1] = (unsigned char)( ( value & 0x0000FF00 ) >> 8 );
  bytes[2] = (unsigned char)( ( value & 0x00FF0000 ) >> 16 );
  bytes[3] = (unsigned char)( ( value & 0xFF000000 ) >> 24 );
  return vector<unsigned char> { bytes[3], bytes[2], bytes[1], bytes[0] };
}

vector<unsigned char> unpackFloatLittleEndian( UINT64 value )
{
  float f;
  unsigned char *bytes = reinterpret_cast<unsigned char*>( &f );
  bytes[0] = (unsigned char)( value & 0x000000FF );
  bytes[1] = (unsigned char)( ( value & 0x0000FF00 ) >> 8 );
  bytes[2] = (unsigned char)( ( value & 0x00FF0000 ) >> 16 );
  bytes[3] = (unsigned char)( ( value & 0xFF000000 ) >> 24 );
  return vector<unsigned char> { bytes[0], bytes[1], bytes[2], bytes[3] };
}

// -----------------------------------------------------------------------
// Pack Double value

UINT64 packDoubleAscii( string value )
{
  float d;
  d = stod( value );
  unsigned char *bytes = reinterpret_cast<unsigned char*>( &d );
  return ( (UINT64)bytes[7] << 56 ) +
         ( (UINT64)bytes[6] << 48 ) +
         ( (UINT64)bytes[5] << 40 ) +
         ( (UINT64)bytes[4] << 32 ) +
         ( (UINT64)bytes[3] << 24 ) +
         ( (UINT64)bytes[2] << 16 ) +
         ( (UINT64)bytes[1] << 8 ) +
         (UINT64)bytes[0];
}

UINT64 packDoubleBigEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( (UINT64)value.at(0) << 56 ) +
      ( (UINT64)value.at(1) << 48 ) +
      ( (UINT64)value.at(2) << 40 ) +
      ( (UINT64)value.at(3) << 32 ) +
      ( (UINT64)value.at(4) << 24 ) +
      ( (UINT64)value.at(5) << 16 ) +
      ( (UINT64)value.at(6) << 8 ) +
      (UINT64)value.at(7);
  return i;
}

UINT64 packDoubleLittleEndian( vector<unsigned char> value )
{
  UINT64 i;
  i = ( (UINT64)value.at(7) << 56 ) +
      ( (UINT64)value.at(6) << 48 ) +
      ( (UINT64)value.at(5) << 40 ) +
      ( (UINT64)value.at(4) << 32 ) +
      ( (UINT64)value.at(3) << 24 ) +
      ( (UINT64)value.at(2) << 16 ) +
      ( (UINT64)value.at(1) << 8 ) +
      (UINT64)value.at(0);
  return i;
}

// Unpack Double value

string unpackDoubleAscii( UINT64 value )
{
  double d;
  string out;
  int offset = 1;

  unsigned char *bytes = reinterpret_cast<unsigned char*>( &d );
  bytes[0] =   (unsigned char)( value & 0x00000000000000FF );
  bytes[1] = (unsigned char)( ( value & 0x000000000000FF00 ) >> 8 );
  bytes[2] = (unsigned char)( ( value & 0x0000000000FF0000 ) >> 16 );
  bytes[3] = (unsigned char)( ( value & 0x00000000FF000000 ) >> 24 );
  bytes[4] = (unsigned char)( ( value & 0x000000FF00000000 ) >> 32 );
  bytes[5] = (unsigned char)( ( value & 0x0000FF0000000000 ) >> 40 );
  bytes[6] = (unsigned char)( ( value & 0x00FF000000000000 ) >> 48 );
  bytes[7] = (unsigned char)( ( value & 0xFF00000000000000 ) >> 56 );

  // Delete any trailing zeros
  out = to_string( d );
  if( out.find_last_not_of('0') == out.find('.') )
  {
    offset = 0;
  }
  out.erase( out.find_last_not_of('0') + offset, string::npos);

  return out;
}

vector<unsigned char> unpackDoubleBigEndian( UINT64 value )
{
  double d;
  unsigned char *bytes = reinterpret_cast<unsigned char*>( &d );
  bytes[0] =   (unsigned char)( value & 0x00000000000000FF );
  bytes[1] = (unsigned char)( ( value & 0x000000000000FF00 ) >> 8 );
  bytes[2] = (unsigned char)( ( value & 0x0000000000FF0000 ) >> 16 );
  bytes[3] = (unsigned char)( ( value & 0x00000000FF000000 ) >> 24 );
  bytes[4] = (unsigned char)( ( value & 0x000000FF00000000 ) >> 32 );
  bytes[5] = (unsigned char)( ( value & 0x0000FF0000000000 ) >> 40 );
  bytes[6] = (unsigned char)( ( value & 0x00FF000000000000 ) >> 48 );
  bytes[7] = (unsigned char)( ( value & 0xFF00000000000000 ) >> 56 );

  return vector<unsigned char> { bytes[7], bytes[6], bytes[5], bytes[4],
                                 bytes[3], bytes[2], bytes[1], bytes[0] };
}

vector<unsigned char> unpackDoubleLittleEndian( UINT64 value )
{
  double d;
  unsigned char *bytes = reinterpret_cast<unsigned char*>( &d );
  bytes[0] =   (unsigned char)( value & 0x00000000000000FF );
  bytes[1] = (unsigned char)( ( value & 0x000000000000FF00 ) >> 8 );
  bytes[2] = (unsigned char)( ( value & 0x0000000000FF0000 ) >> 16 );
  bytes[3] = (unsigned char)( ( value & 0x00000000FF000000 ) >> 24 );
  bytes[4] = (unsigned char)( ( value & 0x000000FF00000000 ) >> 32 );
  bytes[5] = (unsigned char)( ( value & 0x0000FF0000000000 ) >> 40 );
  bytes[6] = (unsigned char)( ( value & 0x00FF000000000000 ) >> 48 );
  bytes[7] = (unsigned char)( ( value & 0xFF00000000000000 ) >> 56 );

  return vector<unsigned char> { bytes[0], bytes[1], bytes[2], bytes[3],
                                 bytes[4], bytes[5], bytes[6], bytes[7] };
}
