////////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
////////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////
#include "otpch.h"
#include "tools.h"

#include <iostream>
#include <iomanip>

#include <boost/filesystem.hpp>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <random>

#include "vocation.h"
#include "configmanager.h"

extern ConfigManager g_config;

std::string transformToMD5(std::string plainText, bool upperCase)
{
	MD5_CTX c;
	MD5_Init(&c);
	MD5_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[MD5_DIGEST_LENGTH];
	MD5_Final(md, &c);

	char output[(MD5_DIGEST_LENGTH << 1) + 1];
	for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
		sprintf(output + (i << 1), "%.2X", md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

std::string transformToSHA1(std::string plainText, bool upperCase)
{
	SHA_CTX c;
	SHA1_Init(&c);
	SHA1_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[SHA_DIGEST_LENGTH];
	SHA1_Final(md, &c);

	char output[(SHA_DIGEST_LENGTH << 1) + 1];
	for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
		sprintf(output + (i << 1), "%.2X", md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

std::string transformToSHA256(std::string plainText, bool upperCase)
{
	SHA256_CTX c;
	SHA256_Init(&c);
	SHA256_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[SHA256_DIGEST_LENGTH];
	SHA256_Final(md, &c);

	char output[(SHA256_DIGEST_LENGTH << 1) + 1];
	for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
		sprintf(output + (i << 1), "%.2X", md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

std::string transformToSHA512(std::string plainText, bool upperCase)
{
	SHA512_CTX c;
	SHA512_Init(&c);
	SHA512_Update(&c, plainText.c_str(), plainText.length());

	uint8_t md[SHA512_DIGEST_LENGTH];
	SHA512_Final(md, &c);

	char output[(SHA512_DIGEST_LENGTH << 1) + 1];
	for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
		sprintf(output + (i << 1), "%.2X", md[i]);

	if(upperCase)
		return std::string(output);

	return asLowerCaseString(std::string(output));
}

void _encrypt(std::string& str, bool upperCase)
{
	switch(g_config.getNumber(ConfigManager::ENCRYPTION))
	{
		case ENCRYPTION_MD5:
			str = transformToMD5(str, upperCase);
			break;
		case ENCRYPTION_SHA1:
			str = transformToSHA1(str, upperCase);
			break;
		case ENCRYPTION_SHA256:
			str = transformToSHA256(str, upperCase);
			break;
		case ENCRYPTION_SHA512:
			str = transformToSHA512(str, upperCase);
			break;
		default:
		{
			if(upperCase)
				std::transform(str.begin(), str.end(), str.begin(), upchar);

			break;
		}
	}
}

bool encryptTest(std::string plain, std::string& hash)
{
	std::transform(hash.begin(), hash.end(), hash.begin(), upchar);
	_encrypt(plain, true);
	return plain == hash;
}

bool replaceString(std::string& text, const std::string& key, const std::string& value)
{
	if(text.find(key) == std::string::npos)
		return false;

	std::string::size_type start = 0, pos = 0;
	while((start = text.find(key, pos)) != std::string::npos)
	{
		text.replace(start, key.size(), value);
		pos = start + value.size();
	}

	return true;
}

void trim_right(std::string& source, const std::string& t)
{
	source.erase(source.find_last_not_of(t) + 1);
}

void trim_left(std::string& source, const std::string& t)
{
	source.erase(0, source.find_first_not_of(t));
}

void toLowerCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), tolower);
}

void toUpperCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), upchar);
}

std::string asLowerCaseString(const std::string& source)
{
	std::string s = source;
	toLowerCaseString(s);
	return s;
}

std::string asUpperCaseString(const std::string& source)
{
	std::string s = source;
	toUpperCaseString(s);
	return s;
}

bool booleanString(std::string source)
{
	toLowerCaseString(source);
	return (source == "yes" || source == "true" || atoi(source.c_str()) > 0);
}

std::string ucfirst(std::string source)
{
	for(uint16_t i = 0; i < (uint16_t)source.length(); ++i)
	{
		if(source[i] != ' ')
		{
			source[i] = upchar(source[i]);
			break;
		}
	}

	return source;
}

std::string ucwords(std::string source)
{
	bool tmp = true;
	for(uint16_t i = 0; i < (uint16_t)source.length(); ++i)
	{
		if(source[i] == ' ')
			tmp = true;
		else if(tmp)
		{
			source[i] = upchar(source[i]);
			tmp = false;
		}
	}

	return source;
}

bool readXMLInteger(xmlNodePtr node, const char* tag, int32_t& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	value = atoi(nodeValue);
	xmlFree(nodeValue);
	return true;
}

bool readXMLInteger64(xmlNodePtr node, const char* tag, int64_t& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	value = atoll(nodeValue);
	xmlFree(nodeValue);
	return true;
}

bool readXMLFloat(xmlNodePtr node, const char* tag, float& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	value = atof(nodeValue);
	xmlFree(nodeValue);
	return true;
}

bool readXMLString(xmlNodePtr node, const char* tag, std::string& value)
{
	char* nodeValue = (char*)xmlGetProp(node, (xmlChar*)tag);
	if(!nodeValue)
		return false;

	if(!utf8ToLatin1(nodeValue, value))
		value = nodeValue;

	xmlFree(nodeValue);
	return true;
}

bool readXMLContentString(xmlNodePtr node, std::string& value)
{
	char* nodeValue = (char*)xmlNodeGetContent(node);
	if(!nodeValue)
		return false;

	if(!utf8ToLatin1(nodeValue, value))
		value = nodeValue;

	xmlFree(nodeValue);
	return true;
}

bool parseXMLContentString(xmlNodePtr node, std::string& value)
{
	bool result = false;
	std::string compareValue;
	while(node)
	{
		if(xmlStrcmp(node->name, (const xmlChar*)"text") && node->type != XML_CDATA_SECTION_NODE)
		{
			node = node->next;
			continue;
		}

		if(!readXMLContentString(node, compareValue))
		{
			node = node->next;
			continue;
		}

		trim_left(compareValue, "\r");
		trim_left(compareValue, "\n");
		trim_left(compareValue, " ");
		if(compareValue.length() > value.length())
		{
			value = compareValue;
			if(!result)
				result = true;
		}

		node = node->next;
	}

	return result;
}

std::string getLastXMLError()
{
	std::ostringstream ss;
	xmlErrorPtr lastError = xmlGetLastError();
	if(lastError->line)
		ss << "Line: " << lastError->line << ", ";

	ss << "Info: " << lastError->message << std::endl;
	return ss.str();
}

bool utf8ToLatin1(char* inText, std::string& outText)
{
	outText = "";
	if(!inText)
		return false;

	int32_t inLen = strlen(inText);
	if(!inLen)
		return false;

	int32_t outLen = inLen << 1;
	uint8_t* outBuf = new uint8_t[outLen];

	int32_t res = UTF8Toisolat1(outBuf, &outLen, (uint8_t*)inText, &inLen);
	if(res < 0)
	{
		delete[] outBuf;
		return false;
	}

	outBuf[outLen] = '\0';
	outText = (char*)outBuf;

	delete[] outBuf;
	return true;
}

bool latin1ToUtf8(char* inText, std::string& outText)
{
	outText = "";
	if(!inText)
		return false;

	int32_t inLen = strlen(inText);
	if(!inLen)
		return false;

	int32_t outLen = inLen << 1;
	uint8_t* outBuf = new uint8_t[outLen];

	int32_t res = isolat1ToUTF8(outBuf, &outLen, (uint8_t*)inText, &inLen);
	if(res < 0)
	{
		delete[] outBuf;
		return false;
	}

	outBuf[outLen] = '\0';
	outText = (char*)outBuf;

	delete[] outBuf;
	return true;
}

StringVec explodeString(const std::string& string, const std::string& separator, bool trim/* = true*/, uint16_t limit/* = 0*/)
{
	StringVec returnVector;
	size_t start = 0, end = 0;

	uint16_t i = 1;
	while((end = string.find(separator, start)) != std::string::npos)
	{
		std::string t = string.substr(start, end - start);
		if(trim)
			trimString(t);

		returnVector.push_back(t);
		start = end + separator.size();

		++i;
		if(limit > 0 && i > limit)
			break;
	}

	returnVector.push_back(string.substr(start));
	return returnVector;
}

IntegerVec vectorAtoi(StringVec stringVector)
{
	IntegerVec returnVector;
	for(StringVec::iterator it = stringVector.begin(); it != stringVector.end(); ++it)
	{
		int32_t number = atoi((*it).c_str());
		if(number || (*it) == "0")
			returnVector.push_back(number);
	}

	return returnVector;
}

bool hasBitSet(uint32_t flag, uint32_t flags)
{
	return ((flags & flag) == flag);
}

#if !defined(_MSC_VER) || _MSC_VER < 1800
double round(double v)
{
	if(v >= 0.0)
		return std::floor(v + 0.5);
	else
		return std::ceil(v - 0.5);
}
#endif

uint32_t rand24b()
{
	return ((rand() << 12) ^ rand()) & 0xFFFFFF;
}

float box_muller(float m, float s)
{
	// normal random variate generator
	// mean m, standard deviation s
	float x1, x2, w, y1;
	static float y2;

	static bool useLast = false;
	if(useLast) // use value from previous call
	{
		y1 = y2;
		useLast = false;
		return (m + y1 * s);
	}

	do
	{
		double r1 = (((float)(rand()) / RAND_MAX));
		double r2 = (((float)(rand()) / RAND_MAX));

		x1 = 2.0 * r1 - 1.0;
		x2 = 2.0 * r2 - 1.0;
		w = x1 * x1 + x2 * x2;
	}
	while(w >= 1.0);
	w = sqrt((-2.0 * log(w)) / w);

	y1 = x1 * w;
	y2 = x2 * w;

	useLast = true;
	return (m + y1 * s);
}

std::mt19937& getRandomGenerator()
{
	static std::random_device rd;
	static std::mt19937 generator(rd());
	return generator;
}

int32_t normal_random(int32_t minNumber, int32_t maxNumber)
{
	static std::normal_distribution<float> normalRand(0.45f, 0.22f);

	if (minNumber == maxNumber) {
		return minNumber;
	}
	else if (minNumber > maxNumber) {
		std::swap(minNumber, maxNumber);
	}

	int32_t increment;
	const int32_t diff = maxNumber - minNumber;
	const float v = normalRand(getRandomGenerator());
	if (v < 0.0) {
		increment = diff / 2;
	}
	else if (v > 1.0) {
		increment = (diff + 1) / 2;
	}
	else {
		increment = round(v * diff);
	}
	return minNumber + increment;
}

int32_t uniform_random(int32_t minNumber, int32_t maxNumber)
{
	static std::uniform_int_distribution<int32_t> uniformRand;
	if (minNumber == maxNumber) {
		return minNumber;
	}
	else if (minNumber > maxNumber) {
		std::swap(minNumber, maxNumber);
	}
	return uniformRand(getRandomGenerator(), std::uniform_int_distribution<int32_t>::param_type(minNumber, maxNumber));
}

int32_t random_range(int32_t lowestNumber, int32_t highestNumber, DistributionType_t type /*= DISTRO_UNIFORM*/)
{
	switch(type)
	{
		case DISTRO_UNIFORM:
			return uniform_random(lowestNumber, highestNumber);
		case DISTRO_NORMAL:
			return normal_random(lowestNumber, highestNumber);
		default:
			break;
	}

	const float randMax = 16777216;
	return (lowestNumber + int32_t(float(highestNumber - lowestNumber) * float(1.f - sqrt((1.f * rand24b()) / randMax))));
}

char upchar(char character)
{
	if((character >= 97 && character <= 122) || (character <= -1 && character >= -32))
		character -= 32;

	return character;
}

bool isNumber(char character)
{
	return (character >= 48 && character <= 57);
}

bool isLowercaseLetter(char character)
{
	return (character >= 97 && character <= 122);
}

bool isUppercaseLetter(char character)
{
	return (character >= 65 && character <= 90);
}

bool isPasswordCharacter(char character)
{
	return ((character >= 33 && character <= 47) || (character >= 58 && character <= 64) || (character >= 91 && character <= 96) || (character >= 123 && character <= 126));
}

bool isValidAccountName(std::string text)
{
	toLowerCaseString(text);

	uint32_t textLength = text.length();
	for(uint32_t size = 0; size < textLength; size++)
	{
		if(!isLowercaseLetter(text[size]) && !isNumber(text[size]))
			return false;
	}

	return true;
}

bool isValidPassword(std::string text)
{
	toLowerCaseString(text);

	uint32_t textLength = text.length();
	for(uint32_t size = 0; size < textLength; size++)
	{
		if(!isLowercaseLetter(text[size]) && !isNumber(text[size]) && !isPasswordCharacter(text[size]))
			return false;
	}

	return true;
}

bool isValidName(std::string text, bool forceUppercaseOnFirstLetter/* = true*/)
{
	uint32_t textLength = text.length(), lenBeforeSpace = 1, lenBeforeQuote = 1, lenBeforeDash = 1, repeatedCharacter = 0;
	char lastChar = 32;
	if(forceUppercaseOnFirstLetter)
	{
		if(!isUppercaseLetter(text[0]))
			return false;
	}
	else if(!isLowercaseLetter(text[0]) && !isUppercaseLetter(text[0]))
		return false;

	for(uint32_t size = 1; size < textLength; size++)
	{
		if(text[size] != 32)
		{
			lenBeforeSpace++;
			if(text[size] != 39)
				lenBeforeQuote++;
			else
			{
				if(lenBeforeQuote <= 1 || size == textLength - 1 || text[size + 1] == 32)
					return false;

				lenBeforeQuote = 0;
			}

			if(text[size] != 45)
				lenBeforeDash++;
			else
			{
				if(lenBeforeDash <= 1 || size == textLength - 1 || text[size + 1] == 32)
					return false;

				lenBeforeDash = 0;
			}

			if(text[size] == lastChar)
			{
				repeatedCharacter++;
				if(repeatedCharacter > 2)
					return false;
			}
			else
				repeatedCharacter = 0;

			lastChar = text[size];
		}
		else
		{
			if(lenBeforeSpace <= 1 || size == textLength - 1 || text[size + 1] == 32)
				return false;

			lenBeforeSpace = lenBeforeQuote = lenBeforeDash = 0;
		}

		if(!(isLowercaseLetter(text[size]) || text[size] == 32 || text[size] == 39 || text[size] == 45
			|| (isUppercaseLetter(text[size]) && text[size - 1] == 32)))
			return false;
	}

	return true;
}

bool isNumbers(std::string text)
{
	uint32_t textLength = text.length();
	for(uint32_t size = 0; size < textLength; size++)
	{
		if(!isNumber(text[size]))
			return false;
	}

	return true;
}

bool checkText(std::string text, std::string str)
{
	trimString(text);
	return asLowerCaseString(text) == str;
}

std::string generateRecoveryKey(int32_t fieldCount, int32_t fieldLenght, bool mixCase/* = false*/)
{
	std::ostringstream key;
	int32_t i = 0, j = 0, lastNumber = 99, number = 0;

	char character = 0, lastCharacter = 0;
	bool madeNumber = false, madeCharacter = false;
	do
	{
		do
		{
			madeNumber = madeCharacter = false;
			if((mixCase && !random_range(0, 2)) || (!mixCase && !random_range(0, 1)))
			{
				number = random_range(2, 9);
				if(number != lastNumber)
				{
					key << number;
					lastNumber = number;
					madeNumber = true;
				}
			}
			else
			{
				if(mixCase && !random_range(0, 1))
					character = (char)random_range(97, 122);
				else
					character = (char)random_range(65, 90);

				if(character != lastCharacter)
				{
					key << character;
					lastCharacter = character;
					madeCharacter = true;
				}
			}
		}
		while((!madeCharacter && !madeNumber) ? true : (++j && j < fieldLenght));
		lastCharacter = character = number = j = 0;

		lastNumber = 99;
		if(i < fieldCount - 1)
			key << "-";
	}
	while(++i && i < fieldCount);
	return key.str();
}

std::string trimString(std::string& str)
{
	str.erase(str.find_last_not_of(" ") + 1);
	return str.erase(0, str.find_first_not_of(" "));
}

std::string convertIPToString(uint32_t ip)
{
	char buffer[17];

	int res = sprintf(buffer, "%u.%u.%u.%u", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24));
	if (res < 0) {
		return {};
	}

	return buffer;
}

std::string parseParams(tokenizer::iterator &it, tokenizer::iterator end)
{
	if(it == end)
		return "";

	std::string tmp = (*it);
	++it;
	if(tmp[0] == '"')
	{
		tmp.erase(0, 1);
		while(it != end && tmp[tmp.length() - 1] != '"')
		{
			tmp += " " + (*it);
			++it;
		}

		if(tmp.length() > 0 && tmp[tmp.length() - 1] == '"')
			tmp.erase(tmp.length() - 1);
	}

	return tmp;
}

std::string formatDate(time_t _time/* = 0*/)
{
	if(!_time)
		_time = time(NULL);

	const tm* tms = localtime(&_time);
	std::ostringstream s;
	if(tms)
		s << tms->tm_mday << "/" << (tms->tm_mon + 1) << "/" << (tms->tm_year + 1900) << " " << tms->tm_hour << ":" << tms->tm_min << ":" << tms->tm_sec;
	else
		s << "UNIX Time: " << (int32_t)_time;

	return s.str();
}

std::string formatDateEx(time_t _time/* = 0*/, std::string format/* = "%d %b %Y, %H:%M:%S"*/)
{
	if(!_time)
		_time = time(NULL);

	const tm* tms = localtime(&_time);
	char buffer[100];
	if(tms)
		strftime(buffer, 25, format.c_str(), tms);
	else
		sprintf(buffer, "UNIX Time: %d", (int32_t)_time);

	return buffer;
}

std::string formatTime(time_t _time/* = 0*/, bool ms/* = false*/)
{
	if(!_time)
		_time = time(NULL);
	else if(ms)
		ms = false;

	const tm* tms = localtime(&_time);
	std::ostringstream s;
	if(tms)
	{
		s << tms->tm_hour << ":" << tms->tm_min << ":";
		if(tms->tm_sec < 10)
			s << "0";

		s << tms->tm_sec;
		if(ms)
		{
			timeb t;
			ftime(&t);

			s << "."; // make it format zzz
			if(t.millitm < 10)
				s << "0";

			if(t.millitm < 100)
				s << "0";

			s << t.millitm;
		}
	}
	else
		s << "UNIX Time: " << (int32_t)_time;

	return s.str();
}

std::string convertIPAddress(uint32_t ip)
{
	char buffer[17];
	sprintf(buffer, "%d.%d.%d.%d", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24));
	return buffer;
}

Skulls_t getSkulls(std::string strValue)
{
	std::string tmpStrValue = asLowerCaseString(strValue);
	if(tmpStrValue == "red" || tmpStrValue == "4")
		return SKULL_RED;

	if(tmpStrValue == "white" || tmpStrValue == "3")
		return SKULL_WHITE;

	if(tmpStrValue == "green" || tmpStrValue == "2")
		return SKULL_GREEN;

	if(tmpStrValue == "yellow" || tmpStrValue == "1")
		return SKULL_YELLOW;

	return SKULL_NONE;
}

PartyShields_t getShields(std::string strValue)
{
	std::string tmpStrValue = asLowerCaseString(strValue);
	if(tmpStrValue == "yellow" || tmpStrValue == "4")
		return SHIELD_YELLOW;

	if(tmpStrValue == "blue" || tmpStrValue == "3")
		return SHIELD_BLUE;

	if(tmpStrValue == "whiteyellow" || tmpStrValue == "2")
		return SHIELD_WHITEYELLOW;

	if(tmpStrValue == "whiteblue" || tmpStrValue == "1")
		return SHIELD_WHITEBLUE;

	return SHIELD_NONE;
}

Direction getDirection(std::string string)
{
	if(string == "north" || string == "n" || string == "0")
		return NORTH;

	if(string == "east" || string == "e" || string == "1")
		return EAST;

	if(string == "south" || string == "s" || string == "2")
		return SOUTH;

	if(string == "west" || string == "w" || string == "3")
		return WEST;

	if(string == "southwest" || string == "south west" || string == "south-west" || string == "sw" || string == "4")
		return SOUTHWEST;

	if(string == "southeast" || string == "south east" || string == "south-east" || string == "se" || string == "5")
		return SOUTHEAST;

	if(string == "northwest" || string == "north west" || string == "north-west" || string == "nw" || string == "6")
		return NORTHWEST;

	if(string == "northeast" || string == "north east" || string == "north-east" || string == "ne" || string == "7")
		return NORTHEAST;

	return SOUTH;
}

Direction getDirectionTo(Position pos1, Position pos2, bool extended/* = true*/)
{
	Direction direction = NORTH;
	if(pos1.x > pos2.x)
	{
		direction = WEST;
		if(extended)
		{
			if(pos1.y > pos2.y)
				direction = NORTHWEST;
			else if(pos1.y < pos2.y)
				direction = SOUTHWEST;
		}
	}
	else if(pos1.x < pos2.x)
	{
		direction = EAST;
		if(extended)
		{
			if(pos1.y > pos2.y)
				direction = NORTHEAST;
			else if(pos1.y < pos2.y)
				direction = SOUTHEAST;
		}
	}
	else
	{
		if(pos1.y > pos2.y)
			direction = NORTH;
		else if(pos1.y < pos2.y)
			direction = SOUTH;
	}

	return direction;
}

Direction getReverseDirection(Direction dir)
{
	switch(dir)
	{
		case NORTH:
			return SOUTH;
		case SOUTH:
			return NORTH;
		case WEST:
			return EAST;
		case EAST:
			return WEST;
		case SOUTHWEST:
			return NORTHEAST;
		case NORTHWEST:
			return SOUTHEAST;
		case NORTHEAST:
			return SOUTHWEST;
		case SOUTHEAST:
			return NORTHWEST;
		default:
			break;
	}

	return SOUTH;
}

Position getNextPosition(Direction direction, Position pos)
{
	switch(direction)
	{
		case NORTH:
			pos.y--;
			break;
		case SOUTH:
			pos.y++;
			break;
		case WEST:
			pos.x--;
			break;
		case EAST:
			pos.x++;
			break;
		case SOUTHWEST:
			pos.x--;
			pos.y++;
			break;
		case NORTHWEST:
			pos.x--;
			pos.y--;
			break;
		case SOUTHEAST:
			pos.x++;
			pos.y++;
			break;
		case NORTHEAST:
			pos.x++;
			pos.y--;
			break;
		default:
			break;
	}

	return pos;
}

struct AmmoTypeNames
{
	const char* name;
	Ammo_t ammoType;
};

struct MagicEffectNames
{
	const char* name;
	MagicEffect_t magicEffect;
};

struct ShootTypeNames
{
	const char* name;
	ShootEffect_t shootType;
};

struct CombatTypeNames
{
	const char* name;
	CombatType_t combatType;
};

struct AmmoActionNames
{
	const char* name;
	AmmoAction_t ammoAction;
};

struct FluidTypeNames
{
	const char* name;
	FluidTypes_t fluidType;
};

struct SkillIdNames
{
	const char* name;
	skills_t skillId;
};

struct WeaponTypeNames
{
	const char* name;
	WeaponType_t weaponType;
};

MagicEffectNames magicEffectNames[] =
{
	{"redspark", MAGIC_EFFECT_DRAW_BLOOD},
	{"bluebubble", MAGIC_EFFECT_LOSE_ENERGY},
	{"poff", MAGIC_EFFECT_POFF},
	{"yellowspark", MAGIC_EFFECT_BLOCKHIT},
	{"explosionarea", MAGIC_EFFECT_EXPLOSION_AREA},
	{"explosion", MAGIC_EFFECT_EXPLOSION_DAMAGE},
	{"firearea", MAGIC_EFFECT_FIRE_AREA},
	{"yellowbubble", MAGIC_EFFECT_YELLOW_RINGS},
	{"greenbubble", MAGIC_EFFECT_POISON_RINGS},
	{"blackspark", MAGIC_EFFECT_HIT_AREA},
	{"teleport", MAGIC_EFFECT_TELEPORT},
	{"energy", MAGIC_EFFECT_ENERGY_DAMAGE},
	{"blueshimmer", MAGIC_EFFECT_WRAPS_BLUE},
	{"redshimmer", MAGIC_EFFECT_WRAPS_RED},
	{"greenshimmer", MAGIC_EFFECT_WRAPS_GREEN},
	{"fire", MAGIC_EFFECT_HITBY_FIRE},
	{"greenspark", MAGIC_EFFECT_POISON},
	{"mortarea", MAGIC_EFFECT_MORT_AREA},
	{"greennote", MAGIC_EFFECT_SOUND_GREEN},
	{"rednote", MAGIC_EFFECT_SOUND_RED},
	{"poison", MAGIC_EFFECT_POISON_AREA},
	{"yellownote", MAGIC_EFFECT_SOUND_YELLOW},
	{"purplenote", MAGIC_EFFECT_SOUND_PURPLE},
	{"bluenote", MAGIC_EFFECT_SOUND_BLUE},
	{"whitenote", MAGIC_EFFECT_SOUND_WHITE}
};

ShootTypeNames shootTypeNames[] =
{
	{"spear", SHOOT_EFFECT_SPEAR},
	{"bolt", SHOOT_EFFECT_BOLT},
	{"arrow", SHOOT_EFFECT_ARROW},
	{"fire", SHOOT_EFFECT_FIRE},
	{"energy", SHOOT_EFFECT_ENERGY},
	{"poisonarrow", SHOOT_EFFECT_POISONARROW},
	{"burstarrow", SHOOT_EFFECT_BURSTARROW},
	{"throwingstar", SHOOT_EFFECT_THROWINGSTAR},
	{"throwingknife", SHOOT_EFFECT_THROWINGKNIFE},
	{"smallstone", SHOOT_EFFECT_SMALLSTONE},
	{"death", SHOOT_EFFECT_DEATH},
	{"largerock", SHOOT_EFFECT_LARGEROCK},
	{"snowball", SHOOT_EFFECT_SNOWBALL},
	{"powerbolt", SHOOT_EFFECT_POWERBOLT},
	{"poison", SHOOT_EFFECT_POISONFIELD}
};

CombatTypeNames combatTypeNames[] =
{
	{"physical", COMBAT_PHYSICALDAMAGE},
	{"energy", COMBAT_ENERGYDAMAGE},
	{"earth", COMBAT_EARTHDAMAGE},
	{"fire", COMBAT_FIREDAMAGE},
	{"undefined", COMBAT_UNDEFINEDDAMAGE},
	{"lifedrain", COMBAT_LIFEDRAIN},
	{"life drain", COMBAT_LIFEDRAIN},
	{"manadrain", COMBAT_MANADRAIN},
	{"mana drain", COMBAT_MANADRAIN},
	{"healing", COMBAT_HEALING},
	{"drown", COMBAT_DROWNDAMAGE},
	{"ice", COMBAT_ICEDAMAGE},
	{"holy", COMBAT_HOLYDAMAGE},
	{"death", COMBAT_DEATHDAMAGE}
};

AmmoTypeNames ammoTypeNames[] =
{
	{"spear", AMMO_SPEAR},
	{"arrow", AMMO_ARROW},
	{"poisonarrow", AMMO_ARROW},
	{"burstarrow", AMMO_ARROW},
	{"bolt", AMMO_BOLT},
	{"powerbolt", AMMO_BOLT},
	{"smallstone", AMMO_STONE},
	{"largerock", AMMO_STONE},
	{"throwingstar", AMMO_THROWINGSTAR},
	{"throwingknife", AMMO_THROWINGKNIFE},
	{"snowball", AMMO_SNOWBALL}
};

AmmoActionNames ammoActionNames[] =
{
	{"move", AMMOACTION_MOVE},
	{"moveback", AMMOACTION_MOVEBACK},
	{"move back", AMMOACTION_MOVEBACK},
	{"removecharge", AMMOACTION_REMOVECHARGE},
	{"remove charge", AMMOACTION_REMOVECHARGE},
	{"removecount", AMMOACTION_REMOVECOUNT},
	{"remove count", AMMOACTION_REMOVECOUNT}
};

FluidTypeNames fluidTypeNames[] =
{
	{"none", FLUID_NONE},
	{"water", FLUID_WATER},
	{"blood", FLUID_BLOOD},
	{"beer", FLUID_BEER},
	{"slime", FLUID_SLIME},
	{"lemonade", FLUID_LEMONADE},
	{"milk", FLUID_MILK},
	{"mana", FLUID_MANA},
	{"life", FLUID_LIFE},
	{"oil", FLUID_OIL},
	{"urine", FLUID_URINE},
	{"coconutmilk", FLUID_COCONUTMILK},
	{"coconut milk", FLUID_COCONUTMILK},
	{"wine", FLUID_WINE},
	{"mud", FLUID_MUD},
	{"fruitjuice", FLUID_FRUITJUICE},
	{"fruit juice", FLUID_FRUITJUICE},
	{"lava", FLUID_LAVA},
	{"rum", FLUID_RUM},
	{"swamp", FLUID_SWAMP},
	{"tea", FLUID_TEA},
	{"mead", FLUID_MEAD}
};

SkillIdNames skillIdNames[] =
{
	{"fist", SKILL_FIST},
	{"club", SKILL_CLUB},
	{"sword", SKILL_SWORD},
	{"axe", SKILL_AXE},
	{"distance", SKILL_DIST},
	{"dist", SKILL_DIST},
	{"shielding", SKILL_SHIELD},
	{"shield", SKILL_SHIELD},
	{"fishing", SKILL_FISH},
	{"fish", SKILL_FISH},
	{"level", SKILL__LEVEL},
	{"magiclevel", SKILL__MAGLEVEL},
	{"magic level", SKILL__MAGLEVEL},
	{"experience", SKILL__EXPERIENCE}
};

WeaponTypeNames weaponTypeNames[] = {
	{"", WEAPON_NONE},
	{"sword", WEAPON_SWORD},
	{"club", WEAPON_CLUB},
	{"axe", WEAPON_AXE},
	{"distance", WEAPON_DIST},
	{"shield", WEAPON_SHIELD},
	{"fist", WEAPON_FIST},
	{"wand", WEAPON_WAND},
	{"ammunition", WEAPON_AMMO}
};

MagicEffect_t getMagicEffect(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(magicEffectNames) / sizeof(MagicEffectNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, magicEffectNames[i].name))
			return magicEffectNames[i].magicEffect;
	}

	return MAGIC_EFFECT_UNKNOWN;
}

ShootEffect_t getShootType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(shootTypeNames) / sizeof(ShootTypeNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, shootTypeNames[i].name))
			return shootTypeNames[i].shootType;
	}

	return SHOOT_EFFECT_UNKNOWN;
}

CombatType_t getCombatType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(combatTypeNames) / sizeof(CombatTypeNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, combatTypeNames[i].name))
			return combatTypeNames[i].combatType;
	}

	return COMBAT_NONE;
}

Ammo_t getAmmoType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(ammoTypeNames) / sizeof(AmmoTypeNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, ammoTypeNames[i].name))
			return ammoTypeNames[i].ammoType;
	}

	return AMMO_NONE;
}

AmmoAction_t getAmmoAction(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(ammoActionNames) / sizeof(AmmoActionNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, ammoActionNames[i].name))
			return ammoActionNames[i].ammoAction;
	}

	return AMMOACTION_NONE;
}

FluidTypes_t getFluidType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(fluidTypeNames) / sizeof(FluidTypeNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, fluidTypeNames[i].name))
			return fluidTypeNames[i].fluidType;
	}

	return FLUID_NONE;
}

skills_t getSkillId(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(skillIdNames) / sizeof(SkillIdNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, skillIdNames[i].name))
			return skillIdNames[i].skillId;
	}

	return SKILL_FIST;
}

WeaponType_t getWeaponType(const std::string& strValue)
{
	for(uint32_t i = 0; i < sizeof(weaponTypeNames) / sizeof(WeaponTypeNames); ++i)
	{
		if(boost::algorithm::iequals(strValue, weaponTypeNames[i].name))
			return weaponTypeNames[i].weaponType;
	}

	return WEAPON_NONE;
}

void getCombatDetails(CombatType_t combatType, MagicEffect_t& magicEffect, Color_t& textColor)
{
	switch(combatType)
	{
		case COMBAT_PHYSICALDAMAGE:
		{
			textColor = COLOR_RED;
			magicEffect = MAGIC_EFFECT_DRAW_BLOOD;
			break;
		}

		case COMBAT_ENERGYDAMAGE:
		{
			textColor = COLOR_LIGHTBLUE;
			magicEffect = MAGIC_EFFECT_ENERGY_DAMAGE;
			break;
		}

		case COMBAT_EARTHDAMAGE:
		{
			textColor = COLOR_LIGHTGREEN;
			magicEffect = MAGIC_EFFECT_POISON_RINGS;
			break;
		}

		case COMBAT_DROWNDAMAGE:
		{
			textColor = COLOR_LIGHTBLUE;
			magicEffect = MAGIC_EFFECT_LOSE_ENERGY;
			break;
		}

		case COMBAT_FIREDAMAGE:
		{
			textColor = COLOR_ORANGE;
			magicEffect = MAGIC_EFFECT_HITBY_FIRE;
			break;
		}

		case COMBAT_DEATHDAMAGE:
		{
			textColor = COLOR_DARKRED;
			magicEffect = MAGIC_EFFECT_DRAW_BLOOD;
			break;
		}

		case COMBAT_LIFEDRAIN:
		{
			textColor = COLOR_RED;
			magicEffect = MAGIC_EFFECT_WRAPS_RED;
			break;
		}

		case COMBAT_MANADRAIN:
		{
			textColor = COLOR_BLUE;
			magicEffect = MAGIC_EFFECT_LOSE_ENERGY;
		}

		default:
			break;
	}
}

std::string getCombatName(CombatType_t combatType)
{
	switch(combatType)
	{
		case COMBAT_PHYSICALDAMAGE:
			return "physical";
		case COMBAT_ENERGYDAMAGE:
			return "energy";
		case COMBAT_EARTHDAMAGE:
			return "earth";
		case COMBAT_FIREDAMAGE:
			return "fire";
		case COMBAT_UNDEFINEDDAMAGE:
			return "undefined";
		case COMBAT_LIFEDRAIN:
			return "life drain";
		case COMBAT_MANADRAIN:
			return "mana drain";
		case COMBAT_HEALING:
			return "healing";
		case COMBAT_DROWNDAMAGE:
			return "drown";
		case COMBAT_ICEDAMAGE:
			return "ice";
		case COMBAT_HOLYDAMAGE:
			return "holy";
		case COMBAT_DEATHDAMAGE:
			return "death";
		default:
			break;
	}

	return "unknown";
}

std::string getSkillName(uint16_t skillId, bool suffix/* = true*/)
{
	switch(skillId)
	{
		case SKILL_FIST:
		{
			std::string tmp = "fist";
			if(suffix)
				tmp += " fighting";

			return tmp;
		}
		case SKILL_CLUB:
		{
			std::string tmp = "club";
			if(suffix)
				tmp += " fighting";

			return tmp;
		}
		case SKILL_SWORD:
		{
			std::string tmp = "sword";
			if(suffix)
				tmp += " fighting";

			return tmp;
		}
		case SKILL_AXE:
		{
			std::string tmp = "axe";
			if(suffix)
				tmp += " fighting";

			return tmp;
		}
		case SKILL_DIST:
		{
			std::string tmp = "distance";
			if(suffix)
				tmp += " fighting";

			return tmp;
		}
		case SKILL_SHIELD:
			return "shielding";
		case SKILL_FISH:
			return "fishing";
		case SKILL__MAGLEVEL:
			return "magic level";
		case SKILL__LEVEL:
			return "level";
		default:
			break;
	}

	return "unknown";
}

std::string getReason(int32_t reasonId)
{
	switch(reasonId)
	{
		case 0:
			return "Offensive Name";
		case 1:
			return "Invalid Name Format";
		case 2:
			return "Unsuitable Name";
		case 3:
			return "Name Inciting Rule Violation";
		case 4:
			return "Offensive Statement";
		case 5:
			return "Spamming";
		case 6:
			return "Illegal Advertising";
		case 7:
			return "Off-Topic Public Statement";
		case 8:
			return "Non-English Public Statement";
		case 9:
			return "Inciting Rule Violation";
		case 10:
			return "Bug Abuse";
		case 11:
			return "Game Weakness Abuse";
		case 12:
			return "Using Unofficial Software to Play";
		case 13:
			return "Hacking";
		case 14:
			return "Multi-Clienting";
		case 15:
			return "Account Trading or Sharing";
		case 16:
			return "Threatening Gamemaster";
		case 17:
			return "Pretending to Have Influence on Rule Enforcement";
		case 18:
			return "False Report to Gamemaster";
		case 19:
			return "Destructive Behaviour";
		case 20:
			return "Excessive Unjustified Player Killing";
		default:
			break;
	}

	return "Unknown Reason";
}

std::string getAction(ViolationAction_t actionId, bool ipBanishment)
{
	std::string action = "Unknown";
	switch(actionId)
	{
		case ACTION_NOTATION:
			action = "Notation";
			break;
		case ACTION_NAMEREPORT:
			action = "Name Report";
			break;
		case ACTION_BANISHMENT:
			action = "Banishment";
			break;
		case ACTION_BANREPORT:
			action = "Name Report + Banishment";
			break;
		case ACTION_BANFINAL:
			action = "Banishment + Final Warning";
			break;
		case ACTION_BANREPORTFINAL:
			action = "Name Report + Banishment + Final Warning";
			break;
		case ACTION_STATEMENT:
			action = "Statement Report";
			break;
		//internal use
		case ACTION_DELETION:
			action = "Deletion";
			break;
		case ACTION_NAMELOCK:
			action = "Name Lock";
			break;
		case ACTION_BANLOCK:
			action = "Name Lock + Banishment";
			break;
		case ACTION_BANLOCKFINAL:
			action = "Name Lock + Banishment + Final Warning";
			break;
		default:
			break;
	}

	if(ipBanishment)
		action += " + IP Banishment";

	return action;
}

std::string getWeaponName(WeaponType_t weaponType)
{
	switch(weaponType)
	{
		case WEAPON_SWORD:
			return "sword";
		case WEAPON_CLUB:
			return "club";
		case WEAPON_AXE:
			return "axe";
		case WEAPON_DIST:
			return "distance";
		case WEAPON_SHIELD:
			return "shield";
		case WEAPON_WAND:
			return "wand";
		case WEAPON_FIST:
			return "fist";
		case WEAPON_AMMO:
			return "ammunition";
		default:
			break;
	}

	return "";
}

std::string parseVocationString(StringVec vocStringVec)
{
	std::string str = "";
	if(!vocStringVec.empty())
	{
		for(StringVec::iterator it = vocStringVec.begin(); it != vocStringVec.end(); ++it)
		{
			if((*it) != vocStringVec.front())
			{
				if((*it) != vocStringVec.back())
					str += ", ";
				else
					str += " and ";
			}

			str += (*it);
			str += "s";
		}
	}

	return str;
}

bool parseVocationNode(xmlNodePtr vocationNode, VocationMap& vocationMap, StringVec& vocStringVec, std::string& errorStr)
{
	if(xmlStrcmp(vocationNode->name,(const xmlChar*)"vocation"))
		return true;

	int32_t vocationId = -1;
	std::string strValue, tmpStrValue;
	if(readXMLString(vocationNode, "name", strValue))
	{
		vocationId = Vocations::getInstance()->getVocationId(strValue);
		if(vocationId != -1)
		{
			vocationMap[vocationId] = true;
			int32_t promotedVocation = Vocations::getInstance()->getPromotedVocation(vocationId);
			if(promotedVocation != -1)
				vocationMap[promotedVocation] = true;
		}
		else
		{
			errorStr = "Wrong vocation name: " + strValue;
			return false;
		}
	}
	else if(readXMLString(vocationNode, "id", strValue))
	{
		IntegerVec intVector;
		if(!parseIntegerVec(strValue, intVector))
		{
			errorStr = "Invalid vocation id - '" + strValue + "'";
			return false;
		}

		size_t size = intVector.size();
		for(size_t i = 0; i < size; ++i)
		{
			Vocation* vocation = Vocations::getInstance()->getVocation(intVector[i]);
			if(vocation && vocation->getName() != "")
			{
				vocationId = vocation->getId();
				strValue = vocation->getName();

				vocationMap[vocationId] = true;
				int32_t promotedVocation = Vocations::getInstance()->getPromotedVocation(vocationId);
				if(promotedVocation != -1)
					vocationMap[promotedVocation] = true;
			}
			else
			{
				std::ostringstream ss;
				ss << "Wrong vocation id: " << intVector[i];

				errorStr = ss.str();
				return false;
			}
		}
	}

	if(vocationId != -1 && (!readXMLString(vocationNode, "showInDescription", tmpStrValue) || booleanString(tmpStrValue)))
		vocStringVec.push_back(asLowerCaseString(strValue));

	return true;
}

bool parseIntegerVec(std::string str, IntegerVec& intVector)
{
	StringVec strVector = explodeString(str, ";");
	IntegerVec tmpIntVector;
	for(StringVec::iterator it = strVector.begin(); it != strVector.end(); ++it)
	{
		tmpIntVector = vectorAtoi(explodeString((*it), "-"));
		
		if (tmpIntVector.size() == 0)
			return false;
		
		if(!tmpIntVector[0] && it->substr(0, 1) != "0")
			continue;

		intVector.push_back(tmpIntVector[0]);
		if(tmpIntVector.size() > 1)
		{
			while(tmpIntVector[0] < tmpIntVector[1])
				intVector.push_back(++tmpIntVector[0]);
		}
	}

	return true;
}

bool fileExists(const std::string& filename)
{
	return boost::filesystem::exists(filename);
}

std::string getFilePath(FileType_t type, std::string name/* = ""*/)
{
	#ifdef __FILESYSTEM_HIERARCHY_STANDARD__
	std::string path = "/var/lib/tfs/";
	#else
	std::string path = g_config.getString(ConfigManager::DATA_DIRECTORY);
	#endif
	switch(type)
	{
		case FILE_TYPE_OTHER:
			path += name;
			break;
		case FILE_TYPE_XML:
			path += "XML/" + name;
			break;
		case FILE_TYPE_LOG:
			#ifndef __FILESYSTEM_HIERARCHY_STANDARD__
			path = g_config.getString(ConfigManager::LOGS_DIRECTORY) + name;
			#else
			path = "/var/log/tfs/" + name;
			#endif
			break;
		case FILE_TYPE_MOD:
		{
			#ifndef __FILESYSTEM_HIERARCHY_STANDARD__
			path = "mods/" + name;
			#else
			path = "/usr/share/tfs/" + name;
			#endif
			break;
		}
		case FILE_TYPE_CONFIG:
		{
			#if defined(__HOMEDIR_CONF__)
			if(fileExists("~/.tfs/" + name))
				path = "~/.tfs/" + name;
			else
			#endif
			#if defined(__FILESYSTEM_HIERARCHY_STANDARD__)
				path = "/etc/tfs/" + name;
			#else
				path = name;
			#endif
			break;
		}
		default:
			std::clog << "> ERROR: Wrong file type!" << std::endl;
			break;
	}
	return path;
}

uint8_t serverFluidToClient(uint8_t serverFluid)
{
	uint8_t size = sizeof(clientToServerFluidMap) / sizeof(int8_t);
	for(uint8_t i = 0; i < size; ++i)
	{
		if(clientToServerFluidMap[i] == serverFluid)
			return i;
	}
	return 0;
}

uint8_t clientFluidToServer(uint8_t clientFluid)
{
	uint8_t size = sizeof(clientToServerFluidMap) / sizeof(int8_t);
	if(clientFluid >= size)
		return 0;

	return clientToServerFluidMap[clientFluid];
}