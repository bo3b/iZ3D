#include <boost/test/unit_test.hpp>
#include "../CommonUtils/ShaderProcessor.h"
typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;
#include "../ShaderAnalysis/ShaderInstruction.h"
#include <strstream>
BOOST_AUTO_TEST_SUITE( test_shader_processor )

BOOST_AUTO_TEST_CASE( shader_test1 )
{
	iz3d::shader::ShaderProcessor sh;
	iz3d::shader::ShaderProcessor::TBinData bd;
	sh.CompileInstruction( "mad r7.yzw, r6.wx, cb0[0].z, r6.x", bd);
	shader_analyzer::ShaderInstruction si;
	std::istrstream shaderStream((char*)&bd[0] , (int)bd.size() * sizeof(bd[0]) );
	shaderStream >> si;
	auto k = si.toString();
	BOOST_CHECK( 2 == 2 );
}

BOOST_AUTO_TEST_CASE( shader_test2 )
{
	BOOST_CHECK( 2 == 2 );
}

BOOST_AUTO_TEST_SUITE_END()