
// テクスチャ作成サンプルプログラム

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <windows.h>
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>


struct CUSTOMVERTEX{  
   float x, y, z; // 頂点座標 
   float rhw;     // 除算数 
   float u, v;    //　テクスチャ座標 
}; 

#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_TEX1 )

TCHAR gName[100] = _T("テクスチャ作成サンプルプログラム");


// 頂点位置設定関数
BOOL SetPos(IDirect3DVertexBuffer9* vtx, float Cx, float Cy, float width, float height){
   CUSTOMVERTEX v[4]= 
   {  
      { Cx + width/2, Cy - height/2, 0.0f, 1.0f, 1.0f, 0.0f},
      { Cx + width/2, Cy + height/2, 0.0f, 1.0f, 1.0f, 1.0f}, 
      { Cx - width/2, Cy - height/2, 0.0f, 1.0f, 0.0f, 0.0f}, 
      { Cx - width/2, Cy + height/2, 0.0f, 1.0f, 0.0f, 1.0f} 
   };

   void *pData;
   if(SUCCEEDED(vtx->Lock(0, sizeof(CUSTOMVERTEX)*4, (void**)&pData, 0))){
      memcpy(pData, v, sizeof(CUSTOMVERTEX)*4);
      vtx->Unlock();
      return TRUE;
   }
   return FALSE;
}


// ウィンドウプロシージャ
LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam){
   if(mes == WM_DESTROY) {PostQuitMessage(0); return 0;}
   return DefWindowProc(hWnd, mes, wParam, lParam);
}


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
   // アプリケーションの初期化
   MSG msg; HWND hWnd;
   WNDCLASSEX wcex ={sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance, NULL, NULL,
                                    (HBRUSH)(COLOR_WINDOW+1), NULL, (TCHAR*)gName, NULL};
   if(!RegisterClassEx(&wcex))
      return 0;

   if(!(hWnd = CreateWindow(gName, gName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
                                    NULL, NULL, hInstance, NULL)))
      return 0;

   // Direct3Dの初期化
   LPDIRECT3D9 g_pD3D;
   LPDIRECT3DDEVICE9 g_pD3DDev;
   if( !(g_pD3D = Direct3DCreate9( D3D_SDK_VERSION )) ) return 0;

   D3DPRESENT_PARAMETERS d3dpp = {0,0,D3DFMT_UNKNOWN,0,D3DMULTISAMPLE_NONE,0,
                                                      D3DSWAPEFFECT_DISCARD,NULL,TRUE,0,D3DFMT_UNKNOWN,0,0}; 

   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
   {
      g_pD3D->Release();
      return 0;
   }

   ShowWindow(hWnd, nCmdShow);

   // 頂点バッファの作成
   IDirect3DVertexBuffer9* pVertex;
   if(FAILED(g_pD3DDev->CreateVertexBuffer( sizeof(CUSTOMVERTEX)*4, D3DUSAGE_WRITEONLY, FVF_CUSTOM, D3DPOOL_MANAGED, &pVertex, NULL))){
           g_pD3DDev->Release(); g_pD3D->Release();
           return 0;
   }

   
     // テクスチャの作成
   IDirect3DTexture9* pTex[3];
   DWORD FilterFlag[3] = { D3DX_FILTER_POINT, D3DX_FILTER_LINEAR, (D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER)};
   int i;
   int Tex_W = 64;
   for(i=0; i<3; i++){
      if(FAILED(D3DXCreateTextureFromFileEx(g_pD3DDev, _T("pict.jpg"), Tex_W, Tex_W, 0, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
                   FilterFlag[i], D3DX_DEFAULT, 0, NULL, NULL, &pTex[i]))){
           return 0;
          }
   }

   // メッセージ ループ
   float width = 256.0f;
   float height= 256.0f;
   do{
      Sleep(1);
      if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ){ DispatchMessage(&msg);}
         g_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );
         g_pD3DDev->BeginScene();

         for(i=0; i<3; i++){
             g_pD3DDev->SetTexture(0, pTex[i]);
             // 頂点バッファに頂点を書き込みできた時にだけ描画
             if(SetPos(pVertex, (10+width)*i+10+width/2, 20+height, width, height)){
                // 描画
                g_pD3DDev->SetStreamSource(0, pVertex, 0, sizeof(CUSTOMVERTEX));
                g_pD3DDev->SetFVF(FVF_CUSTOM);
                g_pD3DDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
             }
         }
         g_pD3DDev->EndScene();
         g_pD3DDev->Present( NULL, NULL, NULL, NULL );
   }while(msg.message != WM_QUIT);

   for(i=0; i<3; i++) pTex[i]->Release();
   pVertex->Release();
   g_pD3DDev->Release();
   g_pD3D->Release();

   return 0;
}