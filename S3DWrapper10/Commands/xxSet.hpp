#pragma once

#include "Command.h"

namespace Commands
{
	
	template<typename T>
	class xxSet
		: public Command
	{
	public:
		typedef typename fast_vector<T>::type			value_vector;
		typedef typename value_vector::allocator_type	allocator_type;
	
	public:
		xxSet()
		:	Offset_(0)
		,	NumValues_(0)
		,	Values_(allocator)
		{
		}

		xxSet(UINT Offset, UINT NumValues, const T* Values)
		:	Offset_(Offset)
		,	NumValues_(NumValues)
		,	Values_(Values, Values + NumValues, allocator)
		{
		}

		/** This function generates command that is equivalent to the following execution:
		 * \code
		 * pFullCmd->Execute();
		 * this->Execute();
		 * pCmd->Execute();
		 * \uncode
		 * And stores result in the this command.
		 * @param pCmd - command executed after this.
		 * @param pFullCmd - command storing values(constant buffers, shader resources, ...) for every stage (you can leave it NULL if this command is full. 
		 */
		virtual void MergeCommands( D3DDeviceWrapper* pWrapper, const xxSet* pCmd, const xxSet* pFullCmd = 0 )
		{
			// nothing to merge
			if (pCmd->NumValues_ == 0) {
				return;
			}

			if ( IsFull() )
			{
				// optimized merge
				memcpy( &Values_[pCmd->Offset_],
						&pCmd->Values_[0], 
						pCmd->NumValues_ * sizeof(T) );
			}
			else 
			{
				// heavy complex merge
				UINT NewStart     = (std::min)(Offset_, pCmd->Offset_);
				UINT NewNumValues = (std::max)(Offset_ + NumValues_, pCmd->Offset_ + pCmd->NumValues_) - NewStart;
				if (NewNumValues != NumValues_) {
					Values_.resize(NewNumValues);
				}

				UINT minEnd   = (std::min)(Offset_ + NumValues_, pCmd->NumValues_ + pCmd->Offset_);
				UINT maxStart = (std::max)(Offset_, pCmd->Offset_);
				int  gap      = int(maxStart - minEnd);
				int  overlap  = gap < 0 ? -gap : 0; // how much new values override old

				// if we have to shift old values and they are not totally overlapped
				if (Offset_ - NewStart > 0 && NumValues_ - overlap > 0)
				{
					// shift old values
					memmove( &Values_[Offset_ - NewStart + overlap],
							 &Values_[overlap], 
							 (NumValues_ - overlap) * sizeof(T) );
				}

				// if there is gap between commands
				if (gap > 0)
				{
					// fill the gap with data from full cmd
					memcpy( &Values_[minEnd - NewStart],
							&pFullCmd->Values_[minEnd],
							gap * sizeof(T) );
				}

				// override old values with new
				memcpy( &Values_[pCmd->Offset_ - NewStart],
						&pCmd->Values_[0],
						pCmd->NumValues_ * sizeof(T) );
		
				Offset_    = NewStart;
				NumValues_ = NewNumValues;
			}

			CheckIsStereo(pWrapper);
		}
		
		virtual bool IsFull() const = 0;
		virtual void CheckIsStereo(D3DDeviceWrapper* /*pWrapper*/) { /* if can't have stereo resources, do nothing */ }

		void SetLastResources(D3DDeviceWrapper* pWrapper) {
			Bd().SetLastResources(pWrapper,CommandId,
				Values_.empty() ? 0 : Values_.begin(),NumValues_,Offset_); 
		}

	public:
		UINT			Offset_;
		UINT			NumValues_;
		value_vector	Values_;

	public:
		static allocator_type allocator;
	};

} // namespace Commands