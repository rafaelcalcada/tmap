/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#ifndef _ANDINVERTERGRAPH_H
#define _ANDINVERTERGRAPH_H

#include <string>
#include <vector>

#include "AndNode.h"
#include "LatchNode.h"

class AndInverterGraph {
 public:
  AndInverterGraph() = delete;

  /**
   * @brief Constructs a new AndInverterGraph object and initializes it from
   * @c filePath.
   *
   * @param filePath A string with the path to the AIGER file. Both binary and
   * ASCII formats are supported. Example: "/home/user/myaigerfile.aig".
   */
  AndInverterGraph(const std::string &filePath);

  /**
   * @brief Returns @c true if the AndInverterGraph object is sucessfully
   * initialized, and @c false otherwise.
   *
   * @return bool
   */
  bool successfullyInitialized() const noexcept;

  /**
   * @brief Returns @c true if the AIG does not have any latches.
   * Returns @c false otherwise.
   *
   * @return bool
   */
  bool isCombinational() const noexcept;

  /**
   * @brief Returns @c if the AIG has latches. Returns @c false otherwise.
   *
   * @return bool
   */
  bool isSequential() const noexcept;

  /**
   * @brief Returns @c true if the node literal provided represents an input
   * node of the AIG. Returns @c false otherwise. This method returns
   * @c false if the node literal is logic FALSE (0) or TRUE (1).
   *
   * @param nodeLiteral The literal to be tested
   * @return bool
   */
  bool nodeIsInput(unsigned int nodeLiteral) const noexcept;

  /**
   * @brief Returns @c true if the node literal provided represents a latch node
   * of the AIG. Returns @c false otherwise.
   *
   * @param nodeLiteral The literal to be tested
   * @return bool
   */
  bool nodeIsLatch(unsigned int nodeLiteral) const noexcept;

  /**
   * @brief Returns @c true if the node literal provided represents an and-node
   * of the AIG. Returns @c false otherwise.
   *
   * @param nodeLiteral The node literal to be tested
   * @return bool
   */
  bool nodeIsAnd(unsigned int nodeLiteral) const noexcept;

  /**
   * @brief Returns a read-only reference to an AndNode object given the
   * And node literal
   *
   * @param andLiteral The literal of an and-node
   * @return const AndNode&
   */
  const AndNode &getAndNodeFromLiteral(const unsigned int &andLiteral) const;

  /**
   * @brief Returns a read-only reference to a LatchNode object given the Latch
   * node literal
   *
   * @param latchLiteral The literal of a latch-node
   * @return const LatchNode&
   */
  const LatchNode &getLatchNodeFromLiteral(
      const unsigned int &latchLiteral) const;

  /**
   * @brief Converts a literal into a variable index.
   *
   * @param literal The literal do be converted into a variable index
   * @return unsigned int
   */
  static unsigned int indexFromLiteral(unsigned int literal) noexcept;

  /**
   * @brief Converts a variable index into a literal.
   *
   * @param index The variable index to be converted into a literal
   * @return unsigned int
   */
  static unsigned int literalFromIndex(unsigned int index) noexcept;

  /**
   * @brief Returns a read-only reference for the vector that stores the output
   * literals.
   *
   * @return const std::vector<unsigned int>&
   */
  const std::vector<unsigned int> &getOutputLiteralVector() const noexcept;

  /**
   * @brief Returns a string with the path of the file used to initialize the
   * AndInverterGraph object.
   *
   * @return std::string
   */
  std::string getFilePath() const noexcept;

  /**
   * @brief Returns the maximum variable index in the AndInverterGraph object
   *
   * @return unsigned int
   */
  unsigned int getMaxVariableIndex() const noexcept;

  /**
   * @brief Returns the number of inputs in the AndInverterGraph object
   *
   * @return unsigned int
   */
  unsigned int getNumInputs() const noexcept;

  /**
   * @brief Returns the number of latches in the AndInverterGraph object
   *
   * @return unsigned int
   */
  unsigned int getNumLatches() const noexcept;

  /**
   * @brief Returns the number of outputs in the AndInverterGraph object
   *
   * @return unsigned int
   */
  unsigned int getNumOutputs() const noexcept;

  /**
   * @brief Returns the number of and-nodes in the AndInverterGraph object
   *
   * @return unsigned int
   */
  unsigned int getNumAnds() const noexcept;

  /**
   * @brief Returns the and-node literal with the lowest value
   *
   * @return unsigned int
   */
  unsigned int getFirstAndLiteral() const noexcept;

  /**
   * @brief Returns the latch-node literal with the lowest value
   *
   * @return unsigned int
   */
  unsigned int getFirstLatchLiteral() const noexcept;

  /**
   * @brief Prints all AIG information to a C++ output stream.
   *
   * @param os A C++ output stream
   */
  void print(std::ostream &os) const;
  friend std::ostream &operator<<(std::ostream &os,
                                  const AndInverterGraph &aig);

  /**
   * @brief Overloads operator << so that AIG data can be transfered to
   * an C++ output stream.
   *
   * @param os
   * @param aig
   * @return std::ostream&
   */

 private:
  std::string _filePath = "";
  unsigned int _maxVariableIndex = 0;
  unsigned int _numInputs = 0;
  unsigned int _numLatches = 0;
  unsigned int _numOutputs = 0;
  unsigned int _numAnds = 0;
  std::vector<unsigned int> _outputLiteralVector;
  std::vector<AndNode> _andVector;
  std::vector<LatchNode> _latchVector;
  std::vector<std::string> _inputNameVector;
  std::vector<std::string> _latchNameVector;
  std::vector<std::string> _outputNameVector;
  std::vector<std::string> _commentVector;
  bool _hasNamedInputs = false;
  bool _hasNamedLatches = false;
  bool _hasNamedOutputs = false;
  bool _hasComments = false;
  bool _initialized = false;
  bool _isBinary = false;

  /**
   * @brief Converts an and-literal into an index to access _andVector.
   * Throws @c std::overflow_error() if the index is equal or greater than
   * the size of _andVector
   *
   * @param andLiteral
   * @return unsigned int
   */
  unsigned int andVectorIndexFromLiteral(unsigned int andLiteral) const;

  /**
   * @brief Converts an index of _andVector into its equivalent AND
   * literal. Throws @c std::runtime_error() if the literal is not valid for
   * the AIG.
   *
   * @param andVectorIndex
   * @return unsigned int
   */
  unsigned int literalFromAndVectorIndex(unsigned int andVectorIndex) const;

  /**
   * @brief Converts a latch literal into an index to access _latchVector.
   * Throws @c std::overflow_error() if the index is equal or greater than
   * the size of _latchVector
   *
   * @param latchLiteral
   * @return unsigned int
   */
  unsigned int latchVectorIndexFromLiteral(unsigned int latchLiteral) const;

  /**
   * @brief Convert an index of _latchVector into its equivalent latch
   * literal. Throw @c std::runtime_error() if the literal is not valid for
   * the AIG.
   *
   * @param latchVectorIndex
   * @return unsigned int
   */
  unsigned int literalFromLatchVectorIndex(unsigned int latchVectorIndex) const;
};

#endif