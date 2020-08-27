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

#include <test/tools/ossfuzz/SolidityMutator.h>

#include <boost/algorithm/string/join.hpp>
#include <libsolutil/Whiskers.h>

using namespace std;
using namespace solidity::util;
using namespace solidity::test::fuzzer;

string SolidityMutator::genRandString(unsigned int _length)
{
	uniform_int_distribution<int> dist('!', '~');
	string result{};
	generate_n(back_inserter(result), _length, [&]{ return dist(*Rand); });
	return result;
}

antlrcpp::Any SolidityMutator::visitPragmaDirective(SolidityParser::PragmaDirectiveContext*)
{
	auto PickLiteral = [](unsigned const len) -> string
	{
		// TODO: Add to this list of valid pragmas
		static const vector<string> alphanum = {
			"solidity >=0.0.0",
			"solidity ^0.4.24",
			"experimental SMTChecker",
			"experimental ABIEncoderV2",
		};

		return alphanum[len % alphanum.size()];
	};

	if (coinFlip())
		Out << Whiskers(R"(pragma <string>;<nl>)")
			("string", PickLiteral(random()))
			("nl", "\n")
			.render();
	else
		Out << Whiskers(R"(pragma <string>;<nl>)")
			("string", genRandString(randAtMost()))
			("nl", "\n")
			.render();

	return antlrcpp::Any();
}

antlrcpp::Any SolidityMutator::visitImportDirective(SolidityParser::ImportDirectiveContext* _ctx)
{
	// Pseudo randomly chosen five character ID
	string id{genRandString(5)};
	// Pseudo randomly chosen single character path + .sol extension
	string path{"\"" + genRandString(1) + ".sol" + "\""};
	switch (randAtMost(4))
	{
	case 1:
		Out << Whiskers(R"(import <path> as <id>;<nl>)")
			("path", path)
			("id", id)
			("nl", "\n")
			.render();
		break;
	case 2:
		Out << Whiskers(R"(import * as <id> from <path>;<nl>)")
			("id", id)
			("path", path)
			("nl", "\n")
			.render();
		break;
	case 3:
	{
		vector<string> symbols{};
		unsigned numElements = randAtMost(10);
		generate_n(
			back_inserter(symbols),
			numElements,
			[&](){
				return Whiskers((R"(<?as><path> as <id><!as><symbol></as>)"))
					("as", coinFlip())
					("path", "\"" + genRandString(1) + ".sol" + "\"")
					("id", genRandString(5))
					("symbol", genRandString(1))
					.render();
			}
        );
		Out << Whiskers(R"(import {<symbolAliases>} from <path>;<nl>)")
			("symbolAliases", boost::algorithm::join(symbols, ", "))
			("path", path)
			("nl", "\n")
			.render();
		break;
	}
	case 4:
		Out << Whiskers(R"(import "<path>.sol";<nl>)")
			("path", genRandString(1))
			("nl", "\n")
			.render();
		break;
	}

	return antlrcpp::Any();
}