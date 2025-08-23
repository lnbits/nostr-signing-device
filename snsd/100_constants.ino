#define VERSION "v0.3.0"

#define DELAY_MS 5
#define PAIRING_CONTROL_TEXT "lnbits"

// filenames
#define FILE_KEY_NAMES "/key_names.txt"
#define FILE_PIN "/pin.txt"
#define FILE_PIN_ATTEMPTS "/pinattempts.txt"
#define FILE_PRIVATE_KEYS "/private_keys.txt"
#define FILE_ACTIVE_KEY_INDEX "/active_key_index.txt"
#define FILE_LEGACY_NOSTR_SECRET "/nostr-secret.txt"
#define FILE_ACCENT_COLOR "/accent_color.txt"
#define FILE_DARKMODE "/dark_mode.txt"

// COMMANDS
#define COMMAND_HELP "/help"
#define COMMAND_PING "/ping"
#define COMMAND_PUBLIC_KEY "/public-key"
#define COMMAND_SIGN_MESSAGE "/sign-message"
#define COMMAND_SHARED_SECRET "/shared-secret"
#define COMMAND_ADD_KEY "/add-key"
#define COMMAND_REMOVE_KEY "/remove-key"
#define COMMAND_LIST_KEYS "/list-keys"
#define COMMAND_SWITCH_KEY "/switch-key"
#define COMMAND_NEW_KEY "/new-key"
#define COMMAND_NAME_KEY "/name-key"
#define COMMAND_ENCRYPT_MESSAGE "/encrypt-message"
#define COMMAND_DECRYPT_MESSAGE "/decrypt-message"
#define COMMAND_REBOOT "/reboot"
#define COMMAND_WIPE "/wipe"
// For backwards compatibility, same as /add-key
#define COMMAND_RESTORE "/restore"

// EVENTS
// data received over the serial port
#define EVENT_SERIAL_DATA "serial"
// action detected on one of the buttons
#define EVENT_BUTTON_ACTION "button"
// execute un-encrypted commands
#define EVENT_INTERNAL_COMMAND "intern"
// screen has gone idle/timed out
#define EVENT_SCREEN_IDLE "screenidle"
