/*********************************************************************
 * \file   InstrumentControl.h
 * \brief  Header file for the InstrumentControl class
 *
 * \author Piotr
 * \date   March 2024
 *********************************************************************/
#pragma once

#include <cstdbool>
#include <cstring>
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <tuple>
#include <vector>
#include <visa.h>
#include <visatype.h>

constexpr size_t BUFFER_SIZE_B = 8000;

#ifdef _WIN32
#define ViRsrc ViConstRsrc
#define ViBuf ViConstBuf
#endif

namespace InstrumentControl {
class InstrumentControl {
  /*
   * PRIVATE VARIABLES BEGIN
   */
private:
  ViSession instrument;
  ViChar buffer[BUFFER_SIZE_B] = {0};
  ViUInt32 io_bytes;
  const ViAccessMode access_mode = VI_NULL;
  const ViUInt32 timeout_ms = 200;

  std::string resource_string;
  std::vector<ViChar> ID_string;
  ViSession resource_manager;
  ViStatus status;
  /*
   * PRIVATE VARIABLES END
   */

  /*
   * PRIVATE METHODS BEGIN
   */
private:
  void SetResourceString(ViChar ResourceString[]);
  bool ReadIDString();
  void SetIDString(ViChar IDString[]);
  /*
   * PRIVATE METHODS END
   */

  /*
   * PUBLIC METHODS BEGIN
   */
public:
  InstrumentControl();
  ~InstrumentControl();

  ViRsrc GetResourceString();
  std::string GetIDString();

  bool Connect(ViChar ResourceString[]);
  bool Disconnect();

  std::tuple<bool, ViChar *> Query(const char *scpi_command);
  bool Write(const char *scpi_command);
  std::tuple<bool, ViChar *> Read();
  ViStatus ViClear();

  /*
   * PUBLIC METHODS END
   */
}; // class InstrumentControl
} // namespace InstrumentControl
