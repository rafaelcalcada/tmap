/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#ifndef _CUTSET_H
#define _CUTSET_H

#include <vector>

#include "Cut.h"

class CutSet : public std::vector<Cut>
{
public:
  /**
   * @brief Try to add a new cut to the set. If the new cut is already in the
   * set, returns a @c std::pair with an iterator to the cut as first element
   * and @c bool(false) as second element. If the new cut is not in the set,
   * emplaces the new cut and return a @c std::pair with an iterator to the
   * new cut as first element and @c bool(true) as second element.
   *
   * @param newCut
   * @return std::pair<iterator, bool>
   */
  std::pair<std::vector<Cut>::iterator, bool> emplace (const Cut &newCut);

  // Delete some std::vector modifiers
  void assign () = delete;
  void push_back () = delete;
  void pop_back () = delete;
  void insert () = delete;
  void swap () = delete;
  void emplace () = delete;
  void emplace_back () = delete;
};

#endif