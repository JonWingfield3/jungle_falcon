#include <commands.hpp>

#include <climits>

#include <command_interpreter.hpp>
#include <register_file.hpp>

int DumpRegistersCommand::RunCommand() {
  std::vector<RegisterFile::Register> registers;
  for (int ii = 0; ii < RegisterFile::NumCPURegisters; ++ii) {
    registers.push_back(static_cast<RegisterFile::Register>(ii));
  }
  reg_file_->DumpRegisters(registers);
  return RunOnce;
}

int DumpMemoryCommand::RunCommand() {
  int start_address = 0;
  int stop_address = 0;
  std::cin >> std::hex >> start_address >> stop_address;
  mem_->CoreDump(start_address, stop_address);
  return RunOnce;
}

int StepCommand::RunCommand() {
  int steps = 0;
  std::cin >> steps;
  cpu_->ExecuteCycle(steps);
  return RunOnce;
}

int ContinueCommand::RunCommand() {
  cpu_->ExecuteCycle(INT_MAX);
  return RunOnce;
}

int ResetCommand::RunCommand() {
  cpu_->Reset();
  return RunOnce;
}

int QuitCommand::RunCommand() { return Exit; }

int SetBreakpointCommand::RunCommand() {
  int address = 0;
  std::cin >> std::hex >> address;
  cpu_->SetBreakpoint(address);
  return RunOnce;
}

int DeleteBreakpointCommand::RunCommand() {
  int bkpt_num;
  std::cin >> bkpt_num;
  cpu_->DeleteBreakpoint(bkpt_num);
  return RunOnce;
}

int ShowBreakpointsCommand::RunCommand() {
  const auto breakpoints = cpu_->GetBreakpoints();
  if (breakpoints.empty()) {
    std::cout << "No breakpoints" << std::endl;
  } else {
    for (const auto breakpoint : breakpoints) {
      std::cout << "Breakpoint " << breakpoint.first << " : " << std::hex
                << std::showbase << breakpoint.second;
    }
  }
  return RunOnce;
}
