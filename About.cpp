#include "precomp.h"
#include "../../bfc/std.h"
#include "About.H"

const char *AboutWnd::getWindowTypeName() { return "About Wasabi-IRC"; }

GUID AboutWnd::getWindowTypeGuid() {
	return nsGUID::fromChar("{8DDA9D48-B915-4320-A888-831A1D837516}");
}


AboutWnd::AboutWnd() {
	setName(getWindowTypeName());
}
