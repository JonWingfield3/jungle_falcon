#pragma once

#include <csignal>
#include <iostream>
#include <list>
#include <string>

#include <commands.hpp>
#include <cpu.hpp>
#include <hazard_detection.hpp>
#include <memory.hpp>

class CommandInterpreter {
 public:
  CommandInterpreter(CpuPtr cpu, MemoryPtr mem,
                     std::istream& cmd_stream = std::cin);

  void MainLoop();

 private:
  class CommandFactory {
   public:
    CommandFactory(CpuPtr cpu, MemoryPtr mem) : cpu_(cpu), mem_(mem) {}

    enum Commands {
      Command_Help,
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
      Command_Stats,
    };

    CommandPtr Create(std::string command_string);

   private:
    CpuPtr cpu_;
    MemoryPtr mem_;
  };

  std::istream& cmd_stream_;
  CpuPtr cpu_;
  MemoryPtr mem_;
  CommandFactory command_factory_;
  HazardDetectionPtr data_hazard_unit_;
  HazardDetectionPtr control_hazard_unit_;

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
      "\tsbr : show all breakpoints\n"
      "\tstat: show sim stats\n\n"};
};
