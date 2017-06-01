// hook ExtTextOutW ( 0xff010e39, 12, 67, ETO_IGNORELANGUAGE, NULL, "[User info] ID: 265079 | User name: Houstin?f", 43, NULL )	TRUE		0.0000287
// in the hook make sure it is being called to draw on the screenshot
// if it is drawing on the screenshot, create mutex to signal the hack to disable visuals.  after waiting so long, take a new screenshot and overwrite the one in hDC passed in the function, then call the original text drawing function again.
