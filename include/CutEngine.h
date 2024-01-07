/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#ifndef _CUTENGINE_H
#define _CUTENGINE_H

#include <map>

#include "AndInverterGraph.h"
#include "Cut.h"
#include "CutSet.h"

enum class MappingGoal
{
  MinimizeArea,
  MinimizeDelay
};

class CutEngine
{
public:
  CutEngine () = delete;

  /**
   * @brief Construct a new CutEngine object from an AndInverterGraph object.
   *
   * @param aig An AndInverterGraph object
   */
  CutEngine (const AndInverterGraph &aig,
             MappingGoal mappingGoal = MappingGoal::MinimizeArea,
             unsigned int k = 6, unsigned int c = 0);

  /**
   * @brief Given two cuts (cutA and cutB), this method decides whether cutA is
   * better than cutB in terms of area cost to implement. This method returns
   * @c true if cutA is better than cutB an @c false otherwise.
   *
   * If the cuts have equal values for area cost, their delay cost is used as
   * tie-breaker. If the cuts also have equal values for delay cost, their
   * number of variables is used as a second tie breaker.
   *
   * @param cutA
   * @param cutB
   * @return boolean
   */
  static bool cutAreaComparision (const Cut &cutA, const Cut &cutB);

  /**
   * @brief Given two cuts (cutA and cutB), this method decides whether cutA is
   * better than cutB in terms of delay cost to implement. This method returns
   * @c true if cutA is better than cutB an @c false otherwise.
   *
   * If the cuts have equal values for delay cost, their area cost is used as
   * tie-breaker. If the cuts also have equal values for area cost, their
   * number of variables is used as a second tie breaker.
   *
   * @param cutA
   * @param cutB
   * @return boolean
   */
  static bool cutDelayComparision (const Cut &cutA, const Cut &cutB);

  /**
   * @brief Choose the best @c c cuts of a CutSet according to some criteria
   * (area, delay or power), returning a CutSet object with the best cuts in
   * it.
   *
   * The cuts in the returned CutSet object are ordered from the best cut
   * (first element) to the worse cut (last element).
   *
   * @param cutSet A CutSet object with the cuts to be sorted and chosen
   * @param c The number of best cuts to choose
   * @param mappingGoal The goal of the mapping, which defines the algorithm
   * used to select the best cuts
   * @return A new CutSet object
   */
  static CutSet
  sortAndChooseBestCuts (const CutSet &cutSet, const unsigned int &c = 8,
                         MappingGoal mappingGoal = MappingGoal::MinimizeArea);

  /**
   * @brief Sort a CutSet according to some criteria (area, delay or power),
   * returning a new CutSet object.
   *
   * The cuts in the returned CutSet object are ordered from the best cut
   * (first element) to the worse cut (last element).
   *
   * @param cutSet A CutSet object with the cuts to be sorted
   * @param c The number of best cuts to choose
   * @param mappingGoal The goal of the mapping, which defines the algorithm
   * used to select the best cuts
   * @return A new CutSet object
   */
  static CutSet sortCutSet (const CutSet &cutSet, MappingGoal mappingGoal
                                                  = MappingGoal::MinimizeArea);

  /**
   * @brief Returns a read-only reference for the AndInverterGraph object used
   * to initialize the CutEngine.
   *
   * @return const AndInverterGraph&
   */
  const AndInverterGraph &getAndInverterGraph () const noexcept;

  /**
   * @brief Boolean predicate that returns @c true if the best cut has been
   * found for @c andLiteral. Returns @c false otherwise.
   *
   * @param andLiteral The literal of an and-node
   * @return boolean
   */
  bool hasBestCut (unsigned int andLiteral) const;

  /**
   * @brief Returns a read-only reference for the CutSet of an and-node. If the
   * CutSet has not been evaluated it returns a reference for an empty CutSet.
   *
   * @param andLiteral The literal of an and-node
   * @return const CutSet&
   */
  const CutSet &getCutSet (unsigned int andLiteral) const;

  /**
   * @brief Returns a read-only reference for the best Cut of an and-node.
   * Throws an exception if the best Cut of @c andLiteral has not been defined
   * yet.
   *
   * @param andLiteral The literal of an and-node
   * @return const Cut&
   */
  const Cut &getBestCut (unsigned int andLiteral) const;

  /**
   * @brief Finds K-feasible cuts for an and-node in the AndInverterGraph
   * by applying Phi operation to it. K is the number of inputs in the lookup
   * tables and is set when creating the CutEngine object. The cuts found are
   * stored in a CutSet object. A read-only reference for this CutSet object is
   * returned.
   *
   * If parameter @c c was provided when creating the CutEngine object and its
   * value is different than zero only @c c best cuts are stored for each
   * and-node in the AIG.
   *
   * If the CutSet of @c andLiteral is already defined it is simply returned
   * (the operation is not applied again).
   *
   * @param andLiteral The literal of an and-node
   * @return const CutSet&
   */
  const CutSet &findCuts (const unsigned int &andLiteral);

  /**
   * @brief Run the CutEngine. Find cuts for all and-nodes in the
   * AndInverterGraph object used to create the CutEngine.
   *
   */
  void run ();

  /**
   * @brief Overloads operator << so that all cuts found by the CutEngine can
   * be transferred to a C++ output stream.
   *
   * @param os
   * @param cutEngine
   * @return std::ostream&
   */
  void printOutputsBestCuts (std::ostream &os) const;

  friend std::ostream &operator<< (std::ostream &os,
                                   const CutEngine &cutEngine);
  void printImplementation (std::ostream &os);

private:
  std::vector<CutSet> _cutSetVector = {};
  MappingGoal _mappingGoal = MappingGoal::MinimizeArea;
  std::map<unsigned int, bool> _implementationMap = {};
  const AndInverterGraph &_aig;
  unsigned int _k = 6;
  unsigned int _c = 0;

  /**
   * @brief Converts an and-literal into an index to access internal vectors.
   * Throws @c std::overflow_error() if the index is equal or greater than the
   * size of one of those vectors
   *
   * @param andLiteral The literal of an and-node
   * @return unsigned int
   */
  unsigned int vectorIndexFromAndLiteral (unsigned int andLiteral) const;

  /**
   * @brief Converts a vector index into its equivalent and-literal. Throws
   * @c std::runtime_error() if the literal is not valid for the AIG.
   *
   * @param vectorIndex
   * @return unsigned int
   */
  unsigned int andLiteralFromVectorIndex (unsigned int vectorIndex) const;

  /**
   * @brief Generates an autocut with appropriate costs for a given node.
   *
   * The cost of the autocut is determined as follows:
   *
   * - If the node is an input, the cost is
   * -> For area and power, zero;
   * -> For delay, 1;
   *
   * - If the node is an and-node, the cost is:
   * -> For power, zero;
   * -> For delay, the value returned from @c estimateAutoCutDelayCost();
   * -> For area, the value returned from @c estimateAutoCutAreaCost();
   *
   * @param nodeLiteral
   * @return Cut
   */
  Cut generateAutoCut (unsigned int nodeLiteral) const;

  /**
   * @brief Applies Phi operation for an and-node in the AndInverterGraph
   * object, returning the CutSet of all K-feasible cuts.
   *
   * Phi operation consists of the following steps:
   * - generate an autocut for each child node of @c andLiteral;
   * - add the autocut to the child node cut set;
   * - apply Diamond operation.
   *
   * The cost of the child node autocut is determined as follows:
   * - If the child node is an input, the cost is 1 for area, 1 for delay, and
   *   zero for power;
   * - If the child node is an and-node, the autocut cost is the cost of the
   * best cut in the child node CutSet plus 1 for area and delay, and zero for
   * power;
   *
   * @param andLiteral The literal of an and-node
   * @param k Number of inputs of the lookup tables.
   * @return A CutSet object with all K-feasible cuts for node @c andLiteral
   */
  CutSet phiOperation (const unsigned int &andLiteral);

  /**
   * @brief Applies Diamond operation between two sets of cuts.
   *
   * Diamond operation combines all cuts from @c cutSetA with all cuts from
   * @c cutSetB, discarding those with more than @c k variables.
   *
   * The cost of a cut resulting from the union of two others is defined as
   * follows:
   * - For power, zero;
   * - For delay, the value returned from @c estimateUnionCutDelayCost();
   * - For area, the value returned from @c estimateUnionCutAreaCost();
   *
   * @param cutSetA First CutSet
   * @param cutSetB Second CutSet
   * @param k Number of inputs of the lookup tables.
   *
   * @return A CutSet object with cuts formed by the combination of cuts from
   * @c cutSetA and @c cutSetB with up to @c k variables.
   */
  CutSet diamondOperation (const unsigned int &andLiteral,
                           const CutSet &cutSetA, const CutSet &cutSetB,
                           const unsigned int &k = 6);

  /**
   * @brief Estimate the area cost of a cut resulting from the union of two
   * others. The area cost is estimated to be the sum of the cost to implement
   * each and-node in the union cut divided by its fanout.
   *
   * @param unionCut
   * @return unsigned int
   */
  unsigned int estimateUnionCutAreaCost (const unsigned int &andLiteral,
                                         const Cut &unionCut) const;

  /**
   * @brief Estimate the delay cost of a cut resulting from the union of two
   * others. The delay cost is estimated from the cuts that gave rise to the
   * unionCut. The cost is set to be equal the delay cost of the cut with the
   * longest delay.
   *
   * @param cutA The first cut that gave rise to the unionCut
   * @param cutB The second cut that gave rise to the unionCut
   * @return unsigned int
   */
  unsigned int estimateUnionCutDelayCost (const Cut &cutA,
                                          const Cut &cutB) const;

  /**
   * @brief Estimate the area cost for the auto cut of @c andLiteral. The area
   * cost is estimated to be equal the area cost of the best cut of @c
   * andLiteral.
   *
   * @param andLiteral
   * @return unsigned int
   */
  unsigned int estimateAutoCutAreaCost (unsigned int andLiteral) const;

  /**
   * @brief Estimate the delay cost for the auto cut of @c andLiteral. The
   * delay cost is estimated to be equal the delay cost of the best cut of
   * @c andLiteral plus 1.
   *
   * @param andLiteral
   * @return unsigned int
   */
  unsigned int estimateAutoCutDelayCost (unsigned int andLiteral) const;
};

#endif