// OSX forward-back button enabler
// maps extra buttons on mouse to Command-[ and Command-]
//
// THIS WILL NOT WORK ON EVERY KEYBOARD LAYOUT. CHECK YOUR KEYCODES IF NOT US QWERTY
//
// Garrett Mace Feb 2016
// Heavily modified from altermouse.c on http://osxbook.com
//
// Scroll wheel event tap adapted from DiscreteScroll by Emre Yolcu
//
// Compile with: gcc -Wall -o mtExtraButtons mtExtraButtons.c -framework ApplicationServices

#include <ApplicationServices/ApplicationServices.h>

const int LINES = 5;

CGEventRef myCGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon) {

	if (type == kCGEventOtherMouseDown){	// mouse buttons detected
		
		// determine which mouse button was pressed
		int mouseButton = CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);

		// left and right buttons are 0 and 1, middle is 2
		// middle works as usual in most apps but is still an OtherMouseDown event
		// only trigger on buttons 3 and 4
		if ((type == kCGEventOtherMouseDown) && (mouseButton == 3 || mouseButton == 4)) {

			CGEventRef keyPressEvent = NULL;

			if (mouseButton == 3) {
				
				// send Command-[ key down
				keyPressEvent = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)0x21, true);
				CGEventSetFlags(keyPressEvent,kCGEventFlagMaskCommand);
				CGEventPost(kCGHIDEventTap, keyPressEvent);
				
				// send Command-[ key up
				keyPressEvent = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)0x21, false);
				CGEventSetFlags(keyPressEvent,kCGEventFlagMaskCommand);
				CGEventPost(kCGHIDEventTap, keyPressEvent);

			} else if (mouseButton == 4) {
				
				// send Command-] key down
				keyPressEvent = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)0x1E, true);
				CGEventSetFlags(keyPressEvent,kCGEventFlagMaskCommand);
				CGEventPost(kCGHIDEventTap, keyPressEvent);
				
				// send Command-] key up
				keyPressEvent = CGEventCreateKeyboardEvent (NULL, (CGKeyCode)0x1E, false);
				CGEventSetFlags(keyPressEvent,kCGEventFlagMaskCommand);
				CGEventPost(kCGHIDEventTap, keyPressEvent);
				
			}
			
			CFRelease(keyPressEvent);

			// if we return the event, it'll be passed through as usual to the UI
			// extra button clicks still register as clicks on OSX, causing problems
			// so we return NULL to eat these events
			return NULL;
		}
		
	} else if (type == kCGEventScrollWheel) {	// scroll wheel detected
		
		// The external scroll wheel seems to get pulled into the OS as a "pixel" per wheel click rather than a line. Yet when using
		// an external wheel, the UI won't scroll with only one pixel per click. I have taken the pixel value and piped it to the line
		// value with a max clamp. Each slow click will deliver one line scroll, and faster clicks will do a little acceleration up to
		// the maximum value. It's not like acceleration was bad, just sucks to have zero movement on the first few scroll clicks.
		if (!CGEventGetIntegerValueField(event, kCGScrollWheelEventIsContinuous)) {	// ignore continuous-scroll devices like the touchpad
			int64_t delta = CGEventGetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis1);	// signed scroll wheel motion
			fprintf(stderr, "Scroll delta %d", (int)delta);
			if (delta > LINES) delta = LINES; if (delta < -LINES) delta = -LINES;	// clamp to fixed +/- increment
			CGEventSetIntegerValueField(event,kCGScrollWheelEventDeltaAxis1, delta);	// edit the event fields
			return event;	// pass the modified event through to the UI
		}
		
	}
    	
	// if we detect other button presses just pass them through
	return event;
		
}

int main(void)
{
    CFMachPortRef      eventTap;
    CGEventMask        eventMask;
    CFRunLoopSourceRef runLoopSource;

    // Create an event tap.
    eventMask = (1 << kCGEventOtherMouseDown) | (1 << kCGEventScrollWheel);
    eventTap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, eventMask, myCGEventCallback, NULL);
                   
    if (!eventTap) {
        fprintf(stderr, "Failed to create event tap\n");
        exit(1);
    }

    // Create a run loop source
    runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);

    // Add to the current run loop
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);

    // Enable the event tap
    CGEventTapEnable(eventTap, true);

    // Start the run loop
    CFRunLoopRun();

    // Attempt some cleanup
    CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopCommonModes);
    CFRelease(runLoopSource);
    exit(0);
}
