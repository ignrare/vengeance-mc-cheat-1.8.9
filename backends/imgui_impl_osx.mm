

#import "imgui.h"
#import "imgui_impl_osx.h"
#import <Cocoa/Cocoa.h>
#import <mach/mach_time.h>
#import <Carbon/Carbon.h>
#import <GameController/GameController.h>

@class ImFocusObserver;
@class KeyEventResponder;

static double               g_HostClockPeriod = 0.0;
static double               g_Time = 0.0;
static NSCursor*            g_MouseCursors[ImGuiMouseCursor_COUNT] = {};
static bool                 g_MouseCursorHidden = false;
static ImFocusObserver*     g_FocusObserver = nil;
static KeyEventResponder*   g_KeyEventResponder = nil;
static NSTextInputContext*  g_InputContext = nil;

@interface NSCursor()
+ (id)_windowResizeNorthWestSouthEastCursor;
+ (id)_windowResizeNorthEastSouthWestCursor;
+ (id)_windowResizeNorthSouthCursor;
+ (id)_windowResizeEastWestCursor;
@end

static void InitHostClockPeriod()
{
    struct mach_timebase_info info;
    mach_timebase_info(&info);
    g_HostClockPeriod = 1e-9 * ((double)info.denom / (double)info.numer);
}

static double GetMachAbsoluteTimeInSeconds()
{
    return (double)mach_absolute_time() * g_HostClockPeriod;
}

@interface KeyEventResponder: NSView<NSTextInputClient>
@end

@implementation KeyEventResponder
{
    float _posX;
    float _posY;
    NSRect _imeRect;
}

#pragma mark - Public

- (void)setImePosX:(float)posX imePosY:(float)posY
{
    _posX = posX;
    _posY = posY;
}

- (void)updateImePosWithView:(NSView *)view
{
    NSWindow *window = view.window;
    if (!window)
        return;
    NSRect contentRect = [window contentRectForFrameRect:window.frame];
    NSRect rect = NSMakeRect(_posX, contentRect.size.height - _posY, 0, 0);
    _imeRect = [window convertRectToScreen:rect];
}

- (void)viewDidMoveToWindow
{

    [self.window makeFirstResponder:self];
}

- (void)keyDown:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);

    [self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent*)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)insertText:(id)aString replacementRange:(NSRange)replacementRange
{
    ImGuiIO& io = ImGui::GetIO();

    NSString* characters;
    if ([aString isKindOfClass:[NSAttributedString class]])
        characters = [aString string];
    else
        characters = (NSString*)aString;

    io.AddInputCharactersUTF8(characters.UTF8String);
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)doCommandBySelector:(SEL)myselector
{
}

- (nullable NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange
{
    return nil;
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point
{
    return 0;
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(nullable NSRangePointer)actualRange
{
    return _imeRect;
}

- (BOOL)hasMarkedText
{
    return NO;
}

- (NSRange)markedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (NSRange)selectedRange
{
    return NSMakeRange(NSNotFound, 0);
}

- (void)setMarkedText:(nonnull id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange
{
}

- (void)unmarkText
{
}

- (nonnull NSArray<NSAttributedStringKey>*)validAttributesForMarkedText
{
    return @[];
}

@end

@interface ImFocusObserver : NSObject

- (void)onApplicationBecomeActive:(NSNotification*)aNotification;
- (void)onApplicationBecomeInactive:(NSNotification*)aNotification;

@end

@implementation ImFocusObserver

- (void)onApplicationBecomeActive:(NSNotification*)aNotification
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(true);
}

- (void)onApplicationBecomeInactive:(NSNotification*)aNotification
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(false);
}

@end

static ImGuiKey ImGui_ImplOSX_KeyCodeToImGuiKey(int key_code)
{
    switch (key_code)
    {
        case kVK_ANSI_A: return ImGuiKey_A;
        case kVK_ANSI_S: return ImGuiKey_S;
        case kVK_ANSI_D: return ImGuiKey_D;
        case kVK_ANSI_F: return ImGuiKey_F;
        case kVK_ANSI_H: return ImGuiKey_H;
        case kVK_ANSI_G: return ImGuiKey_G;
        case kVK_ANSI_Z: return ImGuiKey_Z;
        case kVK_ANSI_X: return ImGuiKey_X;
        case kVK_ANSI_C: return ImGuiKey_C;
        case kVK_ANSI_V: return ImGuiKey_V;
        case kVK_ANSI_B: return ImGuiKey_B;
        case kVK_ANSI_Q: return ImGuiKey_Q;
        case kVK_ANSI_W: return ImGuiKey_W;
        case kVK_ANSI_E: return ImGuiKey_E;
        case kVK_ANSI_R: return ImGuiKey_R;
        case kVK_ANSI_Y: return ImGuiKey_Y;
        case kVK_ANSI_T: return ImGuiKey_T;
        case kVK_ANSI_1: return ImGuiKey_1;
        case kVK_ANSI_2: return ImGuiKey_2;
        case kVK_ANSI_3: return ImGuiKey_3;
        case kVK_ANSI_4: return ImGuiKey_4;
        case kVK_ANSI_6: return ImGuiKey_6;
        case kVK_ANSI_5: return ImGuiKey_5;
        case kVK_ANSI_Equal: return ImGuiKey_Equal;
        case kVK_ANSI_9: return ImGuiKey_9;
        case kVK_ANSI_7: return ImGuiKey_7;
        case kVK_ANSI_Minus: return ImGuiKey_Minus;
        case kVK_ANSI_8: return ImGuiKey_8;
        case kVK_ANSI_0: return ImGuiKey_0;
        case kVK_ANSI_RightBracket: return ImGuiKey_RightBracket;
        case kVK_ANSI_O: return ImGuiKey_O;
        case kVK_ANSI_U: return ImGuiKey_U;
        case kVK_ANSI_LeftBracket: return ImGuiKey_LeftBracket;
        case kVK_ANSI_I: return ImGuiKey_I;
        case kVK_ANSI_P: return ImGuiKey_P;
        case kVK_ANSI_L: return ImGuiKey_L;
        case kVK_ANSI_J: return ImGuiKey_J;
        case kVK_ANSI_Quote: return ImGuiKey_Apostrophe;
        case kVK_ANSI_K: return ImGuiKey_K;
        case kVK_ANSI_Semicolon: return ImGuiKey_Semicolon;
        case kVK_ANSI_Backslash: return ImGuiKey_Backslash;
        case kVK_ANSI_Comma: return ImGuiKey_Comma;
        case kVK_ANSI_Slash: return ImGuiKey_Slash;
        case kVK_ANSI_N: return ImGuiKey_N;
        case kVK_ANSI_M: return ImGuiKey_M;
        case kVK_ANSI_Period: return ImGuiKey_Period;
        case kVK_ANSI_Grave: return ImGuiKey_GraveAccent;
        case kVK_ANSI_KeypadDecimal: return ImGuiKey_KeypadDecimal;
        case kVK_ANSI_KeypadMultiply: return ImGuiKey_KeypadMultiply;
        case kVK_ANSI_KeypadPlus: return ImGuiKey_KeypadAdd;
        case kVK_ANSI_KeypadClear: return ImGuiKey_NumLock;
        case kVK_ANSI_KeypadDivide: return ImGuiKey_KeypadDivide;
        case kVK_ANSI_KeypadEnter: return ImGuiKey_KeypadEnter;
        case kVK_ANSI_KeypadMinus: return ImGuiKey_KeypadSubtract;
        case kVK_ANSI_KeypadEquals: return ImGuiKey_KeypadEqual;
        case kVK_ANSI_Keypad0: return ImGuiKey_Keypad0;
        case kVK_ANSI_Keypad1: return ImGuiKey_Keypad1;
        case kVK_ANSI_Keypad2: return ImGuiKey_Keypad2;
        case kVK_ANSI_Keypad3: return ImGuiKey_Keypad3;
        case kVK_ANSI_Keypad4: return ImGuiKey_Keypad4;
        case kVK_ANSI_Keypad5: return ImGuiKey_Keypad5;
        case kVK_ANSI_Keypad6: return ImGuiKey_Keypad6;
        case kVK_ANSI_Keypad7: return ImGuiKey_Keypad7;
        case kVK_ANSI_Keypad8: return ImGuiKey_Keypad8;
        case kVK_ANSI_Keypad9: return ImGuiKey_Keypad9;
        case kVK_Return: return ImGuiKey_Enter;
        case kVK_Tab: return ImGuiKey_Tab;
        case kVK_Space: return ImGuiKey_Space;
        case kVK_Delete: return ImGuiKey_Backspace;
        case kVK_Escape: return ImGuiKey_Escape;
        case kVK_CapsLock: return ImGuiKey_CapsLock;
        case kVK_Control: return ImGuiKey_LeftCtrl;
        case kVK_Shift: return ImGuiKey_LeftShift;
        case kVK_Option: return ImGuiKey_LeftAlt;
        case kVK_Command: return ImGuiKey_LeftSuper;
        case kVK_RightControl: return ImGuiKey_RightCtrl;
        case kVK_RightShift: return ImGuiKey_RightShift;
        case kVK_RightOption: return ImGuiKey_RightAlt;
        case kVK_RightCommand: return ImGuiKey_RightSuper;

        case kVK_F5: return ImGuiKey_F5;
        case kVK_F6: return ImGuiKey_F6;
        case kVK_F7: return ImGuiKey_F7;
        case kVK_F3: return ImGuiKey_F3;
        case kVK_F8: return ImGuiKey_F8;
        case kVK_F9: return ImGuiKey_F9;
        case kVK_F11: return ImGuiKey_F11;
        case kVK_F13: return ImGuiKey_PrintScreen;

        case kVK_F10: return ImGuiKey_F10;
        case 0x6E: return ImGuiKey_Menu;
        case kVK_F12: return ImGuiKey_F12;

        case kVK_Help: return ImGuiKey_Insert;
        case kVK_Home: return ImGuiKey_Home;
        case kVK_PageUp: return ImGuiKey_PageUp;
        case kVK_ForwardDelete: return ImGuiKey_Delete;
        case kVK_F4: return ImGuiKey_F4;
        case kVK_End: return ImGuiKey_End;
        case kVK_F2: return ImGuiKey_F2;
        case kVK_PageDown: return ImGuiKey_PageDown;
        case kVK_F1: return ImGuiKey_F1;
        case kVK_LeftArrow: return ImGuiKey_LeftArrow;
        case kVK_RightArrow: return ImGuiKey_RightArrow;
        case kVK_DownArrow: return ImGuiKey_DownArrow;
        case kVK_UpArrow: return ImGuiKey_UpArrow;
        default: return ImGuiKey_None;
    }
}

bool ImGui_ImplOSX_Init(NSView* view)
{
    ImGuiIO& io = ImGui::GetIO();

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    io.BackendPlatformName = "imgui_impl_osx";

    g_MouseCursorHidden = false;
    g_MouseCursors[ImGuiMouseCursor_Arrow] = [NSCursor arrowCursor];
    g_MouseCursors[ImGuiMouseCursor_TextInput] = [NSCursor IBeamCursor];
    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = [NSCursor closedHandCursor];
    g_MouseCursors[ImGuiMouseCursor_Hand] = [NSCursor pointingHandCursor];
    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = [NSCursor operationNotAllowedCursor];
    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = [NSCursor respondsToSelector:@selector(_windowResizeNorthSouthCursor)] ? [NSCursor _windowResizeNorthSouthCursor] : [NSCursor resizeUpDownCursor];
    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = [NSCursor respondsToSelector:@selector(_windowResizeEastWestCursor)] ? [NSCursor _windowResizeEastWestCursor] : [NSCursor resizeLeftRightCursor];
    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = [NSCursor respondsToSelector:@selector(_windowResizeNorthEastSouthWestCursor)] ? [NSCursor _windowResizeNorthEastSouthWestCursor] : [NSCursor closedHandCursor];
    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = [NSCursor respondsToSelector:@selector(_windowResizeNorthWestSouthEastCursor)] ? [NSCursor _windowResizeNorthWestSouthEastCursor] : [NSCursor closedHandCursor];

    io.SetClipboardTextFn = [](void*, const char* str) -> void
    {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        [pasteboard declareTypes:[NSArray arrayWithObject:NSPasteboardTypeString] owner:nil];
        [pasteboard setString:[NSString stringWithUTF8String:str] forType:NSPasteboardTypeString];
    };

    io.GetClipboardTextFn = [](void*) -> const char*
    {
        NSPasteboard* pasteboard = [NSPasteboard generalPasteboard];
        NSString* available = [pasteboard availableTypeFromArray: [NSArray arrayWithObject:NSPasteboardTypeString]];
        if (![available isEqualToString:NSPasteboardTypeString])
            return NULL;

        NSString* string = [pasteboard stringForType:NSPasteboardTypeString];
        if (string == nil)
            return NULL;

        const char* string_c = (const char*)[string UTF8String];
        size_t string_len = strlen(string_c);
        static ImVector<char> s_clipboard;
        s_clipboard.resize((int)string_len + 1);
        strcpy(s_clipboard.Data, string_c);
        return s_clipboard.Data;
    };

    g_FocusObserver = [[ImFocusObserver alloc] init];
    [[NSNotificationCenter defaultCenter] addObserver:g_FocusObserver
                                             selector:@selector(onApplicationBecomeActive:)
                                                 name:NSApplicationDidBecomeActiveNotification
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:g_FocusObserver
                                             selector:@selector(onApplicationBecomeInactive:)
                                                 name:NSApplicationDidResignActiveNotification
                                               object:nil];

    g_KeyEventResponder = [[KeyEventResponder alloc] initWithFrame:NSZeroRect];
    g_InputContext = [[NSTextInputContext alloc] initWithClient:g_KeyEventResponder];
    [view addSubview:g_KeyEventResponder];

    NSEventMask eventMask = NSEventMaskFlagsChanged;
    [NSEvent addLocalMonitorForEventsMatchingMask:eventMask handler:^NSEvent * _Nullable(NSEvent *event)
    {
        ImGui_ImplOSX_HandleEvent(event, g_KeyEventResponder);
        return event;
    }];

    io.SetPlatformImeDataFn = [](ImGuiViewport* viewport, ImGuiPlatformImeData* data) -> void
    {
        if (data->WantVisible)
        {
            [g_InputContext activate];
        }
        else
        {
            [g_InputContext discardMarkedText];
            [g_InputContext invalidateCharacterCoordinates];
            [g_InputContext deactivate];
        }
        [g_KeyEventResponder setImePosX:data->InputPos.x imePosY:data->InputPos.y + data->InputLineHeight];
    };

    return true;
}

void ImGui_ImplOSX_Shutdown()
{
    g_FocusObserver = NULL;
}

static void ImGui_ImplOSX_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
    {

        if (!g_MouseCursorHidden)
        {
            g_MouseCursorHidden = true;
            [NSCursor hide];
        }
    }
    else
    {
        NSCursor* desired = g_MouseCursors[imgui_cursor] ?: g_MouseCursors[ImGuiMouseCursor_Arrow];

        if (desired != NSCursor.currentCursor)
        {
            [desired set];
        }
        if (g_MouseCursorHidden)
        {
            g_MouseCursorHidden = false;
            [NSCursor unhide];
        }
    }
}

static void ImGui_ImplOSX_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;

    GCController* controller;
    if (@available(macOS 11.0, *))
        controller = GCController.current;
    else
        controller = GCController.controllers.firstObject;

    if (controller == nil || controller.extendedGamepad == nil)
    {
        io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
        return;
    }

    GCExtendedGamepad* gp = controller.extendedGamepad;

    #define IM_SATURATE(V)                        (V < 0.0f ? 0.0f : V > 1.0f ? 1.0f : V)
    #define MAP_BUTTON(KEY_NO, BUTTON_NAME)       { io.AddKeyEvent(KEY_NO, gp.BUTTON_NAME.isPressed); }
    #define MAP_ANALOG(KEY_NO, AXIS_NAME, V0, V1) { float vn = (float)(gp.AXIS_NAME.value - V0) / (float)(V1 - V0); vn = IM_SATURATE(vn); io.AddKeyAnalogEvent(KEY_NO, vn > 0.1f, vn); }
    const float thumb_dead_zone = 0.0f;
    if (@available(macOS 10.15, *))
    {
        MAP_BUTTON(ImGuiKey_GamepadStart,           buttonMenu);
        MAP_BUTTON(ImGuiKey_GamepadBack,            buttonOptions);
    }
    MAP_BUTTON(ImGuiKey_GamepadFaceDown,        buttonA);
    MAP_BUTTON(ImGuiKey_GamepadFaceRight,       buttonB);
    MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        buttonX);
    MAP_BUTTON(ImGuiKey_GamepadFaceUp,          buttonY);
    MAP_BUTTON(ImGuiKey_GamepadDpadLeft,        dpad.left);
    MAP_BUTTON(ImGuiKey_GamepadDpadRight,       dpad.right);
    MAP_BUTTON(ImGuiKey_GamepadDpadUp,          dpad.up);
    MAP_BUTTON(ImGuiKey_GamepadDpadDown,        dpad.down);
    MAP_ANALOG(ImGuiKey_GamepadL1,              leftShoulder, 0.0f, 1.0f);
    MAP_ANALOG(ImGuiKey_GamepadR1,              rightShoulder, 0.0f, 1.0f);
    MAP_ANALOG(ImGuiKey_GamepadL2,              leftTrigger,  0.0f, 1.0f);
    MAP_ANALOG(ImGuiKey_GamepadR2,              rightTrigger, 0.0f, 1.0f);
    if (@available(macOS 10.14.1, *))
    {
        MAP_BUTTON(ImGuiKey_GamepadL3,              leftThumbstickButton);
        MAP_BUTTON(ImGuiKey_GamepadR3,              rightThumbstickButton);
    }
    MAP_ANALOG(ImGuiKey_GamepadLStickLeft,      leftThumbstick.xAxis,  -thumb_dead_zone, -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickRight,     leftThumbstick.xAxis,  +thumb_dead_zone, +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickUp,        leftThumbstick.yAxis,  +thumb_dead_zone, +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadLStickDown,      leftThumbstick.yAxis,  -thumb_dead_zone, -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickLeft,      rightThumbstick.xAxis, -thumb_dead_zone, -1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickRight,     rightThumbstick.xAxis, +thumb_dead_zone, +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickUp,        rightThumbstick.yAxis, +thumb_dead_zone, +1.0f);
    MAP_ANALOG(ImGuiKey_GamepadRStickDown,      rightThumbstick.yAxis, -thumb_dead_zone, -1.0f);
    #undef MAP_BUTTON
    #undef MAP_ANALOG

    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
}

static void ImGui_ImplOSX_UpdateImePosWithView(NSView* view)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput)
        [g_KeyEventResponder updateImePosWithView:view];
}

void ImGui_ImplOSX_NewFrame(NSView* view)
{

    ImGuiIO& io = ImGui::GetIO();
    if (view)
    {
        const float dpi = (float)[view.window backingScaleFactor];
        io.DisplaySize = ImVec2((float)view.bounds.size.width, (float)view.bounds.size.height);
        io.DisplayFramebufferScale = ImVec2(dpi, dpi);
    }

    if (g_Time == 0.0)
    {
        InitHostClockPeriod();
        g_Time = GetMachAbsoluteTimeInSeconds();
    }
    double current_time = GetMachAbsoluteTimeInSeconds();
    io.DeltaTime = (float)(current_time - g_Time);
    g_Time = current_time;

    ImGui_ImplOSX_UpdateMouseCursor();
    ImGui_ImplOSX_UpdateGamepads();
    ImGui_ImplOSX_UpdateImePosWithView(view);
}

bool ImGui_ImplOSX_HandleEvent(NSEvent* event, NSView* view)
{
    ImGuiIO& io = ImGui::GetIO();

    if (event.type == NSEventTypeLeftMouseDown || event.type == NSEventTypeRightMouseDown || event.type == NSEventTypeOtherMouseDown)
    {
        int button = (int)[event buttonNumber];
        if (button >= 0 && button < ImGuiMouseButton_COUNT)
            io.AddMouseButtonEvent(button, true);
        return io.WantCaptureMouse;
    }

    if (event.type == NSEventTypeLeftMouseUp || event.type == NSEventTypeRightMouseUp || event.type == NSEventTypeOtherMouseUp)
    {
        int button = (int)[event buttonNumber];
        if (button >= 0 && button < ImGuiMouseButton_COUNT)
            io.AddMouseButtonEvent(button, false);
        return io.WantCaptureMouse;
    }

    if (event.type == NSEventTypeMouseMoved || event.type == NSEventTypeLeftMouseDragged || event.type == NSEventTypeRightMouseDragged || event.type == NSEventTypeOtherMouseDragged)
    {
        NSPoint mousePoint = event.locationInWindow;
        mousePoint = [view convertPoint:mousePoint fromView:nil];
        mousePoint = NSMakePoint(mousePoint.x, view.bounds.size.height - mousePoint.y);
        io.AddMousePosEvent((float)mousePoint.x, (float)mousePoint.y);
    }

    if (event.type == NSEventTypeScrollWheel)
    {

        if (event.phase == NSEventPhaseCancelled)
            return false;

        double wheel_dx = 0.0;
        double wheel_dy = 0.0;

        #if MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
        if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
        {
            wheel_dx = [event scrollingDeltaX];
            wheel_dy = [event scrollingDeltaY];
            if ([event hasPreciseScrollingDeltas])
            {
                wheel_dx *= 0.1;
                wheel_dy *= 0.1;
            }
        }
        else
        #endif
        {
            wheel_dx = [event deltaX];
            wheel_dy = [event deltaY];
        }
        if (wheel_dx != 0.0 || wheel_dx != 0.0)
            io.AddMouseWheelEvent((float)wheel_dx * 0.1f, (float)wheel_dy * 0.1f);

        return io.WantCaptureMouse;
    }

    if (event.type == NSEventTypeKeyDown || event.type == NSEventTypeKeyUp)
    {
        if ([event isARepeat])
            return io.WantCaptureKeyboard;

        int key_code = (int)[event keyCode];
        ImGuiKey key = ImGui_ImplOSX_KeyCodeToImGuiKey(key_code);
        io.AddKeyEvent(key, event.type == NSEventTypeKeyDown);
        io.SetKeyEventNativeData(key, key_code, -1);

        return io.WantCaptureKeyboard;
    }

    if (event.type == NSEventTypeFlagsChanged)
    {
        unsigned short key_code = [event keyCode];
        NSEventModifierFlags modifier_flags = [event modifierFlags];

        io.AddKeyEvent(ImGuiKey_ModShift, (modifier_flags & NSEventModifierFlagShift)   != 0);
        io.AddKeyEvent(ImGuiKey_ModCtrl,  (modifier_flags & NSEventModifierFlagControl) != 0);
        io.AddKeyEvent(ImGuiKey_ModAlt,   (modifier_flags & NSEventModifierFlagOption)  != 0);
        io.AddKeyEvent(ImGuiKey_ModSuper, (modifier_flags & NSEventModifierFlagCommand) != 0);

        ImGuiKey key = ImGui_ImplOSX_KeyCodeToImGuiKey(key_code);
        if (key != ImGuiKey_None)
        {

            NSEventModifierFlags mask = 0;
            switch (key)
            {
                case ImGuiKey_LeftCtrl:   mask = 0x0001; break;
                case ImGuiKey_RightCtrl:  mask = 0x2000; break;
                case ImGuiKey_LeftShift:  mask = 0x0002; break;
                case ImGuiKey_RightShift: mask = 0x0004; break;
                case ImGuiKey_LeftSuper:  mask = 0x0008; break;
                case ImGuiKey_RightSuper: mask = 0x0010; break;
                case ImGuiKey_LeftAlt:    mask = 0x0020; break;
                case ImGuiKey_RightAlt:   mask = 0x0040; break;
                default:
                    return io.WantCaptureKeyboard;
            }

            NSEventModifierFlags modifier_flags = [event modifierFlags];
            io.AddKeyEvent(key, (modifier_flags & mask) != 0);
            io.SetKeyEventNativeData(key, key_code, -1);
        }

        return io.WantCaptureKeyboard;
    }

    return false;
}
