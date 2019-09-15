#pragma once

#include <ObjIdl.h>
#include <stdint.h>

namespace agalia
{
	// {CBF93462-881C-451B-9F49-18D655D9061B}
	DEFINE_GUID(IID_AgaliaStream,
		0xcbf93462, 0x881c, 0x451b, 0x9f, 0x49, 0x18, 0xd6, 0x55, 0xd9, 0x6, 0x1b);

	struct agaliaStreamParam;

	class agaliaStream : public IStream
	{
	public:
		agaliaStream();
		virtual ~agaliaStream();

		// 
		// IUnknown 
		// 

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override;

		virtual ULONG STDMETHODCALLTYPE AddRef(void) override;

		virtual ULONG STDMETHODCALLTYPE Release(void) override;


		// 
		// ISequentialStream 
		// 

		virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
			/* [annotation] */
			_Out_writes_bytes_to_(cb, *pcbRead)  void* pv,
			/* [annotation][in] */
			_In_  ULONG cb,
			/* [annotation] */
			_Out_opt_  ULONG* pcbRead) override;

		virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
			/* [annotation] */
			_In_reads_bytes_(cb)  const void* pv,
			/* [annotation][in] */
			_In_  ULONG cb,
			/* [annotation] */
			_Out_opt_  ULONG* pcbWritten) override;



		// 
		// IStream 
		// 

		virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
			/* [in] */ LARGE_INTEGER dlibMove,
			/* [in] */ DWORD dwOrigin,
			/* [annotation] */
			_Out_opt_  ULARGE_INTEGER* plibNewPosition) override;

		virtual HRESULT STDMETHODCALLTYPE SetSize(
			/* [in] */ ULARGE_INTEGER libNewSize) override;

		virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo(
			/* [annotation][unique][in] */
			_In_  IStream* pstm,
			/* [in] */ ULARGE_INTEGER cb,
			/* [annotation] */
			_Out_opt_  ULARGE_INTEGER* pcbRead,
			/* [annotation] */
			_Out_opt_  ULARGE_INTEGER* pcbWritten) override;

		virtual HRESULT STDMETHODCALLTYPE Commit(
			/* [in] */ DWORD grfCommitFlags) override;

		virtual HRESULT STDMETHODCALLTYPE Revert(void) override;

		virtual HRESULT STDMETHODCALLTYPE LockRegion(
			/* [in] */ ULARGE_INTEGER libOffset,
			/* [in] */ ULARGE_INTEGER cb,
			/* [in] */ DWORD dwLockType) override;

		virtual HRESULT STDMETHODCALLTYPE UnlockRegion(
			/* [in] */ ULARGE_INTEGER libOffset,
			/* [in] */ ULARGE_INTEGER cb,
			/* [in] */ DWORD dwLockType) override;

		virtual HRESULT STDMETHODCALLTYPE Stat(
			/* [out] */ __RPC__out STATSTG* pstatstg,
			/* [in] */ DWORD grfStatFlag) override;

		virtual HRESULT STDMETHODCALLTYPE Clone(
			/* [out] */ __RPC__deref_out_opt IStream** ppstm) override;


		// 
		// agaliaStream 
		// 

		virtual HRESULT GetRawAddr(void* addr);


	public:
		agaliaStreamParam* param;
	};

	HRESULT agalia_create_stream(agaliaStream** stream, const wchar_t* path, uint64_t offset, uint64_t size);
}
