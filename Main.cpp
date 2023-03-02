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
			Elf elf("sample_library");
			//Elf elf("dsp_lib_example_lib_mtl_release");
			elf.print();
		}

	}
	catch (std::exception& err) {
		//::SetConsoleOutputCP(CP_UTF8);
		printf("Error: %s\n", err.what());
	}

	return 0;
}
