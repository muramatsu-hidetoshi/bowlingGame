
/*==========================================================================
音声再生・管理 [Audio.cpp]

概要：
    XAudio2を使用した音声再生機能を実装する。
    WAV形式の音声ファイルを読み込み、再生に必要なデータを管理する。

主な処理：
    ・XAudio2の初期化と終了処理
    ・WAVファイルの読み込みと音声データの管理
    ・音声再生用ボイスの生成
    ・通常再生およびループ再生
    ・使用済み音声リソースの解放

==========================================================================*/

#include <xaudio2.h>
#include <assert.h>
#include "audio.h"

#pragma comment(lib, "winmm.lib")


//==========================================================================
// 音声システム管理
//==========================================================================

// XAudio2のインターフェース
static IXAudio2* g_Xaudio{};

// すべての音声出力をまとめるマスタリングボイス
static IXAudio2MasteringVoice* g_MasteringVoice{};


//==========================================================================
// 音声システムの初期化
//==========================================================================
void InitAudio()
{
    // XAudio2のインスタンスを生成する。
    XAudio2Create(&g_Xaudio, 0);

    // 音声出力の最終段階となるマスタリングボイスを生成する。
    g_Xaudio->CreateMasteringVoice(&g_MasteringVoice);
}


//==========================================================================
// 音声システムの終了
//==========================================================================
void UninitAudio()
{
    // マスタリングボイスを破棄する。
    g_MasteringVoice->DestroyVoice();

    // XAudio2のインターフェースを解放する。
    g_Xaudio->Release();
}


//==========================================================================
// 音声データ管理構造体
//==========================================================================
struct AUDIO
{
    IXAudio2SourceVoice* SourceVoice{}; // 音声の再生を制御するボイス
    BYTE* SoundData{};                  // 読み込んだ音声データの格納先

    int Length{};                       // 音声データのサイズ（バイト単位）
    int PlayLength{};                   // 再生する音声データの長さ（サンプル数）
};


// 管理可能な音声データの最大数
#define AUDIO_MAX 100

// 音声データを管理する配列
static AUDIO g_Audio[AUDIO_MAX]{};


//==========================================================================
// 音声ファイルの読み込み
//==========================================================================
int LoadAudio(const char* FileName)
{
    int index = -1;

    // 未使用の音声管理領域を検索する。
    for (int i = 0; i < AUDIO_MAX; i++)
    {
        if (g_Audio[i].SourceVoice == nullptr)
        {
            index = i;
            break;
        }
    }

    // 空き領域がない場合は読み込みを行わない。
    if (index == -1)
        return -1;


    //======================================================================
    // WAVファイルの読み込み
    //======================================================================

    // WAVファイルに記録されている音声フォーマット情報
    WAVEFORMATEX wfx = { 0 };

    {
        HMMIO hmmio = NULL;
        MMIOINFO mmioinfo = { 0 };

        // RIFFチャンク、音声フォーマットチャンク、音声データチャンクの情報
        MMCKINFO riffchunkinfo = { 0 };
        MMCKINFO datachunkinfo = { 0 };
        MMCKINFO mmckinfo = { 0 };

        UINT32 buflen;
        LONG readlen;


        // WAVファイルを読み取り専用で開く。
        hmmio = mmioOpen((LPSTR)FileName, &mmioinfo, MMIO_READ);
        assert(hmmio);


        // WAVE形式のRIFFチャンクを検索する。
        riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
        mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);


        // 音声フォーマット情報を格納したfmtチャンクを検索する。
        mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
        mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);


        // フォーマット情報のサイズに応じて読み込み方法を切り替える。
        if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
        {
            // WAVEFORMATEXのサイズ以上ある場合は、その構造体に読み込む。
            mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
        }
        else
        {
            // フォーマット情報が小さい場合は、PCM形式の基本情報を読み込む。
            PCMWAVEFORMAT pcmwf = { 0 };

            mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));

            // 読み込んだPCM情報をWAVEFORMATEXに格納する。
            memset(&wfx, 0x00, sizeof(wfx));
            memcpy(&wfx, &pcmwf, sizeof(pcmwf));

            // 追加フォーマット情報がないことを設定する。
            wfx.cbSize = 0;
        }

        // 読み込みが完了したfmtチャンクから移動する。
        mmioAscend(hmmio, &mmckinfo, 0);


        // 音声データを格納したdataチャンクを検索する。
        datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
        mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);


        // 音声データのサイズを取得する。
        buflen = datachunkinfo.cksize;

        // 音声データを格納するためのメモリを確保する。
        g_Audio[index].SoundData = new unsigned char[buflen];

        // WAVファイルから音声データを読み込む。
        readlen = mmioRead(
            hmmio,
            (HPSTR)g_Audio[index].SoundData,
            buflen
        );


        // 読み込んだ音声データのサイズを保存する。
        g_Audio[index].Length = readlen;

        // 再生するサンプル数を計算する。
        // 音声データのバイト数を1サンプルあたりのバイト数で割る。
        g_Audio[index].PlayLength = readlen / wfx.nBlockAlign;


        // WAVファイルを閉じる。
        mmioClose(hmmio, 0);
    }


    //======================================================================
    // 音声再生用ボイスの生成
    //======================================================================

    // 読み込んだ音声フォーマットに対応するソースボイスを生成する。
    g_Xaudio->CreateSourceVoice(
        &g_Audio[index].SourceVoice,
        &wfx
    );

    // ソースボイスが正常に生成されたことを確認する。
    assert(g_Audio[index].SourceVoice);


    // 音声データの管理番号を返す。
    return index;
}


//==========================================================================
// 音声データの解放
//==========================================================================
void UnloadAudio(int Index)
{
    // 音声の再生を停止する。
    g_Audio[Index].SourceVoice->Stop();

    // ソースボイスを破棄する。
    g_Audio[Index].SourceVoice->DestroyVoice();

    // 読み込んだ音声データのメモリを解放する。
    delete[] g_Audio[Index].SoundData;

    // 解放済みであることを示すため、ポインタを初期化する。
    g_Audio[Index].SoundData = nullptr;
}


//==========================================================================
// 音声の再生
//==========================================================================
void PlayAudio(int Index, bool Loop)
{
    // 再生中の音声を停止する。
    g_Audio[Index].SourceVoice->Stop();

    // 再生待ちのバッファを破棄する。
    // 同じ音声を再生し直す際に、以前のデータが残らないようにする。
    g_Audio[Index].SourceVoice->FlushSourceBuffers();


    //======================================================================
    // 再生バッファの設定
    //======================================================================

    // XAudio2に渡す再生バッファ情報
    XAUDIO2_BUFFER bufinfo;

    // バッファ情報を初期化する。
    memset(&bufinfo, 0x00, sizeof(bufinfo));

    // 再生する音声データのサイズを設定する。
    bufinfo.AudioBytes = g_Audio[Index].Length;

    // 再生する音声データの先頭アドレスを設定する。
    bufinfo.pAudioData = g_Audio[Index].SoundData;

    // 再生開始位置を先頭に設定する。
    bufinfo.PlayBegin = 0;

    // 再生するサンプル数を設定する。
    bufinfo.PlayLength = g_Audio[Index].PlayLength;


    //======================================================================
    // ループ再生の設定
    //======================================================================
    if (Loop)
    {
        // ループ開始位置を音声データの先頭に設定する。
        bufinfo.LoopBegin = 0;

        // ループ対象の長さを音声データ全体に設定する。
        bufinfo.LoopLength = g_Audio[Index].PlayLength;

        // 再生を無限に繰り返すように設定する。
        bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
    }


    // 音声データをXAudio2の再生キューに登録する。
    g_Audio[Index].SourceVoice->SubmitSourceBuffer(&bufinfo, NULL);


    //======================================================================
    // 音声再生の開始
    //======================================================================

    // ソースボイスを開始し、音声を再生する。
    g_Audio[Index].SourceVoice->Start();
}
