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

#pragma once

#include <test/tools/ossfuzz/SolidityBaseVisitor.h>

#include <random>

namespace solidity::test::fuzzer {
	using RandomEngine = std::mt19937_64;
	class SolidityMutator: public SolidityBaseVisitor
	{
	public:
		SolidityMutator(std::shared_ptr<RandomEngine> _r): Rand(_r) {}
		std::string toString()
		{
			return Out.str();
		}
		antlrcpp::Any visitPragmaDirective(SolidityParser::PragmaDirectiveContext* _ctx) override;
		antlrcpp::Any visitImportDirective(SolidityParser::ImportDirectiveContext* _ctx) override;

	private:
		bool coinFlip()
		{
			return (*Rand)() % 2 == 0;
		}

		unsigned randAtMost(unsigned _max = 20)
		{
			return (*Rand)() % _max + 1;
		}

		std::string genRandString(unsigned _length);

		std::ostringstream Out;
		std::shared_ptr<RandomEngine> Rand;
	};
}