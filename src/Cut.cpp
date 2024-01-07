/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/Cut.h"

Cut::Cut (std::set<unsigned int> nodeVariables, unsigned int areaCost,
          unsigned int delayCost, unsigned int powerCost)
{
  _nodeVariables = nodeVariables;
  _areaCost = areaCost;
  _delayCost = delayCost;
  _powerCost = powerCost;
}

const std::set<unsigned int> &
Cut::getVariableSet () const noexcept
{
  return _nodeVariables;
}

unsigned int
Cut::numNodeVariables () const noexcept
{
  return _nodeVariables.size ();
}

unsigned int
Cut::getAreaCost () const noexcept
{
  return _areaCost;
}

unsigned int
Cut::getDelayCost () const noexcept
{
  return _delayCost;
}

unsigned int
Cut::getPowerCost () const noexcept
{
  return _powerCost;
}

void
Cut::setAreaCost (unsigned int areaCost)
{
  if (areaCost == -1)
    throw std::runtime_error ("Area cost must be in the range [0, "
                              + std::to_string (-1) + "].");
  else
    _areaCost = areaCost;
}

void
Cut::setDelayCost (unsigned int delayCost)
{
  if (delayCost == -1)
    throw std::runtime_error ("Delay cost must be in the range [0, "
                              + std::to_string (-1) + "].");
  else
    _delayCost = delayCost;
}

void
Cut::setPowerCost (unsigned int powerCost)
{
  if (powerCost == -1)
    throw std::runtime_error ("Power cost must be in the range [0, "
                              + std::to_string (-1) + "].");
  else
    _powerCost = powerCost;
}

void
Cut::unsetAreaCost () noexcept
{
  _areaCost = -1;
}

void
Cut::unsetDelayCost () noexcept
{
  _delayCost = -1;
}

void
Cut::unsetPowerCost () noexcept
{
  _powerCost = -1;
}

bool
Cut::isEmptyCut () const noexcept
{
  return _nodeVariables.empty ();
}

bool
Cut::allCostsSet () const noexcept
{
  if (this->_areaCost == -1 || this->_delayCost == -1
      || this->_powerCost == -1)
    return false;
  else
    return true;
}

bool
Cut::areaCostsSet () const noexcept
{
  if (this->_areaCost == -1)
    return false;
  else
    return true;
}

bool
Cut::delayCostsSet () const noexcept
{
  if (this->_delayCost == -1)
    return false;
  else
    return true;
}

bool
Cut::powerCostsSet () const noexcept
{
  if (this->_powerCost == -1)
    return false;
  else
    return true;
}

const std::set<unsigned int>::const_iterator
Cut::begin () const
{
  return _nodeVariables.begin ();
}

const std::set<unsigned int>::const_iterator
Cut::end () const
{
  return _nodeVariables.end ();
}

std::ostream &
operator<< (std::ostream &os, const Cut &cut)
{
  os << "( ";
  for (const auto &elem : cut._nodeVariables)
    os << (elem * 2) << " ";
  os << ")";

  os << " : area = " << cut._areaCost << " : delay = " << cut._delayCost
     << " : power = " << cut._powerCost;

  return os;
}

Cut
Cut::operator+ (const Cut &rhsCut) const
{
  // There is no cut union if any of the variable sets are empty
  if (this->_nodeVariables.empty () || rhsCut._nodeVariables.empty ())
    throw std::runtime_error ("The union of two cuts (operator +) cannot "
                              "be evaluated if any of the two cuts have an "
                              "empty variable set.");

  // Evaluate the variable set of the union cut
  std::set<unsigned int> unionCutVariables = this->_nodeVariables;
  for (const auto &variable : rhsCut._nodeVariables)
    unionCutVariables.insert (variable);

  // Return a new Cut object
  unsigned int maxValue = -1;
  return Cut (unionCutVariables, maxValue, maxValue, maxValue);
}

bool
Cut::operator== (const Cut &rhsCut) const
{
  if (this->_nodeVariables == rhsCut._nodeVariables)
    return true;
  else
    return false;
}