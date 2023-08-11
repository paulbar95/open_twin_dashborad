/*****************************************************************//**
 * \file   EncodingConverter_UTF16ToUTF8.h
 * \brief  Converter that can transform a set of UTF-16 LE BOM or UTF16 BE BOM characters into a set of UTF-8 character.
 * 
 * \author Wagner
 * \date   August 2023
 *********************************************************************/
#pragma once
#include <vector>
#include <string>
#include "OpenTwinCore/CoreAPIExport.h"
#include"OpenTwinCore/TextEncoding.h"

namespace ot
{
	class OT_CORE_API_EXPORT EncodingConverter_UTF16ToUTF8
	{
	public:
		std::string operator()(ot::TextEncoding::EncodingStandard utf16Flavour, const std::vector<char>& fileContent);

	private:
		using byte = unsigned char;
		ot::TextEncoding::EncodingStandard _utf16Flavour;
		byte* _currentByte;
		std::string _out;

		char16_t getNextUFT16();
		void setNextUTF8Character(byte&& character);
		void TransformNextWORD();
	};
}
