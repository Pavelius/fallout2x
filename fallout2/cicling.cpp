static unsigned char Slime[] = {0, 108, 0, 11, 115, 7, 27, 123, 15, 43, 131, 27}; // Slime
static unsigned char Monitors[] = {107, 107, 111, 99, 103, 127, 87, 107, 143, 0, 147, 163, 107, 187, 255}; // Monitors
static unsigned char FireSlow[] = {255, 0, 0, 215, 0, 0, 147, 43, 11, 255, 119, 0, 255, 59, 0}; // Slow fire
static unsigned char FireFast[] = {71, 0, 0, 123, 0, 0, 179, 0, 0, 123, 0, 0, 71, 0, 0}; // Fast fire
static unsigned char Shoreline[] = {83, 63, 43, 75, 59, 43, 67, 55, 39, 63, 51, 39, 55, 47, 35, 51, 43, 35}; // Shoreline
static unsigned char BlinkingRed = 252; // Alarm

// Current parameters of cycle
static unsigned SlimeCurrent = 0;
static unsigned MonitorsCurrent = 0;
static unsigned FireSlowCurrent = 0;
static unsigned FireFastCurrent = 0;
static unsigned ShorelineCurrent = 0;
static unsigned BlinkingRedCurrent = 0;

// Time of Last changinge
static unsigned dwLastCycleSlow = 0;
static unsigned dwLastCycleMedium = 0;
static unsigned dwLastCycleFast = 0;
static unsigned dwLastCycleVeryFast = 0;

// Current speed factor
const int dwCycleSpeedFactor = 2;

void apply_pallette_cicle(unsigned char* pal, unsigned dwCurrentTime) {

	if(dwCurrentTime - dwLastCycleSlow >= 200 * dwCycleSpeedFactor) {
		// Slime
		unsigned dwSlimeCurrentWork = SlimeCurrent;

		for(int i = 3; i >= 0; i--) {
			pal[229 * 4 + i * 4 + 0] = Slime[dwSlimeCurrentWork * 3]; // Red  
			pal[229 * 4 + i * 4 + 1] = Slime[dwSlimeCurrentWork * 3 + 1]; // Green
			pal[229 * 4 + i * 4 + 2] = Slime[dwSlimeCurrentWork * 3 + 2]; // Blue

			if(dwSlimeCurrentWork == 3)
				dwSlimeCurrentWork = 0;
			else
				dwSlimeCurrentWork++;
		}

		if(SlimeCurrent == 3)
			SlimeCurrent = 0;
		else
			SlimeCurrent++;

		// Shoreline
		unsigned ShorelineCurrentWork = ShorelineCurrent;

		for(int i = 5; i >= 0; i--) {
			pal[(744 / 3) * 4 + i * 4] = Shoreline[ShorelineCurrentWork * 3];               // Red  
			pal[(744 / 3) * 4 + i * 4 + 1] = Shoreline[ShorelineCurrentWork * 3 + 1];       // Green
			pal[(744 / 3) * 4 + i * 4 + 2] = Shoreline[ShorelineCurrentWork * 3 + 2];       // Blue 

			if(ShorelineCurrentWork == 5)
				ShorelineCurrentWork = 0;
			else
				ShorelineCurrentWork++;
		}

		if(ShorelineCurrent == 5)
			ShorelineCurrent = 0;
		else
			ShorelineCurrent++;

		// Fire_slow
		unsigned FireSlowCurrentWork = FireSlowCurrent;

		for(int i = 4; i >= 0; i--) {
			pal[(714 / 3) * 4 + i * 4] = FireSlow[FireSlowCurrentWork * 3];                 // Red  
			pal[(714 / 3) * 4 + i * 4 + 1] = FireSlow[FireSlowCurrentWork * 3 + 1];         // Green
			pal[(714 / 3) * 4 + i * 4 + 2] = FireSlow[FireSlowCurrentWork * 3 + 2];         // Blue 

			if(FireSlowCurrentWork == 4)
				FireSlowCurrentWork = 0;
			else
				FireSlowCurrentWork++;
		}

		if(FireSlowCurrent == 4)
			FireSlowCurrent = 0;
		else
			FireSlowCurrent++;

		dwLastCycleSlow = dwCurrentTime;
	}

	if(dwCurrentTime - dwLastCycleMedium >= 142 * dwCycleSpeedFactor) {
		// Fire_fast
		unsigned FireFastCurrentWork = FireFastCurrent;

		for(int i = 4; i >= 0; i--) {
			pal[(729 / 3) * 4 + i * 4] = FireFast[FireFastCurrentWork * 3];                 // Red  
			pal[(729 / 3) * 4 + i * 4 + 1] = FireFast[FireFastCurrentWork * 3 + 1];         // Green
			pal[(729 / 3) * 4 + i * 4 + 2] = FireFast[FireFastCurrentWork * 3 + 2];         // Blue 

			if(FireFastCurrentWork == 4)
				FireFastCurrentWork = 0;
			else
				FireFastCurrentWork++;
		}

		if(FireFastCurrent == 4)
			FireFastCurrent = 0;
		else
			FireFastCurrent++;

		dwLastCycleMedium = dwCurrentTime;
	}

	if(dwCurrentTime - dwLastCycleFast >= 100 * dwCycleSpeedFactor) {
		// Monitors
		unsigned dwMonitorsCurrentWork = MonitorsCurrent;

		for(int i = 4; i >= 0; i--) {
			pal[(699 / 3) * 4 + i * 4] = Monitors[dwMonitorsCurrentWork * 3]; // Red  
			pal[(699 / 3) * 4 + i * 4 + 1] = Monitors[dwMonitorsCurrentWork * 3 + 1]; // Green
			pal[(699 / 3) * 4 + i * 4 + 2] = Monitors[dwMonitorsCurrentWork * 3 + 2]; // Blue 

			if(dwMonitorsCurrentWork == 4)
				dwMonitorsCurrentWork = 0;
			else
				dwMonitorsCurrentWork++;
		}

		if(MonitorsCurrent == 4)
			MonitorsCurrent = 0;
		else
			MonitorsCurrent++;

		dwLastCycleFast = dwCurrentTime;
	}

	if(dwCurrentTime - dwLastCycleVeryFast >= 33 * dwCycleSpeedFactor) {
		// Blinking red
		if((BlinkingRedCurrent == 0) || (BlinkingRedCurrent == 60))
			BlinkingRed = unsigned char(-BlinkingRed);

		pal[(762 / 3) * 4] = BlinkingRed + BlinkingRedCurrent;                                  // Red  
		pal[(762 / 3) * 4 + 1] = 0;                                                                       // Green
		pal[(762 / 3) * 4 + 2] = 0;                                                                       // Blue 

		BlinkingRedCurrent = BlinkingRed + BlinkingRedCurrent;

		dwLastCycleVeryFast = dwCurrentTime;
	}
}