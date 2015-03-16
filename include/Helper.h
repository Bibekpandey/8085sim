#pragma once
#include <vector>
#include <cmath>
#include <sstream>

namespace Helper
{
	inline bool IsDigit(char c)
	{
		return (c>='0' and c<='9');
	}

	inline bool IsHex(char c)
	{
		return (IsDigit(c) or (c>='A' and c<='F'));
	}

	inline void LTrim(std::string& s, char delim=' ')
	{
		if(s.length()==0)
		{
			s = "";
			return;
		}
		unsigned begin=0;
		while(s[begin++]==delim);
		s = s.substr(begin-1, s.length());
	}
	
	inline void RTrim(std::string& s, char delim=' ')
	{
		if(s.length()==0)
		{
			s="";
			return;
		}
		unsigned end=s.length()-1;
		while(s[end--]==delim);
		s = s.substr(0, end+2);
	}

	inline std::vector<std::string> SplitIntoTwo(std::string s, char delim=' ')
	{
		// splits into two at the first occurence of the delimiter
		std::vector<std::string> splitted;
		unsigned occurence = 0;
		while(s[occurence++] != delim)
		{
			if(occurence >= s.length())
			{
				splitted.push_back(s);
				return splitted;
			}
		}
		// now occurence has the first occurence index of delim
		splitted.push_back(s.substr(0,occurence-1));// -1 because we dont need the delim in the string
		s = s.substr(occurence, s.length());
		LTrim(s);
		if(s.length()==0)
			return splitted;

		splitted.push_back(s);
		return splitted;
	}

	inline std::vector<std::string> Split(std::string s, char delim= ' ')
	{
		LTrim(s);
		RTrim(s);
		// splits into multiple strings
		std::vector<std::string> splitted;
		unsigned occurence = 0;
		unsigned length = s.length();
		while(occurence <= length)
		{
			if (s[occurence] == delim)
			{
				if(occurence>0)
					splitted.push_back(s.substr(0,occurence));
				else
					splitted.push_back(std::string(""));

				s = s.substr(++occurence,length);
				if(s.length() == 0)
				{
					s="";
					splitted.push_back(s);
					return splitted;
				}
				occurence = 0;
				length = s.length();
			}
			occurence +=1;
		}
		if (s.length() >0)
				splitted.push_back(s);
		return splitted;
	}

	inline std::string ToUpper(std::string s)
	{
		for(unsigned i=0;i<s.length();i++)
			s[i] = (s[i] >= 'a' and s[i] <= 'z') ? 'A' + s[i] - 'a' : s[i];
		return s;
	}

	inline int ToDec(std::string hex)
	{
		hex = ToUpper(hex);
		// trim zeros of left
		LTrim(hex, '0');

		int sum = 0, power=0;
		for(int i=hex.length()-1;i>=0;i--)
		{
			if(!IsHex(hex[i]))
				throw "value error in toDec()"; // value error
			if(!IsDigit(hex[i]))
				sum+=((hex[i]-'A'+10)*pow(16,power));
			else
				sum+=((hex[i]-'0')*pow(16,power));
			power++;
		}
		return sum;
	}

	inline bool IsHexStr(std::string str)
	{
		for(int i=0;i<str.length();i++)
			if(!IsHex(str[i])) return false;
		return true;
	}

	inline bool IsDecStr(std::string str)
	{
		for(int i=0;i<str.length();i++)
			if(!IsDigit(str[i])) return false;
		return true;
	}

	inline std::string ToHexStr(int i)
	{
		// j payo tei vayo yo. kaam garcha
		std::string s;
		std::stringstream ss, ssss;

		int cnt =0;

		while(i!=0)
		{
			int rem = i%16;
			ss << char(rem<10?'0'+rem : 'A' + rem-10);
			cnt+=1;
			i = i/16;
		}
		ss >> s;
		for(int i=s.length()-1;i>=0;i--)
			ssss << s[i];
		s="";
		ssss >> s;
        if(s.length()==1)
            s= "0"+s;
		return s;
	}
}
