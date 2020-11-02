#pragma once

#include "xxSet.hpp"

namespace Commands
{

	struct IaBuffer
	{
		D3D10DDI_HRESOURCE	buffer;
		UINT				offset;
		UINT				stride;
	};

	class IaSetVertexBuffers : 
		public CommandWithAllocator< IaSetVertexBuffers, xxSet<IaBuffer> >
	{
	public:
		typedef CommandWithAllocator< IaSetVertexBuffers, xxSet<IaBuffer> > base_type;
		std::vector< std::vector<int> > databin_poses;

	public:
		IaSetVertexBuffers();
		IaSetVertexBuffers( UINT						StartBuffer, 
							UINT						NumBuffers, 
							CONST D3D10DDI_HRESOURCE*	phBuffers, 
							CONST UINT*					pStrides, 
							CONST UINT*					pOffsets );
		IaSetVertexBuffers( UINT			StartBuffer, 
							UINT			NumBuffers, 
							CONST IaBuffer*	buffers );

		// Override Command
		virtual void	Init() override;
		virtual void	UpdateDeviceState( D3DDeviceWrapper *pWrapper, D3DDeviceState* pState ) override;
		virtual void	Execute( D3DDeviceWrapper *pWrapper ) override;
		virtual bool	WriteToFile( D3DDeviceWrapper *pWrapper ) const override;
		virtual bool	ReadFromFile() override;

		void BuildCommand(CDumpState *ds);
		
		// Override xxSet
		virtual bool IsFull() const override { return (NumValues_ == D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT); }

		TIaSetVertexBuffersCmdPtr Clone() const { return TIaSetVertexBuffersCmdPtr(new IaSetVertexBuffers(Offset_, NumValues_, &Values_[0])); }
	
	public:
		static const IaBuffer		NULL_VALUE;	
		static D3D10DDI_HRESOURCE	phTempBuffers_[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		static UINT					phTempOffsets_[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		static UINT					phTempStrides_[D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

		DEPENDS_ON_VECTOR_MEMBER(Values_, buffer);
	};

}

extern VOID ( APIENTRY IaSetVertexBuffers )( D3D10DDI_HDEVICE  hDevice, UINT  StartBuffer, UINT  NumBuffers, 
											CONST D3D10DDI_HRESOURCE*  phBuffers, CONST UINT*  pStrides, CONST UINT*  pOffsets );
