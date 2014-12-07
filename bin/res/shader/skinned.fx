
// Matrix Pallette
static const int MAX_MATRICES = 16;
float4x4    mWorldMatrixArray[MAX_MATRICES] ;//: WORLDMATRIXARRAY;

void skinTransform(float4 position, float3 normal, 
    int nBones, float4 blendWeights, float4 blendIndices,
    out float3 oPos, out float3 oNormal)
{
    float weights[4] = (float[4])blendWeights;
    int indices[4]  = {(int)blendIndices.x, (int)blendIndices.y,
        (int)blendIndices.z, (int)blendIndices.w};
    float lastWeight = 0.0f;

    oPos = 0.0f;
    oNormal = 0.0f;
    
    for (int iBone = 0; iBone < nBones-1; iBone++)
    {
        lastWeight = lastWeight + weights[iBone];
        oPos += mul(position, mWorldMatrixArray[indices[iBone]]) * weights[iBone];
        oNormal += mul(normal, mWorldMatrixArray[indices[iBone]]) * weights[iBone];
    }

    // Now that we have the calculated weight, add in the final influence
    lastWeight = 1.0f - lastWeight; 
    oPos += (mul(position, mWorldMatrixArray[indices[nBones-1]]) * lastWeight);
    oNormal += (mul(normal, mWorldMatrixArray[indices[nBones-1]]) * lastWeight);
}
