﻿#include "pch.h"
#include "MeshSyncClientMQ3.h"

static MeshSyncClientPlugin g_plugin;

void InitializeSettingDialog(MeshSyncClientPlugin *plugin, HWND parent);
void FinalizeSettingDialog();
void ShowSettingDialog(bool show);
bool IsSettingDialogActive();


MeshSyncClientPlugin::MeshSyncClientPlugin()
{
}

MeshSyncClientPlugin::~MeshSyncClientPlugin()
{
}

void MeshSyncClientPlugin::GetPlugInID(DWORD *Product, DWORD *ID)
{
    // プロダクト名(制作者名)とIDを、全部で64bitの値として返す
    // 値は他と重複しないようなランダムなもので良い
    *Product = MQPluginProduct;
    *ID = MQPluginID;
}

//---------------------------------------------------------------------------
//  GetPlugInName
//    プラグイン名を返す。
//    この関数は[プラグインについて]表示時に呼び出される。
//---------------------------------------------------------------------------
const char *MeshSyncClientPlugin::GetPlugInName(void)
{
    return "Unity Mesh Sync       Copyright(C) 2017, Unity Technologies Japan.";
}

//---------------------------------------------------------------------------
//  EnumString
//    ポップアップメニューに表示される文字列を返す。
//    この関数は起動時に呼び出される。
//---------------------------------------------------------------------------
const char *MeshSyncClientPlugin::EnumString(void)
{
    return "Unity Mesh Sync";
}


//---------------------------------------------------------------------------
//  Initialize
//    アプリケーションの初期化
//---------------------------------------------------------------------------
BOOL MeshSyncClientPlugin::Initialize()
{
    InitializeSettingDialog(this, MQ_GetWindowHandle());
    return TRUE;
}

//---------------------------------------------------------------------------
//  Exit
//    アプリケーションの終了
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::Exit()
{
    FinalizeSettingDialog();
}

//---------------------------------------------------------------------------
//  Activate
//    表示・非表示切り替え要求
//---------------------------------------------------------------------------
BOOL MeshSyncClientPlugin::Activate(MQDocument doc, BOOL flag)
{
    ShowSettingDialog(IsSettingDialogActive() ? false : true);
    return IsSettingDialogActive();
}

//---------------------------------------------------------------------------
//  IsActivated
//    表示・非表示状態の返答
//---------------------------------------------------------------------------
BOOL MeshSyncClientPlugin::IsActivated(MQDocument doc)
{
    return IsSettingDialogActive();
}

//---------------------------------------------------------------------------
//  OnMinimize
//    ウインドウの最小化への返答
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnMinimize(MQDocument doc, BOOL flag)
{
}

//---------------------------------------------------------------------------
//  OnReceiveUserMessage
//    プラグイン独自のメッセージの受け取り
//---------------------------------------------------------------------------
int MeshSyncClientPlugin::OnReceiveUserMessage(MQDocument doc, DWORD src_product, DWORD src_id, const char *description, void *message)
{
    return 0;
}


//---------------------------------------------------------------------------
//  OnDraw
//    描画時の処理
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnDraw(MQDocument doc, MQScene scene, int width, int height)
{
}


//---------------------------------------------------------------------------
//  OnNewDocument
//    ドキュメント初期化時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnNewDocument(MQDocument doc, const char *filename, NEW_DOCUMENT_PARAM& param)
{
    m_sync.setDocument(doc);
    autoSync();
}

//---------------------------------------------------------------------------
//  OnEndDocument
//    ドキュメント終了時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnEndDocument(MQDocument doc)
{
    m_sync.setDocument(nullptr);
}

//---------------------------------------------------------------------------
//  OnUndo
//    アンドゥ実行時
//---------------------------------------------------------------------------
BOOL MeshSyncClientPlugin::OnUndo(MQDocument doc, int undo_state)
{
    autoSync();
    return TRUE;
}

//---------------------------------------------------------------------------
//  OnRedo
//    リドゥ実行時
//---------------------------------------------------------------------------
BOOL MeshSyncClientPlugin::OnRedo(MQDocument doc, int redo_state)
{
    autoSync();
    return TRUE;
}

//---------------------------------------------------------------------------
//  OnUpdateUndo
//    アンドゥ状態更新時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnUpdateUndo(MQDocument doc, int undo_state, int undo_size)
{
}

//---------------------------------------------------------------------------
//  OnObjectModified
//    オブジェクトの編集時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnObjectModified(MQDocument doc)
{
    autoSync();
}

//---------------------------------------------------------------------------
//  OnObjectSelected
//    オブジェクトの選択状態の変更時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnObjectSelected(MQDocument doc)
{
    autoSync();
}

//---------------------------------------------------------------------------
//  OnUpdateObjectList
//    カレントオブジェクトの変更時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnUpdateObjectList(MQDocument doc)
{
}

//---------------------------------------------------------------------------
//  OnMaterialModified
//    マテリアルのパラメータ変更時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnMaterialModified(MQDocument doc)
{
}

//---------------------------------------------------------------------------
//  OnUpdateMaterialList
//    カレントマテリアルの変更時
//---------------------------------------------------------------------------
void MeshSyncClientPlugin::OnUpdateMaterialList(MQDocument doc)
{
}

//---------------------------------------------------------------------------
//  ExecuteCallback
//    コールバックに対する実装部
//---------------------------------------------------------------------------
bool MeshSyncClientPlugin::ExecuteCallback(MQDocument doc, void *option)
{
    CallbackInfo *info = (CallbackInfo*)option;
    return ((*this).*info->proc)(doc);
}

// コールバックの呼び出し
void MeshSyncClientPlugin::Execute(ExecuteCallbackProc proc)
{
    CallbackInfo info;
    info.proc = proc;
    BeginCallback(&info);
}

// プラグインのベースクラスを返す
MQBasePlugin *GetPluginClass()
{
    return &g_plugin;
}



std::string& GetServer(MeshSyncClientPlugin *plugin)
{
    return plugin->getClientSettings().server;
}
uint16_t& GetPort(MeshSyncClientPlugin *plugin)
{
    return plugin->getClientSettings().port;
}
bool& GetAutoSync(MeshSyncClientPlugin *plugin)
{
    return plugin->getAutoSync();
}
void Send(MeshSyncClientPlugin *plugin)
{
    plugin->send();
}
void Import(MeshSyncClientPlugin *plugin)
{
    plugin->import();
}

bool& MeshSyncClientPlugin::getAutoSync() { return m_auto_sync; }

ms::ClientSettings& MeshSyncClientPlugin::getClientSettings()
{
    return m_sync.getClientSettings();
}

void MeshSyncClientPlugin::send()
{
    m_sync.send();
}

void MeshSyncClientPlugin::import()
{
    m_sync.import();
}


void MeshSyncClientPlugin::autoSync()
{
    if (m_auto_sync) {
        m_sync.send();
    }
}
