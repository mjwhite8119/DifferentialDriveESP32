#pragma once
#include <array>
#include <vector>

typedef std::tuple<double, double, double, double, double, double, double> logEntry;
typedef std::array <logEntry, 1200> LogArray;
typedef std::vector<std::vector<int> > intVector2d;
typedef std::vector<int> intVector;

typedef float DeltaTimeSeconds;  // In seconds
typedef float DistanceMeters;
typedef float MetersPerSecond;
typedef float MetersPerSecondSquared;

typedef float HeadingDegrees;
typedef float HeadingRadians;
typedef float RadiansPerSecond;
typedef float DegreesPerSecond;

typedef float DutyCycle; // Wheel speed distributed from -1.0 (min) to 1.0 (max)
typedef int EncoderPulsesPerSecond; // Encoder pulses per second

typedef float Seconds;
typedef float MilliSeconds;

struct GridLocation {
  int x;
  int y; 
};

inline bool operator==(const GridLocation& lhs, const GridLocation& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}
inline bool operator!=(const GridLocation& lhs, const GridLocation& rhs) {
  return !operator==(lhs, rhs);
}

const int maxGridSize = 255;

typedef std::vector<GridLocation> GridLocations;

const int blocked = 254;
const int boundary = 255;
const int badPath = 99;
const int startValue = 98;