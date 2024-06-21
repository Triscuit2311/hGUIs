#pragma once

#include "pch.h"

#include "logging.hpp"

#pragma comment(lib, "D3D11.lib")

namespace dxgi_cap
{
	struct d3d_device
	{
		CComPtr<ID3D11Device> device;
		CComPtr<ID3D11DeviceContext> device_context;
		D3D_FEATURE_LEVEL feature_level;

		d3d_device()
		{
			static constexpr D3D_DRIVER_TYPE driver_types[] = {
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP,
				D3D_DRIVER_TYPE_REFERENCE
			};

			static constexpr D3D_FEATURE_LEVEL feature_levels[] = {
				D3D_FEATURE_LEVEL_11_0,
				D3D_FEATURE_LEVEL_10_1,
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_1
			};

			for (const auto& driver_type : driver_types)
			{
				if (const auto hr = D3D11CreateDevice(
					nullptr,
					driver_type,
					nullptr,
					0,
					feature_levels,
					static_cast<UINT>(std::size(feature_levels)),
					D3D11_SDK_VERSION,
					&device,
					&feature_level,
					&device_context
				); SUCCEEDED(hr))
				{
					break;
				}
				device.Release();
				device_context.Release();
			}
		}
	};

	struct output_duplication
	{
		CComPtr<IDXGIOutputDuplication> dxgi_output_duplication;

		explicit output_duplication(ID3D11Device* device)
		{
			HRESULT hr;

			CComPtr<IDXGIDevice> dxgi_device;
			hr = device->QueryInterface(__uuidof(dxgi_device), reinterpret_cast<void**>(&dxgi_device)
			);
			if (FAILED(hr))
			{
				g_conlog->error("device->QueryInterface Failed");
				return;
			}

			CComPtr<IDXGIAdapter> dxgi_adapter;
			hr = dxgi_device->GetParent(__uuidof(dxgi_adapter), reinterpret_cast<void**>(&dxgi_adapter)
			);
			if (FAILED(hr))
			{
				g_conlog->error("dxgi_device->GetParent Failed");
				return;
			}

			CComPtr<IDXGIOutput> dxgi_output;
			hr = dxgi_adapter->EnumOutputs(0, &dxgi_output);
			if (FAILED(hr))
			{
				g_conlog->error("dxgi_adapter->EnumOutputs Failed");
				return;
			}

			CComPtr<IDXGIOutput1> dxgi_output1;
			hr = dxgi_output->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&dxgi_output1)
			);
			if (FAILED(hr))
			{
				g_conlog->error("dxgi_output->QueryInterface Failed");
				return;
			}

			hr = dxgi_output1->DuplicateOutput(device, &dxgi_output_duplication);
			if (FAILED(hr))
			{
				g_conlog->error("dxgi_output1->DuplicateOutput Failed");
				return;
			}
		}
	};

	struct gpu_buffer
	{
		CComPtr<ID3D11Texture2D> acquiredDesktopImage;

		explicit gpu_buffer(IDXGIOutputDuplication* dxgi_output_duplication)
		{
			CComPtr<IDXGIResource> desktop_resource;
			HRESULT hr = E_FAIL;
			for (int i = 0; i < 10; ++i)
			{
				DXGI_OUTDUPL_FRAME_INFO fi{};
				constexpr int timeout_ms = 500;
				hr = dxgi_output_duplication->AcquireNextFrame(timeout_ms, &fi, &desktop_resource);
				if (SUCCEEDED(hr) && (fi.LastPresentTime.QuadPart == 0))
				{
					desktop_resource.Release();

					// ReSharper disable once CppFunctionResultShouldBeUsed
					dxgi_output_duplication->ReleaseFrame();
					//Sleep(1);

					continue;
				}
				else
				{
					break;
				}
			}
			if (FAILED(hr))
			{
				g_conlog->error("gpu_buffer::dxgi_output_duplication->AcquireNextFrame() Failed");
				return;
			}

			hr = desktop_resource->QueryInterface(__uuidof(ID3D11Texture2D),
				reinterpret_cast<void**>(&acquiredDesktopImage)
			);
			if (FAILED(hr))
			{
				g_conlog->error("gpu_buffer::desktop_resource->QueryInterface() Failed");
				return;
			}
		}
	};

	inline cv::Mat capture_desktop_mat()
	{
		const d3d_device dev;
		ID3D11Device* device = dev.device;
		ID3D11DeviceContext* device_context = dev.device_context;
		if (device == nullptr)
		{
			g_conlog->error("capture_desktop[device == nullptr]");
			return {};
		}

		CComPtr<ID3D11Texture2D> tex2d;
		{
			const output_duplication od(device);
			IDXGIOutputDuplication* idxgi_output_duplication = od.dxgi_output_duplication;
			if (idxgi_output_duplication == nullptr)
			{
				g_conlog->error("capture_desktop[idxgi_output_duplication == nullptr]");
				return {};
			}

			const gpu_buffer adi(idxgi_output_duplication);
			ID3D11Texture2D* acquired_desktop_image = adi.acquiredDesktopImage;
			if (acquired_desktop_image == nullptr)
			{
				g_conlog->error("capture_desktop[acquired_desktop_image == nullptr]");
				return {};
			}

			DXGI_OUTDUPL_DESC dupl_desc;
			idxgi_output_duplication->GetDesc(&dupl_desc);

			const auto f = static_cast<int>(dupl_desc.ModeDesc.Format);
			if (const auto good_format = f == DXGI_FORMAT_B8G8R8A8_UNORM
				|| f == DXGI_FORMAT_B8G8R8X8_UNORM
				|| f == DXGI_FORMAT_B8G8R8A8_TYPELESS
				|| f == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB
				|| f == DXGI_FORMAT_B8G8R8X8_TYPELESS
				|| f == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB; !good_format)
			{
				g_conlog->error("capture_desktop[!good_format]");
				return {};
			}

			D3D11_TEXTURE2D_DESC desc{};
			desc.Width = dupl_desc.ModeDesc.Width;
			desc.Height = dupl_desc.ModeDesc.Height;
			desc.Format = dupl_desc.ModeDesc.Format;
			desc.ArraySize = 1;
			desc.BindFlags = 0;
			desc.MiscFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MipLevels = 1;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;
			if (const auto hr = device->CreateTexture2D(&desc, nullptr, &tex2d); FAILED(hr))
			{
				g_conlog->error("capture_desktop[device->CreateTexture2D failed]");
				return {};
			}
			if (tex2d == nullptr)
			{
				g_conlog->error("capture_desktop[tex2d == nullptr]");
				return {};
			}

			device_context->CopyResource(tex2d, acquired_desktop_image);
		}


		// Lock tex2dStaging and copy its content from GPU to CPU memory.
		D3D11_TEXTURE2D_DESC desc;
		tex2d->GetDesc(&desc);

		D3D11_MAPPED_SUBRESOURCE res;
		if (const auto hr = device_context->Map(
			tex2d,
			D3D11CalcSubresource(0, 0, 0),
			D3D11_MAP_READ,
			0,
			&res
		); FAILED(hr))
		{
			g_conlog->error("capture_desktop[device_context->Map failed]");
			return {};
		}


		cv::Mat mat(static_cast<int>(desc.Height), static_cast<int>(desc.Width), CV_8UC4);

		for (UINT y = 0; y < desc.Height; ++y)
		{
			void* mat_row = mat.ptr(static_cast<int>(y));
			const void* texture_row = static_cast<unsigned char*>(res.pData) + res.RowPitch * y;
			memcpy(mat_row, texture_row, static_cast<int>(desc.Width) * 4);
		}

		device_context->Unmap(tex2d, 0);

		return mat;
	}
}