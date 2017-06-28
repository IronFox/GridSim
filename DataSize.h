#pragma once

class DataSize
{
public:
	count_t		numIC=0;
	count_t		numInstructions=0;
	
	/**/		DataSize()	{}
	/**/		DataSize(count_t numIC, count_t numInstructions):numIC(numIC),numInstructions(numInstructions)
	{}
	void		operator+=(const DataSize&other)
	{
		numIC += other.numIC;
		numInstructions += other.numInstructions;
	}
	DataSize		operator+(const DataSize&other)	const
	{
		DataSize rs = *this;
		rs += other;
		return rs;
	}
};
