#define BOOST_TEST_MODULE DX10CommandBufferTest

#include "stdafx.h"
#include "Commands/xxSet.hpp"
#include <boost/pool/pool_alloc.hpp>
#include <boost/test/unit_test.hpp>

#define XX_SET_TEST_FULL_SIZE 8
class xxSetTest :
	public Commands::xxSet<int>
{
public:
	xxSetTest(UINT			offset,
			  UINT			size,
			  const int*	values)
	:	xxSet<int>(offset, size, values)
	{
	}

	// Override useless stuff
	bool IsFull() const										{ return XX_SET_TEST_FULL_SIZE == NumValues_; }
	void Execute(D3DDeviceWrapper* pWrapper)				{ assert(false); }
	bool WriteToFile( D3DDeviceWrapper* pWrapper ) const	{ assert(false); return false; }
	bool ReadFromFile()										{ assert(false); return false; }
};

const allocation_desc allocation_distribution[] = 
{
	{XX_SET_TEST_FULL_SIZE, 1024}
};
Commands::xxSet<int>::value_vector::allocator_type Commands::xxSet<int>::allocator(allocation_distribution, allocation_distribution + 1);

void do_merge_test(int start0, int size0, int start1, int size1)
{
	std::vector<int> valuesFull(XX_SET_TEST_FULL_SIZE, 0);
	std::vector<int> values0(size0);
	std::vector<int> values1(size1);

	std::generate(values0.begin(), values0.end(), rand);
	std::generate(values1.begin(), values1.end(), rand);

	xxSetTest cmdFull(0, XX_SET_TEST_FULL_SIZE, &valuesFull[0]);
	xxSetTest cmd0(start0, size0, &values0[0]);
	xxSetTest cmd1(start1, size1, &values1[0]);

	cmd0.MergeCommands(0, &cmd1, &cmdFull);
	BOOST_CHECK( (std::min)(start0, start1) == cmd0.Offset_ );
	BOOST_CHECK( (std::max)(start0 + size0, start1 + size1) - (std::min)(start0, start1) == cmd0.NumValues_ );

	for (int i = 0; i<XX_SET_TEST_FULL_SIZE; ++i)
	{
		if (i >= (int)cmd0.Offset_ && i < (int)(cmd0.Offset_ + cmd0.NumValues_)) 
		{
			int iIn0      = i - start0;
			int iIn1      = i - start1;
			int iInMerged = int(i - cmd0.Offset_);
			if (i >= start1 && i < start1 + size1) {
				BOOST_CHECK(cmd0.Values_[iInMerged] == values1[iIn1]);
			}
			else if (i >= start0 && i < start0 + size0) {
				BOOST_CHECK(cmd0.Values_[iInMerged] == values0[iIn0]);
			}
			else {
				BOOST_CHECK(cmd0.Values_[iInMerged] == valuesFull[i]);
			}
		}
	}
}

BOOST_AUTO_TEST_CASE(merge_commands_test)
{
	// any combinations of start and sizes
	for (int start0 = 0; start0 < XX_SET_TEST_FULL_SIZE; ++start0)
	{
		for (int size0 = 1; size0 <= XX_SET_TEST_FULL_SIZE - start0; ++size0)
		{
			for (int start1 = 0; start1 < XX_SET_TEST_FULL_SIZE; ++start1)
			{
				for (int size1 = 1; size1 <= XX_SET_TEST_FULL_SIZE - start1; ++size1)
				{
					do_merge_test(start0, size0, start1, size1);
				}
			}
		}
	}
}