/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <test/tools/ossfuzz/libFuzzerMutator.h>

#include <antlr4-runtime.h>
#include <cstring>

using namespace std;
using namespace antlr4;
using namespace solidity::test::fuzzer;

extern "C" size_t LLVMFuzzerMutate(uint8_t *Data, size_t Size, size_t MaxSize);

SolCustomMutator::SolCustomMutator(uint8_t* _data, size_t _size, size_t _maxSize, unsigned int _seed)
{
	Data = _data;
	Size = _size;
	In = string(_data, _data + _size),
	MaxMutantSize = _maxSize;
	antlr4::ANTLRInputStream AStream(In);
	SolidityLexer Lexer(&AStream);
	Lexer.removeErrorListeners();
	antlr4::CommonTokenStream Tokens(&Lexer);
	Tokens.fill();
	SolidityParser Parser(&Tokens);
	Parser.removeErrorListeners();
	SolidityMutator Visitor(make_shared<RandomEngine>(_seed));
	Parser.sourceUnit()->accept(&Visitor);
	Out = Visitor.toString();
}

size_t SolCustomMutator::mutate()
{
	if (Out.empty())
		return LLVMFuzzerMutate(Data, Size, MaxMutantSize);
	else
	{
		std::cout << "Mutation: " << Out << std::endl;
		size_t mutantSize = Out.size() >= MaxMutantSize ? MaxMutantSize - 1 : Out.size();
		mempcpy(Data, Out.data(), mutantSize);
		return mutantSize;
	}
}