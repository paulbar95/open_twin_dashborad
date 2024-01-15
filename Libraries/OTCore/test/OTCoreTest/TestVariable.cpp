#include <iostream>
#include "gtest/gtest.h"
#include "OTCore/Variable.h"
#include "OTCore/VariableToJSONConverter.h"
#include "OTCore/JSONToVariableConverter.h"
#include "OTCore/StringToVariableConverter.h"

#include "FixtureVariable.h"

#include <string>
#include <limits>

TEST(VariableTest, CopyAssignOperator)
{
	ot::Variable first(5);
	ot::Variable second(0);
	second = first;
	EXPECT_EQ(first, second);
}

TEST(VariableTest, VariableToJSON)
{
	ot::JsonDocument (doc);
	ot::Variable var = 5;
	ot::VariableToJSONConverter converter;
	rapidjson::Value result = converter(var,doc.GetAllocator());
	EXPECT_TRUE(result.IsInt());
	EXPECT_EQ(result.GetInt(), var.getInt32());
}


TEST(VariableTest, JSONToVariable)
{
	int expected = 5;
	rapidjson::Value var(expected);

	ot::JSONToVariableConverter converter;
	ot::Variable result = converter(var);
	
	EXPECT_TRUE(result.isInt32());
	EXPECT_EQ(var.GetInt(), result.getInt32());
}

TEST(VariableTest, JSONToVariableNotSupportedType)
{
	rapidjson::Value var;
	ot::JSONToVariableConverter converter;
	
	EXPECT_ANY_THROW(converter(var));
}

double calculate(double start, double decrement, int count)
{
	for (int i = 0; i < count; ++i)
		start -= decrement;
	return start;
}

//float calculate(float start, float decrement, int count)
//{
//	for (int i = 0; i < count; ++i)
//		start -= decrement;
//	return start;
//}

TEST(VariableTest, DoubleComparision)
{
	const int total = 10000;
	int count = 0;
	for (auto i = 0; i < total; ++i)
	{
		double expected = (i / 10.0);
		double actual = calculate(9.0 + expected, 0.1, 90);
		if(ot::Variable(actual) == ot::Variable(expected))
		{
			++count;
		}
	}
	EXPECT_EQ(count, total);
}

TEST_F(FixtureVariable, UniqueList)
{
	std::list< ot::Variable> list = GetSMA8_333_FA_Amplitudes();
	list.sort();
	list.unique();

	const size_t expectedNumberOfEntries = 327;
	const size_t actualNumberOfEntries = list.size();

	EXPECT_EQ(expectedNumberOfEntries,actualNumberOfEntries);
}

TEST(VariableTest, FloatComparision)
{
	const int total = 10000;
	int count = 0;
	for (auto i = 0; i < total; ++i)
	{
		float expected = (i / 10.0f);
		float actual = calculate(9.0f + expected, 0.01f, 900);
		if (ot::Variable(actual) == ot::Variable(expected))
		{
			++count;
		}
	}
	EXPECT_EQ(count, total);
}

TEST_P(FixtureVariable, Equal)
{
	ot::Variable isValue = GetParam();
	auto expectedValues= GetExpectedEqualEntries();
	bool equal = false;
	int count = 0;
	for (ot::Variable& var : expectedValues)
	{
		if (var == isValue)
		{
			equal = true;
			count++;
		}
	}
	EXPECT_EQ(count, 1) << "Failed with type: " + isValue.getTypeName();
	EXPECT_TRUE(equal) << "Failed with type: " + isValue.getTypeName();
}

TEST_P(FixtureVariable, Larger)
{
	ot::Variable isValue = GetParam();
	if (isValue.isBool())
	{
		EXPECT_TRUE(true);
	}
	else
	{
		auto expectedValues = GetExpectedLargerEntries();
		bool larger = false;
		int count = 0;
		for (ot::Variable& var : expectedValues)
		{
			if (var > isValue)
			{
				larger = true;
				count++;
			}
		}
		EXPECT_EQ(count, 1) << "Failed with type: " + isValue.getTypeName();
		EXPECT_TRUE(larger) << "Failed with type: " + isValue.getTypeName();
	}
}

TEST_P(FixtureVariable, Smaller)
{
	ot::Variable isValue = GetParam();
	if (isValue.isBool())
	{
		EXPECT_TRUE(true);
	}
	else
	{
		auto expectedValues = GetExpectedLargerEntries();
		bool larger = false;
		int count = 0;
		for (ot::Variable& var : expectedValues)
		{
			if (isValue < var)
			{
				larger = true;
				count++;
			}
		}
		EXPECT_EQ(count, 1) << "Failed with type: " + isValue.getTypeName();
		EXPECT_TRUE(larger) << "Failed with type: " + isValue.getTypeName();
	}
}

TEST(StringToVariable, StringToInt32Variable)
{	
	constexpr const int32_t expectedValue = std::numeric_limits<int32_t>::max();
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue));
	
	EXPECT_TRUE(actualValue.isInt32());
	EXPECT_EQ(actualValue.getInt32(), expectedValue);
}

TEST(StringToVariable, StringToInt64Variable)
{
	//const int64_t expectedValue = std::numeric_limits<int64_t>::max();
	constexpr const long long expectedValue = std::numeric_limits<long long>::max();
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue));
	
	EXPECT_TRUE(actualValue.isInt64());
	EXPECT_EQ(actualValue.getInt64(), expectedValue);
}

TEST(StringToVariable, StringToFloatVariable)
{
	const float expectedValue = 4.f;
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue));

	EXPECT_TRUE(actualValue.isFloat());
	EXPECT_TRUE(actualValue.getFloat() == expectedValue);
}

TEST(StringToVariable, StringToDoubleVariable)
{
	constexpr const double expectedValue = std::numeric_limits<double>::max();
	ot::Variable expectedVariable(expectedValue);
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter(std::to_string(expectedValue));

	EXPECT_TRUE(actualValue.isDouble());
	EXPECT_TRUE(actualValue == expectedVariable);
}

TEST(StringToVariable, StringToStringVariable)
{
	const std::string expectedValue = "4";
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter("\""+expectedValue+"\"");

	EXPECT_TRUE(actualValue.isConstCharPtr());
	EXPECT_EQ(actualValue.getConstCharPtr(), expectedValue);
}
TEST(StringToVariable, StringToBoolVariable)
{
	const bool expectedValue = true;
	ot::StringToVariableConverter converter;
	const ot::Variable actualValue = converter("true");

	EXPECT_TRUE(actualValue.isBool());
	EXPECT_EQ(actualValue.getBool(), expectedValue);
}