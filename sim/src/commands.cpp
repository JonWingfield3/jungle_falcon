#include <commands.hpp>

#include <climits>

#include <command_interpreter.hpp>
#include <register_file.hpp>

void DumpRegistersCommand::RunCommand() {
  std::vector<RegisterFile::Registers> registers;
  for (int ii = 0; ii < RegisterFile::NumCPURegisters; ++ii) {
    registers.push_back(static_cast<RegisterFile::Registers>(ii));
  }
  reg_file_->DumpRegisters(registers);
}

void DumpMemoryCommand::RunCommand() {
  int start_address = 0;
  int stop_address = 0;
  std::cin >> std::hex >> start_address >> stop_address;
  mem_->CoreDump(start_address, stop_address);
}

void StepCommand::RunCommand() {
  int steps = 0;
  std::cin >> steps;
  cpu_->ExecuteCycle(steps);
}

void ContinueCommand::RunCommand() { cpu_->ExecuteCycle(INT_MAX); }

void ResetCommand::RunCommand() { cpu_->Reset(); }

void SetBreakpointCommand::RunCommand() {
  int address = 0;
  std::cin >> std::hex >> address;
  cpu_->SetBreakpoint(address);
}

void DeleteBreakpointCommand::RunCommand() {
  int bkpt_num;
  std::cin >> bkpt_num;
  cpu_->DeleteBreakpoint(bkpt_num);
}

void ShowBreakpointsCommand::RunCommand() {
  const auto breakpoints = cpu_->GetBreakpoints();
  if (breakpoints.empty()) {
    std::cout << "No breakpoints" << std::endl;
  } else {
    for (const auto breakpoint : breakpoints) {
      std::cout << "Breakpoint " << breakpoint.first << " : " << std::hex
                << std::showbase << breakpoint.second;
    }
  }
}
