#include "pch-il2cpp.h"
#include "_events.h"
#include "utility.h"
#include "il2cpp-helpers.h"
using namespace app;

WalkEvent::WalkEvent(const EVENT_PLAYER& source, const Vector2& position) : EventInterface(source, EVENT_TYPES::EVENT_WALK) {
	this->position = position;
}

void WalkEvent::Output() {
	// not needed
}

void WalkEvent::ColoredEventOutput() {
	// not needed
}
