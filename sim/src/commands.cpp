#include <commands.hpp>

#include <climits>

#include <command_interpreter.hpp>
#include <register_file.hpp>

////////////////////////////////////////////////////////////////////////////////
CommandBase::CommandBase(const char* mnemonic, const char* command_name,
                         const char* help_string, const std::string& command)
    : mnemonic_(mnemonic),
      command_name_(command_name),
      help_string_(help_string),
      command_(command) {}

////////////////////////////////////////////////////////////////////////////////
NoActionCommand::NoActionCommand(const std::string& command)
    : CommandBase("", "No Action", "Performs no actions.", command) {}

////////////////////////////////////////////////////////////////////////////////
DumpRegistersCommand::DumpRegistersCommand(const std::string& command,
                                           RegFilePtr reg_file)
    : CommandBase("dr", "Dump Registers",
                  "Dump contents of specified registers. Specify individual "
                  "registers as space seperated list, a range using a dash "
                  "between registers. The default is to dump all registers",
                  command),
      reg_file_(reg_file) {}

////////////////////////////////////////////////////////////////////////////////
void DumpRegistersCommand::RunCommand() { reg_file_->DumpRegisters(); }

////////////////////////////////////////////////////////////////////////////////
DumpMemoryCommand::DumpMemoryCommand(const std::string& command, MemoryPtr mem)
    : CommandBase("dm", "Dump Memory",
                  "Dump contents of main memory. The user may specify "
                  "address ranges by supplying two numbers. The default is "
                  "to dump all of it.",
                  command),
      mem_(mem) {}

////////////////////////////////////////////////////////////////////////////////
void DumpMemoryCommand::RunCommand() {
  int start_address = 0;
  int stop_address = 0;
  std::cin >> std::hex >> start_address >> stop_address;
  mem_->CoreDump(start_address, stop_address);
}

////////////////////////////////////////////////////////////////////////////////
StepCommand::StepCommand(const std::string& command, CpuPtr cpu)
    : CommandBase("s", "Step n cycles", "Execute n cycles", command),
      cpu_(cpu) {}

////////////////////////////////////////////////////////////////////////////////
void StepCommand::RunCommand() {
  int steps = 0;
  std::cin >> steps;
  cpu_->ExecuteCycle(steps);
}

////////////////////////////////////////////////////////////////////////////////
ContinueCommand::ContinueCommand(const std::string& command, CpuPtr cpu)
    : CommandBase("c", "Continue Execution", "Continue execution of simulation",
                  command),
      cpu_(cpu) {}

////////////////////////////////////////////////////////////////////////////////
void ContinueCommand::RunCommand() { cpu_->ExecuteCycle(INT_MAX); }

////////////////////////////////////////////////////////////////////////////////
ResetCommand::ResetCommand(const std::string& command, CpuPtr cpu)
    : CommandBase("r", "Reset simulation",
                  "Completely reset the simulation, and pause at beginning",
                  command),
      cpu_(cpu) {}

////////////////////////////////////////////////////////////////////////////////
void ResetCommand::RunCommand() { cpu_->Reset(); }

////////////////////////////////////////////////////////////////////////////////
SetBreakpointCommand::SetBreakpointCommand(const std::string& command,
                                           CpuPtr cpu)
    : CommandBase("br", "Set a breakpoint",
                  "Pause execution when program reaches breakpoint address",
                  command),
      cpu_(cpu) {}

////////////////////////////////////////////////////////////////////////////////
void SetBreakpointCommand::RunCommand() {
  int address = 0;
  std::cin >> std::hex >> address;
  cpu_->SetBreakpoint(address);
}

////////////////////////////////////////////////////////////////////////////////
DeleteBreakpointCommand::DeleteBreakpointCommand(const std::string& command,
                                                 CpuPtr cpu)
    : CommandBase("del", "Delete a breakpoint", "Remove specified breakpoint",
                  command),
      cpu_(cpu) {}

////////////////////////////////////////////////////////////////////////////////
void DeleteBreakpointCommand::RunCommand() {
  int bkpt_num;
  std::cin >> bkpt_num;
  cpu_->DeleteBreakpoint(bkpt_num);
}

////////////////////////////////////////////////////////////////////////////////
ShowBreakpointsCommand::ShowBreakpointsCommand(const std::string& command,
                                               CpuPtr cpu)
    : CommandBase("sbr", "Show all breakpoints",
                  "Lists all active breakpoints ", command),
      cpu_(cpu) {}

////////////////////////////////////////////////////////////////////////////////
void ShowBreakpointsCommand::RunCommand() {
  const auto& breakpoints = cpu_->GetBreakpoints();
  if (breakpoints.empty()) {
    std::cout << "No breakpoints" << std::endl;
  } else {
    for (const auto& breakpoint : breakpoints) {
      std::cout << "Breakpoint " << breakpoint.first << " : " << std::hex
                << std::showbase << breakpoint.second;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
ShowStatsCommand::ShowStatsCommand(const std::string& command, CpuPtr cpu,
                                   HazardDetectionPtr data_hazard_unit,
                                   HazardDetectionPtr control_hazard_unit)
    : CommandBase(
          "stat", "Show CPU sim stats (cycles, instructions, CPI, etc.)",
          "Displays useful metrics to evaluate sim's performance", command),
      cpu_(cpu),
      data_hazard_unit_(data_hazard_unit),
      control_hazard_unit_(control_hazard_unit) {}

////////////////////////////////////////////////////////////////////////////////
void ShowStatsCommand::RunCommand() {
  std::cout << "Cycles Executed: " << cpu_->GetCycles() << std::endl
            << "Instructions Executed: "
            << cpu_->PipeLine()->InstructionsCompleted() << std::endl
            << "CPI: " << cpu_->GetCPI() << std::endl
            << "Data Hazards: " << data_hazard_unit_->HazardsDetected()
            << std::endl
            << "Delay added from data hazards: "
            << data_hazard_unit_->DelayAdded() << std::endl
            << "Control Hazards: " << control_hazard_unit_->HazardsDetected()
            << std::endl
            << "Delay added from control hazards: "
            << control_hazard_unit_->DelayAdded() << std::endl;
}
