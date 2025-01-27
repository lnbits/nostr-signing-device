const int DELAY_MS = 5;
const String PAIRING_CONTROL_TEXT = "lnbits";

const String COMMAND_HELP = "/help";
const String COMMAND_PING = "/ping";
const String COMMAND_PUBLIC_KEY = "/public-key";
const String COMMAND_SIGN_MESSAGE = "/sign-message";
const String COMMAND_SHARED_SECRET = "/shared-secret";

// For backwards compatibility, same as /add-key
const String COMMAND_RESTORE = "/restore";

const String COMMAND_ADD_KEY = "/add-key";
const String COMMAND_REMOVE_KEY = "/remove-key";
const String COMMAND_LIST_KEYS = "/list-keys";
const String COMMAND_SWITCH_KEY = "/switch-key";
const String COMMAND_NEW_KEY = "/new-key";
const String COMMAND_NAME_KEY = "/name-key";

const String COMMAND_ENCRYPT_MESSAGE_NIP04 = "/encrypt-message-nip04";
const String COMMAND_DECRYPT_MESSAGE_NIP04 = "/decrypt-message-nip04";

const String COMMAND_ENCRYPT_MESSAGE_NIP44 = "/encrypt-message-nip44";
const String COMMAND_DECRYPT_MESSAGE_NIP44 = "/decrypt-message-nip44";

const String COMMAND_REBOOT = "/reboot";
const String COMMAND_WIPE = "/wipe";

// data received over the serial port
const String EVENT_SERIAL_DATA = "serial";
// action detected on one of the buttons
const String EVENT_BUTTON_ACTION = "button";
// execute un-encrypted commands
const String EVENT_INTERNAL_COMMAND = "intern";
// screen has gone idle/timed out
const String EVENT_SCREEN_IDLE = "screenidle";