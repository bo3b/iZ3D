#define BOOST_TEST_MODULE SmallObjectAllocatorTest

#include <boost/pool/pool_alloc.hpp>
#include <boost/test/unit_test.hpp>	
#include <iostream>
#include <vector>
#include "../../S3DWrapper10/Array.h"
#include "../../S3DWrapper10/SmallObjectAllocator.h"
#include "../../Testing/timings.h"

#ifdef BOOST_NO_EXCEPTIONS

void boost::throw_exception(const stdext::exception& ex)
{
	BOOST_CHECK(false);
	exit(1); // what you can do with allocation problem?
}

#endif // BOOST_NO_EXCEPTIONS

struct allocation
{
	int*	mem;
	size_t	size;
	int		value;
};

allocation_desc distribution2_16[] =
{
	{2,  4},
	{4,  4},
	{8,  4},
	{12, 4},
	{16, 4}
};
auto distribution2_16_begin = distribution2_16;
auto distribution2_16_end   = distribution2_16 + sizeof(distribution2_16) / sizeof(allocation_desc);

std::string generate_test_name()
{
	std::string name = "small_object_allocator";
#ifdef _DEBUG
	name += "_d";
#endif
#ifdef _WIN64
	name += "_x64";
#endif
	return name;
}

struct vector_instantiation_test :
	public performance_test
{
public:
	vector_instantiation_test()
	:	performance_test( generate_test_name() )
	{
	}

	~vector_instantiation_test()
	{
		plot(name + ".png");
	}

	template<typename T, size_t MAX_SIZE>
	void do_test(size_t					NUM_INSTANCES, 
				 const allocation_desc* distr_begin = small_object_allocator<T>::base_classes,
				 const allocation_desc* distr_end   = small_object_allocator<T>::base_classes + small_object_allocator<T>::base_num_classes)
	{
		typedef std::vector<T>								value_vector;
		typedef array<T>									value_array;
		typedef std::vector<T, small_object_allocator<T>>	custom_value_vector;
		typedef array<T, small_object_allocator<T>>			custom_value_array;
		typedef std::vector<T, boost::pool_allocator<T>>	boost_value_vector;
		typedef array<T, boost::pool_allocator<T>>			boost_value_array;

		// list competitors
		static const char* vector_std   = "std::vector<std::allocator>";
		static const char* vector_sma   = "std::vector<small_object_allocator>";
		static const char* vector_pool  = "std::vector<boost::pool_allocator>";
		static const char* array_std    = "array<std::allocator>";
		static const char* array_sma    = "array<small_object_allocator>";
		static const char* array_pool   = "array<boost::pool_allocator>";
		static const char* static_array = "static array";

		std::ostringstream ss;
		ss << "vector<" << typeid(T).name() << "> up to " << MAX_SIZE << " elements (x " << NUM_INSTANCES << ")";
		
		small_object_allocator<T> allocator(distr_begin, distr_end);
		{
			performance_test::test test(*this, ss.str());

			// vector_std
			{
				performance_test::performance p(test, vector_std);
				for (size_t i = 0; i<NUM_INSTANCES; ++i) 
				{
					size_t size = i % MAX_SIZE + 1;
					value_vector vector(size, T()); 
				}
			}
			
			// array_std
			{
				performance_test::performance p(test, array_std);
				for (size_t i = 0; i<NUM_INSTANCES; ++i) 
				{
					size_t size = i % MAX_SIZE + 1;
					value_array array(size, T()); 
				}
			}
			
			// vector_sma
			{
				performance_test::performance p(test, vector_sma);
				for (size_t i = 0; i<NUM_INSTANCES; ++i) 
				{
					size_t size = i % MAX_SIZE + 1;
					custom_value_vector vector(size, T(), allocator);
				}
			}
		
			// array_sma
			{
				performance_test::performance p(test, array_sma);
				for (size_t i = 0; i<NUM_INSTANCES; ++i) 
				{
					size_t size = i % MAX_SIZE + 1;
					custom_value_array array(size, T(), allocator); 
				}
			}

			// vector_pool
			{
				performance_test::performance p(test, vector_pool);
				for (size_t i = 0; i<NUM_INSTANCES; ++i) 
				{
					size_t size = i % MAX_SIZE + 1;
					boost_value_vector vector(size, T());
				}
			}
			
			// array_pool
			{
				performance_test::performance p(test, array_pool);
				for (size_t i = 0; i<NUM_INSTANCES; ++i) 
				{
					size_t size = i % MAX_SIZE + 1;
					boost_value_array array(size, T()); 
				}
			}
			
			// static_array
			{
				performance_test::performance p(test, static_array);
				for (size_t i = 0; i<NUM_INSTANCES; ++i) 
				{
					T array[MAX_SIZE];
					size_t size = i % MAX_SIZE + 1;
					std::fill(array, array + MAX_SIZE, T());
				}
			}
		}
	}
};

vector_instantiation_test vinst_test;
/*
BOOST_AUTO_TEST_CASE(allocator_test)
{
	small_object_allocator<int>	allocator;
	std::vector<allocation>		allocations;

	const size_t MAX_ALLOCATION_SIZE = 512;
	const size_t NUM_ALLOCATIONS     = 5000;
	for(size_t i = 0; i<NUM_ALLOCATIONS; ++i)
	{
		allocation al;
		al.size  = rand() % MAX_ALLOCATION_SIZE;
		al.value = rand();
		al.mem	 = allocator.allocate(al.size);
		std::fill(al.mem, al.mem + al.size, al.value);

		allocations.push_back(al);
	}

	for(size_t i = 0; i<NUM_ALLOCATIONS; ++i)
	{
		for(int* pmem = allocations[i].mem; pmem != allocations[i].mem + allocations[i].size; ++pmem) {
			BOOST_CHECK_EQUAL(*pmem, allocations[i].value);
		}

		allocator.deallocate(allocations[i].mem, allocations[i].size);
	}
}

BOOST_AUTO_TEST_CASE(allocator_custom_distribution_test)
{
	small_object_allocator<int>	allocator(distribution2_16_begin, distribution2_16_end);
	std::vector<allocation>		allocations;

	const size_t MAX_ALLOCATION_SIZE = 16;
	const size_t NUM_ALLOCATIONS     = 5000;
	for(size_t i = 0; i<NUM_ALLOCATIONS; ++i)
	{
		allocation al;
		al.size  = rand() % MAX_ALLOCATION_SIZE;
		al.value = rand();
		al.mem	 = allocator.allocate(al.size);
		std::fill(al.mem, al.mem + al.size, al.value);

		allocations.push_back(al);
	}

	for(size_t i = 0; i<NUM_ALLOCATIONS; ++i)
	{
		for(int* pmem = allocations[i].mem; pmem != allocations[i].mem + allocations[i].size; ++pmem) {
			BOOST_CHECK_EQUAL(*pmem, allocations[i].value);
		}

		allocator.deallocate(allocations[i].mem, allocations[i].size);
	}
}

BOOST_AUTO_TEST_CASE(fill_test)
{
	std::vector< int, small_object_allocator<int> > intVector;

	const size_t n1 = 8;
	const size_t n2 = 32;
	const size_t n3 = 100;
	const size_t n4 = 1000;

	intVector.resize(n1);
	std::fill(intVector.begin(), intVector.end(), 1);
	std::for_each(intVector.begin(), intVector.begin() + n1, [] (int v) { BOOST_CHECK_EQUAL(v, 1); });

	intVector.resize(n2);
	std::fill(intVector.begin() + n1, intVector.begin() + n2, 2);
	std::for_each(intVector.begin(), intVector.begin() + n1, [] (int v) { BOOST_CHECK_EQUAL(v, 1); });
	std::for_each(intVector.begin() + n1, intVector.begin() + n2, [] (int v) { BOOST_CHECK_EQUAL(v, 2); });

	intVector.resize(n3);
	std::fill(intVector.begin() + n2, intVector.begin() + n3, 3);
	std::for_each(intVector.begin(), intVector.begin() + n1, [] (int v) { BOOST_CHECK_EQUAL(v, 1); });
	std::for_each(intVector.begin() + n1, intVector.begin() + n2, [] (int v) { BOOST_CHECK_EQUAL(v, 2); });
	std::for_each(intVector.begin() + n2, intVector.begin() + n3, [] (int v) { BOOST_CHECK_EQUAL(v, 3); });

	intVector.resize(n4);
	std::fill(intVector.begin() + n3, intVector.begin() + n4, 4);
	std::for_each(intVector.begin(), intVector.begin() + n1, [] (int v) { BOOST_CHECK_EQUAL(v, 1); });
	std::for_each(intVector.begin() + n1, intVector.begin() + n2, [] (int v) { BOOST_CHECK_EQUAL(v, 2); });
	std::for_each(intVector.begin() + n2, intVector.begin() + n3, [] (int v) { BOOST_CHECK_EQUAL(v, 3); });
	std::for_each(intVector.begin() + n3, intVector.begin() + n4, [] (int v) { BOOST_CHECK_EQUAL(v, 4); });
}

BOOST_AUTO_TEST_CASE(allocator_vector_test)
{
	small_object_allocator<int>						allocator;
	std::vector< int, small_object_allocator<int> > intVector(allocator);
	
	const size_t NUM_PBACK = 100;
	for(size_t i = 0; i<NUM_PBACK; ++i) {
		intVector.push_back(i);
	}

	for(size_t i = 0; i<NUM_PBACK; ++i) {
		BOOST_CHECK_EQUAL(intVector[i], i);
	}
}

BOOST_AUTO_TEST_CASE(copy_test)
{
	const size_t NUM_PBACK = 50000;

	std::vector<short, small_object_allocator<short>>	vector1;
	std::vector<short, small_object_allocator<short>>	vector2;
	std::vector<short, small_object_allocator<short>>	vector3;

	for (size_t i = 0; i<NUM_PBACK; ++i)
	{
		short value = rand();
		vector1.push_back(value);
	}

	vector2 = vector1;
	vector3 = vector2;
	
	for (size_t i = 0; i<NUM_PBACK; ++i)
	{
		BOOST_CHECK_EQUAL(vector1[i], vector2[i]);
		BOOST_CHECK_EQUAL(vector2[i], vector3[i]);
	}
}

BOOST_AUTO_TEST_CASE(big_test)
{
	const size_t NUM_PBACK = 500000;

	std::vector<int, small_object_allocator<int>>	intVector;
	std::vector<int>								defaultVector;
	for (size_t i = 0; i<NUM_PBACK; ++i)
	{
		int value = rand();
		intVector.push_back(value);
		defaultVector.push_back(value);
	}

	for (size_t i = 0; i<NUM_PBACK; ++i)
	{
		BOOST_CHECK_EQUAL(intVector[i], defaultVector[i]);
	}
}

BOOST_AUTO_TEST_CASE(performace_allocator_test)
{
	const size_t NUM_INSTANCES = 50000;
	const size_t MAX_SIZE      = 256;
	
	//BOOST_TEST_MESSAGE("Testing performance: ");
	std::cout << "Testing performance: " << std::endl;

	{
		std::allocator<int>			allocator;
		std::vector<allocation>		allocations(NUM_INSTANCES);

		getTimeInterval();
		for (size_t i = 0; i<NUM_INSTANCES; ++i) 
		{
			int size = i % MAX_SIZE + 1;
			allocations[i].mem  = allocator.allocate(size);
			allocations[i].size = size;
		}
		//BOOST_TEST_MESSAGE( "Default allocator(allocation): " << getTimeInterval() );
		std::cout << "Default allocator(allocation): " << getTimeInterval() << std::endl;
		
		getTimeInterval();
		for (size_t i = 0; i<NUM_INSTANCES; ++i) {
			allocator.deallocate(allocations[i].mem, allocations[i].size);
		}
		//BOOST_TEST_MESSAGE( "Default allocator(allocation): " << getTimeInterval() );
		std::cout << "Default allocator(deallocation): " << getTimeInterval() << std::endl;
	}

	{
		small_object_allocator<int>	allocator;
		std::vector<allocation>		allocations(NUM_INSTANCES);

		getTimeInterval();
		for (size_t i = 0; i<NUM_INSTANCES; ++i) 
		{
			int size = i % MAX_SIZE + 1;
			allocations[i].mem  = allocator.allocate(size);
			allocations[i].size = size;
		}
		//BOOST_TEST_MESSAGE( "Default allocator(allocation): " << getTimeInterval() );
		std::cout << "Custom allocator(allocation): " << getTimeInterval() << std::endl;
		
		getTimeInterval();
		for (size_t i = 0; i<NUM_INSTANCES; ++i) {
			allocator.deallocate(allocations[i].mem, allocations[i].size);
		}
		//BOOST_TEST_MESSAGE( "Default allocator(allocation): " << getTimeInterval() );
		std::cout << "Custom allocator(deallocation): " << getTimeInterval() << std::endl;
	}
}
*/
BOOST_AUTO_TEST_CASE(vector_instantiation_performance_test)
{
	const size_t test_size = 20000;

	vinst_test.do_test<int, 256>(test_size);
	vinst_test.do_test<void*, 256>(test_size);
	vinst_test.do_test<std::pair<int, int>, 256>(test_size);
	vinst_test.do_test<int, 16>(test_size, distribution2_16_begin, distribution2_16_end);
	vinst_test.do_test<void*, 16>(test_size, distribution2_16_begin, distribution2_16_end);
	vinst_test.do_test<std::pair<int, int>, 16>(test_size, distribution2_16_begin, distribution2_16_end);
}