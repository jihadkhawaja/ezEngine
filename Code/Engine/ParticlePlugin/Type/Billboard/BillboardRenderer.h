#pragma once

#include <ParticlePlugin/Basics.h>
#include <RendererCore/Pipeline/Declarations.h>
#include <RendererCore/Pipeline/RenderData.h>
#include <Core/ResourceManager/ResourceHandle.h>
#include <Foundation/Types/SharedPtr.h>
#include <Foundation/Containers/DynamicArray.h>
#include <ParticlePlugin/Renderer/ParticleRenderer.h>

#include <RendererCore/../../../Data/Base/Shaders/Particles/ParticleSystemConstants.h>
#include <RendererCore/../../../Data/Base/Shaders/Particles/BillboardShaderData.h>

typedef ezTypedResourceHandle<class ezShaderResource> ezShaderResourceHandle;
typedef ezTypedResourceHandle<class ezTextureResource> ezTextureResourceHandle;

typedef ezRefCountedContainer<ezDynamicArray<ezBillboardParticleData>> ezBillboardParticleDataContainer;

class EZ_PARTICLEPLUGIN_DLL ezParticleBillboardRenderData : public ezRenderData
{
  EZ_ADD_DYNAMIC_REFLECTION(ezParticleBillboardRenderData, ezRenderData);

public:
  ezParticleBillboardRenderData();

  ezTextureResourceHandle m_hTexture;
  ezUInt32 m_uiNumParticles;
  ezSharedPtr<ezBillboardParticleDataContainer> m_GpuData;
};



/// \brief Implements rendering of particle systems
class EZ_PARTICLEPLUGIN_DLL ezParticleBillboardRenderer : public ezParticleRenderer
{
  EZ_ADD_DYNAMIC_REFLECTION(ezParticleBillboardRenderer, ezParticleRenderer);
  EZ_DISALLOW_COPY_AND_ASSIGN(ezParticleBillboardRenderer);

public:
  ezParticleBillboardRenderer();
  ~ezParticleBillboardRenderer();

  virtual void GetSupportedRenderDataTypes(ezHybridArray<const ezRTTI*, 8>& types) override;
  virtual void RenderBatch(const ezRenderViewContext& renderContext, ezRenderPipelinePass* pPass, const ezRenderDataBatch& batch) override;

protected:
  void CreateDataBuffer();

  static const ezUInt32 s_uiMaxBufferSize = 256 * 1024;
  ezGALBufferHandle m_hDataBuffer;
  ezShaderResourceHandle m_hShader;
};

