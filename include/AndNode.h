/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#ifndef _ANDNODE_H
#define _ANDNODE_H

#include "AigNode.h"

class AndNode : public AigNode
{
public:
  /**
   * @brief Constructs a new AndNode object
   *
   * @param firstChild Literal of the first child
   * @param secondChild Literal of the second child
   * @param fanout Number of edges starting from this AND node.
   */
  AndNode (unsigned int firstChild = -1, unsigned int secondChild = -1,
           unsigned int fanout = 0) noexcept;

  /**
   * @brief Set the literal of the first child
   *
   * @param firstChild Literal of the first child
   */
  void setFirstChild (unsigned int firstChild);

  /**
   * @brief Set the literal of the second child
   *
   * @param secondChild Literal of the second child
   */
  void setSecondChild (unsigned int secondChild);

  /**
   * @brief Getter for the first child attribute
   *
   * @return unsigned int
   */
  unsigned int getFirstChild () const noexcept;

  /**
   * @brief Getter for the second child attribute
   *
   * @return unsigned int
   */
  unsigned int getSecondChild () const noexcept;

private:
  unsigned int _firstChild = -1;
  unsigned int _secondChild = -1;
};

#endif