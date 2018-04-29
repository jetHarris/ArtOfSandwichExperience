#ifndef _Input_HG_
#define _Input_HG_

#include "globalGameStuff.h"
#include "globalOpenGL_GLFW.h"
extern GLFWwindow* g_pGLFWWindow;

namespace Input
{
	namespace IsKeyPressed
	{
		inline bool __InternalIsKeyPressed(int key) { return glfwGetKey(g_pGLFWWindow, key) == GLFW_PRESS; }

		// Arrow Keys
		inline bool Left() { return __InternalIsKeyPressed(GLFW_KEY_LEFT); }
		inline bool Right() { return __InternalIsKeyPressed(GLFW_KEY_RIGHT); }
		inline bool Up() { return __InternalIsKeyPressed(GLFW_KEY_UP); }
		inline bool Down() { return __InternalIsKeyPressed(GLFW_KEY_DOWN); }

		// Letters
		inline bool A() { return __InternalIsKeyPressed(GLFW_KEY_A); }
		inline bool B() { return __InternalIsKeyPressed(GLFW_KEY_B); }
		inline bool C() { return __InternalIsKeyPressed(GLFW_KEY_C); }
		inline bool D() { return __InternalIsKeyPressed(GLFW_KEY_D); }
		inline bool E() { return __InternalIsKeyPressed(GLFW_KEY_E); }
		inline bool F() { return __InternalIsKeyPressed(GLFW_KEY_F); }
		inline bool G() { return __InternalIsKeyPressed(GLFW_KEY_G); }
		inline bool H() { return __InternalIsKeyPressed(GLFW_KEY_H); }
		inline bool I() { return __InternalIsKeyPressed(GLFW_KEY_I); }
		inline bool J() { return __InternalIsKeyPressed(GLFW_KEY_J); }
		inline bool K() { return __InternalIsKeyPressed(GLFW_KEY_K); }
		inline bool L() { return __InternalIsKeyPressed(GLFW_KEY_L); }
		inline bool M() { return __InternalIsKeyPressed(GLFW_KEY_M); }
		inline bool N() { return __InternalIsKeyPressed(GLFW_KEY_N); }
		inline bool O() { return __InternalIsKeyPressed(GLFW_KEY_O); }
		inline bool P() { return __InternalIsKeyPressed(GLFW_KEY_P); }
		inline bool Q() { return __InternalIsKeyPressed(GLFW_KEY_Q); }
		inline bool R() { return __InternalIsKeyPressed(GLFW_KEY_R); }
		inline bool S() { return __InternalIsKeyPressed(GLFW_KEY_S); }
		inline bool T() { return __InternalIsKeyPressed(GLFW_KEY_T); }
		inline bool U() { return __InternalIsKeyPressed(GLFW_KEY_U); }
		inline bool V() { return __InternalIsKeyPressed(GLFW_KEY_V); }
		inline bool W() { return __InternalIsKeyPressed(GLFW_KEY_W); }
		inline bool X() { return __InternalIsKeyPressed(GLFW_KEY_X); }
		inline bool Y() { return __InternalIsKeyPressed(GLFW_KEY_Y); }
		inline bool Z() { return __InternalIsKeyPressed(GLFW_KEY_Z); }
		// Numbers
		inline bool Num0() { return __InternalIsKeyPressed(GLFW_KEY_0); }
		inline bool Num1() { return __InternalIsKeyPressed(GLFW_KEY_1); }
		inline bool Num2() { return __InternalIsKeyPressed(GLFW_KEY_2); }
		inline bool Num3() { return __InternalIsKeyPressed(GLFW_KEY_3); }
		inline bool Num4() { return __InternalIsKeyPressed(GLFW_KEY_4); }
		inline bool Num5() { return __InternalIsKeyPressed(GLFW_KEY_5); }
		inline bool Num6() { return __InternalIsKeyPressed(GLFW_KEY_6); }
		inline bool Num7() { return __InternalIsKeyPressed(GLFW_KEY_7); }
		inline bool Num8() { return __InternalIsKeyPressed(GLFW_KEY_8); }
		inline bool Num9() { return __InternalIsKeyPressed(GLFW_KEY_9); }
		// Functions
		inline bool F1() { return __InternalIsKeyPressed(GLFW_KEY_F1); }
		inline bool F2() { return __InternalIsKeyPressed(GLFW_KEY_F2); }
		inline bool F3() { return __InternalIsKeyPressed(GLFW_KEY_F3); }
		inline bool F4() { return __InternalIsKeyPressed(GLFW_KEY_F4); }
		inline bool F5() { return __InternalIsKeyPressed(GLFW_KEY_F5); }
		inline bool F6() { return __InternalIsKeyPressed(GLFW_KEY_F6); }
		inline bool F7() { return __InternalIsKeyPressed(GLFW_KEY_F7); }
		inline bool F8() { return __InternalIsKeyPressed(GLFW_KEY_F8); }
		inline bool F9() { return __InternalIsKeyPressed(GLFW_KEY_F9); }
		inline bool F10() { return __InternalIsKeyPressed(GLFW_KEY_F10); }
		inline bool F11() { return __InternalIsKeyPressed(GLFW_KEY_F11); }
		inline bool F12() { return __InternalIsKeyPressed(GLFW_KEY_F12); }

		// Other Characters
		inline bool Space() { return __InternalIsKeyPressed(GLFW_KEY_SPACE); }
		inline bool Enter() { return __InternalIsKeyPressed(GLFW_KEY_ENTER); }
		inline bool Escape() { return __InternalIsKeyPressed(GLFW_KEY_ESCAPE); }
		inline bool Tab() { return __InternalIsKeyPressed(GLFW_KEY_TAB); }
		inline bool Plus() { return __InternalIsKeyPressed(GLFW_KEY_EQUAL); }
		inline bool Minus() { return __InternalIsKeyPressed(GLFW_KEY_MINUS); }
		inline bool LessThan() { return __InternalIsKeyPressed(GLFW_KEY_COMMA); }
		inline bool GreaterThan() { return __InternalIsKeyPressed(GLFW_KEY_PERIOD); }
		inline bool Tildy() { return __InternalIsKeyPressed(GLFW_KEY_GRAVE_ACCENT); }
		inline bool Slash() { return __InternalIsKeyPressed(GLFW_KEY_SLASH); }
		inline bool BackSlash() { return __InternalIsKeyPressed(GLFW_KEY_BACKSLASH); }
		inline bool SemiColon() { return __InternalIsKeyPressed(GLFW_KEY_SEMICOLON); }
		inline bool Quote() { return __InternalIsKeyPressed(GLFW_KEY_APOSTROPHE); }
		inline bool LeftBrace() { return __InternalIsKeyPressed(GLFW_KEY_LEFT_BRACKET); }
		inline bool RightBrace() { return __InternalIsKeyPressed(GLFW_KEY_RIGHT_BRACKET); }
		inline bool LeftCtrl() { return __InternalIsKeyPressed(GLFW_KEY_LEFT_CONTROL); }
		inline bool RightCtrl() { return __InternalIsKeyPressed(GLFW_KEY_RIGHT_CONTROL); }
		inline bool LeftAlt() { return __InternalIsKeyPressed(GLFW_KEY_LEFT_ALT); }
		inline bool RightAlt() { return __InternalIsKeyPressed(GLFW_KEY_RIGHT_ALT); }
		inline bool LeftShift() { return __InternalIsKeyPressed(GLFW_KEY_LEFT_SHIFT); }
		inline bool RightShift() { return __InternalIsKeyPressed(GLFW_KEY_RIGHT_SHIFT); }
		inline bool Insert() { return __InternalIsKeyPressed(GLFW_KEY_INSERT); }
		inline bool Delete() { return __InternalIsKeyPressed(GLFW_KEY_DELETE); }
		inline bool PageUp() { return __InternalIsKeyPressed(GLFW_KEY_PAGE_UP); }
		inline bool PageDown() { return __InternalIsKeyPressed(GLFW_KEY_PAGE_DOWN); }
		inline bool Home() { return __InternalIsKeyPressed(GLFW_KEY_HOME); }
		inline bool End() { return __InternalIsKeyPressed(GLFW_KEY_END); }
		inline bool CapsLock() { return __InternalIsKeyPressed(GLFW_KEY_CAPS_LOCK); }
		inline bool ScrollLock() { return __InternalIsKeyPressed(GLFW_KEY_SCROLL_LOCK); }
		inline bool NumLock() { return __InternalIsKeyPressed(GLFW_KEY_NUM_LOCK); }
		inline bool PrintScreen() { return __InternalIsKeyPressed(GLFW_KEY_PRINT_SCREEN); }
		inline bool Pause() { return __InternalIsKeyPressed(GLFW_KEY_PAUSE); }
	} // end namespace - IsKeyPressed
} // end namespace - Input

#endif

