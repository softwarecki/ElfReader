// SPDX-License-Identifier: BSD-3-Clause
//
// Copyright(c) 2023 Koko Software. All rights reserved.
//
// Author: Adrian Warecki <embedded@kokosoftware.pl>

#include <iostream>
#include "Elf.hpp"

int main(int argc, char** argv) {
	try {

		std::cout << "Hello World! " << argc << "\n";
		if (1)
		{
			elf::Elf elf("sample_library");
			//Elf elf("dsp_lib_example_lib_mtl_release");
			//elf.print();

			elf::StringsTable strtab;
			elf.read_section(strtab, ".strtab");

			elf::SymbolTable symtab;
			elf.read_section(symtab, ".symtab");

			strtab.print();
			symtab.print(&strtab);
		}

	}
	catch (std::exception& err) {
		//::SetConsoleOutputCP(CP_UTF8);
		printf("Error: %s\n", err.what());
	}

	return 0;
}
