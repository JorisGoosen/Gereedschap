// web/emscriptenWebBackend.cpp — Platform-laag voor Emscripten/WebGPU
// Dit bestand vervangt GLFW+surface-creatie door browser-native WebGPU.
// Op Emscripten wordt WebGPU bediend via library_webgpu.js (JS-binding).

#ifdef __EMSCRIPTEN__

#include "weergaveScherm.h"
#include <emscripten/html5.h>
#include <emscripten.h>
#include <string.h>
#include <iostream>

// ── Globale pointers voor callbacks ────────────────────────────────────────

static weergaveScherm* _s_scherm = nullptr;

// ── Canvas resize callback ─────────────────────────────────────────────────

static EM_BOOL _canvasResizeCallback(int eventType, const EmscriptenUiEvent* ev, void* userData)
{
	(void)eventType; (void)userData;
	int w = 0, h = 0;
	emscripten_get_canvas_element_size("mars-canvas", &w, &h);
	if(w > 0 && h > 0 && _s_scherm)
		_s_scherm->zetCanvasGrootte((uint32_t)w, (uint32_t)h);
	return EM_TRUE;
}

// ── Muis/wiel/tekst (naar de generieke weergaveScherm-verwerkers) ────────────

static EM_BOOL _muisMoveCallback(int eventType, const EmscriptenMouseEvent* ev, void* userData)
{
	(void)eventType; (void)userData;
	weergaveScherm::muisPosCentraal(nullptr, ev->clientX, ev->clientY);
	return EM_TRUE;
}

static EM_BOOL _muisKnopCallback(int eventType, const EmscriptenMouseEvent* ev, void* userData)
{
	(void)userData;
	//Zet Emscripten-knop (0=links,1=midden,2=rechts) om naar GLFW-conventie
	//(0=links,1=rechts,2=midden) zodat ImGui hem direct kan gebruiken.
	int knop = -1;
	if(ev->button == 0)      knop = 0;
	else if(ev->button == 2) knop = 1;
	else if(ev->button == 1) knop = 2;
	if(knop >= 0)
	{
		int actie = (eventType == EMSCRIPTEN_EVENT_MOUSEUP) ? GLFW_RELEASE : GLFW_PRESS;
		weergaveScherm::muisKnopCentraal(nullptr, knop, actie, 0);
	}
	return EM_TRUE;
}

static EM_BOOL _muisDownCallback(int eventType, const EmscriptenMouseEvent* ev, void* userData)
{
	return _muisKnopCallback(EMSCRIPTEN_EVENT_MOUSEDOWN, ev, userData);
}
static EM_BOOL _muisUpCallback(int eventType, const EmscriptenMouseEvent* ev, void* userData)
{
	return _muisKnopCallback(EMSCRIPTEN_EVENT_MOUSEUP, ev, userData);
}

static EM_BOOL _wielCallback(int eventType, const EmscriptenWheelEvent* ev, void* userData)
{
	(void)eventType; (void)userData;
	weergaveScherm::muisWielCentraal(nullptr, ev->deltaX, ev->deltaY);
	return EM_TRUE;
}

//Tekstinvoer (cijfers/letters voor bijv. het diepte-veld)
static EM_BOOL _charCallback(int eventType, const EmscriptenKeyboardEvent* ev, void* userData)
{
	(void)eventType; (void)userData;
	if(ev->charValue[0])
		weergaveScherm::charCentraal(nullptr, (unsigned int)(unsigned char)ev->charValue[0]);
	return EM_TRUE;
}

// ── Keyboard: DOM key → GLFW key mapping ───────────────────────────────────

static int glfwKeyFromDOMKey(const char* domKey)
{
	if(!domKey || !domKey[0]) return GLFW_KEY_UNKNOWN;
	
	if(strcmp(domKey, " ") == 0) return GLFW_KEY_SPACE;
	if(strcmp(domKey, "a") == 0 || strcmp(domKey, "A") == 0) return GLFW_KEY_A;
	if(strcmp(domKey, "b") == 0 || strcmp(domKey, "B") == 0) return GLFW_KEY_B;
	if(strcmp(domKey, "c") == 0 || strcmp(domKey, "C") == 0) return GLFW_KEY_C;
	if(strcmp(domKey, "d") == 0 || strcmp(domKey, "D") == 0) return GLFW_KEY_D;
	if(strcmp(domKey, "e") == 0 || strcmp(domKey, "E") == 0) return GLFW_KEY_E;
	if(strcmp(domKey, "f") == 0 || strcmp(domKey, "F") == 0) return GLFW_KEY_F;
	if(strcmp(domKey, "g") == 0 || strcmp(domKey, "G") == 0) return GLFW_KEY_G;
	if(strcmp(domKey, "h") == 0 || strcmp(domKey, "H") == 0) return GLFW_KEY_H;
	if(strcmp(domKey, "i") == 0 || strcmp(domKey, "I") == 0) return GLFW_KEY_I;
	if(strcmp(domKey, "j") == 0 || strcmp(domKey, "J") == 0) return GLFW_KEY_J;
	if(strcmp(domKey, "k") == 0 || strcmp(domKey, "K") == 0) return GLFW_KEY_K;
	if(strcmp(domKey, "l") == 0 || strcmp(domKey, "L") == 0) return GLFW_KEY_L;
	if(strcmp(domKey, "m") == 0 || strcmp(domKey, "M") == 0) return GLFW_KEY_M;
	if(strcmp(domKey, "n") == 0 || strcmp(domKey, "N") == 0) return GLFW_KEY_N;
	if(strcmp(domKey, "o") == 0 || strcmp(domKey, "O") == 0) return GLFW_KEY_O;
	if(strcmp(domKey, "p") == 0 || strcmp(domKey, "P") == 0) return GLFW_KEY_P;
	if(strcmp(domKey, "q") == 0 || strcmp(domKey, "Q") == 0) return GLFW_KEY_Q;
	if(strcmp(domKey, "r") == 0 || strcmp(domKey, "R") == 0) return GLFW_KEY_R;
	if(strcmp(domKey, "s") == 0 || strcmp(domKey, "S") == 0) return GLFW_KEY_S;
	if(strcmp(domKey, "t") == 0 || strcmp(domKey, "T") == 0) return GLFW_KEY_T;
	if(strcmp(domKey, "u") == 0 || strcmp(domKey, "U") == 0) return GLFW_KEY_U;
	if(strcmp(domKey, "v") == 0 || strcmp(domKey, "V") == 0) return GLFW_KEY_V;
	if(strcmp(domKey, "w") == 0 || strcmp(domKey, "W") == 0) return GLFW_KEY_W;
	if(strcmp(domKey, "x") == 0 || strcmp(domKey, "X") == 0) return GLFW_KEY_X;
	if(strcmp(domKey, "y") == 0 || strcmp(domKey, "Y") == 0) return GLFW_KEY_Y;
	if(strcmp(domKey, "z") == 0 || strcmp(domKey, "Z") == 0) return GLFW_KEY_Z;
	if(strcmp(domKey, "0") == 0) return GLFW_KEY_0;
	if(strcmp(domKey, "1") == 0) return GLFW_KEY_1;
	if(strcmp(domKey, "2") == 0) return GLFW_KEY_2;
	if(strcmp(domKey, "3") == 0) return GLFW_KEY_3;
	if(strcmp(domKey, "4") == 0) return GLFW_KEY_4;
	if(strcmp(domKey, "5") == 0) return GLFW_KEY_5;
	if(strcmp(domKey, "6") == 0) return GLFW_KEY_6;
	if(strcmp(domKey, "7") == 0) return GLFW_KEY_7;
	if(strcmp(domKey, "8") == 0) return GLFW_KEY_8;
	if(strcmp(domKey, "9") == 0) return GLFW_KEY_9;
	if(strcmp(domKey, "Enter") == 0) return GLFW_KEY_ENTER;
	if(strcmp(domKey, "Escape") == 0) return GLFW_KEY_ESCAPE;
	if(strcmp(domKey, "ArrowUp") == 0) return GLFW_KEY_UP;
	if(strcmp(domKey, "ArrowDown") == 0) return GLFW_KEY_DOWN;
	if(strcmp(domKey, "ArrowLeft") == 0) return GLFW_KEY_LEFT;
	if(strcmp(domKey, "ArrowRight") == 0) return GLFW_KEY_RIGHT;
	if(strcmp(domKey, ";") == 0) return GLFW_KEY_SEMICOLON;
	if(strcmp(domKey, "'") == 0) return GLFW_KEY_APOSTROPHE;
	if(strcmp(domKey, "[") == 0) return GLFW_KEY_LEFT_BRACKET;
	if(strcmp(domKey, "]") == 0) return GLFW_KEY_RIGHT_BRACKET;
	if(strcmp(domKey, ".") == 0) return GLFW_KEY_PERIOD;
	if(strcmp(domKey, "/") == 0) return GLFW_KEY_SLASH;
	
	return GLFW_KEY_UNKNOWN;
}

static int glfwModsFromEmscripten(const EmscriptenKeyboardEvent* ev)
{
	int mods = 0;
	if(ev->shiftKey) mods |= GLFW_MOD_SHIFT;
	if(ev->ctrlKey) mods |= GLFW_MOD_CONTROL;
	if(ev->altKey) mods |= GLFW_MOD_ALT;
	if(ev->metaKey) mods |= GLFW_MOD_SUPER;
	return mods;
}

	static EM_BOOL _keyDownCallback(int eventType, const EmscriptenKeyboardEvent* ev, void* userData)
	{
		(void)eventType; (void)userData;
		
		int key = glfwKeyFromDOMKey(ev->key);
		int mods = glfwModsFromEmscripten(ev);
		
		if(_s_scherm && key != GLFW_KEY_UNKNOWN)
			weergaveScherm::verwerkToets(_s_scherm, key, 0, GLFW_PRESS, mods);
		
		return EM_TRUE;
	}

// ── Externe functie uit mars.cpp/Simulatie ─────────────────────────────────

extern "C" {
	void _schermStap(); //Frame-functie (wordt gedefinieerd in mars.cpp)
}

// ── Main-loop wrapper (geen params voor emscripten_set_main_loop) ──────────

static void emscriptenMainLoopWrapper()
{
	//_schermStap() heeft eigen null-check; _s_scherm is optioneel
	_schermStap();
}

// ── Init web-platform (aanroep vanuit JS) ──────────────────────────────────

extern "C" {
EMSCRIPTEN_KEEPALIVE
void initWebPlatform(weergaveScherm* scherm, const char* /*canvasId*/, int /*width*/, int /*height*/)
{
	_s_scherm = scherm;
	
	//Koppel event handlers aan document/window/canvas.
	//Let op: gebruik EMSCRIPTEN_EVENT_TARGET_DOCUMENT i.p.v. "#document" — sinds
	//de querySelector-gebaseerde target-resolutie lost "#document" niet meer op
	//(geen element met id="document") en blijft de listener dode code.
	emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, false, _canvasResizeCallback);
	emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, _keyDownCallback);
	emscripten_set_keypress_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, true, _charCallback);
	emscripten_set_mousemove_callback("#mars-canvas", nullptr, false, _muisMoveCallback);
	emscripten_set_mousedown_callback("#mars-canvas", nullptr, true, _muisDownCallback);
	emscripten_set_mouseup_callback("#mars-canvas", nullptr, true, _muisUpCallback);
	emscripten_set_wheel_callback("#mars-canvas", nullptr, true, _wielCallback);
	
	//Start de requestAnimationFrame-loop
	emscripten_set_main_loop(emscriptenMainLoopWrapper, 0, 1);
}
}

#endif // __EMSCRIPTEN__
