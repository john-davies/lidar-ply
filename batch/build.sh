#!/bin/bash
#
# build.sh - a shell script to read and process LiDAR files and to
#            convert them to a single PLY model
# 				 usage: build.sh <config file>
# Copyright (C) 2018 John Davies
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# Set up some default values

lidarDataPath=""
lidarImageDataPath=""
osm2pngDir=""
lidar2plyExec=""
lidarFileExt=".asc"
lidarFileBuiltinImageExt=".jpg"
overlay=true
builtinImage=false
xDim=0
yDim=0
imageX=0
imageY=0
zoomLevel=16

# Gridrefs
declare -A bottomLeft
declare -A topRight
bottomLeft[lat]=0.0
bottomLeft[long]=0.0
topRight[lat]=0.0
topRight[long]=0.0

# LiDAR files: top left to bottom right, row by row
declare -a lidarFiles
# image overlay for LiDAR files
declare -a imageFilesList

# Check that config file is valid
if [ -f "$1" ]
then
	# Read values
	source "$1"
fi

# Check settings
echo "Checking settings file ... "
#Check that there are enough LiDAR files
if [ $(( xDim * yDim )) -ne ${#lidarFiles[@]} ]
then
  echo "ERROR - wrong number of LiDAR files"
  exit 1
fi

# Create output batch file
echo "Create batch file ..."
outputFile=$1.sh
echo "#! /bin/bash" > $outputFile
echo "" >> $outputFile

# Copy the LiDAR files
echo "Copy LiDAR files ..."
echo "# Copy LiDAR files" >> $outputFile
for i in "${lidarFiles[@]}"; do
	echo "cp $lidarDataPath/$i$lidarFileExt ." >> $outputFile
done

# Download image files if necessary
if [ "$overlay" = true ]; then
	if [ "$builtinImage" = true ]; then
		echo "Copy the built in image files ..."
		echo "# Copy LiDAR image files" >> $outputFile
		for i in "${lidarFiles[@]}"; do
			echo "cp $lidarDataPath/$i$lidarFileBuiltinImageExt ." >> $outputFile
			imageFilesList+=($i$lidarFileBuiltinImageExt)
		done
	else
		# Fetch the OSM data
		echo "Fetch the OSM data ..."
		echo "# Fetch the OSM data" >> $outputFile

		# Fetch overall image from Open Street Map
		# wget -O target.osm 'http://overpass-api.de/api/map?bbox=-3.7476,51.8741,-3.7272,51.8843'
		echo "wget -O "$1".osm 'http://overpass-api.de/api/map?bbox="${bottomLeft[long]},${bottomLeft[lat]},${topRight[long]},${topRight[lat]}\'  >> $outputFile

		#Write the converter lines
		echo "Convert OSM file ..."
		echo "# Convert OSM file" >> $outputFile

		thisDir=`pwd`
		echo "cd "$osm2pngDir >> $outputFile
		echo "./osm2png.py "$thisDir"/"$1".osm -o "$thisDir"/"$1".png -z "$zoomLevel" -s "$(($xDim*$imageX))"x"$(($yDim*$imageY))" -r style.mapcss" >> $outputFile
		echo "cd "$thisDir >> $outputFile
		echo "convert "$1".png -crop "$imageX"x"$imageY" +repage %d.png" >> $outputFile
		# Build image file list
		for ((id=0;id<$(($xDim*$yDim));id++))
		{
			f=$id".png"
			imageFilesList+=($f)
		}
	fi

	# Convert the image files
	echo "Convert the image files ..."
  echo "# Convert the image files" >> $outputFile
	for i in "${imageFilesList[@]}"; do
		echo "convert "$i" "$i".txt" >> $outputFile
	done
fi

# Create the list file
echo "Creating the list file ..."

echo "# Created by build file on "`date` > plyListFile
for ((id=0;id<${#lidarFiles[@]};id++))
{
	echo ${lidarFiles[id]}$lidarFileExt" "${imageFilesList[id]}".txt" >> plyListFile
}

echo "# Run PLY list file" >> $outputFile
echo $lidar2plyExec" -l plyListFile -m" >> $outputFile
