/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/AndNode.h"

#include <stdexcept>

AndNode::AndNode (unsigned int firstChild, unsigned int secondChild,
                  unsigned int fanout) noexcept : AigNode (fanout)
{
  _firstChild = firstChild;
  _secondChild = secondChild;
}

void
AndNode::setFirstChild (unsigned int firstChild)
{
  if (firstChild < -1)
    _firstChild = firstChild;
  else
    throw std::overflow_error ("Overflow error when setting the literal of "
                               "the first child for an AND "
                               "node. Literal must be lower than "
                               + std::to_string (-1));
}

void
AndNode::setSecondChild (unsigned int secondChild)
{
  if (secondChild < -1)
    _secondChild = secondChild;
  else
    throw std::overflow_error (
        "Overflow error when setting the literal of the second child for an "
        "AND node. Literal must be lower than "
        + std::to_string (-1));
}

unsigned int
AndNode::getFirstChild () const noexcept
{
  return _firstChild;
}

unsigned int
AndNode::getSecondChild () const noexcept
{
  return _secondChild;
}