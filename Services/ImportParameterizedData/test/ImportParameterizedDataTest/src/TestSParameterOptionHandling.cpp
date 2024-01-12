#include "FixtureSParameterHandler.h"
#include "OptionSettings.h"
#include "Options.h"
#include <sstream>

TEST_F(FixtureSParameterHandler, OptionDetectionSuccess)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Admittance, ot::Variable(100));
	
	std::string setting = "# kHz Y RI R 100";
	const sp::OptionSettings actualSettings	= AnalyseOptionSettings(setting);
	

	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionFrequencyOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::GHz, sp::option::Format::real_imaginary, sp::option::Parameter::Admittance, ot::Variable(100));

	std::string setting = "# Y RI R 100";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionFormatOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::magnitude_angle, sp::option::Parameter::Admittance, ot::Variable(100));

	std::string setting = "# kHz Y R 100";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionParameterOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(100));

	std::string setting = "# kHz S RI R 100";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionResistanceOmitted)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Admittance, ot::Variable(50));

	std::string setting = "# kHz Y RI";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, BlankOptionLine)
{
	const sp::OptionSettings expectedOptionSettings;

	std::string setting = "#";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionTwoEntries)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(50));

	std::string setting = "# kHz RI";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionOnlyResistance)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::GHz, sp::option::Format::magnitude_angle, sp::option::Parameter::Scattering, ot::Variable(80));

	std::string setting = "#R 80";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionUnevenNumberOfWhitespaces)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(50));

	std::string setting = "#          kHz      RI";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, OptionChangedOrder)
{
	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::kHz, sp::option::Format::real_imaginary, sp::option::Parameter::Scattering, ot::Variable(50));

	std::string setting = "# RI kHz";
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);


	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}


TEST_F(FixtureSParameterHandler, OptionDetectedAfterComments)
{
	const sp::OptionSettings expectedOptionSettings;

	std::string setting = "! This is a first comment.\n"
		"!And a second\n"
		"#";
	AnalyseFile(setting);
	const sp::OptionSettings actualSettings = AnalyseOptionSettings(setting);

	EXPECT_TRUE(expectedOptionSettings == actualSettings);
}

TEST_F(FixtureSParameterHandler, CommentsDetected)
{
	const std::string expectedComments = " This is a first comment.\nAnd a second\nOne more\n";

	std::string setting = "! This is a first comment.\n"
		"!And a second\n"
		"#\n"
		"!One more\n";
	AnalyseFile(setting);

	const std::string actualComments = GetComments();

	EXPECT_TRUE(actualComments == expectedComments);
}

TEST_F(FixtureSParameterHandler, FreeDataLineOfComment)
{
	const std::string expectedLine = " This is no comment.";
	const std::string line = " This is no comment.!But this is";
	const std::string actualLine = CleansLineOfComments(line);

	EXPECT_EQ(expectedLine, actualLine);	
}

TEST_F(FixtureSParameterHandler, V1_Example1Port)
{
	std::string line = "2.000 0.894 -12.136\n";
	SetNumberOfPorts(1);
	AnalyseLine(line);
	auto& portData =	GetPortData();
	EXPECT_EQ(portData.size(), 1);
}

TEST_F(FixtureSParameterHandler, V1_Example4Ports_Comments)
{
	const int numberOfPorts = 4;
	const std::string expectedComments =	"4 - port S - parameter data, taken at three frequency points\n"
											"note that data points need not be aligned\n";
	
	const std::string& file = GetFullExampleFourPorts();
	SetNumberOfPorts(numberOfPorts);
	AnalyseFile(file);
	
	const std::string& actualComments = GetComments();
	
	EXPECT_EQ(expectedComments, actualComments);
}

TEST_F(FixtureSParameterHandler, V1_Example4Ports_Options)
{
	const int numberOfPorts = 4;

	const sp::OptionSettings expectedOptionSettings(sp::option::Frequency::Hz, sp::option::Format::real_imaginary, sp::option::Parameter::Admittance, ot::Variable(90));

	const std::string& file = GetFullExampleFourPorts();
	SetNumberOfPorts(numberOfPorts);
	AnalyseFile(file);

	const sp::OptionSettings& actualOptionSettings = GetOptionSettings();

	EXPECT_EQ(expectedOptionSettings, actualOptionSettings);
}

TEST_F(FixtureSParameterHandler, V1_Example4Ports_Data_Sizes)
{
	const int numberOfPorts = 4;
	
	const size_t expectedNumberOfEntries = 3;
	const size_t _expectedNumberOfEntriesPerPortData = numberOfPorts * numberOfPorts;

	const std::string& file = GetFullExampleFourPorts();
	SetNumberOfPorts(numberOfPorts);
	AnalyseFile(file);

	const auto& actualPortData = GetPortData();

	EXPECT_EQ(actualPortData.size(), expectedNumberOfEntries);
	int counter(0);
	for (const sp::PortData& portData : actualPortData)
	{
		counter++;
		const auto& actualPortDataEntries = actualPortData.back().getPortDataEntries();
		EXPECT_EQ(actualPortDataEntries.size(), _expectedNumberOfEntriesPerPortData);
	}
}

TEST_F(FixtureSParameterHandler, V1_Example4Ports_Data_Values)
{
	const int numberOfPorts = 4;

	const std::vector<float> expectedFrequencyValues{ 5.00000f,6.00000f,7.00000f };
	const sp::PortDataEntry lastDataEntry = { sp::PortDataEntry(sp::PortDataSingleEntry {0.50f},sp::PortDataSingleEntry {136.69f}) };

	const std::string& file = GetFullExampleFourPorts();
	SetNumberOfPorts(numberOfPorts);
	AnalyseFile(file);

	const auto& actualPortData = GetPortData();

	int counter(0);
	for (const sp::PortData& portData : actualPortData)
	{
		bool typeFound = std::holds_alternative <float>(portData.getFrequency());
		EXPECT_TRUE(typeFound);
		float actualValue =	std::get<float>(portData.getFrequency());

		EXPECT_FLOAT_EQ(actualValue, expectedFrequencyValues[counter]);
		counter++;
	
		if (counter == 2)
		{
			const auto& actualPortDataEntries = actualPortData.back().getPortDataEntries();
			EXPECT_EQ(std::get<float>(std::get<0>(actualPortDataEntries.back())), std::get<float>(std::get<0>(lastDataEntry)));
			EXPECT_EQ(std::get<float>(std::get<1>(actualPortDataEntries.back())), std::get<float>(std::get<1>(lastDataEntry)));
		}
	}
}