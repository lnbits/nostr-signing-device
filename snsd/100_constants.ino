const int DELAY_MS = 5;
const String PAIRING_CONTROL_TEXT = "lnbits";


const String COMMAND_HELP = "/help";
const String COMMAND_PING = "/ping";
const String COMMAND_PUBLIC_KEY = "/public-key";
const String COMMAND_SEED = "/seed";
const String COMMAND_SIGN_MESSAGE = "/sign-message";
const String COMMAND_SHARED_SECRET = "/shared-secret";


// data received over the serial port
const String EVENT_SERIAL_DATA = "serial";
// action detected on one of the buttons
const String EVENT_BUTTON_ACTION = "button";
// execute un-encrypted commands
const String EVENT_INTERNAL_COMMAND = "intern";