#include <windows.h>
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam,
			  LPARAM lParam) 
{
  char *cr = "I don't know any magic.";

  HDC hdc;
  PAINTSTRUCT ps;
  RECT rect;

  LOGFONT lf = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "" };
  HFONT hfont_small = NULL;
  lf.lfWidth = 0;
  lf.lfHeight = 18;
  lf.lfWeight = 600;
  strcpy (lf.lfFaceName, "Arial");
  hfont_small = CreateFontIndirect (&lf);

  switch (iMsg)
    {
    case WM_PAINT:
      hdc = BeginPaint (hwnd, &ps);
      GetClientRect (hwnd, &rect);

      SetBkMode (hdc, TRANSPARENT);
      SetTextColor (hdc, RGB (255, 0, 0));
      /* Default font - vgasys.fon */
      DrawText (hdc, "Map # 3, (C)opy or (S)tamp tile", 31, &rect, DT_CENTER | DT_WORDBREAK);

      OffsetRect (&rect, 0, 30);

      /* Switch to arial */
      SelectObject (hdc, hfont_small);

      /* Draw text "borders" */
      SetTextColor (hdc, RGB (8, 14, 21));
      DrawText (hdc, cr, strlen (cr), &rect, DT_CENTER | DT_WORDBREAK);
      OffsetRect (&rect, -2, 0);
      DrawText (hdc, cr, strlen (cr), &rect, DT_CENTER | DT_WORDBREAK);
      OffsetRect (&rect, 1, 1);
      DrawText (hdc, cr, strlen (cr), &rect, DT_CENTER | DT_WORDBREAK);
      OffsetRect (&rect, 0, -2);
      DrawText (hdc, cr, strlen (cr), &rect, DT_CENTER | DT_WORDBREAK);
      OffsetRect (&rect, 0, 1);

      /* Draw text */
      SetTextColor (hdc, RGB (255, 255, 2));
      DrawText (hdc, cr, strlen (cr), &rect, DT_CENTER | DT_WORDBREAK);
      /* Emulate bold (bad kerning/spacing though): */
      /* OffsetRect (&rect, 1, 0); */
      /* DrawText (hdc, cr, strlen (cr), &rect, DT_CENTER | DT_WORDBREAK); */
      /* OffsetRect (&rect, -1, 0); */

      OffsetRect (&rect, 0, 30);
      SetTextColor (hdc, RGB (255, 255, 255));
      DrawText (hdc, cr, strlen (cr), &rect, DT_CENTER | DT_WORDBREAK);

      EndPaint (hwnd, &ps);
      return 0;

    case WM_DESTROY:
      PostQuitMessage (0);
      return 0;
    }
  return DefWindowProc (hwnd, iMsg, wParam, lParam);
}

int WINAPI
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine,
	 int iCmdShow) 
{
  HWND hwnd;
  MSG msg;
  WNDCLASSEX wndclass;
  memset(&wndclass, 0, sizeof (wndclass));
  wndclass.cbSize = sizeof (wndclass);
  wndclass.lpfnWndProc = WndProc;
  wndclass.lpszClassName = "font";
  wndclass.hbrBackground = (HBRUSH) GetStockObject (GRAY_BRUSH);

  RegisterClassEx (&wndclass);
  hwnd =
    CreateWindow ("font", "FreeDink font simulation", WS_OVERLAPPEDWINDOW,
		  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		  CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
  ShowWindow (hwnd, iCmdShow);
  UpdateWindow (hwnd);

  while (GetMessage (&msg, NULL, 0, 0))
    {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
    }

  return msg.wParam;
}

/**
 * Local Variables:
 * compile-command: "i586-mingw32msvc-gcc woefont.c -lgdi32"
 * End:
 */
