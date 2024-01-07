/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#ifndef _AIGNODE_H
#define _AIGNODE_H

class AigNode {
 public:
  /**
   * @brief Constructs a new AigNode object
   *
   * @param fanout Number of edges starting from this node.
   */
  AigNode(unsigned int fanout = 0) noexcept;

  /**
   * @brief Increases the fanout by 1. Throws an exception if the fanout is
   * already at the maximum allowed value.
   */
  void incFanout();

  /**
   * @brief Decreases the fanout by 1. Throws an exception if the fanout is
   * zero.
   */
  void decFanout();

  /**
   * @brief Set the fanout to an arbitrary value. Does not throw exceptions.
   *
   * @param fanout The new value for the @c fanout attribute
   */
  void setFanout(unsigned int fanout) noexcept;

  /**
   * @brief Getter method for the fanout attribute
   *
   * @return unsigned int
   */
  unsigned int getFanout() const noexcept;

 private:
  // Store the fanout of this node. Initialized to 0.
  unsigned int _fanout = 0;
};

#endif