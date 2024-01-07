/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/AigNode.h"

#include <stdexcept>

AigNode::AigNode (unsigned int fanout) noexcept : _fanout{ fanout } {}

void
AigNode::incFanout ()
{
  if (_fanout == -1)
    throw std::overflow_error (
        "Current value of _fanout is its maximum value.");
  _fanout++;
}

void
AigNode::decFanout ()
{
  if (_fanout == 0)
    throw std::underflow_error (
        "Current value of _fanout is its minimum value.");
  _fanout--;
}

void
AigNode::setFanout (unsigned int fanout) noexcept
{
  _fanout = fanout;
}

unsigned int
AigNode::getFanout () const noexcept
{
  return _fanout;
}