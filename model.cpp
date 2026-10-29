
/*==========================================================================
モデルの読み込みと描画 [model.cpp]

Assimpを使用してモデルを読み込み、メッシュごとの頂点バッファと
インデックスバッファを作成する。
読み込んだモデルの描画と、使用したリソースの解放も管理する。
==========================================================================*/

#include "direct3d.h"
#include "DirectXTex.h"
#include "model.h"
#include "WICTextureLoader11.h"
#include "shader3d.h"
#include "texture.h"
#include <algorithm>

using namespace DirectX;

// 3Dモデルの頂点情報
struct Vertex3d
{
    XMFLOAT3 position; // 頂点座標
    XMFLOAT3 normal;   // 法線ベクトル
    XMFLOAT4 color;    // 頂点カラー
    XMFLOAT2 texcoord; // テクスチャ座標
};

// 白色テクスチャのID
static int g_TextureWhite = -1;

// モデルを読み込む
MODEL* ModelLoad(const char* FileName, float scale, bool isblender)
{
    // モデル情報を初期化して確保する
    MODEL* model = new MODEL{};

    // Assimpでモデルファイルを読み込む
    // 左手座標系への変換など、指定した処理を適用する
    model->AiScene =
        aiImportFile(FileName,
            aiProcessPreset_TargetRealtime_MaxQuality |
            aiProcess_ConvertToLeftHanded);

    // 読み込みに失敗した場合はメモリを解放して終了する
    if (!model->AiScene)
    {
        delete model;
        return nullptr;
    }

    // メッシュごとの頂点バッファとインデックスバッファを確保する
    model->VertexBuffer =
        new ID3D11Buffer * [model->AiScene->mNumMeshes] {};
    model->IndexBuffer =
        new ID3D11Buffer * [model->AiScene->mNumMeshes] {};

    // モデル全体の境界ボックスを求めるための最小値と最大値
    XMFLOAT3 min{}, max{};

    // 最初の頂点を処理したかどうか
    bool firstVertex = true;

    // メッシュごとに頂点データとバッファを作成する
    for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
    {
        aiMesh* mesh = model->AiScene->mMeshes[m];

        // メッシュの頂点数に合わせて頂点配列を確保する
        Vertex3d* vertex = new Vertex3d[mesh->mNumVertices];

        // 頂点情報を取り出して格納する
        for (unsigned int v = 0; v < mesh->mNumVertices; v++)
        {
            // 頂点座標に読み込み時のスケールを適用する
            vertex[v].position = {
                mesh->mVertices[v].x * scale,
                mesh->mVertices[v].y * scale,
                mesh->mVertices[v].z * scale
            };

            // テクスチャ座標が存在する場合は読み込む
            if (mesh->mTextureCoords[0])
            {
                vertex[v].texcoord = {
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                };
            }
            else
            {
                // テクスチャ座標がない場合は原点を設定する
                vertex[v].texcoord = { 0.0f, 0.0f };
            }

            // 頂点カラーを白色に設定する
            vertex[v].color = { 1, 1, 1, 1 };

            // 頂点の法線を格納する
            vertex[v].normal = {
                mesh->mNormals[v].x,
                mesh->mNormals[v].y,
                mesh->mNormals[v].z
            };

            // 最初の頂点を境界ボックスの初期値にする
            if (firstVertex)
            {
                min = max = vertex[v].position;
                firstVertex = false;
            }
            else
            {
                // 各軸の最小値を更新する
                min.x = std::min(min.x, vertex[v].position.x);
                min.y = std::min(min.y, vertex[v].position.y);
                min.z = std::min(min.z, vertex[v].position.z);

                // 各軸の最大値を更新する
                max.x = std::max(max.x, vertex[v].position.x);
                max.y = std::max(max.y, vertex[v].position.y);
                max.z = std::max(max.z, vertex[v].position.z);
            }
        }

        // 頂点バッファの設定を作成する
        D3D11_BUFFER_DESC vbd{};
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.ByteWidth = sizeof(Vertex3d) * mesh->mNumVertices;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        // 頂点バッファに登録するデータを指定する
        D3D11_SUBRESOURCE_DATA vsd{};
        vsd.pSysMem = vertex;

        // Direct3Dの頂点バッファを作成する
        HRESULT hr = Direct3D_GetDevice()->CreateBuffer(
            &vbd, &vsd, &model->VertexBuffer[m]);

        // 一時的に確保した頂点配列を解放する
        delete[] vertex;

        // 作成に失敗したメッシュはスキップする
        if (FAILED(hr)) continue;

        // 三角形ごとに3頂点のインデックスを格納する配列
        unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

        // 面のインデックス情報を配列へ格納する
        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            const aiFace& face = mesh->mFaces[f];

            index[f * 3 + 0] = face.mIndices[0];
            index[f * 3 + 1] = face.mIndices[1];
            index[f * 3 + 2] = face.mIndices[2];
        }

        // インデックスバッファの設定を作成する
        D3D11_BUFFER_DESC ibd{};
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

        // インデックスバッファに登録するデータを指定する
        D3D11_SUBRESOURCE_DATA isd{};
        isd.pSysMem = index;

        // Direct3Dのインデックスバッファを作成する
        hr = Direct3D_GetDevice()->CreateBuffer(
            &ibd, &isd, &model->IndexBuffer[m]);

        // 一時的に確保したインデックス配列を解放する
        delete[] index;
    }

    // 計算した最小値と最大値を境界ボックスとして保存する
    model->Aabb = { min, max };

    // モデルに埋め込まれたテクスチャを読み込む
    for (unsigned int i = 0; i < model->AiScene->mNumTextures; i++)
    {
        aiTexture* aitex = model->AiScene->mTextures[i];

        // テクスチャ画像を一時的に格納するオブジェクト
        ScratchImage image;
        TexMetadata meta;

        // メモリ上の画像データを読み込む
        if (FAILED(LoadFromWICMemory(
            aitex->pcData, aitex->mWidth,
            WIC_FLAGS_NONE, &meta, image)))
            continue;

        // シェーダーから参照するためのテクスチャビュー
        ID3D11ShaderResourceView* srv = nullptr;

        // テクスチャビューの作成に失敗した場合は次へ進む
        if (FAILED(CreateShaderResourceView(
            Direct3D_GetDevice(),
            image.GetImages(),
            image.GetImageCount(),
            meta,
            &srv)))
            continue;

        // テクスチャ名をキーにして保存する
        model->Texture[aitex->mFilename.C_Str()] = srv;
    }

    // テクスチャがない場合に使用する白色テクスチャを読み込む
    g_TextureWhite = Texture_Load(L"rom\\white_8x8.png");

    return model;
}

// モデルのリソースを解放する
void ModelRelease(MODEL* model)
{
    // nullptrの場合は処理を行わない
    if (!model) return;

    // 頂点バッファとインデックスバッファを解放する
    if (model->VertexBuffer && model->IndexBuffer)
    {
        for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
        {
            if (model->VertexBuffer[m])
            {
                model->VertexBuffer[m]->Release();
                model->VertexBuffer[m] = nullptr;
            }

            if (model->IndexBuffer[m])
            {
                model->IndexBuffer[m]->Release();
                model->IndexBuffer[m] = nullptr;
            }
        }

        // バッファ配列自体を解放する
        delete[] model->VertexBuffer;
        delete[] model->IndexBuffer;
    }

    // モデルに関連付けられたテクスチャビューを解放する
    for (auto& pair : model->Texture)
    {
        if (pair.second)
        {
            pair.second->Release();
            pair.second = nullptr;
        }
    }

    // テクスチャ情報をクリアする
    model->Texture.clear();

    // Assimpが読み込んだシーンデータを解放する
    if (model->AiScene)
    {
        aiReleaseImport(model->AiScene);
        model->AiScene = nullptr;
    }

    // モデル情報そのものを解放する
    delete model;
}

// モデルを描画する
void ModelDraw(const MODEL* model, const XMMATRIX& mtxWorld)
{
    // モデルが読み込まれていない場合は描画しない
    if (!model || !model->AiScene) return;

    // 3Dシェーダーを描画用に設定する
    Shader3d_Begin();

    // モデル内の各メッシュを描画する
    for (unsigned int i = 0; i < model->AiScene->mNumMeshes; i++)
    {
        // 頂点1つあたりのサイズとバッファの開始位置
        UINT stride = sizeof(Vertex3d);
        UINT offset = 0;

        // 必要なバッファがないメッシュはスキップする
        if (!model->VertexBuffer[i] || !model->IndexBuffer[i])
            continue;

        // 頂点バッファを設定する
        Direct3D_GetContext()->IASetVertexBuffers(
            0, 1, &model->VertexBuffer[i], &stride, &offset);

        // インデックスバッファを設定する
        Direct3D_GetContext()->IASetIndexBuffer(
            model->IndexBuffer[i], DXGI_FORMAT_R32_UINT, 0);

        // 描画するモデルのワールド行列を設定する
        Shader3d_SetWorldMatrix(mtxWorld);

        // 3頂点で1つの三角形を描画する設定にする
        Direct3D_GetContext()->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // メッシュに対応するマテリアルを取得する
        aiMaterial* mat =
            model->AiScene->mMaterials[
                model->AiScene->mMeshes[i]->mMaterialIndex];

        // ディフューズテクスチャの名前を取得する
        aiString texName;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &texName);

        // 読み込んだテクスチャを名前で検索する
        auto it = model->Texture.find(texName.C_Str());

        if (it != model->Texture.end())
        {
            // テクスチャが見つかった場合はシェーダーへ設定する
            ID3D11ShaderResourceView* srv = it->second;
            Direct3D_GetContext()->PSSetShaderResources(0, 1, &srv);

            // マテリアルの基本色を白色にする
            Shader3d_SetMaterialDiffuse({ 1, 1, 1, 1 });
        }
        else
        {
            // テクスチャがない場合は白色テクスチャを設定する
            Tecture_SetTexture(g_TextureWhite);

            // マテリアルに設定されている拡散反射色を取得する
            aiColor3D diffuse{};
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);

            // 取得した色をシェーダーへ渡す
            Shader3d_SetMaterialDiffuse(
                { diffuse.r, diffuse.g, diffuse.b, 1.0f });
        }

        // インデックスを使用してメッシュを描画する
        Direct3D_GetContext()->DrawIndexed(
            model->AiScene->mMeshes[i]->mNumFaces * 3, 0, 0);
    }
}

