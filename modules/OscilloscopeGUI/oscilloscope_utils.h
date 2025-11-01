#pragma once
#include <math.h>
#include <string>
#include <visa.h>

namespace oscilloscope_utils {
std::tuple<double, int> convertMeasurementResult(const std::string &input);
std::string convertExponentToSI(const int exponent);
int convertSIToExponent(std::string si);
std::string viCharArrToString(const ViChar *);
}; // namespace oscilloscope_utils

// OSCILLOSCOPE_UTILS_H
