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

enum CommandReturnCodes {
  Exit = -1,
  RunOnce = 0,
  Enqueue = 1,
};

class CommandBase {
 public:
  CommandBase(const char* mnemonic, const char* command_name,
              const char* help_string, const std::string& command)
      : mnemonic_(mnemonic),
        command_name_(command_name),
        help_string_(help_string),
        command_(command) {}

  virtual ~CommandBase() {}

  virtual int RunCommand() = 0;

 protected:
  std::string& command() { return command_; }

 private:
  std::string command_;
  std::string mnemonic_;
  std::string command_name_;
  std::string help_string_;
};

class DumpRegistersCommand : public CommandBase {
 public:
  DumpRegistersCommand(const std::string& command, RegFilePtr reg_file)
      : CommandBase("dr", "Dump Registers",
                    "Dump contents of specified registers. Specify individual "
                    "registers as space seperated list, a range using a dash "
                    "between registers. The default is to dump all registers",
                    command),
        reg_file_(reg_file) {}

  ~DumpRegistersCommand() override = default;

  int RunCommand() final;

 private:
  RegFilePtr reg_file_;
};

class DumpMemoryCommand : public CommandBase {
 public:
  DumpMemoryCommand(const std::string& command, MemoryPtr mem)
      : CommandBase("dm", "Dump Memory",
                    "Dump contents of main memory. The user may specify "
                    "address ranges by supplying two numbers. The default is "
                    "to dump all of it.",
                    command),
        mem_(mem) {}

  ~DumpMemoryCommand() override = default;

  int RunCommand() final;

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
  StepCommand(const std::string& command, CpuPtr cpu)
      : CommandBase("s", "Step n cycles", "Execute n cycles", command),
        cpu_(cpu) {}

  ~StepCommand() override = default;

  int RunCommand() final;

 private:
  CpuPtr cpu_;
};

class ContinueCommand : public CommandBase {
 public:
  ContinueCommand(const std::string& command, CpuPtr cpu)
      : CommandBase("c", "Continue Execution",
                    "Continue execution of simulation", command),
        cpu_(cpu) {}

  ~ContinueCommand() override = default;

  int RunCommand() final;

 private:
  CpuPtr cpu_;
};

class ResetCommand : public CommandBase {
 public:
  ResetCommand(const std::string& command, CpuPtr cpu)
      : CommandBase("r", "Reset simulation",
                    "Completely reset the simulation, and pause at beginning",
                    command),
        cpu_(cpu) {}

  ~ResetCommand() override = default;

  int RunCommand() final;

 private:
  CpuPtr cpu_;
};

class QuitCommand : public CommandBase {
 public:
  QuitCommand(const std::string& command)
      : CommandBase("q", "Quit the simulation", "Exit from the simulation",
                    command) {}

  ~QuitCommand() override = default;

  int RunCommand() final;
};

class SetBreakpointCommand : public CommandBase {
 public:
  SetBreakpointCommand(const std::string& command, CpuPtr cpu)
      : CommandBase("br", "Set a breakpoint",
                    "Pause execution when program reaches breakpoint address",
                    command),
        cpu_(cpu) {}

  ~SetBreakpointCommand() override = default;

  int RunCommand() final;

 private:
  CpuPtr cpu_;
};

class DeleteBreakpointCommand : public CommandBase {
 public:
  DeleteBreakpointCommand(const std::string& command, CpuPtr cpu)
      : CommandBase("del", "Delete a breakpoint", "Remove specified breakpoint",
                    command),
        cpu_(cpu) {}

  ~DeleteBreakpointCommand() override = default;

  int RunCommand() final;

 private:
  CpuPtr cpu_;
};

class ShowBreakpointsCommand : public CommandBase {
 public:
  ShowBreakpointsCommand(const std::string& command, CpuPtr cpu)
      : CommandBase("sbr", "Show all breakpoints",
                    "Lists all active breakpoints ", command),
        cpu_(cpu) {}

  ~ShowBreakpointsCommand() override = default;

  int RunCommand() final;

 private:
  CpuPtr cpu_;
};
