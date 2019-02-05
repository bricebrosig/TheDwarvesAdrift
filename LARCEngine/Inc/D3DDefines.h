/// \file D3DDefines.h
/// \brief Includes for working with DirectX 12.
///
/// I can't take any credit for this because it's copied
/// from Microsoft demos.

#pragma once

#include <wrl/client.h>
#include <wrl/event.h>

#include <d3d12.h>
#include <dxgi1_5.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wincodec.h>

#include "d3dx12.h"

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <pix.h>

#include "DeviceResources.h"

#include "Audio.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "DDSTextureLoader.h"
#include "DescriptorHeap.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PrimitiveBatch.h"
#include "ResourceUploadBatch.h"
#include "RenderTargetState.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h" 
#include "VertexTypes.h"

#include "SimpleMath.h"

namespace DX{
  /// \brief Helper class for COM exceptions.

  class com_exception: public std::exception{
    public:
      com_exception(HRESULT hr): result(hr){}

      virtual const char* what() const override{
        static char s_str[64] = { 0 };
        sprintf_s(s_str, "Failure with HRESULT of %08X", result);
        return s_str;
      } //what

      private:
        HRESULT result;
    };

    /// \brief Helper utility converts D3D API failures into exceptions.

    inline void ThrowIfFailed(HRESULT hr){
      if(FAILED(hr))
        throw com_exception(hr);
    } //ThrowIfFailed
} //DX