# getRemoteProcAddress
別プロセスにロードされているdllがエクスポートした関数のアドレスを取得するためのライブラリです。  
# 使い方  
RemodeHandle.hとRemodeHandle.cppをプロジェクトに入れてヘッダーをインクルードするだけです。
# 仕様
## class rth::RemoteHandle
## rth::RemoteHandle::RemoteHandle 
コンストラクタ
<table>
  <tr>
    <th colspan="2">RemoteHandle(DWORD pid)</th>
  </tr>
  <tr>
    <td>pid</td> <td>pidを指定</td>
  </tr>
   <tr>
    <td colspan="2">プロセスハンドルはデストラクタで解放されます</td>
  </tr>
</table>
<table>
  <tr>
    <th>例外条件</th><th>クラス</th>
  </tr>
  <tr>
    <td>pidが無効(OpenProcessに失敗)</td> <td>FAILED_FUNCTION</td>
  </tr>
</table>

<table>
  <tr>
    <th colspan="2">RemoteHandle(HANDLE hProcess)</th></th>
  </tr>
  <tr>
    <td>hProcess</td> <td>プロセスハンドルを指定。読み書きの権限が必要</td>
  </tr>
   <tr>
    <td colspan="2">プロセスハンドルはデストラクタでは解放されません</td>
  </tr>
</table>  

## rth::RemoteHandle::getModuleList  
プロセスにロードされているdllの一覧マップを返します。
<table>
  <tr>
    <th colspan="2">std::unordered_map&lt;std::string, HMODULE&gt; const getModuleList()</th>
  </tr>
  <tr>
    <td>戻り値</td> <td>名前をキーとしたマップ。</td>
  </tr>
</table>

## rth::RemoteHandle::getRemoteModuleHandle
プロセスにロードされているdllの一覧から指定されたモジュール名を検索してアドレスを取得します。
<table>
  <tr>
    <th colspan="2">HMODULE getRemoteModule(std::string mName)</th>
  </tr>
    <tr>
    <td>mName</td> <td>検索するモジュール名</td>
  </tr>
  <tr>
    <td>戻り値</td> <td>モジュールのベースアドレス。存在しない場合0</td>
  </tr>
</table>  

## rth::RemoteHandle::getRemoteProcAdress  
指定したモジュール名とその関数名から関数のアドレスを取得します。
<table>
  <tr>
    <th colspan="2">HMODULE getRemoteProcAdress(std::string mName, std::string fName)</th>
  </tr>
    <tr>
    <td>mName</td> <td>検索するモジュール名</td>
  </tr>
      <tr>
    <td>fName</td> <td>検索する関数名</td>
  </tr>
  <tr>
    <td>戻り値</td> <td>関数のアドレス。存在しない場合0</td>
  </tr>
</table>
指定したモジュールハンドルとその関数名から関数のアドレスを取得します。
<table>
  <tr>
    <th colspan="2">HMODULE getRemoteProcAdress(HMODULE module, std::string fName)</th>
  </tr>
    <tr>
    <td>module</td> <td>検索するモジュールハンドル</td>
  </tr>
      <tr>
    <td>fName</td> <td>検索する関数名</td>
  </tr>
  <tr>
    <td>戻り値</td> <td>関数のアドレス。存在しない場合0</td>
  </tr>
</table>
