#ifndef CLIENT_MESSAGES_H
#define CLIENT_MESSAGES_H

#include <string>
#include "../common/server_responses.h"

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

