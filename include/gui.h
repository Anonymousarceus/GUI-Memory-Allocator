#ifndef GUI_H
#define GUI_H

#include <windows.h>
#include <wingdi.h>
#include <stdio.h>
#include "allocator.h"

// GUI Constants
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define CANVAS_WIDTH 1000
#define CANVAS_HEIGHT 400
#define CANVAS_X 50
#define CANVAS_Y 100
#define BLOCK_HEIGHT 30
#define BYTES_PER_PIXEL 1024  // How many bytes each pixel represents

// Colors
#define COLOR_FREE RGB(144, 238, 144)      // Light green for free blocks
#define COLOR_ALLOCATED RGB(255, 160, 160) // Light red for allocated blocks
#define COLOR_HEADER RGB(200, 200, 200)    // Gray for headers
#define COLOR_BG RGB(240, 240, 240) // Light gray background
#define COLOR_TEXT RGB(0, 0, 0)            // Black text

// GUI Controls IDs
#define ID_BUTTON_MALLOC 1001
#define ID_BUTTON_FREE 1002
#define ID_BUTTON_REALLOC 1003
#define ID_BUTTON_CALLOC 1004
#define ID_BUTTON_CLEAR_ALL 1005
#define ID_EDIT_SIZE 1006
#define ID_LISTBOX_PTRS 1007
#define ID_BUTTON_STRESS_TEST 1008
#define ID_BUTTON_MERGE_BLOCKS 1009

// Structure to track allocated pointers for GUI
typedef struct ptr_info {
    void* ptr;
    size_t size;
    char label[32];
    struct ptr_info* next;
} ptr_info_t;

// GUI State
typedef struct {
    HWND hwnd;
    HWND hEditSize;
    HWND hListBoxPtrs;
    HWND hStatus;
    ptr_info_t* ptr_list_head;
    int next_id;
    HBRUSH brush_free;
    HBRUSH brush_allocated;
    HBRUSH brush_header;
    HBRUSH brush_background;
    HPEN pen_border;
} gui_state_t;

// Function declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitializeGUI(HINSTANCE hInstance);
void CreateControls(HWND hwnd);
void DrawMemoryVisualization(HDC hdc, HWND hwnd);
void UpdateStatusBar(HWND hwnd);
void AddPointerToList(void* ptr, size_t size, const char* type);
void RemovePointerFromList(void* ptr);
void ClearAllPointers(void);
ptr_info_t* FindPointerInList(void* ptr);
void UpdatePointerListBox(HWND hwnd);
void DrawBlock(HDC hdc, int x, int y, int width, int height, COLORREF color, const char* text);
void HandleMallocButton(HWND hwnd);
void HandleFreeButton(HWND hwnd);
void HandleReallocButton(HWND hwnd);
void HandleCallocButton(HWND hwnd);
void HandleStressTest(HWND hwnd);
int GetSizeFromEdit(HWND hwnd);
void ShowError(const char* message);
void ShowInfo(const char* message);

// Memory visualization helpers
int GetPixelPositionFromAddress(void* addr);
int GetBlockWidthInPixels(size_t size);
void DrawMemoryMap(HDC hdc);
void DrawLegend(HDC hdc);

#endif // GUI_H