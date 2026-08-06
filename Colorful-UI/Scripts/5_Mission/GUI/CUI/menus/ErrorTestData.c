// The list of errors shown on the debug screen (ErrorTestScreen.c): one
// entry per error code DayZ can raise, grouped by category.
//
// Codes are written as the game's own named values rather than numbers, so
// if a name changes the mod fails to build instead of raising the wrong
// error. Codes meaning "no error" are left out, as is the Generic category,
// which the game never registers a message for.
class CUI_ErrorTestEntry
{
	ErrorCategory m_Category;
	int m_Code;
	string m_Label;

	void CUI_ErrorTestEntry(ErrorCategory category, int code, string label)
	{
		m_Category = category;
		m_Code = code;
		m_Label = label;
	}
}

class CUI_ErrorTestData
{
	static void GetEntries(out array<ref CUI_ErrorTestEntry> entries)
	{
		entries = new array<ref CUI_ErrorTestEntry>();

		// --- ErrorCategory.ConnectErrorClient (16) ---
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.UNKNOWN, "CEC -1  UNKNOWN (fallback)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.SERVER_UNREACHABLE, "CEC 1  SERVER_UNREACHABLE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.ALREADY_CONNECTING, "CEC 2  ALREADY_CONNECTING"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.ERROR_MSG_SHOWED, "CEC 3  ERROR_MSG_SHOWED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.INCORRECT_CLIENT_STATE, "CEC 4  INCORRECT_CLIENT_STATE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.INVALID_SESSION, "CEC 5  INVALID_SESSION"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.VERSION_MISMATCH, "CEC 6  VERSION_MISMATCH"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.VERSION_MISMATCH_RB, "CEC 7  VERSION_MISMATCH_RB"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.VERSION_MISMATCH_AB, "CEC 8  VERSION_MISMATCH_AB"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.DLC_CHECK_FAILED, "CEC 9  DLC_CHECK_FAILED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.EMPTY_PASSWORD, "CEC 10  EMPTY_PASSWORD"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.PASSWORD, "CEC 11  PASSWORD (silent)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.BE_LICENCE, "CEC 12  BE_LICENCE (silent)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.ALREADY_ON_ANOTHER_SERVER, "CEC 13  ALREADY_ON_ANOTHER_SERVER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.COMMUNICATION_TIMED_OUT, "CEC 14  COMMUNICATION_TIMED_OUT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorClient, EConnectErrorClient.ALREADY_ON_SERVER, "CEC 15  ALREADY_ON_SERVER"));

		// --- ErrorCategory.ConnectErrorServer (26) ---
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.UNKNOWN, "CES -1  UNKNOWN (fallback)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.WRONG_PASSWORD, "CES 1  WRONG_PASSWORD"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.WRONG_VERSION, "CES 2  WRONG_VERSION"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.CONNECT_ERROR, "CES 3  CONNECT_ERROR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.SESSION_FULL, "CES 4  SESSION_FULL"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.FILE_PATCHING, "CES 5  FILE_PATCHING"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.ALREADY_CONNECTED, "CES 6  ALREADY_CONNECTED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.HOST_ADDRESS, "CES 7  HOST_ADDRESS"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.NET_CHANNEL, "CES 8  NET_CHANNEL"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.CONNECT, "CES 9  CONNECT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.REQUEST_PLAYER, "CES 10  REQUEST_PLAYER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.CREATE_PLAYER, "CES 11  CREATE_PLAYER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.TIMEOUT, "CES 12  TIMEOUT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.UID, "CES 13  UID"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.NET_CHANNEL_SERVER, "CES 14  NET_CHANNEL_SERVER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.ACTIVATE, "CES 15  ACTIVATE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.SERVER_LOCKED, "CES 16  SERVER_LOCKED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.SERVER_SHUTDOWN, "CES 17  SERVER_SHUTDOWN"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.BUILD_SMALLER, "CES 18  BUILD_SMALLER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.BUILD_GREATER, "CES 19  BUILD_GREATER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.VERSION_SMALLER, "CES 20  VERSION_SMALLER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.VERSION_GREATER, "CES 21  VERSION_GREATER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.EQUAL_MOD, "CES 22  EQUAL_MOD"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.CLIENT_DIAG, "CES 23  CLIENT_DIAG"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.SERVER_DIAG, "CES 24  SERVER_DIAG"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorServer, EConnectErrorServer.NO_DEVICE_ID, "CES 25  NO_DEVICE_ID"));

		// --- ErrorCategory.ConnectErrorScript (3) ---
		// ALREADY_CONNECTING shows a three-button prompt, not a plain OK box.
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorScript, EConnectErrorScript.UNKNOWN, "CSC -1  UNKNOWN (fallback)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorScript, EConnectErrorScript.ALREADY_CONNECTING, "CSC 1  ALREADY_CONNECTING (Yes/No/Cancel)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ConnectErrorScript, EConnectErrorScript.ALREADY_CONNECTING_THIS, "CSC 2  ALREADY_CONNECTING_THIS"));

		// --- ErrorCategory.ClientKicked (116) ---
		// These are the kick messages, shown in the Colorful-UI dialog.
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNKNOWN, "CK -1  UNKNOWN (fallback)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.SERVER_EXIT, "CK 2  SERVER_EXIT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.KICK_ALL_ADMIN, "CK 3  KICK_ALL_ADMIN"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.KICK_ALL_SERVER, "CK 4  KICK_ALL_SERVER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.TIMEOUT, "CK 5  TIMEOUT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.LOGOUT, "CK 6  LOGOUT (silent)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.KICK, "CK 7  KICK"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.BAN, "CK 8  BAN"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.PING, "CK 9  PING"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.MODIFIED_DATA, "CK 10  MODIFIED_DATA"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNSTABLE_NETWORK, "CK 11  UNSTABLE_NETWORK"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.SERVER_SHUTDOWN, "CK 12  SERVER_SHUTDOWN"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NOT_WHITELISTED, "CK 13  NOT_WHITELISTED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NO_IDENTITY, "CK 14  NO_IDENTITY"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NO_INPUT_INTERFACE, "CK 15  NO_INPUT_INTERFACE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.INVALID_UID, "CK 16  INVALID_UID"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.BANK_COUNT, "CK 17  BANK_COUNT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.ADMIN_KICK, "CK 18  ADMIN_KICK"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.INVALID_ID, "CK 19  INVALID_ID"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.INPUT_HACK, "CK 20  INPUT_HACK"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.QUIT, "CK 21  QUIT (silent)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.LEAVE, "CK 22  LEAVE (silent)"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.LOGIN_MACHINE_ERROR, "CK 48  LOGIN_MACHINE_ERROR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.PLAYER_STATE_TIMEOUT, "CK 49  PLAYER_STATE_TIMEOUT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.IDENTITY_NOT_CREATED, "CK 50  IDENTITY_NOT_CREATED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DUPLICATE_UID, "CK 51  DUPLICATE_UID"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_PING_RESPONSE, "CK 52  DB_PING_RESPONSE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_IDENTITY, "CK 53  DB_IDENTITY"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_TIMEOUT, "CK 54  DB_TIMEOUT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_BAD_VERSION, "CK 55  DB_BAD_VERSION"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_ERROR, "CK 56  DB_ERROR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_APP_ERROR, "CK 57  DB_APP_ERROR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_APP_EXIT, "CK 58  DB_APP_EXIT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_BAD_DATA, "CK 59  DB_BAD_DATA"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.DB_GET_RESPONSE, "CK 60  DB_GET_RESPONSE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NO_PLAYER, "CK 61  NO_PLAYER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.CREATE_NEW_CLIENT, "CK 62  CREATE_NEW_CLIENT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.CREATE_LOADED_CHAR, "CK 63  CREATE_LOADED_CHAR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.LOAD_LOADED_CHAR, "CK 64  LOAD_LOADED_CHAR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NO_NETWORK_INFO, "CK 65  NO_NETWORK_INFO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.CREATE_READY_CLIENT, "CK 66  CREATE_READY_CLIENT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.RECONNECT_INFO, "CK 67  RECONNECT_INFO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.RECONNECT_IDENTITY, "CK 68  RECONNECT_IDENTITY"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.RECONNECT_CHAR, "CK 69  RECONNECT_CHAR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.ALREADY_ON_ANOTHER_SERVER, "CK 70  ALREADY_ON_ANOTHER_SERVER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.BIOS_ERROR, "CK 71  BIOS_ERROR"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.INIT_RESPAWN_IDENTITY, "CK 80  INIT_RESPAWN_IDENTITY"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.RESPAWN_IDENTITY, "CK 81  RESPAWN_IDENTITY"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NEW_RESPAWN_NETWORK, "CK 82  NEW_RESPAWN_NETWORK"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NEW_RESPAWN_IDENTITY, "CK 83  NEW_RESPAWN_IDENTITY"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NEW_RESPAWN_INFO, "CK 84  NEW_RESPAWN_INFO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.RESPAWN_NEW_CLIENT, "CK 85  RESPAWN_NEW_CLIENT"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.PLAYER_VERIFICATION, "CK 96  PLAYER_VERIFICATION"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.NO_INFO, "CK 97  NO_INFO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.BROKEN_DATA, "CK 98  BROKEN_DATA"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VERIFICATION_ERROR, "CK 112  VERIFICATION_ERROR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_UNKNOWN, "CK 113  VE_UNKNOWN"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_DECOMPRESS, "CK 114  VE_DECOMPRESS"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_MISSING_MOD, "CK 115  VE_MISSING_MOD"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_EXTRA_MOD, "CK 116  VE_EXTRA_MOD"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_CLIENT_CORRUPT, "CK 117  VE_CLIENT_CORRUPT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_SERVER_CORRUPT, "CK 118  VE_SERVER_CORRUPT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_UNEXPECTED_MOD_PBO, "CK 119  VE_UNEXPECTED_MOD_PBO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_UM_ROGUE_PBO, "CK 120  VE_UM_ROGUE_PBO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_UM_CLIENT_UPDATED, "CK 121  VE_UM_CLIENT_UPDATED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_UM_SERVER_UPDATED, "CK 122  VE_UM_SERVER_UPDATED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_UNEXPECTED_SOURCE, "CK 123  VE_UNEXPECTED_SOURCE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_PATCHED_PBO, "CK 124  VE_PATCHED_PBO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_INTEGRITY, "CK 125  VE_INTEGRITY"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_MISSING_BISIGN, "CK 126  VE_MISSING_BISIGN"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_MISSING_PBO, "CK 127  VE_MISSING_PBO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_M_ROGUE_PBO, "CK 128  VE_M_ROGUE_PBO"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_M_CLIENT_UPDATED, "CK 129  VE_M_CLIENT_UPDATED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_M_SERVER_UPDATED, "CK 130  VE_M_SERVER_UPDATED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_M_SERVER_CORRUPT, "CK 131  VE_M_SERVER_CORRUPT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_M_CLIENT_CORRUPT, "CK 132  VE_M_CLIENT_CORRUPT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_M_UNEXPECTED_SOURCE, "CK 133  VE_M_UNEXPECTED_SOURCE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.VE_GPROJ, "CK 134  VE_GPROJ"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.PBO_MISMATCH, "CK 144  PBO_MISMATCH"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.PBO_VERSION_MISMATCH, "CK 145  PBO_VERSION_MISMATCH"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.PBO_CLIENT_UPDATED, "CK 146  PBO_CLIENT_UPDATED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.PBO_SERVER_UPDATED, "CK 147  PBO_SERVER_UPDATED"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_ERROR, "CK 160  AUTH_ERROR"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_NO_USER, "CK 161  AUTH_NO_USER"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_INVALID_STEAM_VALIDATE, "CK 176  AUTH_INVALID_STEAM_VALIDATE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_NO_SERVER, "CK 177  AUTH_NO_SERVER"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_INVALID_TICKET, "CK 178  AUTH_INVALID_TICKET"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_DUPLICATE_REQUEST, "CK 179  AUTH_DUPLICATE_REQUEST"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_INVALID_VERSION, "CK 180  AUTH_INVALID_VERSION"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_GAME_MISMATCH, "CK 181  AUTH_GAME_MISMATCH"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_EXPIRED_TICKET, "CK 182  AUTH_EXPIRED_TICKET"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_INVALID_STEAM_ONVALIDATE, "CK 192  AUTH_INVALID_STEAM_ONVALIDATE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_USER_NOT_CONNECTED, "CK 193  AUTH_USER_NOT_CONNECTED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_LICENCE, "CK 194  AUTH_LICENCE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_SERVICE_BAN, "CK 195  AUTH_SERVICE_BAN"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_LOGGED_IN_ELSEWHERE, "CK 196  AUTH_LOGGED_IN_ELSEWHERE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_TIMEOUT, "CK 197  AUTH_TIMEOUT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_CANCELED, "CK 198  AUTH_CANCELED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_ALREADY_USED, "CK 199  AUTH_ALREADY_USED"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_TICKET_INVALID, "CK 200  AUTH_TICKET_INVALID"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_PUBLISHER_BAN, "CK 201  AUTH_PUBLISHER_BAN"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_INVALID_BIOS_VALIDATE, "CK 208  AUTH_INVALID_BIOS_VALIDATE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_BIOS_REQUEST_CREATE, "CK 209  AUTH_BIOS_REQUEST_CREATE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_BIOS_REQUEST_SEND, "CK 210  AUTH_BIOS_REQUEST_SEND"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_BIOS_REQUEST_WAIT, "CK 211  AUTH_BIOS_REQUEST_WAIT"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_UNKNOWN_BIOS_VALIDATE, "CK 212  AUTH_UNKNOWN_BIOS_VALIDATE"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_INVALID_BIOS_ONVALIDATE, "CK 224  AUTH_INVALID_BIOS_ONVALIDATE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_BIOS_WRONG_RESPONSE, "CK 225  AUTH_BIOS_WRONG_RESPONSE"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.AUTH_UNKNOWN_BIOS_ONVALIDATE, "CK 226  AUTH_UNKNOWN_BIOS_ONVALIDATE"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.BATTLEYE, "CK 240  BATTLEYE"));

		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNM1, "CK 256  UNM1"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNM2, "CK 257  UNM2"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNM3, "CK 258  UNM3"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNM4, "CK 259  UNM4"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNM5, "CK 260  UNM5"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.ClientKicked, EClientKicked.UNM6, "CK 261  UNM6"));

		// --- ErrorCategory.BIOSError (24) ---
		// Console-only: on PC all but the first show nothing when clicked.
		// That is expected, not a fault.
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, -1, "BIOS -1  UNKNOWN (fallback, always dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.CANCEL, "BIOS 1  CANCEL (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.BAD_PARAMETER, "BIOS 2  BAD_PARAMETER (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.NOT_FOUND, "BIOS 3  NOT_FOUND (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.NOT_IMPLEMENTED, "BIOS 4  NOT_IMPLEMENTED (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.PURCHASE_REQUIRED, "BIOS 5  PURCHASE_REQUIRED (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.NOT_ALLOWED, "BIOS 6  NOT_ALLOWED (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.BANNED, "BIOS 7  BANNED (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.LOGICAL, "BIOS 8  LOGICAL (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.WRONG_RESPONSE_DATA, "BIOS 9  WRONG_RESPONSE_DATA (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.SERVER_HEARTBEAT_SERVER_NOT_FOUND, "BIOS 10  SERVER_HEARTBEAT_SERVER_NOT_FOUND (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.SERVER_REGISTER_ALREADY_EXISTS, "BIOS 11  SERVER_REGISTER_ALREADY_EXISTS (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.REQUEST_CREATE_FAIL_XHR, "BIOS 12  REQUEST_CREATE_FAIL_XHR (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.REQUEST_SEND_FAIL, "BIOS 13  REQUEST_SEND_FAIL (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.REQUEST_WAIT_FAIL, "BIOS 14  REQUEST_WAIT_FAIL (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.NON_JSON_RESPONSE, "BIOS 15  NON_JSON_RESPONSE (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.UPDATE_REQUIRED, "BIOS 21  UPDATE_REQUIRED (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.UPDATE_REQUIRED_AND_DOWNLOADED, "BIOS 22  UPDATE_REQUIRED_AND_DOWNLOADED (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.COMUNICATION_ERROR, "BIOS 23  COMUNICATION_ERROR (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.UNKNOWN, "BIOS 24  UNKNOWN (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.BAD_SCRIPT, "BIOS 25  BAD_SCRIPT (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.COMMUNICATION_TIMED_OUT, "BIOS 26  COMMUNICATION_TIMED_OUT (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.COMMUNICATION_RESET, "BIOS 27  COMMUNICATION_RESET (console-only dialog)"));
		entries.Insert(new CUI_ErrorTestEntry(ErrorCategory.BIOSError, EBiosError.COMMUNICATION_ABORTED, "BIOS 28  COMMUNICATION_ABORTED (console-only dialog)"));
	}
}
