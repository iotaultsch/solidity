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
// SPDX-License-Identifier: GPL-3.0
#pragma once

#include <libyul/optimiser/NameDispenser.h>
#include <libyul/AsmData.h>
#include <libyul/Dialect.h>
#include <libyul/Exceptions.h>

#include <liblangutil/SourceLocation.h>

#include <libsolutil/CommonData.h>

#include <variant>

namespace solidity::yul
{

template<typename T>
std::vector<T> applyBooleanMask(std::vector<T> const& _vec, std::vector<bool> const& _mask)
{
	yulAssert(_vec.size() == _mask.size(), "");

	std::vector<T> ret;

	for (size_t i = 0; i < _mask.size(); ++i)
		if (_mask[i])
			ret.push_back(_vec[i]);

	return ret;
}

/// Find functions whose arguments are not used in its body. Also, find functions whose body
/// satisfies a heuristic about pruning.
bool wasPruned(FunctionDefinition const& _f, Dialect const& _dialect)
{
	// We skip the function body if it
	// 1. is empty, or
	// 2. is a single statement that is an assignment statement whose value is a non-builtin
	//    function call, or
	// 3. is a single expression-statement that is a non-builtin function call.
	// The above cases are simple enough so that the inliner alone can remove the parameters.
	if (_f.body.statements.empty())
		return true;
	if (_f.body.statements.size() == 1)
	{
		Statement const& e = _f.body.statements[0];
		if (std::holds_alternative<Assignment>(e))
		{
			if (std::holds_alternative<FunctionCall>(*std::get<Assignment>(e).value))
			{
				FunctionCall c = std::get<FunctionCall>(*std::get<Assignment>(e).value);
				if (!_dialect.builtin(c.functionName.name))
					return true;
			}
		}
		else if (std::holds_alternative<ExpressionStatement>(e))
			if (std::holds_alternative<FunctionCall>(std::get<ExpressionStatement>(e).expression))
			{
				FunctionCall c = std::get<FunctionCall>(std::get<ExpressionStatement>(e).expression);
				if (!_dialect.builtin(c.functionName.name))
					return true;
			}
	}
	return false;
}

FunctionDefinition createReplacement(
	FunctionDefinition& _old,
	std::optional<std::vector<bool>> const& _usedParameters,
	std::optional<std::vector<bool>> const& _usedReturnVariables,
	NameDispenser& _nameDispenser,
	YulString const& _newName)
{
	auto generateName = [&](TypedName t)
	{
		return TypedName{
			t.location,
			_nameDispenser.newName(t.name),
			t.type
		};
	};

	langutil::SourceLocation loc = _old.location;
	TypedNameList functionParameters;
	TypedNameList returnVariables;
	TypedNameList renamedParameters = util::applyMap(_old.parameters, generateName);
	TypedNameList reducedRenamedParameters;
	TypedNameList renamedReturnVariables = util::applyMap(_old.returnVariables, generateName);
	TypedNameList reducedRenamedReturnVariables;

	if (_usedParameters)
	{
		std::vector<bool> const& mask = _usedParameters.value();
		functionParameters = applyBooleanMask(_old.parameters, mask);
		reducedRenamedParameters = applyBooleanMask(renamedParameters, mask);
	}
	else
	{
		functionParameters = _old.parameters;
		reducedRenamedParameters = renamedParameters;
	}

	if (_usedReturnVariables)
	{
		std::vector<bool> const& mask = _usedReturnVariables.value();
		returnVariables = applyBooleanMask(_old.returnVariables, mask);
		reducedRenamedReturnVariables = applyBooleanMask(renamedReturnVariables, mask);
	}
	else
	{
		returnVariables = _old.returnVariables;
		reducedRenamedReturnVariables = renamedReturnVariables;
	}

	FunctionDefinition newFunction{
		loc,
		_old.name,
		renamedParameters,
		reducedRenamedParameters,
		{loc, {}} // body
	};

	_old.name = _newName;
	_old.parameters = std::move(functionParameters);
	_old.returnVariables = std::move(returnVariables);

	std::swap(newFunction.parameters, renamedParameters);
	std::swap(newFunction.returnVariables, renamedReturnVariables);

	FunctionCall call{loc, Identifier{loc, _old.name}, {}};
	for (auto const& p: reducedRenamedParameters)
		call.arguments.emplace_back(Identifier{loc, p.name});

	// Replace the body of `f_1` by an assignment which calls `f`, i.e.,
	// `return_parameters = f(reduced_parameters)`
	if (!_old.returnVariables.empty())
	{
		Assignment assignment;
		assignment.location = loc;

		// The LHS of the assignment.
		for (auto const& r: reducedRenamedReturnVariables)
			assignment.variableNames.emplace_back(Identifier{loc, r.name});

		assignment.value = std::make_unique<Expression>(std::move(call));

		newFunction.body.statements.emplace_back(std::move(assignment));
	}
	else
		newFunction.body.statements.emplace_back(ExpressionStatement{loc, std::move(call)});

	return newFunction;
}


}
