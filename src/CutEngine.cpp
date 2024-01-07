/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/CutEngine.h"

#include <algorithm>
#include <stack>

CutEngine::CutEngine (const AndInverterGraph &aig, MappingGoal mappingGoal,
                      unsigned int k, unsigned int c)
    : _aig (aig), _k (k), _c (c), _mappingGoal (mappingGoal)
{
  // Integrity check
  if (_k < 2)
    throw std::runtime_error (
        "Runtime error (CutEngine constructor): value of parameter k (number "
        "of lut inputs) must be greater than 1.");

  // Memory allocation and vector initialization
  try
    {
      _cutSetVector.clear ();
      _cutSetVector.reserve (_aig.getNumAnds ());
      _cutSetVector.insert (_cutSetVector.begin (), _aig.getNumAnds (), {});
      unsigned int firstAndVariable
          = AndInverterGraph::indexFromLiteral (_aig.getFirstAndLiteral ());
      unsigned int lastAndVariable = firstAndVariable + _aig.getNumAnds ();
      for (int i = firstAndVariable; i < lastAndVariable; i++)
        _implementationMap.emplace (AndInverterGraph::literalFromIndex (i),
                                    false);
    }
  catch (const std::exception &e)
    {
      throw std::runtime_error ("Failed to initialize CutEngine object from '"
                                + _aig.getFilePath ()
                                + "'.\n what(): " + e.what ());
    }
}

bool
CutEngine::cutAreaComparision (const Cut &cutA, const Cut &cutB)
{
  if (cutA.getAreaCost () < cutB.getAreaCost ())
    return true;
  else if (cutA.getAreaCost () == cutB.getAreaCost ())
    {
      if (cutA.getDelayCost () < cutB.getDelayCost ())
        return true;
      else
        return false;
    }
  else
    return false;
}

bool
CutEngine::cutDelayComparision (const Cut &cutA, const Cut &cutB)
{
  if (cutA.getDelayCost () < cutB.getDelayCost ())
    return true;
  else if (cutA.getDelayCost () == cutB.getDelayCost ())
    {
      // First tie-breaker for delay: area
      if (cutA.getAreaCost () < cutB.getAreaCost ())
        return true;
      else
        return false;
    }
  else
    return false;
}

CutSet
CutEngine::sortAndChooseBestCuts (const CutSet &cutSet, const unsigned int &c,
                                  MappingGoal mappingGoal)
{
  // Create a new CutSet object for the best cuts and copies cutSet to it
  CutSet bestCuts = cutSet;

  // Use std::sort to sort all cuts according to the mapping goal
  if (mappingGoal == MappingGoal::MinimizeArea)
    std::sort (bestCuts.begin (), bestCuts.end (), cutAreaComparision);
  else if (mappingGoal == MappingGoal::MinimizeDelay)
    std::sort (bestCuts.begin (), bestCuts.end (), cutDelayComparision);
  else
    std::sort (bestCuts.begin (), bestCuts.end (), cutAreaComparision);

  // Keep the first c cuts into the vector only
  if (bestCuts.size () > c)
    bestCuts.erase (bestCuts.begin () + c, bestCuts.end ());

  return bestCuts;
}

CutSet
CutEngine::sortCutSet (const CutSet &cutSet, MappingGoal mappingGoal)
{
  // Create a new CutSet object for the best cuts and copies cutSet to it
  CutSet bestCuts = cutSet;

  // Use std::sort to sort all cuts according to the mapping goal
  if (mappingGoal == MappingGoal::MinimizeArea)
    std::sort (bestCuts.begin (), bestCuts.end (), cutAreaComparision);
  else if (mappingGoal == MappingGoal::MinimizeDelay)
    std::sort (bestCuts.begin (), bestCuts.end (), cutDelayComparision);
  else
    std::sort (bestCuts.begin (), bestCuts.end (), cutAreaComparision);

  return bestCuts;
}

CutSet
CutEngine::diamondOperation (const unsigned int &andLiteral,
                             const CutSet &cutSetA, const CutSet &cutSetB,
                             const unsigned int &k)
{
  // Initialize a new CutSet
  CutSet diamond = {};

  // Diamond operation
  // Note: operation + is not the traditional sum.
  // For a Cut object, operator + evaluates the union of two cuts
  for (const auto &cutA : cutSetA)
    {
      for (const auto &cutB : cutSetB)
        {
          // Evaluate the union of cutA and cutB
          Cut unionCut = cutA + cutB;

          // All cuts whose number of variables is greater than k must be
          // discarded
          if (unionCut.numNodeVariables () > k)
            continue;

          // Check if cutA and cutB have valid values for area, delay
          // and power costs
          if (!cutA.allCostsSet () || !cutB.allCostsSet ())
            throw std::runtime_error ("The cost of the union of two cuts can "
                                      "only be evaluated if the "
                                      "two cuts have their costs for area, "
                                      "delay and power defined.");

          // Add to diamond set
          auto result = diamond.emplace (unionCut);
          auto autoCutIterator = result.first;
          auto wasNotInTheSet = result.second;

          if (wasNotInTheSet)
            {
              // Evaluate area cost for the union cut
              unsigned int unionCutAreaCost
                  = estimateUnionCutAreaCost (andLiteral, unionCut);

              // Evaluate delay cost for the union cut
              unsigned int unionCutDelayCost
                  = estimateUnionCutDelayCost (cutA, cutB);

              // Assign zero to power cost for the union cut
              unsigned int unionCutPowerCost = 0;

              // Set the costs of the unionCut
              autoCutIterator->setAreaCost (unionCutAreaCost);
              autoCutIterator->setDelayCost (unionCutDelayCost);
              autoCutIterator->setPowerCost (unionCutPowerCost);
            }
        }
    }

  return diamond;
}

unsigned int
CutEngine::estimateUnionCutAreaCost (const unsigned int &andLiteral,
                                     const Cut &unionCut) const
{
  // Count the number of unimplemented and-nodes in the union cut
  std::set<unsigned int> unimplementedNodes = {};
  for (const auto &nodeIndex : unionCut)
    {
      unsigned int nodeLiteral
          = AndInverterGraph::literalFromIndex (nodeIndex);
      if (_aig.nodeIsAnd (nodeLiteral)
          && _implementationMap.at (nodeLiteral) == false)
        unimplementedNodes.emplace (nodeLiteral);
    }

  // Return the area cost
  return unimplementedNodes.size ();
}

unsigned int
CutEngine::estimateUnionCutDelayCost (const Cut &cutA, const Cut &cutB) const
{
  return cutA.getDelayCost () >= cutB.getDelayCost () ? cutA.getDelayCost ()
                                                      : cutB.getDelayCost ();
}

unsigned int
CutEngine::estimateAutoCutAreaCost (unsigned int andLiteral) const
{
  const Cut &bestCut = getBestCut (andLiteral);
  return bestCut.getAreaCost ();
}

unsigned int
CutEngine::estimateAutoCutDelayCost (unsigned int andLiteral) const
{
  const Cut &bestCut = getBestCut (andLiteral);
  return 1 + bestCut.getDelayCost ();
}

const AndInverterGraph &
CutEngine::getAndInverterGraph () const noexcept
{
  return _aig;
}

bool
CutEngine::hasBestCut (unsigned int andLiteral) const
{
  if (!_aig.nodeIsAnd (andLiteral))
    throw std::runtime_error (
        "Runtime error (hasBestCut): the value provided in andLiteral "
        "argument "
        "is not a valid and-literal for the AndInverterGraph object.");
  else
    {
      if (_cutSetVector.at (vectorIndexFromAndLiteral (andLiteral)).empty ())
        return false;
      else
        return true;
    }
}

const CutSet &
CutEngine::getCutSet (unsigned int andLiteral) const
{
  if (!_aig.nodeIsAnd (andLiteral))
    throw std::runtime_error (
        "Runtime error (getCutSet): the value provided in andLiteral argument "
        "is not a valid and-literal for the AndInverterGraph object.");
  else
    {
      const CutSet &cutSet
          = _cutSetVector.at (vectorIndexFromAndLiteral (andLiteral));
      return cutSet;
    }
}

const Cut &
CutEngine::getBestCut (unsigned int andLiteral) const
{
  if (!_aig.nodeIsAnd (andLiteral))
    throw std::runtime_error (
        "Runtime error (getBestCut): the value provided in andLiteral "
        "argument "
        "is not a valid and-literal for the AndInverterGraph object.");
  else
    {
      if (_cutSetVector.at (vectorIndexFromAndLiteral (andLiteral)).empty ())
        throw std::runtime_error ("Runtime error (getBestCut): the best cut "
                                  "for the and-node has not "
                                  "been defined yet. Call hasBestCut() to "
                                  "check for it before calling "
                                  "getBestCut().");
      else
        return _cutSetVector.at (vectorIndexFromAndLiteral (andLiteral))
            .at (0);
    }
}

CutSet
CutEngine::phiOperation (const unsigned int &andLiteral)
{
  // Throw an exception if the value provided in andLiteral is not a valid AND
  // node literal
  if (!_aig.nodeIsAnd (andLiteral))
    throw std::runtime_error (
        "Runtime error (phiOperation): value provided in andLiteral argument "
        "is not a valid and-literal for the AndInverterGraph object.");

  // If andLiteral has its cut set already defined, do nothing: simply return
  // the cut set
  if (!getCutSet (andLiteral).empty ())
    return _cutSetVector.at (vectorIndexFromAndLiteral (andLiteral));

  // Get child node literals
  const AndNode &an = _aig.getAndNodeFromLiteral (andLiteral);
  unsigned int firstChildLiteral = an.getFirstChild ();
  unsigned int secondChildLiteral = an.getSecondChild ();

  // If any of the child nodes are also and-nodes, check whether Phi operation
  // was called for them
  if ((_aig.nodeIsAnd (firstChildLiteral)
       && getCutSet (firstChildLiteral).empty ())
      || (_aig.nodeIsAnd (secondChildLiteral)
          && getCutSet (secondChildLiteral).empty ()))
    throw std::runtime_error (
        "Runtime error (phiOperation): one or both child nodes of andLiteral "
        "are and-nodes but have no CutSet defined.");

  // Get child node cut sets
  // If the child node is an input, start a new empty cut set
  CutSet firstChildCutSet = _aig.nodeIsInput (firstChildLiteral)
                                ? CutSet ()
                                : getCutSet (firstChildLiteral);
  CutSet secondChildCutSet = _aig.nodeIsInput (secondChildLiteral)
                                 ? CutSet ()
                                 : getCutSet (secondChildLiteral);

  // Add the autocut to the cut sets
  Cut firstChildAutoCut = generateAutoCut (firstChildLiteral);
  Cut secondChildAutoCut = generateAutoCut (secondChildLiteral);
  firstChildCutSet.emplace (firstChildAutoCut);
  secondChildCutSet.emplace (secondChildAutoCut);

  return diamondOperation (andLiteral, firstChildCutSet, secondChildCutSet,
                           _k);
}

const CutSet &
CutEngine::findCuts (const unsigned int &andLiteral)
{
  // Throw an exception if the value provided in andLiteral is not a valid AND
  // node literal
  if (!_aig.nodeIsAnd (andLiteral))
    throw std::runtime_error (
        "Runtime error (phiOperation): value provided in andLiteral argument "
        "is not a valid and-literal for the AndInverterGraph object.");

  // If andLiteral has its cut set already defined, do nothing: simply
  // return the cut set
  if (!getCutSet (andLiteral).empty ())
    return _cutSetVector.at (vectorIndexFromAndLiteral (andLiteral));

  // Create a stack to save the nodes that will be processed
  std::stack<unsigned int> processingStack;

  // Push andLiteral on top of the stack
  processingStack.push (andLiteral);

  // Process all nodes on the stack until there is no more left
  while (!processingStack.empty ())
    {
      // Get the node on top of the stack to process
      unsigned int currentAndNode = processingStack.top ();

      // Get the literals of currentAndNode child nodes
      unsigned int firstChildLiteral
          = _aig.getAndNodeFromLiteral (currentAndNode).getFirstChild ();
      unsigned int secondChildLiteral
          = _aig.getAndNodeFromLiteral (currentAndNode).getSecondChild ();

      // If the firstChildNode is an and-node and has not yet been processed,
      // put it on top of the stack and put off the current iteration for later
      if (_aig.nodeIsAnd (firstChildLiteral)
          && getCutSet (firstChildLiteral).empty ())
        {
          processingStack.push (firstChildLiteral);
          continue; // put off current iteration
        }

      // Does the same test for the second child node
      if (_aig.nodeIsAnd (secondChildLiteral)
          && getCutSet (secondChildLiteral).empty ())
        {
          processingStack.push (secondChildLiteral);
          continue; // put off current iteration
        }

      // If execution reached this point, the child nodes are either input
      // nodes or have their cut sets defined already, thus phiOperation can be
      // applied to currentAndNode
      CutSet currentNodeCutSet = phiOperation (currentAndNode);

      // If c paramenter was provided, sort all cuts and store only the c best
      if (_c > 0)
        {
          CutSet bestCutsSet
              = sortAndChooseBestCuts (currentNodeCutSet, _c, _mappingGoal);
          _cutSetVector.at (vectorIndexFromAndLiteral (currentAndNode))
              = bestCutsSet;
          if (bestCutsSet.at (0).getAreaCost () == 0)
            {
              _implementationMap[currentAndNode] = true;
              if (_aig.nodeIsAnd (firstChildLiteral))
                {
                  const Cut &firstChildBestCut
                      = getBestCut (firstChildLiteral);
                  if (std::includes (
                          bestCutsSet.at (0).getVariableSet ().begin (),
                          bestCutsSet.at (0).getVariableSet ().end (),
                          firstChildBestCut.getVariableSet ().begin (),
                          firstChildBestCut.getVariableSet ().end ()))
                    {
                      if (firstChildLiteral % 2 == 1)
                        firstChildLiteral--;
                      _implementationMap[firstChildLiteral] = false;
                    }
                }
              if (_aig.nodeIsAnd (secondChildLiteral))
                {
                  const Cut &secondChildBestCut
                      = getBestCut (secondChildLiteral);
                  if (std::includes (
                          bestCutsSet.at (0).getVariableSet ().begin (),
                          bestCutsSet.at (0).getVariableSet ().end (),
                          secondChildBestCut.getVariableSet ().begin (),
                          secondChildBestCut.getVariableSet ().end ()))
                    {
                      if (secondChildLiteral % 2 == 1)
                        secondChildLiteral--;
                      _implementationMap[secondChildLiteral] = false;
                    }
                }
            }
        }
      // Otherwise only sort and store (no prunning)
      else
        {
          CutSet sortedCutSet = sortCutSet (currentNodeCutSet, _mappingGoal);
          _cutSetVector.at (vectorIndexFromAndLiteral (currentAndNode))
              = sortedCutSet;
          Cut autoCut = Cut (
              { AndInverterGraph::indexFromLiteral (firstChildLiteral),
                AndInverterGraph::indexFromLiteral (secondChildLiteral) });
          if (sortedCutSet.at (0).getAreaCost () == 0)
            {
              _implementationMap[currentAndNode] = true;
              if (_aig.nodeIsAnd (firstChildLiteral))
                {
                  const Cut &firstChildBestCut
                      = getBestCut (firstChildLiteral);
                  if (std::includes (
                          sortedCutSet.at (0).getVariableSet ().begin (),
                          sortedCutSet.at (0).getVariableSet ().end (),
                          firstChildBestCut.getVariableSet ().begin (),
                          firstChildBestCut.getVariableSet ().end ()))
                    {
                      if (firstChildLiteral % 2 == 1)
                        firstChildLiteral--;
                      _implementationMap[firstChildLiteral] = false;
                    }
                }
              if (_aig.nodeIsAnd (secondChildLiteral))
                {
                  const Cut &secondChildBestCut
                      = getBestCut (secondChildLiteral);
                  if (std::includes (
                          sortedCutSet.at (0).getVariableSet ().begin (),
                          sortedCutSet.at (0).getVariableSet ().end (),
                          secondChildBestCut.getVariableSet ().begin (),
                          secondChildBestCut.getVariableSet ().end ()))
                    {
                      if (secondChildLiteral % 2 == 1)
                        secondChildLiteral--;
                      _implementationMap[secondChildLiteral] = false;
                    }
                }
            }
        }

      // Remove currentAndNode from the top of the stack
      processingStack.pop ();
    }

  // Sanity check
  if (getCutSet (andLiteral).empty ())
    throw std::runtime_error (
        "Runtime error (evaluateCutSet): cut set for andLiteral remains "
        "undefined after processing due to errors.");

  // Return the CutSet evaluated for andLiteral
  return _cutSetVector.at (vectorIndexFromAndLiteral (andLiteral));
}

void
CutEngine::run ()
{
  for (const auto &outputLiteral : _aig.getOutputLiteralVector ())
    if (_aig.nodeIsAnd (outputLiteral))
      this->findCuts (outputLiteral);
}

void
CutEngine::printOutputsBestCuts (std::ostream &os) const
{
  for (const auto &outputLiteral : _aig.getOutputLiteralVector ())
    {
      if (_aig.nodeIsAnd (outputLiteral))
        {
          os << std::endl;
          os << "Output " << outputLiteral << ":" << std::endl;
          os << "------------------------" << std::endl;
          if (getCutSet (outputLiteral).size () == 0)
            os << "No cut set defined." << std::endl;
          else
            for (const auto &cut : getCutSet (outputLiteral))
              os << cut << std::endl;
        }
    }
}

std::ostream &
operator<< (std::ostream &os, const CutEngine &cutEngine)
{
  os << ">> Current state of the CutEngine for "
     << cutEngine.getAndInverterGraph ().getFilePath () << std::endl;

  for (int i = 0; i < cutEngine._cutSetVector.size (); i++)
    {
      os << std::endl;
      os << "Node " + std::to_string (cutEngine.andLiteralFromVectorIndex (i))
                + ":"
         << std::endl;
      os << "------------------------" << std::endl;
      if (cutEngine._cutSetVector[i].size () == 0)
        os << "No cut set defined." << std::endl;
      else
        for (const auto &cut : cutEngine._cutSetVector[i])
          os << cut << std::endl;
    }

  return os;
}

unsigned int
CutEngine::vectorIndexFromAndLiteral (unsigned int andLiteral) const
{
  unsigned int vectorIndex = _aig.indexFromLiteral (andLiteral)
                             - _aig.getNumInputs () - _aig.getNumLatches ()
                             - 1;
  if (vectorIndex >= _cutSetVector.size ())
    throw std::overflow_error (
        "Range overflow. Index used to access _cutSetVector is greater or "
        "equal their sizes.");
  else
    return vectorIndex;
}

unsigned int
CutEngine::andLiteralFromVectorIndex (unsigned int vectorIndex) const
{
  unsigned int andLiteral = _aig.literalFromIndex (
      vectorIndex + _aig.getNumInputs () + _aig.getNumLatches () + 1);
  if (vectorIndex > _aig.literalFromIndex (_aig.getMaxVariableIndex ()) + 1)
    throw std::runtime_error (
        "Runtime error (CutEngine). " + std::to_string (andLiteral)
        + " is not a valid and-literal for the given AIG.");
  else
    return andLiteral;
}

Cut
CutEngine::generateAutoCut (unsigned int nodeLiteral) const
{
  // If the node is an input, the cost is 1 for area, 1 for delay, and zero for
  // power
  if (_aig.nodeIsInput (nodeLiteral))
    return Cut ({ AndInverterGraph::indexFromLiteral (nodeLiteral) },
                0,  // area cost
                1,  // delay cost
                0); // power cost

  // If the child node is an and-node...
  else if (_aig.nodeIsAnd (nodeLiteral))
    {
      unsigned int autoCutArea = estimateAutoCutAreaCost (nodeLiteral);
      unsigned int autoCutDelay = estimateAutoCutDelayCost (nodeLiteral);
      return Cut ({ AndInverterGraph::indexFromLiteral (nodeLiteral) },
                  autoCutArea,  // area cost
                  autoCutDelay, // delay cost
                  0);           // power cost
    }

  // If the node is neither an input nor an AND an exception is throw
  else
    throw std::runtime_error (
        "Runtime error (phiOperation). Child node is neither input nor AND");
}

void
CutEngine::printImplementation (std::ostream &os)
{
  std::cout << ">> Implementation details: " << std::endl;
  for (const auto &[node, implemented] : _implementationMap)
    if (implemented)
      std::cout << "(" << node << ") => " << this->getBestCut (node)
                << std::endl;
    else
      std::cout << "(" << node << ") => not implemented" << std::endl;
}