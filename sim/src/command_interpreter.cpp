#include <command_interpreter.hpp>

#include <climits>

#include <commands.hpp>

void CommandInterpreter::MainLoop() {
  std::cout << "Beginning RISCV simulator (a.k.a. jungle_falcon)" << std::endl;
  int return_val = 0;
  do {
    return_val = 0;
    std::cout << menu_string_;
    std::string input_str = "";
    std::cout << "\n>>> ";
    std::cin >> input_str;

    for (auto enqueued_command : command_queue_) {
      enqueued_command->RunCommand();
    }

    CommandPtr command = command_factory_.Create(input_str);
    if (command != nullptr) {
      return_val = command->RunCommand();
    } else {
      VLOG(2) << "Unrecognized command: " << input_str;
    }

    if (return_val == Enqueue) {
      command_queue_.push_back(command);
    }

    std::cin.clear();
    std::cin.ignore(INT_MAX, '\n');

    std::cout << std::endl;
  } while (return_val != Exit);
}

CommandPtr CommandInterpreter::CommandFactory::Create(
    std::string command_string) {
  const std::size_t space_index =
      std::min(command_string.find(" ") - 1, command_string.length());
  const std::string command_op_string = command_string.substr(0, space_index);

  VLOG(5) << "Command Op String: " << command_op_string;

  std::map<std::string, Commands> COMMANDS{
      {std::string("dr"), Command_DumpRegisters},
      {std::string("dm"), Command_DumpMemory},
      {std::string("c"), Command_Continue},
      {std::string("s"), Command_Step},
      {std::string("r"), Command_Reset},
      {std::string("q"), Command_Quit},
      {std::string("br"), Command_SetBreakpoint},
      {std::string("del"), Command_DeleteBreakpoint},
      {std::string("sbr"), Command_ShowBreakpoints}};

  // Check if command is valid
  const auto ite = COMMANDS.find(command_op_string);
  if (ite == COMMANDS.end()) {
    return nullptr;
  }

  const Commands command_id = COMMANDS[command_op_string];
  VLOG(5) << "Command ID: " << static_cast<int>(command_id);

  switch (command_id) {
    case Command_DumpRegisters:
      return std::make_shared<DumpRegistersCommand>(
          DumpRegistersCommand(command_string, cpu_->RegFile()));
    case Command_DumpMemory:
      return std::make_shared<DumpMemoryCommand>(
          DumpMemoryCommand(command_string, mem_));
    case Command_Continue:
      return std::make_shared<ContinueCommand>(
          ContinueCommand(command_string, cpu_));
    case Command_Step:
      return std::make_shared<StepCommand>(StepCommand(command_string, cpu_));
    case Command_Reset:
      return std::make_shared<ResetCommand>(ResetCommand(command_string, cpu_));
    case Command_Quit:
      return std::make_shared<QuitCommand>(QuitCommand(command_string));
    case Command_SetBreakpoint:
      return std::make_shared<SetBreakpointCommand>(
          SetBreakpointCommand(command_string, cpu_));
    case Command_DeleteBreakpoint:
      return std::make_shared<DeleteBreakpointCommand>(
          DeleteBreakpointCommand(command_string, cpu_));
    case Command_ShowBreakpoints:
      return std::make_shared<ShowBreakpointsCommand>(
          ShowBreakpointsCommand(command_string, cpu_));
  }
}
