/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include <iostream>

#include "../include/AndInverterGraph.h"
#include "../include/CutEngine.h"
#include "../include/TechMapper.h"

int
main (int argc, char *argv[])
try
  {
    // Basic parameter processing
    int k = 6;
    int c = 0;
    std::string inputFile = "";
    MappingGoal mg = MappingGoal::MinimizeArea;
    if (argc > 1)
      inputFile = argv[1];
    if (argc > 2)
      k = std::atoi (argv[2]);
    if (argc > 3)
      c = std::atoi (argv[3]);
    if (argc > 4 && argv[4][0] == 'd')
      mg = MappingGoal::MinimizeDelay;

    // Only go ahead if inputFile is provided
    if (!inputFile.empty ())
      {
        AndInverterGraph aig (inputFile);
        CutEngine cutEngine (aig, mg, k, c);
        TechMapper techMapper (cutEngine);
        techMapper.run ();
        techMapper.printResults (std::cout);
        techMapper.printImplementation (std::cout);
        std::cout << cutEngine << std::endl;
        cutEngine.printImplementation (std::cout);
      }

    return 0;
  }
catch (const std::exception &e)
  {
    std::cerr << "An error has ocurred.\n  what(): " << e.what () << std::endl;
  }