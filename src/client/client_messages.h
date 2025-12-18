#ifndef CLIENT_MESSAGES_H
#define CLIENT_MESSAGES_H

#include <string>

// ============================================================================
// SERVER RESPONSE CODES (Protocol Messages)
// ============================================================================

// Login responses (RLI)
#define SERVER_RESPONSE_RLI_OK "RLI OK\n"
#define SERVER_RESPONSE_RLI_REG "RLI REG\n"
#define SERVER_RESPONSE_RLI_NOK "RLI NOK\n"

// Logout responses (RLO)
#define SERVER_RESPONSE_RLO_OK "RLO OK\n"
#define SERVER_RESPONSE_RLO_UNR "RLO UNR\n"
#define SERVER_RESPONSE_RLO_NOK "RLO NOK\n"
#define SERVER_RESPONSE_RLO_WRP "RLO WRP\n"

// Unregister responses (UNR)
#define SERVER_RESPONSE_UNR_OK "UNR OK\n"
#define SERVER_RESPONSE_UNR_UNR "UNR UNR\n"
#define SERVER_RESPONSE_UNR_NOK "UNR NOK\n"
#define SERVER_RESPONSE_UNR_WRP "UNR WRP\n"

// Change Password responses (RCP)
#define SERVER_RESPONSE_RCP_OK "RCP OK\n"
#define SERVER_RESPONSE_RCP_UNR "RCP UNR\n"
#define SERVER_RESPONSE_RCP_NLG "RCP NLG\n"
#define SERVER_RESPONSE_RCP_WRP "RCP WRP\n"

// My Events responses (RME)
#define SERVER_RESPONSE_RME_OK "RME OK\n"
#define SERVER_RESPONSE_RME_NOK "RME NOK\n"
#define SERVER_RESPONSE_RME_NLG "RME NLG\n"
#define SERVER_RESPONSE_RME_ERR "RME ERR\n"

// My Reservations responses (RMR)
#define SERVER_RESPONSE_RMR_OK "RMR OK\n"
#define SERVER_RESPONSE_RMR_NOK "RMR NOK\n"
#define SERVER_RESPONSE_RMR_NLG "RMR NLG\n"

// Create Event responses (RCR)
#define SERVER_RESPONSE_RCR_OK "RCR OK\n"
#define SERVER_RESPONSE_RCR_NOK "RCR NOK\n"
#define SERVER_RESPONSE_RCR_NLG "RCR NLG\n"
#define SERVER_RESPONSE_RCR_ERR "RCR ERR\n"

// Close Event responses (RCL)
#define SERVER_RESPONSE_RCL_OK "RCL OK\n"
#define SERVER_RESPONSE_RCL_NOK "RCL NOK\n"
#define SERVER_RESPONSE_RCL_NLG "RCL NLG\n"
#define SERVER_RESPONSE_RCL_END "RCL END\n"
#define SERVER_RESPONSE_RCL_ERR "RCL ERR\n"

// List Events responses (RLS)
#define SERVER_RESPONSE_RLS_OK "RLS OK\n"
#define SERVER_RESPONSE_RLS_NOK "RLS NOK\n"
#define SERVER_RESPONSE_RLS_EMPTY "RLS EMPTY\n"

// Show Event Details responses (RSD)
#define SERVER_RESPONSE_RSD_OK "RSD OK\n"
#define SERVER_RESPONSE_RSD_NOK "RSD NOK\n"
#define SERVER_RESPONSE_RSD_END "RSD END\n"
#define SERVER_RESPONSE_RSD_ERR "RSD ERR\n"

// Reserve responses (RRV)
#define SERVER_RESPONSE_RRV_OK "RRV OK\n"
#define SERVER_RESPONSE_RRV_NOK "RRV NOK\n"
#define SERVER_RESPONSE_RRV_NLG "RRV NLG\n"
#define SERVER_RESPONSE_RRV_END "RRV END\n"
#define SERVER_RESPONSE_RRV_ERR "RRV ERR\n"

// Generic error response
#define SERVER_RESPONSE_ERR "ERR\n"

// ============================================================================
// CLIENT USER-FACING MESSAGES (Display Messages from Enunciado)
// ============================================================================

namespace ClientMessages {
    // Login messages
    namespace Login {
        const std::string SUCCESS = "successful login";
        const std::string NEW_USER_REGISTERED = "new user registered";
        const std::string INCORRECT_ATTEMPT = "incorrect login attempt";
    }

    // Change Password messages
    namespace ChangePass {
        const std::string SUCCESS = "successful password change";
        const std::string UNKNOWN_USER = "unknown user";
        const std::string NOT_LOGGED_IN = "user not logged In";
        const std::string INCORRECT_PASSWORD = "incorrect password";
    }

    // Unregister messages
    namespace Unregister {
        const std::string SUCCESS = "successful unregister";
        const std::string UNKNOWN_USER = "unknown user";
        const std::string INCORRECT_ATTEMPT = "incorrect unregister attempt";
    }

    // Logout messages
    namespace Logout {
        const std::string SUCCESS = "successful logout";
        const std::string UNKNOWN_USER = "unknown user";
        const std::string NOT_LOGGED_IN = "user not logged in";
    }

    // Exit messages
    namespace Exit {
        const std::string USER_LOGGED_IN = "User is logged in, cannot exit, please logout first";
    }

    // My Events messages
    namespace MyEvents {
        const std::string NO_EVENTS = "user has not created any events";
    }

    // List Events messages
    namespace ListEvents {
        const std::string NO_EVENTS = "no events has yet been created";
    }

    // Close Event messages
    namespace CloseEvent {
        const std::string EVENT_EXPIRED = "event has already expired";
        const std::string EVENT_SOLD_OUT = "event is sold-out";
    }

    // Reserve messages
    namespace Reserve {
        const std::string ACCEPTED = "accepted";
        const std::string REFUSED = "refused";
        const std::string EVENT_NOT_ACTIVE = "event is no longer active";
    }

    // My Reservations messages
    namespace MyReservations {
        const std::string NO_RESERVATIONS = "user has not made any reservation";
    }
}

#endif // CLIENT_MESSAGES_H

