#include "ObjFileModel.h"
#include <algorithm> 
#include <fstream>
#include <sstream>
#include <iostream>

ObjFileModel::ObjFileModel(const char* filename, ID3D11Device* device, ID3D11DeviceContext* context)
{
    pD3DDevice = device;
    pImmediateContext = context;

    if (loadfile(filename) == 0)
    {
        parsefile();
        calculateTangents();
        createVB();
    }
}

bool ObjFileModel::SetVertexBuffer(ID3D11Device* D3D11Device, ID3D11DeviceContext* DeviceContext, const MODEL_POS_COL_TEX_NORM_VERTEX* VertexData, unsigned int Count)
{
    if (!VertexData || Count == 0) return false;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(MODEL_POS_COL_TEX_NORM_VERTEX) * Count;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = VertexData;

    ComPtr<ID3D11Buffer> newBuffer;
    HRESULT hr = D3D11Device->CreateBuffer(&bufferDesc, &initData, &newBuffer);
    if (FAILED(hr)) return false;

    pVertexBuffer = newBuffer;
    numverts = Count;

    return true;
}

void ObjFileModel::Draw()
{
    UINT stride = sizeof(MODEL_POS_COL_TEX_NORM_VERTEX);
    UINT offset = 0;
    pImmediateContext->IASetVertexBuffers(0, 1, pVertexBuffer.GetAddressOf(), &stride, &offset);
    pImmediateContext->Draw(numverts, 0);
}

int ObjFileModel::GetNumberOfVertices()
{
    return numverts;
}

int ObjFileModel::loadfile(const char* fname)
{
    ifstream file(fname);
    if (!file.is_open()) return -1;

    filename = fname;
    string line;
    while (getline(file, line))
    {
        istringstream ss(line);
        string type;
        ss >> type;

        if (type == "v")
        {
            xyz v;
            ss >> v.x >> v.y >> v.z;
            position_list.push_back(v);
        }
        else if (type == "vt")
        {
            xy t;
            ss >> t.x >> t.y;
            texcoord_list.push_back(t);
        }
        else if (type == "vn")
        {
            xyz n;
            ss >> n.x >> n.y >> n.z;
            normal_list.push_back(n);
        }
        else if (type == "f")
        {
            for (int i = 0; i < 3; i++)
            {
                string face;
                ss >> face;
                replace(face.begin(), face.end(), '/', ' ');
                istringstream fs(face);
                int pi, ti, ni;
                fs >> pi >> ti >> ni;
                pindices.push_back(pi - 1);
                tindices.push_back(ti - 1);
                nindices.push_back(ni - 1);
            }
        }
    }

    file.close();
    return 0;
}

void ObjFileModel::parsefile()
{
    numverts = static_cast<unsigned int>(pindices.size());
    vertices = make_unique<MODEL_POS_COL_TEX_NORM_VERTEX[]>(numverts);

    for (unsigned int i = 0; i < numverts; ++i)
    {
        MODEL_POS_COL_TEX_NORM_VERTEX& v = vertices[i];
        xyz pos = position_list[pindices[i]];
        xy tex = texcoord_list[tindices[i]];
        xyz norm = normal_list[nindices[i]];

        v.Pos = XMFLOAT3(pos.x, pos.y, pos.z);
        v.TexCoord = XMFLOAT2(tex.x, 1.0f - tex.y); // Flip V
        v.Normal = XMFLOAT3(norm.x, norm.y, norm.z);
        v.Col = XMFLOAT4(1, 1, 1, 1); // default white
        v.Tangent = XMFLOAT3(0, 0, 0);
        v.Bitangent = XMFLOAT3(0, 0, 0);
    }
}

void ObjFileModel::calculateTangents()
{
    for (unsigned int i = 0; i < numverts; i += 3)
    {
        auto& v0 = vertices[i];
        auto& v1 = vertices[i + 1];
        auto& v2 = vertices[i + 2];

        XMVECTOR p0 = XMLoadFloat3(&v0.Pos);
        XMVECTOR p1 = XMLoadFloat3(&v1.Pos);
        XMVECTOR p2 = XMLoadFloat3(&v2.Pos);

        XMVECTOR uv0 = XMLoadFloat2(&v0.TexCoord);
        XMVECTOR uv1 = XMLoadFloat2(&v1.TexCoord);
        XMVECTOR uv2 = XMLoadFloat2(&v2.TexCoord);

        XMVECTOR deltaPos1 = p1 - p0;
        XMVECTOR deltaPos2 = p2 - p0;

        XMFLOAT2 uv0f, uv1f, uv2f;
        XMStoreFloat2(&uv0f, uv0);
        XMStoreFloat2(&uv1f, uv1);
        XMStoreFloat2(&uv2f, uv2);

        float du1 = uv1f.x - uv0f.x;
        float dv1 = uv1f.y - uv0f.y;
        float du2 = uv2f.x - uv0f.x;
        float dv2 = uv2f.y - uv0f.y;

        float r = 1.0f / (du1 * dv2 - dv1 * du2);
        XMVECTOR tangent = (deltaPos1 * dv2 - deltaPos2 * dv1) * r;
        XMVECTOR bitangent = (deltaPos2 * du1 - deltaPos1 * du2) * r;

        XMFLOAT3 tan, bitan;
        XMStoreFloat3(&tan, tangent);
        XMStoreFloat3(&bitan, bitangent);

        v0.Tangent = v1.Tangent = v2.Tangent = tan;
        v0.Bitangent = v1.Bitangent = v2.Bitangent = bitan;
    }
}

bool ObjFileModel::createVB()
{
    return SetVertexBuffer(pD3DDevice.Get(), pImmediateContext.Get(), vertices.get(), numverts);
}