#pragma once

#define USE_WORLDPOS

#include <Shaders/Common/GlobalConstants.h>
#include <Shaders/Common/ObjectConstants.h>
#include <Shaders/Materials/MaterialInterpolator.inc>


VS_OUT FillVertexData(VS_IN Input)
{
	VS_OUT Output;
	Output.Position = mul(ObjectToScreenMatrix, float4(Input.Position, 1.0f));
	Output.WorldPosition = mul(ObjectToWorldMatrix, float4(Input.Position, 1.0f)).xyz;
	
	#if defined(USE_NORMAL)
		// TODO: separate matrix for normal transform
		Output.Normal = normalize(mul((float3x3)ObjectToWorldMatrix, Input.Normal));
	#endif
	
	#if defined(USE_TANGENT)
		float handednessCorrection = 2.0f - dot(Input.Tangent, Input.Tangent);
		Output.Tangent = normalize(mul((float3x3)ObjectToWorldMatrix, Input.Tangent));
		Output.BiTangent = cross(Output.Normal, Output.Tangent) * handednessCorrection;
	#endif
	
	#if defined(USE_TEXCOORD0)
		#if USE_TEXCOORD1
			Output.TexCoords = float4(Input.TexCoord0, Input.TexCoord1);
		#else
			Output.TexCoords = Input.TexCoord0;
		#endif
	#endif
	
	#if defined(USE_COLOR)
		Output.Color = Input.Color;
	#endif	
	
	return Output;
}