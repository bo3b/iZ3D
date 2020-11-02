#pragma once

namespace Commands
{
	class CreateResourceBase : public Command
	{
	protected:
		void BuildCommand(CDumpState *ds);

		void OnRead() const;
		void OnWrite() const;

	protected:
		mutable int databin_pos;
		mutable std::vector<int> databin_poses;

	public:
		union{
			mutable D3D10DDIARG_CREATERESOURCE		CreateResource10_;
			mutable D3D11DDIARG_CREATERESOURCE		CreateResource11_;
		};
		D3D10DDI_HRESOURCE				hResource_;
		D3D10DDI_HRTRESOURCE			hRTResource_;

		CREATES(hResource_);
	};
}
