/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#ifndef _CUT_H
#define _CUT_H

#include <iostream>
#include <set>

class Cut
{
public:
  /**
   * @brief Construct a new Cut object
   *
   * @param nodeVariables Node variables. Nodes can be either inputs or
   * and-nodes
   * @param areaCost The cost to implement the cut regarding to area
   * @param delayCost The cost to implement the cut regarding to delay
   * @param powerCost The cost to implement the cut regarding to power
   */
  Cut (std::set<unsigned int> nodeVariables = {}, unsigned int areaCost = -1,
       unsigned int delayCost = -1, unsigned int powerCost = -1);

  /**
   * @brief Returns a read-only reference to the cut variable set
   *
   * @return const std::set<unsigned int>&
   */
  const std::set<unsigned int> &getVariableSet () const noexcept;

  /**
   * @brief Return the number of node variables for the given Cut object
   *
   * @return unsigned int
   */
  unsigned int numNodeVariables () const noexcept;

  /**
   * @brief Return the calculated cost to implement the cut regarding to area
   *
   * @return unsigned int
   */
  unsigned int getAreaCost () const noexcept;

  /**
   * @brief Return the calculated cost to implement the cut regarding to delay
   *
   * @return unsigned int
   */
  unsigned int getDelayCost () const noexcept;

  /**
   * @brief Return the calculated cost to implement the cut regarding to power
   *
   * @return unsigned int
   */
  unsigned int getPowerCost () const noexcept;

  /**
   * @brief Set the cost to implement the cut regarding to area
   *
   * @param areaCost The new value for area cost
   */
  void setAreaCost (unsigned int areaCost);

  /**
   * @brief Set the cost to implement the cut regarding to delay
   *
   * @param delayCost The new value for delay cost
   */
  void setDelayCost (unsigned int delayCost);

  /**
   * @brief Set the cost to implement the cut regarding to power
   *
   * @param powerCost The new value for power cost
   */
  void setPowerCost (unsigned int powerCost);

  /**
   * @brief Unset the value for area cost. Makes areaCostSet() and
   * allCostsSet() return @c false.
   *
   */
  void unsetAreaCost () noexcept;

  /**
   * @brief Unset the value for delay cost. Makes delayCostSet() and
   * allCostsSet() return @c false.
   *
   */
  void unsetDelayCost () noexcept;

  /**
   * @brief Unset the value for power cost. Makes powerCostSet() and
   * allCostsSet() return @c false.
   *
   */
  void unsetPowerCost () noexcept;

  /**
   * @brief Boolean predicate that returns true if the Cut object has no node
   * variables
   *
   * @return boolean
   */
  bool isEmptyCut () const noexcept;

  /**
   * @brief Boolean predicate that returns true if the Cut object has
   * area, delay and power costs defined. Returns false otherwise.
   *
   * @return boolean
   */
  bool allCostsSet () const noexcept;

  /**
   * @brief Boolean predicate that returns true if the Cut object has
   * cost for area defined. Returns false otherwise.
   *
   * @return boolean
   */
  bool areaCostsSet () const noexcept;

  /**
   * @brief Boolean predicate that returns true if the Cut object has
   * cost for delay defined. Returns false otherwise.
   *
   *
   * @return boolean
   */
  bool delayCostsSet () const noexcept;

  /**
   * @brief Boolean predicate that returns true if the Cut object has
   * cost for power defined. Returns false otherwise.
   *
   *
   * @return boolean
   */
  bool powerCostsSet () const noexcept;

  /**
   * @brief Returns a read-only iterator that points to the first element in
   * the node variables set.
   *
   * @return const std::set<unsigned int>::const_iterator&
   */
  const std::set<unsigned int>::const_iterator begin () const;

  /**
   * @brief Returns a read-only iterator that points one past the last element
   * in the node variables set.
   *
   * @return const std::set<unsigned int>::const_iterator&
   */
  const std::set<unsigned int>::const_iterator end () const;

  /**
   * @brief Overload operator << so that data from a Cut object can be
   * printed to a C++ output stream.
   *
   * @return std::ostream&
   */
  friend std::ostream &operator<< (std::ostream &os, const Cut &cut);

  Cut operator+ (const Cut &rhsCut) const;
  bool operator== (const Cut &rhsCut) const;

private:
  std::set<unsigned int> _nodeVariables = {};
  unsigned int _areaCost = -1;
  unsigned int _delayCost = -1;
  unsigned int _powerCost = -1;
};

#endif