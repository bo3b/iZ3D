#ifndef __S3D_CODEGENERATOR_H__
#define __S3D_CODEGENERATOR_H__

#include <sstream>
#include <string>
#include "DumpState.h"

// Forward D3D types
struct D3D10DDI_HRESOURCE;
struct D3D10DDI_HSHADER;

//////////////////////////////////////////////////////////////////////////

namespace Commands
{
	class Command;
}

struct DependenciesData
{
	void SetDependenciesFromEvent( int eid )
	{
		depends.insert(eid);
	}

	size_t GetDependenciesCount()
	{
		return depends.size();
	}

	inline void SetDependenciesFromResource( void* resHnd );

	inline bool IsResourceUsed(void* resHnd);

	bool IsResourcesUsed(const std::vector<void *>& resHndls);

	inline void SetLastResourceEvent(void* resHnd, int eid);

	inline int GetLastResourceEvent(void* resHnd);

private:
	std::set<int>			depends; // eids of base commands
	std::map<void *,int>	lastResourceUseEID;
};

class CodeGenerator
{
public:
							CodeGenerator			();
							~CodeGenerator			();
	bool					Save					( const char* szCodeName_ );	

	template<typename T>
	void					ProcessCommand			( T& cmd_ )	{ m_cmds.push_back(&cmd_);};
	void AppendCommand(Commands::Command *cmd)
	{
		m_cmds.push_back(cmd);
	}

	void DontNeedSetEidCommands() { ds.DontNeedSetEidCommands(); }

protected:
	void					InternalGenerateHFile	();
	void					InternalGenerateCppFile	();

private:
	inline void				Format					( std::string& str, char* fmt, ... )	const;
	inline std::string		VarVertexBuffer			( const D3D10DDI_HRESOURCE& hRes )		const;
	inline std::string		VarIndexBuffer			( const D3D10DDI_HRESOURCE& hRes )		const;
	inline std::string		VarTexture1D			( const D3D10DDI_HRESOURCE& hRes )		const;
	inline std::string		VarTexture2D			( const D3D10DDI_HRESOURCE& hRes )		const;
	inline std::string		VarTexture3D			( const D3D10DDI_HRESOURCE& hRes )		const;
	inline std::string		VarTextureCube			( const D3D10DDI_HRESOURCE& hRes )		const;
	inline std::string		VarVertexShader			( const D3D10DDI_HSHADER&	hRes )		const;
	inline std::string		VarPixelShader			( const D3D10DDI_HSHADER&	hRes )		const;
	inline std::string		VarGeometryShader		( const D3D10DDI_HSHADER&	hRes )		const;
	
private:
	std::vector<Commands::Command *> m_cmds;

	CDumpState ds;

	// Generated files
	std::stringstream		m_genHeader;
	std::stringstream		m_genCpp;
	std::stringstream		m_genCppDraw;
	std::stringstream		m_genGlob;
	std::stringstream		m_genDecl;
	//std::stringstream		m_genDyn;
	
	// For init by NULL and check for null
	std::stringstream		m_varsSetNull;
	std::stringstream		m_varsCheckNull;

	// Vertex buffers
	std::stringstream		m_varsVBDecl;
	std::stringstream		m_varsVBInit;
	std::stringstream		m_varsVBRelease;
	//

	// Index buffers
	std::stringstream		m_varsIBDecl;
	std::stringstream		m_varsIBInit;
	std::stringstream		m_varsIBRelease;
	//

	// Textures ( 1D, 2D, 3D, Cube )
	std::stringstream		m_varsTextureDecl;
	std::stringstream		m_varsTextureInit;
	std::stringstream		m_varsTextureRelease;
	//

	// Shaders
	std::stringstream		m_varsShaderDecl;
	std::stringstream		m_varsShaderInit;
	std::stringstream		m_varsShaderRelease;
	//

#ifndef FINAL_RELEASE
	void ProcessCommands();

	void RemoveCommandsWithoutDependencies();
	void AnalyzeCommandsForInputLayout();
	void BuildDepenciesSet( DependenciesData &dependenciesData );
	void SetDependenciesFromCommand( Commands::Command* pcmd, DependenciesData &dependenciesData );
	void GenerateSetOfDeviceStateChangingCommands(std::set<UINT>& base);
	Commands::Command* GetCommandByEventId(int eventId);
#endif // FINAL_RELEASE
};

//////////////////////////////////////////////////////////////////////////

namespace Commands
{
	class CreateResource;
	class CreateVertexShader;
	class CreatePixelShader;
	class CreateGeometryShader;
}

//template <> void CodeGenerator::ProcessCommand<Commands::CreateResource>		( Commands::CreateResource& cmd_ );
//template <> void CodeGenerator::ProcessCommand<Commands::CreateVertexShader>	( Commands::CreateVertexShader& cmd_ );
//template <> void CodeGenerator::ProcessCommand<Commands::CreatePixelShader>		( Commands::CreatePixelShader& cmd_ );
//template <> void CodeGenerator::ProcessCommand<Commands::CreateGeometryShader>	( Commands::CreateGeometryShader& cmd_ );

//////////////////////////////////////////////////////////////////////////

#endif//__S3D_CODEGENERATOR_H__