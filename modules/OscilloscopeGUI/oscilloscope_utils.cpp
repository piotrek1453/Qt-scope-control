#include "oscilloscope_utils.h"

namespace oscilloscope_utils {
// Define the set of valid SI exponents for SI prefixes (e.g., 10^3 for kilo,
// 10^-6 for micro)
static const std::set<int> validExponents =
    {-24, -21, -18, -15, -12, -9, -6, -3, 0, 3, 6, 9, 12, 15, 18, 21, 24};

std::tuple<double, int> convertMeasurementResult(const std::string &input) {
  // Regular expression to match scientific notation numbers
  // (e.g., 1.23e-4, 3.45E+6, etc.)
  std::regex regex(R"(([-+]?\d*\.?\d+)([eE][-+]?\d+))");
  std::smatch match;

  if (std::regex_search(input, match, regex)) {
    try {
      // Extract the floating-point number and exponent
      double number =
          std::stod(match[1].str()); // First part: the floating-point number
      int exponent = std::stoi(match[2].str().substr(
          1)); // Second part: the exponent, skip 'e' or 'E'
      if (match[2].str()[0] == '-') {
        exponent = -exponent;
      }

      // Check if the exponent is a valid SI prefix
      while (validExponents.find(exponent) == validExponents.end()) {
        exponent -= 1;
        number *= 10;
      }

      return std::make_tuple(number, exponent); // Return the tuple with valid
                                                // number and exponent
    } catch (const std::exception &e) {
      std::cerr << "Error extracting number or exponent: " << e.what()
                << std::endl;
      throw; // Rethrow exception
    }
  } else {
    throw std::invalid_argument("No valid scientific notation found.");
  }
}

std::string convertExponentToSI(const int exponent) {
  switch (exponent) {
  case -15:
    return "f"; // femto
  case -12:
    return "p"; // pico
  case -9:
    return "n"; // nano
  case -6:
    return "u"; // micro
  case -3:
    return "m"; // milli
  case 0:
    return ""; // no prefix
  case 3:
    return "k"; // kilo
  case 6:
    return "M"; // mega
  case 9:
    return "G"; // giga
  case 12:
    return "T"; // tera
  case 15:
    return "P"; // peta
  default:
    return ""; // no valid prefix
  }
}

int convertSIToExponent(std::string si) {
  const std::unordered_map<char, int> si_prefixes = {{'y', -24},
                                                     {'z', -21},
                                                     {'a', -18},
                                                     {'f', -15},
                                                     {'p', -12},
                                                     {'n', -9},
                                                     {'u', -6},
                                                     {'m', -3},
                                                     {'c', -2},
                                                     {'d', -1},
                                                     {'k', 3},
                                                     {'M', 6},
                                                     {'G', 9},
                                                     {'T', 12},
                                                     {'P', 15},
                                                     {'E', 18},
                                                     {'Z', 21},
                                                     {'Y', 24}};

  const char prefix = si[0];

  // Find and return the exponent for the prefix
  auto it = si_prefixes.find(prefix);
  if (it != si_prefixes.end()) {
    return it->second; // exponent
  }

  // If no prefix found assume exponent is 0
  return 0;
}

std::string viCharArrToString(const ViChar *ptr) {
  std::string str = ptr;
  return str;
}
} // namespace oscilloscope_utils
