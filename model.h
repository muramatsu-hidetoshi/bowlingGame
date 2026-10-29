/*==========================================================================
モデルの管理 [model.h]

モデルの読み込みデータ、描画用バッファ、テクスチャ、
境界ボックスを保持する構造体と関連関数を宣言する。
==========================================================================*/

#pragma once

#include <string>
#include <unordered_map>
#include <d3d11.h>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "DirectXMath.h"
#include "Collision.h"

#pragma comment (lib, "assimp-vc143-mt.lib")

// 読み込んだモデルのデータ
struct MODEL
{
    const aiScene* AiScene = nullptr; // Assimpが読み込んだシーンデータ

    ID3D11Buffer** VertexBuffer;      // メッシュごとの頂点バッファ
    ID3D11Buffer** IndexBuffer;       // メッシュごとのインデックスバッファ

    // テクスチャ名とシェーダーリソースビューの対応
    std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

    AABB Aabb; // モデルの境界ボックス
};

// モデルを読み込む
// scaleは読み込み時の拡大率、isblenderは呼び出し側との互換性を保つ引数
MODEL* ModelLoad(
    const char* FileName,
    float scale = 1.0f,
    bool isblender = false);

// モデルに関連するリソースを解放する
void ModelRelease(MODEL* model);

// ワールド行列を指定してモデルを描画する
void ModelDraw(const MODEL* model, const DirectX::XMMATRIX& mtxWorld);
