/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#ifndef _LATCHNODE_H
#define _LATCHNODE_H

#include "AigNode.h"

class LatchNode : public AigNode
{
public:
  /**
   * @brief Constructs a new LatchNode object
   *
   * @param nextQ Literal of the next Q
   * @param fanout Number of edges starting from this latch node.
   */
  LatchNode (unsigned int nextQ = -1, unsigned int fanout = 0) noexcept;

  /**
   * @brief Set the literal of the next Q
   *
   * @param nextQ Literal of the next Q
   */
  void setNextQ (unsigned int nextQ);

  /**
   * @brief Getter for the next Q attribute
   *
   * @return unsigned int
   */
  unsigned int getNextQ () const noexcept;

private:
  unsigned int _nextQ = -1;
};

#endif