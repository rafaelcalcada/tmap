/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "AndInverterGraph.h"
#include "CutEngine.h"

class TechMapper
{
public:
  TechMapper () = delete;

  /**
   * @brief Construct a new TechMapper from a CutEngine object
   *
   * @param cutEngine
   */
  TechMapper (CutEngine &cutEngine);

  /**
   * @brief Runs FPGA technology mapping.
   *
   * This method uses the CutEngine object (passed as argument during
   * construction of the TechMapper object) to find the best implementation of
   * the AndInverterGraph with K-input lookup-tables.
   *
   */
  void run ();

  /**
   * @brief Print the mapping results to a C++ output stream
   *
   * @param os A std::ostream object
   */
  void printResults (std::ostream &os);

  /**
   * @brief Print the implementation to a C++ output stream
   *
   * @param os A std::ostream object
   */
  void printImplementation (std::ostream &os);

private:
  unsigned int _mappingAreaCost = 0;
  unsigned int _mappingDelayCost = 0;
  unsigned int _mappingPowerCost = 0;
  std::map<unsigned int, bool> _implementationMap = {};
  const AndInverterGraph &_aig;
  CutEngine &_cutEngine;
};