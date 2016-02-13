// OSX forward-back button enabler
// maps extra buttons on mouse to Command-[ and Command-]
//
// THIS WILL NOT WORK ON EVERY KEYBOARD LAYOUT. CHECK YOUR KEYCODES IF NOT US QWERTY
//
// Garrett Mace Feb 2016
// Heavily modified from altermouse.c on http://osxbook.com
//
// Compile with: gcc -Wall -o mtExtraButtons mtExtraButtons.c -framework ApplicationServices

#include <ApplicationServices/ApplicationServices.h>

CGEventRef myCGEventCallback(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
{

	// Pass the event through if it's somehow not what we're looking for
	if (type != kCGEventOtherMouseDown) return event;

	// determine which mouse button was pressed    
	int mouseButton = CGEventGetIntegerValueField(event, kCGMouseEventButtonNumber);
    
	// left and right buttons are 0 and 1, middle is 2
	// middle works as usual in most apps but is still an OtherMouseDown event
	// only trigger on buttons 3 and 4
	if (mouseButton == 3 || mouseButton == 4) {
    
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
        
    } else {
    	
    	// if we detect other button presses just pass them through
    	return event;
    
    }
}

int main(void)
{
	CFMachPortRef      eventTap;
	CGEventMask        eventMask;
	CFRunLoopSourceRef runLoopSource;
	
	// Create an event tap.
	eventMask = (1 << kCGEventOtherMouseDown);
	eventTap = CGEventTapCreate(kCGHIDEventTap, kCGHeadInsertEventTap, 0, eventMask, myCGEventCallback, NULL);
	           
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
