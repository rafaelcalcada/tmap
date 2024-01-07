/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/TechMapper.h"

TechMapper::TechMapper (CutEngine &cutEngine)
    : _aig (cutEngine.getAndInverterGraph ()), _cutEngine (cutEngine)
{
  // Memory allocation and vector initialization
  try
    {
      _mappingAreaCost = 0;
      _mappingDelayCost = 0;
      _mappingPowerCost = 0;
      _implementationMap.clear ();
      unsigned int firstAndVariable
          = AndInverterGraph::indexFromLiteral (_aig.getFirstAndLiteral ());
      unsigned int lastAndVariable = firstAndVariable + _aig.getNumAnds ();
      for (int i = firstAndVariable; i < lastAndVariable; i++)
        _implementationMap.emplace (AndInverterGraph::literalFromIndex (i),
                                    false);
    }
  catch (const std::exception &e)
    {
      throw std::runtime_error ("Failed to initialize TechMapper object from '"
                                + _aig.getFilePath ()
                                + "'.\n what(): " + e.what ());
    }
}

void
TechMapper::run ()
{
  // Iterates over all AIG outputs
  for (const auto &outputLiteral : _aig.getOutputLiteralVector ())
    {
      // The output is an and-node
      if (_aig.nodeIsAnd (outputLiteral))
        {
          // Sometimes the output literal is inverted (odd number)
          unsigned int evenOutputLiteral = outputLiteral;
          if (evenOutputLiteral % 2 == 1)
            evenOutputLiteral--;

          // The output is not yet implemented
          if (_implementationMap[evenOutputLiteral] == false)
            {
              // Find K-feasible cuts for the output
              _cutEngine.findCuts (outputLiteral);

              // Set as implemented
              _implementationMap[evenOutputLiteral] = true;

              // Update mapping area cost
              _mappingAreaCost++;

              // Update mapping delay cost
              const Cut &outputBestCut
                  = _cutEngine.getBestCut (evenOutputLiteral);
              unsigned int maxDepth = outputBestCut.getDelayCost ();
              _mappingDelayCost = _mappingDelayCost < maxDepth
                                      ? maxDepth
                                      : _mappingDelayCost;

              // Get all and-nodes in the best cut of outputLiteral
              std::set<unsigned int> currentIteration;
              for (const auto &nodeIndex : outputBestCut)
                if (_aig.nodeIsAnd (
                        AndInverterGraph::literalFromIndex (nodeIndex)))
                  currentIteration.emplace (
                      AndInverterGraph::literalFromIndex (nodeIndex));

              // Check if some of the and-nodes are unimplemented
              // If so, mark the and-node as implemented and check whether the
              // the and-nodes in its best cut is implemented, iteratively
              while (!currentIteration.empty ())
                {
                  // Prepares a set for the next iteration
                  std::set<unsigned int> nextIteration;

                  // For each unimplemented and-node
                  for (const auto &nodeLiteral : currentIteration)
                    if (_implementationMap[nodeLiteral] == false)
                      {
                        // Mark it as implemented and update area cost
                        _implementationMap[nodeLiteral] = true;
                        _mappingAreaCost++;

                        // Add all its and-nodes to be checked in the next
                        // iteration
                        const Cut &nodeLiteralBestCut
                            = _cutEngine.getBestCut (nodeLiteral);
                        for (const auto &nodeIndex : nodeLiteralBestCut)
                          if (_aig.nodeIsAnd (
                                  AndInverterGraph::literalFromIndex (
                                      nodeIndex)))
                            nextIteration.emplace (
                                AndInverterGraph::literalFromIndex (
                                    nodeIndex));
                      }

                  // Update the set of nodes to be checked
                  currentIteration = nextIteration;
                }
            }
        }

      // The output is directly connected to an input, GND or VDD
      else if (_aig.nodeIsInput (outputLiteral) || outputLiteral < 2)
        {
          // Update area and delay costs
          _mappingAreaCost++;
          _mappingDelayCost = _mappingDelayCost < 1 ? 1 : _mappingDelayCost;
        }
    }
}

void
TechMapper::printResults (std::ostream &os)
{
  os << ">> Technology Mapping results" << std::endl;
  os << "# LUT count: " << _mappingAreaCost << std::endl;
  os << "# Levels: " << _mappingDelayCost << std::endl;
}

void
TechMapper::printImplementation (std::ostream &os)
{
  std::cout << ">> Implementation details: " << std::endl;
  for (const auto &[node, implemented] : _implementationMap)
    if (implemented)
      std::cout << "(" << node << ") => " << _cutEngine.getBestCut (node)
                << std::endl;
    else
      std::cout << "(" << node << ") => not implemented" << std::endl;
}