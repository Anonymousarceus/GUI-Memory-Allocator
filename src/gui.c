#include "../include/gui.h"
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global GUI state
static gui_state_t g_gui_state = {0};
extern char heap[];  // Reference to heap from allocator.c

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize allocator
    allocator_init();
    
    // Initialize GUI
    InitializeGUI(hInstance);
    
    return 0;
}

void InitializeGUI(HINSTANCE hInstance) {
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MemoryAllocatorGUI";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    
    RegisterClass(&wc);
    
    // Create main window
    HWND hwnd = CreateWindowEx(
        0,
        "MemoryAllocatorGUI",
        "Dynamic Memory Allocator Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );
    
    if (hwnd == NULL) {
        MessageBox(NULL, "Failed to create window", "Error", MB_OK);
        return;
    }
    
    g_gui_state.hwnd = hwnd;
    
    // Initialize GDI objects
    g_gui_state.brush_free = CreateSolidBrush(COLOR_FREE);
    g_gui_state.brush_allocated = CreateSolidBrush(COLOR_ALLOCATED);
    g_gui_state.brush_header = CreateSolidBrush(COLOR_HEADER);
    g_gui_state.brush_background = CreateSolidBrush(COLOR_BG);
    g_gui_state.pen_border = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    
    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
            CreateControls(hwnd);
            break;
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // Clear background
            RECT rect;
            GetClientRect(hwnd, &rect);
            FillRect(hdc, &rect, g_gui_state.brush_background);
            
            // Draw memory visualization
            DrawMemoryVisualization(hdc, hwnd);
            
            EndPaint(hwnd, &ps);
            break;
        }
        
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BUTTON_MALLOC:
                    HandleMallocButton(hwnd);
                    break;
                case ID_BUTTON_FREE:
                    HandleFreeButton(hwnd);
                    break;
                case ID_BUTTON_REALLOC:
                    HandleReallocButton(hwnd);
                    break;
                case ID_BUTTON_CALLOC:
                    HandleCallocButton(hwnd);
                    break;
                case ID_BUTTON_CLEAR_ALL:
                    ClearAllPointers();
                    UpdatePointerListBox(hwnd);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case ID_BUTTON_STRESS_TEST:
                    HandleStressTest(hwnd);
                    break;
                case ID_BUTTON_MERGE_BLOCKS:
                    merge_free_blocks();
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
            }
            UpdateStatusBar(hwnd);
            break;
            
        case WM_DESTROY:
            // Cleanup
            DeleteObject(g_gui_state.brush_free);
            DeleteObject(g_gui_state.brush_allocated);
            DeleteObject(g_gui_state.brush_header);
            DeleteObject(g_gui_state.brush_background);
            DeleteObject(g_gui_state.pen_border);
            ClearAllPointers();
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

void CreateControls(HWND hwnd) {
    // Title
    CreateWindow("STATIC", "Dynamic Memory Allocator Visualizer",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 10, CANVAS_WIDTH, 30,
                hwnd, NULL, NULL, NULL);
    
    // Memory canvas frame
    CreateWindow("STATIC", "",
                WS_VISIBLE | WS_CHILD | SS_BLACKFRAME,
                CANVAS_X - 2, CANVAS_Y - 2, CANVAS_WIDTH + 4, CANVAS_HEIGHT + 4,
                hwnd, NULL, NULL, NULL);
    
    // Size input
    CreateWindow("STATIC", "Size (bytes):",
                WS_VISIBLE | WS_CHILD,
                50, 520, 100, 20,
                hwnd, NULL, NULL, NULL);
                
    g_gui_state.hEditSize = CreateWindow("EDIT", "64",
                                        WS_VISIBLE | WS_CHILD | WS_BORDER,
                                        150, 518, 80, 25,
                                        hwnd, (HMENU)ID_EDIT_SIZE, NULL, NULL);
    
    // Buttons
    CreateWindow("BUTTON", "malloc()",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                250, 515, 80, 30,
                hwnd, (HMENU)ID_BUTTON_MALLOC, NULL, NULL);
                
    CreateWindow("BUTTON", "free()",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                340, 515, 80, 30,
                hwnd, (HMENU)ID_BUTTON_FREE, NULL, NULL);
                
    CreateWindow("BUTTON", "realloc()",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                430, 515, 80, 30,
                hwnd, (HMENU)ID_BUTTON_REALLOC, NULL, NULL);
                
    CreateWindow("BUTTON", "calloc()",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                520, 515, 80, 30,
                hwnd, (HMENU)ID_BUTTON_CALLOC, NULL, NULL);
                
    CreateWindow("BUTTON", "Clear All",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                610, 515, 80, 30,
                hwnd, (HMENU)ID_BUTTON_CLEAR_ALL, NULL, NULL);
                
    CreateWindow("BUTTON", "Stress Test",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                700, 515, 80, 30,
                hwnd, (HMENU)ID_BUTTON_STRESS_TEST, NULL, NULL);
                
    CreateWindow("BUTTON", "Merge Blocks",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                790, 515, 90, 30,
                hwnd, (HMENU)ID_BUTTON_MERGE_BLOCKS, NULL, NULL);
    
    // Pointer list
    CreateWindow("STATIC", "Allocated Pointers:",
                WS_VISIBLE | WS_CHILD,
                50, 560, 150, 20,
                hwnd, NULL, NULL, NULL);
                
    g_gui_state.hListBoxPtrs = CreateWindow("LISTBOX", "",
                                           WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL,
                                           50, 580, 300, 120,
                                           hwnd, (HMENU)ID_LISTBOX_PTRS, NULL, NULL);
    
    // Status bar
    g_gui_state.hStatus = CreateWindow("STATIC", "Ready",
                                      WS_VISIBLE | WS_CHILD | SS_SUNKEN,
                                      400, 580, 500, 120,
                                      hwnd, NULL, NULL, NULL);
}

void DrawMemoryVisualization(HDC hdc, HWND hwnd) {
    // Draw memory map
    DrawMemoryMap(hdc);
    
    // Draw legend
    DrawLegend(hdc);
    
    // Draw heap statistics
    char stats[512];
    snprintf(stats, sizeof(stats), 
             "Heap: %dKB | Allocated: %luB | Free: %luB | Fragmentation: %d blocks",
             HEAP_SIZE / 1024, (unsigned long)get_total_allocated(), (unsigned long)get_total_free(), get_fragmentation_count());
             
    SetBkMode(hdc, TRANSPARENT);
    TextOut(hdc, CANVAS_X, CANVAS_Y + CANVAS_HEIGHT + 10, stats, strlen(stats));
}

void DrawMemoryMap(HDC hdc) {
    // Draw the memory as a horizontal bar
    int total_width = CANVAS_WIDTH - 100; // Leave space for labels
    int y = CANVAS_Y + 50;
    
    // Calculate scale factor
    double scale = (double)total_width / HEAP_SIZE;
    
    // Walk through the heap and draw blocks
    char* heap_start = heap;
    char* current_pos = heap_start;
    char* heap_end = heap_start + HEAP_SIZE;
    int x = CANVAS_X + 50;
    
    while (current_pos < heap_end) {
        block_header_t* block = (block_header_t*)current_pos;
        
        if ((char*)block + sizeof(block_header_t) > heap_end) {
            break;
        }
        
        // Calculate block dimensions
        int header_width = (int)(sizeof(block_header_t) * scale);
        int data_width = (int)(block->size * scale);
        
        if (header_width < 1) header_width = 1;
        if (data_width < 1) data_width = 1;
        
        // Draw header
        RECT header_rect = {x, y, x + header_width, y + BLOCK_HEIGHT};
        FillRect(hdc, &header_rect, g_gui_state.brush_header);
        
        // Draw data block
        RECT data_rect = {x + header_width, y, x + header_width + data_width, y + BLOCK_HEIGHT};
        if (block->is_free) {
            FillRect(hdc, &data_rect, g_gui_state.brush_free);
        } else {
            FillRect(hdc, &data_rect, g_gui_state.brush_allocated);
        }
        
        // Draw border
        HPEN oldPen = SelectObject(hdc, g_gui_state.pen_border);
        Rectangle(hdc, x, y, x + header_width + data_width, y + BLOCK_HEIGHT);
        
        // Draw size label if block is large enough
        if (data_width > 30) {
            char size_str[32];
            snprintf(size_str, sizeof(size_str), "%lu", (unsigned long)block->size);
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(0, 0, 0));
            TextOut(hdc, x + header_width + 2, y + 8, size_str, strlen(size_str));
        }
        
        SelectObject(hdc, oldPen);
        
        x += header_width + data_width;
        current_pos += sizeof(block_header_t) + block->size;
        
        if (current_pos >= heap_end) break;
    }
}

void DrawLegend(HDC hdc) {
    int legend_x = CANVAS_X + 50;
    int legend_y = CANVAS_Y + 150;
    int box_size = 20;
    
    // Free block legend
    RECT free_rect = {legend_x, legend_y, legend_x + box_size, legend_y + box_size};
    FillRect(hdc, &free_rect, g_gui_state.brush_free);
    Rectangle(hdc, legend_x, legend_y, legend_x + box_size, legend_y + box_size);
    TextOut(hdc, legend_x + 30, legend_y + 2, "Free Block", 10);
    
    // Allocated block legend
    legend_y += 30;
    RECT alloc_rect = {legend_x, legend_y, legend_x + box_size, legend_y + box_size};
    FillRect(hdc, &alloc_rect, g_gui_state.brush_allocated);
    Rectangle(hdc, legend_x, legend_y, legend_x + box_size, legend_y + box_size);
    TextOut(hdc, legend_x + 30, legend_y + 2, "Allocated Block", 15);
    
    // Header legend
    legend_y += 30;
    RECT header_rect = {legend_x, legend_y, legend_x + box_size, legend_y + box_size};
    FillRect(hdc, &header_rect, g_gui_state.brush_header);
    Rectangle(hdc, legend_x, legend_y, legend_x + box_size, legend_y + box_size);
    TextOut(hdc, legend_x + 30, legend_y + 2, "Block Header", 12);
}

void HandleMallocButton(HWND hwnd) {
    int size = GetSizeFromEdit(hwnd);
    if (size <= 0) {
        ShowError("Please enter a valid size");
        return;
    }
    
    void* ptr = my_malloc(size);
    if (ptr) {
        AddPointerToList(ptr, size, "malloc");
        UpdatePointerListBox(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        
        char msg[128];
        snprintf(msg, sizeof(msg), "Allocated %d bytes at %p", size, ptr);
        ShowInfo(msg);
    } else {
        ShowError("Memory allocation failed");
    }
}

void HandleFreeButton(HWND hwnd) {
    // Get selected pointer from listbox
    int sel = SendMessage(g_gui_state.hListBoxPtrs, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR) {
        ShowError("Please select a pointer to free");
        return;
    }
    
    char buffer[128];
    SendMessage(g_gui_state.hListBoxPtrs, LB_GETTEXT, sel, (LPARAM)buffer);
    
    // Extract pointer address from string
    void* ptr = NULL;
    sscanf(buffer, "%*s %*s %p", &ptr);
    
    if (ptr) {
        my_free(ptr);
        RemovePointerFromList(ptr);
        UpdatePointerListBox(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        
        char msg[128];
        snprintf(msg, sizeof(msg), "Freed memory at %p", ptr);
        ShowInfo(msg);
    }
}

void HandleReallocButton(HWND hwnd) {
    int new_size = GetSizeFromEdit(hwnd);
    if (new_size <= 0) {
        ShowError("Please enter a valid size");
        return;
    }
    
    // Get selected pointer from listbox
    int sel = SendMessage(g_gui_state.hListBoxPtrs, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR) {
        ShowError("Please select a pointer to realloc");
        return;
    }
    
    char buffer[128];
    SendMessage(g_gui_state.hListBoxPtrs, LB_GETTEXT, sel, (LPARAM)buffer);
    
    // Extract pointer address from string
    void* old_ptr = NULL;
    sscanf(buffer, "%*s %*s %p", &old_ptr);
    
    if (old_ptr) {
        void* new_ptr = my_realloc(old_ptr, new_size);
        if (new_ptr) {
            RemovePointerFromList(old_ptr);
            AddPointerToList(new_ptr, new_size, "realloc");
            UpdatePointerListBox(hwnd);
            InvalidateRect(hwnd, NULL, TRUE);
            
            char msg[128];
            snprintf(msg, sizeof(msg), "Reallocated %p to %p (%d bytes)", old_ptr, new_ptr, new_size);
            ShowInfo(msg);
        } else {
            ShowError("Reallocation failed");
        }
    }
}

void HandleCallocButton(HWND hwnd) {
    int size = GetSizeFromEdit(hwnd);
    if (size <= 0) {
        ShowError("Please enter a valid size");
        return;
    }
    
    void* ptr = my_calloc(1, size);
    if (ptr) {
        AddPointerToList(ptr, size, "calloc");
        UpdatePointerListBox(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        
        char msg[128];
        snprintf(msg, sizeof(msg), "Allocated %d zeroed bytes at %p", size, ptr);
        ShowInfo(msg);
    } else {
        ShowError("Memory allocation failed");
    }
}

void HandleStressTest(HWND hwnd) {
    ShowInfo("Running stress test...");
    
    // Allocate multiple blocks of different sizes
    for (int i = 0; i < 10; i++) {
        size_t size = 32 + (i * 16);
        void* ptr = my_malloc(size);
        if (ptr) {
            char label[32];
            snprintf(label, sizeof(label), "stress_%d", i);
            AddPointerToList(ptr, size, label);
        }
    }
    
    UpdatePointerListBox(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);
    ShowInfo("Stress test completed");
}

int GetSizeFromEdit(HWND hwnd) {
    char buffer[32];
    GetWindowText(g_gui_state.hEditSize, buffer, sizeof(buffer));
    return atoi(buffer);
}

void UpdateStatusBar(HWND hwnd) {
    char status[512];
    snprintf(status, sizeof(status),
             "Memory Status:\n"
             "Heap Size: %d bytes\n"
             "Allocated: %lu bytes\n"
             "Free: %lu bytes\n"
             "Fragmentation: %d blocks\n"
             "Total Pointers: %d",
             HEAP_SIZE, (unsigned long)get_total_allocated(), (unsigned long)get_total_free(), 
             get_fragmentation_count(), g_gui_state.next_id);
    
    SetWindowText(g_gui_state.hStatus, status);
}

void AddPointerToList(void* ptr, size_t size, const char* type) {
    ptr_info_t* info = malloc(sizeof(ptr_info_t));
    if (!info) return;
    
    info->ptr = ptr;
    info->size = size;
    snprintf(info->label, sizeof(info->label), "%s_%d", type, g_gui_state.next_id++);
    info->next = g_gui_state.ptr_list_head;
    g_gui_state.ptr_list_head = info;
}

void RemovePointerFromList(void* ptr) {
    ptr_info_t** current = &g_gui_state.ptr_list_head;
    while (*current) {
        if ((*current)->ptr == ptr) {
            ptr_info_t* to_remove = *current;
            *current = (*current)->next;
            free(to_remove);
            return;
        }
        current = &(*current)->next;
    }
}

void ClearAllPointers(void) {
    ptr_info_t* current = g_gui_state.ptr_list_head;
    while (current) {
        my_free(current->ptr);
        ptr_info_t* next = current->next;
        free(current);
        current = next;
    }
    g_gui_state.ptr_list_head = NULL;
    g_gui_state.next_id = 0;
}

void UpdatePointerListBox(HWND hwnd) {
    SendMessage(g_gui_state.hListBoxPtrs, LB_RESETCONTENT, 0, 0);
    
    ptr_info_t* current = g_gui_state.ptr_list_head;
    while (current) {
        char item[128];
        snprintf(item, sizeof(item), "%s (%lu bytes) %p", 
                current->label, (unsigned long)current->size, current->ptr);
        SendMessage(g_gui_state.hListBoxPtrs, LB_ADDSTRING, 0, (LPARAM)item);
        current = current->next;
    }
}

void ShowError(const char* message) {
    MessageBox(g_gui_state.hwnd, message, "Error", MB_OK | MB_ICONERROR);
}

void ShowInfo(const char* message) {
    MessageBox(g_gui_state.hwnd, message, "Information", MB_OK | MB_ICONINFORMATION);
}