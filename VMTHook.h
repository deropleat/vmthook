#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <memory>

class VMTHook_t 
{
private:
	std::uintptr_t** BaseClass = nullptr;
	std::unique_ptr<std::uintptr_t[]> CurrentVTF = nullptr;
	std::uintptr_t* OriginalVTF = nullptr;
	std::size_t TotalFuncs = 0;
public:
	VMTHook_t(void) = default;
	VMTHook_t(void* BaseClass) 
	{
		this->BaseClass = static_cast<std::uintptr_t**>(BaseClass);
		while (static_cast<std::uintptr_t*>(*this->BaseClass)[this->TotalFuncs])++this->TotalFuncs;
		const std::size_t TableSize = this->TotalFuncs * sizeof(std::uintptr_t);
		this->OriginalVTF = *this->BaseClass;
		this->CurrentVTF = std::make_unique<std::uintptr_t[]>(this->TotalFuncs);
		std::memcpy(this->CurrentVTF.get(), this->OriginalVTF, TableSize);
		*this->BaseClass = this->CurrentVTF.get();
	};
	~VMTHook_t()
	{
		*this->BaseClass = this->OriginalVTF;
	};
	template <typename Fn = void*> inline const Fn GetOrigIndex(std::size_t FunctionIndex)
	{
		return reinterpret_cast<Fn>(this->OriginalVTF[FunctionIndex]);
	}
	inline bool TableIndexHook(void* NewFunction, const std::size_t FunctionIndex)
	{
		if (FunctionIndex > this->TotalFuncs) return false;
		this->CurrentVTF[FunctionIndex] = reinterpret_cast<std::uintptr_t>(NewFunction);
		return true;
	}
	inline bool TableIndexUnHook(const std::size_t FunctionIndex) 
	{
		if (FunctionIndex > this->TotalFuncs) return false;
		this->CurrentVTF[FunctionIndex] = this->OriginalVTF[FunctionIndex];
		return true;
	}
	inline std::size_t GetMaxIndex() 
	{
		return this->TotalFuncs;
	}
};
