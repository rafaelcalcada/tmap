/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/LatchNode.h"

#include <stdexcept>

LatchNode::LatchNode (unsigned int nextQ, unsigned int fanout) noexcept
    : AigNode (fanout)
{
  _nextQ = nextQ;
}

void
LatchNode::setNextQ (unsigned int nextQ)
{
  if (nextQ < -1)
    _nextQ = nextQ;
  else
    throw std::overflow_error (
        "Overflow error when setting the literal of the next Q for a latch "
        "node. Literal must be lower than "
        + std::to_string (-1));
}

unsigned int
LatchNode::getNextQ () const noexcept
{
  return _nextQ;
}