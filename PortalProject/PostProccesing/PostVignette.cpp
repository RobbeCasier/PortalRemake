#include "stdafx.h"
#include "PostVignette.h"
#include "RenderTarget.h"

PostVignette::PostVignette()
	: PostProcessingMaterial(L"./Resources/Effects/Post/Vignette.fx")
	, m_pTextureMapVariabele(nullptr)
{
}

void PostVignette::LoadEffectVariables()
{
	auto effectVar = GetEffect()->GetVariableByName("gTexture");
	if (effectVar->IsValid())
		m_pTextureMapVariabele = effectVar->AsShaderResource();
}

void PostVignette::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	if (m_pTextureMapVariabele)
		m_pTextureMapVariabele->SetResource(pRendertarget->GetShaderResourceView());
}
