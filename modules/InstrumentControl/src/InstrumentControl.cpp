/*********************************************************************
 * \file   InstrumentControl.cpp
 * \brief Definition of InstrumentControl class
 *
 * \author Piotr
 * \date   March 2024
 *********************************************************************/

#include "InstrumentControl.hpp"

namespace InstrumentControl {
InstrumentControl::InstrumentControl() {}

InstrumentControl::~InstrumentControl() {
  this->Disconnect();
}

/*
 *   PRIVATE METHODS BEGIN
 */
void InstrumentControl::SetResourceString(ViChar ResourceString[]) {
  this->resource_string = ResourceString;
  spdlog::info("Resource string set to {}", GetResourceString());
}

bool InstrumentControl::ReadIDString() {
  std::tuple<bool, ViChar *> temp = Query("*IDN?");
  SetIDString(std::get<1>(temp));

  return true;
}

void InstrumentControl::SetIDString(ViChar IDString[]) {
  this->ID_string.insert(this->ID_string.end(),
                         IDString,
                         IDString + std::strlen(IDString));
  spdlog::info("ID string set to {}", IDString);
}
/*
 *   PRIVATE METHODS END
 */

/*
 * PUBLIC METHODS BEGIN
 */
ViRsrc InstrumentControl::GetResourceString() {
  return &resource_string[0];
}

std::string InstrumentControl::GetIDString() {
  return std::string(this->ID_string.begin(), this->ID_string.end());
}

bool InstrumentControl::Connect(ViChar ResourceString[]) {
  SetResourceString(ResourceString); // set instrument resource string
  this->status =
      viOpenDefaultRM(&this->resource_manager); // open VISA resource manager
  if (this->status < VI_SUCCESS) {
    spdlog::error("Could not open VISA resource manager.\n{}", this->status);
    return false;
  }

  spdlog::info("Resource manager opened: {}", this->resource_manager);

  this->status = viOpen(this->resource_manager,
                        this->GetResourceString(),
                        this->access_mode,
                        this->timeout_ms,
                        &this->instrument); // connect to instrument
  if (this->status < VI_SUCCESS) {
    viStatusDesc(this->resource_manager, this->status, this->buffer);
    spdlog::error("Error connecting to instrument {}:\n{}\n{}",
                  ResourceString,
                  this->status,
                  this->buffer);
    return false;
  }

  // set timeout on instrument IO
  viSetAttribute(this->instrument, VI_ATTR_TMO_VALUE, this->timeout_ms);

  this->status = ViClear(); // clear session just to make sure

  spdlog::info("Instrument {} connected!", this->GetResourceString());

  InstrumentControl::ReadIDString();

  return true;
}

bool InstrumentControl::Disconnect() {
  this->status = viClose(this->instrument);
  if (this->status < VI_SUCCESS) {
    ;
    viStatusDesc(this->resource_manager, this->status, this->buffer);
    spdlog::error("Error disconnecting instrument:\n{}\n{}",
                  this->status,
                  this->buffer);
    return false;
  }

  spdlog::info("Instrument disconnected succesfully!");
  return true;
}

std::tuple<bool, ViChar *> InstrumentControl::Query(const char *scpi_command) {
  std::tuple<bool, ViChar *> temp;
  Write(scpi_command);
  temp = Read();
  spdlog::info("Query completed!");
  return temp;
}

bool InstrumentControl::Write(const char *scpi_command) {
  // write command
  this->status = viWrite(this->instrument,
                         (ViBuf)scpi_command,
                         (ViUInt32)std::strlen(scpi_command),
                         &this->io_bytes);
  if (this->status < VI_SUCCESS) {
    viStatusDesc(this->resource_manager, this->status, this->buffer);
    spdlog::error("Error writing to instrument. Command: {}\n{}\n{}",
                  scpi_command,
                  this->status,
                  this->buffer);
    return false;
  }
  spdlog::info("Write succesful! Command: {}", scpi_command);
  return true;
}

std::tuple<bool, ViChar *> InstrumentControl::Read() {
  // read response
  this->status = viRead(this->instrument,
                        (ViPBuf)this->buffer,
                        BUFFER_SIZE_B,
                        &this->io_bytes);
  if (this->status < VI_SUCCESS) {
    viStatusDesc(this->resource_manager, this->status, this->buffer);
    spdlog::error("Error reading response from instrument:\n{}\n{}",
                  this->status,
                  this->buffer);
    return {false, this->buffer};
  }

  spdlog::info("Read succesful! Returned value: {}", this->buffer);
  return {true, this->buffer};
}

ViStatus InstrumentControl::ViClear() {
  ViStatus status = viClear(this->resource_manager);
  spdlog::info("VI clear status: {}", this->status);
  return status;
}

/*
 * PUBLIC METHODS END
 */
} // namespace InstrumentControl
