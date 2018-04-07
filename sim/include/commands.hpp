#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <cpu.hpp>
#include <pipeline.hpp>
#include <register_file.hpp>

class CommandBase;
using CommandPtr = std::shared_ptr<CommandBase>;

class CommandBase {
 public:
  CommandBase(const char* mnemonic, const char* command_name,
              const char* help_string, const std::string& command);
  virtual ~CommandBase() {}

  virtual void RunCommand() = 0;

 protected:
  std::string& command() { return command_; }

 private:
  std::string mnemonic_;
  std::string command_name_;
  std::string help_string_;
  std::string command_;
};

class NoActionCommand : public CommandBase {
 public:
  NoActionCommand(const std::string& command);
  ~NoActionCommand() override = default;

  void RunCommand() final {}
};

class DumpRegistersCommand : public CommandBase {
 public:
  DumpRegistersCommand(const std::string& command, RegFilePtr reg_file);
  ~DumpRegistersCommand() override = default;

  void RunCommand() final;

 private:
  RegFilePtr reg_file_;
};

class DumpMemoryCommand : public CommandBase {
 public:
  DumpMemoryCommand(const std::string& command, MemoryPtr mem);
  ~DumpMemoryCommand() override = default;

  void RunCommand() final;

 private:
  MemoryPtr mem_;
};

/*
class DumpCacheCommand : public CommandBase {
 public:
  DumpCacheCommand(const std::string command) : CommandBase() {}
  ~DumpCacheCommand() override = default;

  void RunCommand() final;

 private:
};
*/

class StepCommand : public CommandBase {
 public:
  StepCommand(const std::string& command, CpuPtr cpu);
  ~StepCommand() override = default;

  void RunCommand() final;

 private:
  CpuPtr cpu_;
};

class ContinueCommand : public CommandBase {
 public:
  ContinueCommand(const std::string& command, CpuPtr cpu);
  ~ContinueCommand() override = default;

  void RunCommand() final;

 private:
  CpuPtr cpu_;
};

class ResetCommand : public CommandBase {
 public:
  ResetCommand(const std::string& command, CpuPtr cpu);
  ~ResetCommand() override = default;

  void RunCommand() final;

 private:
  CpuPtr cpu_;
};

class SetBreakpointCommand : public CommandBase {
 public:
  SetBreakpointCommand(const std::string& command, CpuPtr cpu);
  ~SetBreakpointCommand() override = default;

  void RunCommand() final;

 private:
  CpuPtr cpu_;
};

class DeleteBreakpointCommand : public CommandBase {
 public:
  DeleteBreakpointCommand(const std::string& command, CpuPtr cpu);
  ~DeleteBreakpointCommand() override = default;

  void RunCommand() final;

 private:
  CpuPtr cpu_;
};

class ShowBreakpointsCommand : public CommandBase {
 public:
  ShowBreakpointsCommand(const std::string& command, CpuPtr cpu);
  ~ShowBreakpointsCommand() override = default;

  void RunCommand() final;

 private:
  CpuPtr cpu_;
};

class ShowStatsCommand : public CommandBase {
 public:
  ShowStatsCommand(const std::string& command, CpuPtr cpu,
                   HazardDetectionPtr data_hazard_unit,
                   HazardDetectionPtr control_hazard_unit);
  ~ShowStatsCommand() override = default;

  void RunCommand() final;

 private:
  CpuPtr cpu_;
  HazardDetectionPtr data_hazard_unit_;
  HazardDetectionPtr control_hazard_unit_;
};
