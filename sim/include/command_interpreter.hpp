#pragma once

#include <csignal>
#include <iostream>
#include <list>
#include <string>

#include <commands.hpp>
#include <cpu.hpp>
#include <memory.hpp>

class CommandInterpreter {
 public:
  CommandInterpreter(CpuPtr cpu, MemoryPtr mem,
                     std::istream& cmd_stream = std::cin)
      : cmd_stream_(cmd_stream),
        cpu_(cpu),
        mem_(mem),
        command_factory_(cpu, mem) {}

  void MainLoop();

 private:
  class CommandFactory {
   public:
    CommandFactory(CpuPtr cpu, MemoryPtr mem) : cpu_(cpu), mem_(mem) {}

    enum Commands {
      Command_DumpRegisters,
      Command_DumpMemory,
      Command_DumpCache,
      Command_Continue,
      Command_Step,
      Command_Reset,
      Command_Quit,
      Command_SetBreakpoint,
      Command_DeleteBreakpoint,
      Command_ShowBreakpoints,
    };

    CommandPtr Create(std::string command_string);

   private:
    CpuPtr cpu_;
    MemoryPtr mem_;
  };

  CommandFactory command_factory_;
  using CommandQueue = std::list<CommandPtr>;
  CommandQueue command_queue_;

  std::istream& cmd_stream_;
  CpuPtr cpu_;
  MemoryPtr mem_;

  const std::string menu_string_{
      "\n\tdr [registers] : dump register contents\n"
      "\tdm [adress_range] : dump memory contents\n"
      "\tdc [line range] : dump cache contents\n"
      "\tc : continue (runs program til completion)\n"
      "\ts [number cycles] : execute some number of cycles\n"
      "\tr : reset simulation\n"
      "\tq: quit\n"
      "\tbr [address] : set breakpoint\n"
      "\tdel [breakpoint number] : delete breakpoint\n"
      "\tsbr : show all breakpoints\n\n"};
};
