/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/CutSet.h"

std::pair<std::vector<Cut>::iterator, bool>
CutSet::emplace (const Cut &newCut)
{
  std::vector<Cut> *baseClassPointer = this;
  for (auto it = baseClassPointer->begin (); it != baseClassPointer->end ();
       ++it)
    if (newCut == *it)
      return std::make_pair (it, false);
  baseClassPointer->push_back (newCut);
  return std::make_pair (baseClassPointer->end () - 1, true);
}