#pragma once

#include "Command.h"

namespace Commands
{
	class xxUpdateSubresourceUP : public Command
	{
	public:
		xxUpdateSubresourceUP();
		virtual ~xxUpdateSubresourceUP();

		/** Get size of the raw data to store update data */
		UINT Size() const;

		/** Make internal copy of the update data, so user can free it */
		bool MakeInternalDataCopy(D3DDeviceWrapper* pWrapper);

		/** Allocate memory for command storage, redirect pSysMemUP_ pointer. 
		 * @return true if succeeded.
		 */
		bool AllocateMemory(D3DDeviceWrapper* pWrapper, size_t size);

		/** Try to merge commands. Return true if succeeded */
		bool Merge(D3DDeviceWrapper* pWrapper, const xxUpdateSubresourceUP& cmd);

		virtual void SetDestResourceType( TextureType type );
		virtual void GetDestResources(DestResourcesSet &res);
		
	public:
		D3D10DDI_HRESOURCE		hDstResource_;
		UINT					DstSubresource_;
		CONST D3D10_DDI_BOX*	pDstBox_;
		CONST VOID*				pSysMemUP_;
		UINT					RowPitch_;
		UINT					DepthPitch_;

	protected:
		D3D10_DDI_BOX           DstBox_;
		BYTE*                   SysMemUP_; // allocate memory to store cached data
		D3DDeviceWrapper*		pWrapper_;
	};

	typedef boost::intrusive_ptr<xxUpdateSubresourceUP> TUpdateSubresourceUpCmdPtr;
}
