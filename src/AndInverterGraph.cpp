/**
 *
 *  Author: Rafael Calcada (rafaelcalcada@gmail.com)
 *  GitHub: https://github.com/rafaelcalcada/tmap
 *
 *  Licensed under the MIT License. For the full license please read the
 * 'LICENSE.md' file
 *
 */

#include "../include/AndInverterGraph.h"

#include <fstream>
#include <iostream>
#include <sstream>

AndInverterGraph::AndInverterGraph (const std::string &filePath)
{
  // Initialization
  _filePath = "";
  _maxVariableIndex = 0;
  _numInputs = 0;
  _numLatches = 0;
  _numOutputs = 0;
  _numAnds = 0;
  _outputLiteralVector.clear ();
  _andVector.clear ();
  _latchVector.clear ();
  _inputNameVector.clear ();
  _latchNameVector.clear ();
  _outputNameVector.clear ();
  _commentVector.clear ();
  _hasNamedInputs = false;
  _hasNamedLatches = false;
  _hasNamedOutputs = false;
  _hasComments = false;
  _initialized = false;
  _isBinary = false;
  unsigned int lineNumber = 0;

  // Opens the input file
  std::ifstream inputFile;
  inputFile.open (filePath, std::ios::binary | std::ios::in);
  if (!inputFile.is_open ())
    throw std::runtime_error ("Unable to open '" + filePath + "'");
  _filePath = filePath;

  // Gets a line
  std::string buf;
  std::getline (inputFile, buf);
  lineNumber++;

  // Checks file format
  if (buf.size () >= 3 && buf.substr (0, 3) == "aag")
    _isBinary = false;
  else if (buf.size () >= 3 && buf.substr (0, 3) == "aig")
    _isBinary = true;
  else
    throw std::runtime_error ("Unable to process '" + filePath
                              + "'. Invalid/unknown format.");

  // File header processing
  try
    {
      std::stringstream ss (buf.substr (4));
      if (ss.eof ())
        throw std::exception ();
      std::getline (ss, buf, ' ');
      _maxVariableIndex = std::stoul (buf);
      if (ss.eof ())
        throw std::exception ();
      std::getline (ss, buf, ' ');
      _numInputs = std::stoul (buf);
      if (ss.eof ())
        throw std::exception ();
      std::getline (ss, buf, ' ');
      _numLatches = std::stoul (buf);
      if (ss.eof ())
        throw std::exception ();
      std::getline (ss, buf, ' ');
      _numOutputs = std::stoul (buf);
      if (ss.eof ())
        throw std::exception ();
      std::getline (ss, buf, ' ');
      _numAnds = std::stoul (buf);
    }
  catch (std::exception &e)
    {
      throw std::runtime_error ("Unable to read '" + filePath
                                + "'. Bad file header.");
    }

  // Integrity check: AIGER checksum
  if (_maxVariableIndex != _numInputs + _numLatches + _numAnds)
    throw std::runtime_error (
        "Invalid checksum for '" + filePath
        + "'. The sum of the number of inputs, latches and and-nodes must be "
          "equal to the maximum variable index.");

  // Memory allocation
  try
    {
      _outputLiteralVector.reserve (_numOutputs);
      _andVector.reserve (_numAnds);
      _latchVector.reserve (_numLatches);
    }
  catch (const std::exception &e)
    {
      throw std::runtime_error ("Failed to allocate memory for " + filePath
                                + ".\n  what(): " + e.what ());
    }

  // Integrity checks on inputs for ASCII format
  if (!_isBinary)
    {
      for (int i = 1; i <= _numInputs; i++)
        {
          // Integrity check
          if (inputFile.eof ())
            throw std::runtime_error ("Unexpected end of file in '" + filePath
                                      + "' at line "
                                      + std::to_string (lineNumber));

          // Gets a new line
          std::getline (inputFile, buf);
          lineNumber++;
          if (buf.size () < 1)
            throw std::runtime_error ("Unexpected empty line in '" + filePath
                                      + "' at line "
                                      + std::to_string (lineNumber)
                                      + ". Expecting the input literal: "
                                      + std::to_string (((i + 1) * 2)));

          // Integrity check
          if (buf.substr (0, 1) == "-")
            throw std::runtime_error ("Negative input literal in '" + filePath
                                      + "' at line "
                                      + std::to_string (lineNumber));

          // Converts string to unsigned int
          unsigned int literal;
          try
            {
              literal = std::stoul (buf);
            }
          catch (const std::exception &e)
            {
              throw std::runtime_error (
                  "Call to std::stoul() failed to make conversion at line "
                  + std::to_string (lineNumber));
            }

          // Integrity check
          if (literal != literalFromIndex (i))
            throw std::runtime_error (
                "Unexpected input literal in '" + filePath + "' at line "
                + std::to_string (lineNumber) + ". Expected value is "
                + std::to_string (literalFromIndex (i)));
        }
    }

  // Creates a temporary structure to save next Q literals
  // After reading all latch and and-nodes the fanouts are updated
  // with the help of this structure
  std::vector<unsigned int> *nextQLiterals = new std::vector<unsigned int>;
  nextQLiterals->clear ();
  nextQLiterals->reserve (_numLatches);

  // Reads latch nodes
  for (int i = 0; i < _numLatches; i++)
    {
      unsigned int latchLiteral = 0;
      unsigned int nextQLiteral = 0;

      // Reads from binary format
      if (_isBinary)
        {
          std::getline (inputFile, buf);
          lineNumber++;
          std::stringstream ss (buf);
          if (ss.eof ())
            throw std::runtime_error (
                "Unable to read '" + filePath
                + "'. File reached the end before expected.");

          try
            {
              std::getline (ss, buf, ' ');
              nextQLiteral = std::stoul (buf);
            }
          catch (const std::exception &e)
            {
              throw std::runtime_error (
                  "In " + filePath + ": Failed to make conversion at line "
                  + std::to_string (lineNumber));
            }

          // Integrity checks
          if (nextQLiteral < 2)
            throw std::runtime_error ("File " + filePath + " at line "
                                      + std::to_string (lineNumber)
                                      + " does not comply with AIGER "
                                        "specification: latch node tied to "
                                        "logic FALSE (0) or TRUE (1)");
          unsigned int maxNextQLiteral
              = literalFromIndex (_maxVariableIndex) + 1;
          if (nextQLiteral > maxNextQLiteral)
            throw std::runtime_error ("Unexpected next Q literal in '"
                                      + filePath
                                      + ". Literal must be equal or less than "
                                      + std::to_string (maxNextQLiteral));
        }

      // Reads from ASCII format
      else
        {
          std::getline (inputFile, buf);
          lineNumber++;
          std::stringstream ss (buf);
          if (ss.eof ())
            throw std::runtime_error (
                "Unable to read '" + filePath
                + "'. File reached the end before expected.");

          try
            {
              std::getline (ss, buf, ' ');
              latchLiteral = std::stoul (buf);
              std::getline (ss, buf, ' ');
              nextQLiteral = std::stoul (buf);
            }
          catch (const std::exception &e)
            {
              throw std::runtime_error (
                  "In " + filePath + ": Failed to make conversion at line "
                  + std::to_string (lineNumber));
            }

          // Integrity checks
          unsigned int expectedLatchLiteral = literalFromLatchVectorIndex (i);
          if (latchLiteral != expectedLatchLiteral)
            throw std::runtime_error (
                "Unexpected latch literal in " + filePath + " at line "
                + std::to_string (lineNumber) + ". Expected latch literal is: "
                + std::to_string (expectedLatchLiteral));
          if (nextQLiteral < 2)
            throw std::runtime_error ("File " + filePath + " at line "
                                      + std::to_string (lineNumber)
                                      + " does not comply with AIGER "
                                        "specification: latch node tied to "
                                        "logic FALSE (0) or TRUE (1)");
          unsigned int maxNextQLiteral
              = literalFromIndex (_maxVariableIndex) + 1;
          if (nextQLiteral > maxNextQLiteral)
            throw std::runtime_error ("Unexpected next Q literal in '"
                                      + filePath + "' at line "
                                      + std::to_string (lineNumber)
                                      + ". Literal must be equal or less than "
                                      + std::to_string (maxNextQLiteral));
        }

      // Stores the latch node
      _latchVector.push_back (LatchNode ());
      _latchVector.back ().setFanout (0);
      _latchVector.back ().setNextQ (nextQLiteral);

      // Saves next state literal
      nextQLiterals->push_back (nextQLiteral);
    }

  // Saves output literals
  for (int i = 0; i < _numOutputs; i++)
    {
      // Integrity check
      if (inputFile.eof ())
        throw std::runtime_error ("Unexpected end of file in '" + filePath
                                  + "' at line "
                                  + std::to_string (lineNumber));

      // Gets a new line
      std::getline (inputFile, buf);
      lineNumber++;
      if (buf.size () < 1)
        throw std::runtime_error ("Unexpected empty line in '" + filePath
                                  + "' at line " + std::to_string (lineNumber)
                                  + ". Expecting a output literal");

      // Integrity check
      if (buf.substr (0, 1) == "-")
        throw std::runtime_error ("Negative output literal in '" + filePath
                                  + "' at line "
                                  + std::to_string (lineNumber));

      // Converts string to unsigned int
      unsigned int outputLiteral;
      try
        {
          outputLiteral = std::stoul (buf);
        }
      catch (const std::exception &e)
        {
          throw std::runtime_error (
              "Call to std::stoul() failed to make conversion at line "
              + std::to_string (lineNumber));
        }

      // Integrity check
      unsigned int maxOutputLiteral = literalFromIndex (_maxVariableIndex) + 1;
      if (outputLiteral > maxOutputLiteral)
        throw std::runtime_error ("Unexpected output literal in '" + filePath
                                  + "' at line " + std::to_string (lineNumber)
                                  + ". Literal must be equal or less than "
                                  + std::to_string (maxOutputLiteral));

      // Saves the output literal
      _outputLiteralVector.push_back (outputLiteral);
    }

  // Lambda function to decode a "delta encoding" from a binary AIGER file
  // More information on decoding binary AIGER can be found
  // at http://fmv.jku.at/aiger/
  auto decodeBinaryAigerDelta = [] (std::ifstream &fileStream) {
    // Get a single character from the AIGER file
    auto getNonEofChar = [] (std::ifstream &fileStream) {
      int c = fileStream.get ();
      if (c != EOF)
        return c;
      throw std::runtime_error (
          "Unexpected EOF found while decoding delta in binary AIGER file.");
    };

    // The piece of code below comes from AIGER format specification
    unsigned x = 0, i = 0;
    unsigned char ch;
    while ((ch = getNonEofChar (fileStream)) & 0x80)
      {
        x |= (ch & 0x7f) << (7 * i++);
      }
    return x | (ch << (7 * i));
  };

  // Reads and-nodes
  for (int i = 0; i < _numAnds; i++)
    {
      unsigned int andLiteral = 0;
      unsigned int rhs0Literal = 0;
      unsigned int rhs1Literal = 0;
      unsigned int delta0 = 0;
      unsigned int delta1 = 0;

      // Reads from binary format
      if (_isBinary)
        {
          delta0 = decodeBinaryAigerDelta (inputFile);
          delta1 = decodeBinaryAigerDelta (inputFile);
          andLiteral = literalFromAndVectorIndex (i);
          rhs0Literal = andLiteral - delta0;
          rhs1Literal = rhs0Literal - delta1;

          // Integrity checks
          if (rhs0Literal < rhs1Literal)
            throw std::runtime_error (
                "File " + filePath
                + " does not comply with AIGER specification. Condition "
                  "rhs1Literal "
                  ">= rhs0Literal must be satisfied for all AND gates");
          if (andLiteral <= rhs0Literal || andLiteral <= rhs1Literal)
            throw std::runtime_error (
                "File " + filePath
                + " does not comply with AIGER specification: Condition "
                  "andLiteral > "
                  "rhs1Literal >= rhs0Literal must be satisfied for all AND "
                  "gates");
          if (rhs0Literal < 2 || rhs1Literal < 2)
            throw std::runtime_error (
                "File " + filePath
                + " does not comply with AIGER specification: and-node tied "
                  "to logic "
                  "FALSE (0) or TRUE (1)");
        }

      // Reads from ASCII format
      else
        {
          std::getline (inputFile, buf);
          lineNumber++;
          std::stringstream ss (buf);
          if (ss.eof ())
            throw std::runtime_error (
                "Unable to read '" + filePath
                + "'. File reached the end before expected.");

          try
            {
              std::getline (ss, buf, ' ');
              andLiteral = std::stoul (buf);
              std::getline (ss, buf, ' ');
              rhs0Literal = std::stoul (buf);
              std::getline (ss, buf, ' ');
              rhs1Literal = std::stoul (buf);
            }
          catch (const std::exception &e)
            {
              throw std::runtime_error (
                  "In " + filePath + ": Failed to make conversion at line "
                  + std::to_string (lineNumber));
            }

          // Integrity checks
          unsigned int expectedAndLiteral = literalFromAndVectorIndex (i);
          if (andLiteral != expectedAndLiteral)
            throw std::runtime_error (
                "Unexpected and-literal in " + filePath + " at line "
                + std::to_string (lineNumber) + ". Expected and-literal is: "
                + std::to_string (expectedAndLiteral));
          if (rhs0Literal < rhs1Literal)
            throw std::runtime_error (
                "File " + filePath + " at line " + std::to_string (lineNumber)
                + " does not comply with AIGER specification. Condition "
                  "rhs1Literal "
                  ">= rhs0Literal must be satisfied for all AND gates");
          if (andLiteral <= rhs0Literal || andLiteral <= rhs1Literal)
            throw std::runtime_error (
                "File " + filePath + " at line " + std::to_string (lineNumber)
                + " does not comply with AIGER specification: Condition "
                  "andLiteral > "
                  "rhs1Literal >= rhs0Literal must be satisfied for all AND "
                  "gates");
          if (rhs0Literal < 2 || rhs1Literal < 2)
            throw std::runtime_error (
                "File " + filePath + " at line " + std::to_string (lineNumber)
                + " does not comply with AIGER specification: and-node tied "
                  "to logic "
                  "FALSE (0) or TRUE (1)");
        }

      // Stores the and-node
      _andVector.push_back (AndNode ());
      _andVector.back ().setFanout (0);
      _andVector.back ().setFirstChild (rhs0Literal);
      _andVector.back ().setSecondChild (rhs1Literal);

      // Updates the fanout of child nodes
      if (nodeIsAnd (rhs0Literal))
        _andVector[andVectorIndexFromLiteral (rhs0Literal)].incFanout ();
      else if (nodeIsLatch (rhs0Literal))
        _latchVector[latchVectorIndexFromLiteral (rhs0Literal)].incFanout ();
      if (nodeIsAnd (rhs1Literal))
        _andVector[andVectorIndexFromLiteral (rhs1Literal)].incFanout ();
      else if (nodeIsLatch (rhs1Literal))
        _latchVector[latchVectorIndexFromLiteral (rhs1Literal)].incFanout ();
    }

  // Saves named inputs, latches and outputs and comments
  while (std::getline (inputFile, buf))
    {
      // Saves comments
      if (_hasComments)
        {
          _commentVector.push_back (buf);
        }

      // Input, output and latch processing
      else
        {
          // Indicate that the comment section started
          if (buf[0] == 'c')
            _hasComments = true;

          // Input label processing
          if (buf[0] == 'i')
            {
              try
                {
                  if (!_hasNamedInputs)
                    _inputNameVector.reserve (_numInputs);
                  _hasNamedInputs = true;
                  std::stringstream ss (buf);
                  std::getline (ss, buf, ' ');
                  unsigned int inum = std::stoul (buf.substr (1));
                  if (inum > _inputNameVector.size ())
                    throw std::exception ();
                  std::getline (ss, buf, ' ');
                  _inputNameVector.push_back (buf);
                }
              catch (const std::exception &e)
                {
                  throw std::runtime_error (
                      "In " + filePath + ": error reading input symbols.");
                }
            }

          // Latch label processing
          if (buf[0] == 'l')
            {
              try
                {
                  if (!_hasNamedLatches)
                    _latchNameVector.reserve (_numLatches);
                  _hasNamedLatches = true;
                  std::stringstream ss (buf);
                  std::getline (ss, buf, ' ');
                  unsigned int lnum = std::stoul (buf.substr (1));
                  if (lnum > _latchNameVector.size ())
                    throw std::exception ();
                  std::getline (ss, buf, ' ');
                  _latchNameVector.push_back (buf);
                }
              catch (const std::exception &e)
                {
                  throw std::runtime_error (
                      "In " + filePath + ": error reading latch symbols.");
                }
            }

          // Output label processing
          if (buf[0] == 'o')
            {
              try
                {
                  if (!_hasNamedOutputs)
                    _outputNameVector.reserve (_numOutputs);
                  _hasNamedOutputs = true;
                  std::stringstream ss (buf);
                  std::getline (ss, buf, ' ');
                  unsigned int onum = std::stoul (buf.substr (1));
                  if (onum > _outputNameVector.size ())
                    throw std::exception ();
                  std::getline (ss, buf, ' ');
                  _outputNameVector.push_back (buf);
                }
              catch (const std::exception &e)
                {
                  throw std::runtime_error (
                      "In " + filePath + ": error reading output symbols.");
                }
            }
        }
    }

  // Integrity check
  if (_hasNamedInputs && _inputNameVector.size () != _numInputs)
    throw std::runtime_error ("Incomplete specified input symbols. AIG has "
                              + std::to_string (_numInputs)
                              + " inputs but only "
                              + std::to_string (_inputNameVector.size ())
                              + " input symbols were declared.");

  // Integrity check
  if (_hasNamedLatches && _latchNameVector.size () != _numLatches)
    throw std::runtime_error ("Incomplete specified latch symbols. AIG has "
                              + std::to_string (_numLatches)
                              + " latches but only "
                              + std::to_string (_latchNameVector.size ())
                              + " latch symbols were declared.");

  // Integrity check
  if (_hasNamedOutputs && _outputNameVector.size () != _numOutputs)
    throw std::runtime_error ("Incomplete specified output symbols. AIG has "
                              + std::to_string (_numOutputs)
                              + " outputs but only "
                              + std::to_string (_outputNameVector.size ())
                              + " output symbols declared.");

  // Updates the fanout of output latches, ANDs and next state literals
  for (const auto &outputLiteral : _outputLiteralVector)
    {
      if (nodeIsLatch (outputLiteral))
        {
          _latchVector.at (latchVectorIndexFromLiteral (outputLiteral))
              .incFanout ();
        }
      else if (nodeIsAnd (outputLiteral))
        {
          _andVector.at (andVectorIndexFromLiteral (outputLiteral))
              .incFanout ();
        }
    }
  for (const auto &nextQLiteral : *nextQLiterals)
    {
      if (nodeIsAnd (nextQLiteral))
        _andVector[andVectorIndexFromLiteral (nextQLiteral)].incFanout ();
      else if (nodeIsLatch (nextQLiteral))
        _latchVector[latchVectorIndexFromLiteral (nextQLiteral)].incFanout ();
    }

  // Deletes nextQLiterals list, since it is no longer necessary
  delete nextQLiterals;

  // Sets the AndInverterGraph object as initialized
  _initialized = true;
}

bool
AndInverterGraph::successfullyInitialized () const noexcept
{
  return _initialized;
}

bool
AndInverterGraph::isCombinational () const noexcept
{
  if (_numLatches == 0)
    return true;
  else
    return false;
}

bool
AndInverterGraph::isSequential () const noexcept
{
  if (_numLatches > 0)
    return true;
  else
    return false;
}

bool
AndInverterGraph::nodeIsInput (unsigned int nodeLiteral) const noexcept
{
  // Literals 0 (FALSE) and 1 (TRUE) are not inputs
  if (nodeLiteral <= 1)
    return false;

  unsigned int nodeVariableIndex = indexFromLiteral (nodeLiteral);
  if (nodeVariableIndex <= _numInputs)
    return true;
  else
    return false;
}

bool
AndInverterGraph::nodeIsLatch (unsigned int nodeLiteral) const noexcept
{
  // Literals 0 (FALSE) and 1 (TRUE) are not latches
  if (nodeLiteral <= 1)
    return false;

  unsigned int nodeVariableIndex = indexFromLiteral (nodeLiteral);
  if (nodeVariableIndex <= (_numInputs + _numLatches)
      && nodeVariableIndex > _numInputs)
    return true;
  else
    return false;
}

bool
AndInverterGraph::nodeIsAnd (unsigned int nodeLiteral) const noexcept
{
  // Literals 0 (FALSE) and 1 (TRUE) are not ANDs
  if (nodeLiteral <= 1)
    return false;

  unsigned int nodeVariableIndex = indexFromLiteral (nodeLiteral);
  if (nodeVariableIndex <= (_numInputs + _numLatches + _numAnds)
      && nodeVariableIndex > (_numInputs + _numLatches))
    return true;
  else
    return false;
}

const AndNode &
AndInverterGraph::getAndNodeFromLiteral (const unsigned int &andLiteral) const
{
  if (!nodeIsAnd (andLiteral))
    throw std::runtime_error (
        "getAndNodeFromLiteral() : not an and-node literal");

  unsigned int nodeVariableIndex = indexFromLiteral (andLiteral);
  unsigned int andVectorIndex
      = nodeVariableIndex - _numInputs - _numLatches - 1;

  if (andVectorIndex >= _andVector.size ())
    throw std::runtime_error (
        "getAndNodeFromLiteral() : andVectorIndex out of range");

  return _andVector.at (andVectorIndex);
}

const LatchNode &
AndInverterGraph::getLatchNodeFromLiteral (
    const unsigned int &latchLiteral) const
{
  if (!nodeIsLatch (latchLiteral))
    throw std::runtime_error (
        "getLatchNodeFromLiteral() : not a latch node literal");

  unsigned int nodeVariableIndex = indexFromLiteral (latchLiteral);
  unsigned int latchVectorIndex = nodeVariableIndex - _numInputs - 1;

  if (latchVectorIndex >= _latchVector.size ())
    throw std::runtime_error (
        "getLatchNodeFromLiteral() : latchVectorIndex out of range");

  return _latchVector.at (latchVectorIndex);
}

unsigned int
AndInverterGraph::indexFromLiteral (unsigned int literal) noexcept
{
  return literal >> 1;
}

unsigned int
AndInverterGraph::literalFromIndex (unsigned int index) noexcept
{
  return index << 1;
}

const std::vector<unsigned int> &
AndInverterGraph::getOutputLiteralVector () const noexcept
{
  return _outputLiteralVector;
}

std::string
AndInverterGraph::getFilePath () const noexcept
{
  return _filePath;
}

unsigned int
AndInverterGraph::getMaxVariableIndex () const noexcept
{
  return _maxVariableIndex;
}

unsigned int
AndInverterGraph::getNumInputs () const noexcept
{
  return _numInputs;
}

unsigned int
AndInverterGraph::getNumLatches () const noexcept
{
  return _numLatches;
}

unsigned int
AndInverterGraph::getNumOutputs () const noexcept
{
  return _numOutputs;
}

unsigned int
AndInverterGraph::getNumAnds () const noexcept
{
  return _numAnds;
}

unsigned int
AndInverterGraph::getFirstAndLiteral () const noexcept
{
  return literalFromIndex (0 + _numInputs + _numLatches + 1);
}

unsigned int
AndInverterGraph::getFirstLatchLiteral () const noexcept
{
  return literalFromIndex (0 + _numInputs + 1);
}

void
AndInverterGraph::print (std::ostream &os) const
{
  os << *this;
}

std::ostream &
operator<< (std::ostream &os, const AndInverterGraph &aig)
{
  os << ">> Start of AIG information." << std::endl;
  os << std::endl;

  if (aig._isBinary)
    os << "AIGER format: binary" << std::endl;
  else
    os << "AIGER format: ASCII" << std::endl;

  os << std::endl;
  os << "Header:" << std::endl;
  os << "M I L O A = " << aig._maxVariableIndex << " " << aig._numInputs << " "
     << aig._numLatches << " " << aig._numOutputs << " " << aig._numAnds
     << std::endl;

  os << std::endl;
  os << "Inputs: " << std::endl;
  for (int i = 1; i <= aig._numInputs; i++)
    os << aig.literalFromIndex (i) << std::endl;

  os << std::endl;
  os << "Latches: " << std::endl;
  for (int i = 0; i < aig._numLatches; i++)
    os << aig.literalFromLatchVectorIndex (i) << " "
       << aig._latchVector.at (i).getNextQ () << " " << std::endl;

  os << std::endl;
  os << "Outputs: " << std::endl;
  for (const auto &output : aig._outputLiteralVector)
    os << output << std::endl;

  os << std::endl;
  os << "And nodes: " << std::endl;
  for (int i = 0; i < aig._numAnds; i++)
    os << aig.literalFromAndVectorIndex (i) << " "
       << aig._andVector.at (i).getFirstChild () << " "
       << aig._andVector.at (i).getSecondChild () << " " << std::endl;

  os << std::endl;
  os << "Input names (if any):" << std::endl;
  if (aig._hasNamedInputs)
    for (const auto &inputName : aig._inputNameVector)
      os << inputName << std::endl;

  os << std::endl;
  os << "Latch names (if any):" << std::endl;
  if (aig._hasNamedLatches)
    for (const auto &latchName : aig._latchNameVector)
      os << latchName << std::endl;

  os << std::endl;
  os << "Output names (if any):" << std::endl;
  if (aig._hasNamedOutputs)
    for (const auto &outputName : aig._outputNameVector)
      os << outputName << std::endl;

  os << std::endl;
  os << "Comments (if any):" << std::endl;
  if (aig._hasComments)
    for (const auto &comment : aig._commentVector)
      os << comment << std::endl;

  os << std::endl;
  os << ">> End of AIG information." << std::endl;

  return os;
}

unsigned int
AndInverterGraph::andVectorIndexFromLiteral (unsigned int andLiteral) const
{
  unsigned int andVectorIndex
      = indexFromLiteral (andLiteral) - _numInputs - _numLatches - 1;
  if (andVectorIndex >= _andVector.size ())
    throw std::overflow_error (
        "Range overflow. Index used to access _andVector is greater or equal "
        "its size ");
  else
    return andVectorIndex;
}

unsigned int
AndInverterGraph::literalFromAndVectorIndex (unsigned int andVectorIndex) const
{
  unsigned int andLiteral
      = literalFromIndex (andVectorIndex + _numInputs + _numLatches + 1);
  if (andVectorIndex > literalFromIndex (_maxVariableIndex) + 1)
    throw std::runtime_error (
        "Runtime error (AndInverterGraph). " + std::to_string (andLiteral)
        + " is not a valid and-literal for the given AIG.");
  else
    return andLiteral;
}

unsigned int
AndInverterGraph::latchVectorIndexFromLiteral (unsigned int latchLiteral) const
{
  unsigned int latchVectorIndex
      = indexFromLiteral (latchLiteral) - _numInputs - 1;
  if (latchVectorIndex >= _latchVector.size ())
    throw std::overflow_error ("Range overflow. Index used to access "
                               "_latchVector is greater or equal "
                               "its size");
  else
    return latchVectorIndex;
}

unsigned int
AndInverterGraph::literalFromLatchVectorIndex (
    unsigned int latchVectorIndex) const
{
  unsigned int latchLiteral
      = literalFromIndex (latchVectorIndex + _numInputs + 1);
  if (latchLiteral > literalFromIndex (_numInputs + 1))
    throw std::runtime_error (
        "Runtime error. " + std::to_string (latchLiteral)
        + " is not a valid latch literal for the given AIG.");
  else
    return latchLiteral;
}