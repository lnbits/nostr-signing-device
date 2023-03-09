const int DELAY_MS = 5;
const String PAIRING_CONTROL_TEXT = "lnbits";


const String COMMAND_HELP = "/help";
const String COMMAND_PING = "/ping";
const String COMMAND_RESTORE = "/restore";
const String COMMAND_WIPE = "/wipe";
const String COMMAND_PASSWORD = "/password";
const String COMMAND_PASSWORD_CLEAR = "/password-clear";
const String COMMAND_ADDRESS = "/address";
const String COMMAND_SEED = "/seed";
const String COMMAND_SEND_PSBT = "/psbt";
const String COMMAND_SIGN_PSBT = "/sign";
const String COMMAND_XPUB = "/xpub";
const String COMMAND_CONFIRM_NEXT = "/confirm-next";
const String COMMAND_CANCEL = "/cancel";
const String COMMAND_PAIR = "/pair";
const String COMMAND_CHECK_PAIRING = "/check-pairing";

// data received over the serial port
const String EVENT_SERIAL_DATA = "serial";
// action detected on one of the buttons
const String EVENT_BUTTON_ACTION = "button";
// execute un-encrypted commands
const String EVENT_INTERNAL_COMMAND = "intern";