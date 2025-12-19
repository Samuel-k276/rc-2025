# Event Reservation - Commands and Flags Specification

## 1. Flags

### User Application Flags

**Command:** `./user [-n ESIP] [-p ESport]`

- **`-n ESIP`** (optional): IP address of the machine where the Event Reservation Server (ES) runs. If omitted, the ES should be running on the same machine (default: `127.0.0.1`).

- **`-p ESport`** (optional): Well-known port (TCP and UDP) where the ES accepts requests. If omitted, it assumes the value `58000+GN`, where GN is the group number (default: `58011` for group 11).

### Server Flags

**Command:** `./ES [-p ESport] [-v]`

- **`-p ESport`** (optional): Well-known port where the ES server accepts requests, both in UDP and TCP. If omitted, it assumes the value `58000+GN`, where GN is the group number (default: `58011` for group 11).

- **`-v`** (optional): Enables verbose mode. If set, the ES outputs to the screen a short description of the received requests (UID, type of request) and the IP and port originating those requests.

---

## 2. Commands

### 2.1 login

**User Input:** `login UID password`

**Message to Server:** `LIN UID password\n` (UDP)

**Possible Responses from Server:**
- `RLI OK\n` - User is registered and password is correct
- `RLI REG\n` - User was not registered, now registered and logged in
- `RLI NOK\n` - User is registered but password is incorrect
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- `successful login` - When response is `RLI OK`
- `new user registered` - When response is `RLI REG`
- `incorrect login attempt` - When response is `RLI NOK`

**What the Command Does in the Server:**
- Checks if user with UID is registered
- If not registered: registers the user with UID and password, logs them in, responds with `RLI REG`
- If registered: checks password
  - If password matches: logs in the user, responds with `RLI OK`
  - If password doesn't match: responds with `RLI NOK`

---

### 2.2 logout

**User Input:** `logout`

**Message to Server:** `LOU UID password\n` (UDP)  
*Note: UID and password are retrieved from the current session*

**Possible Responses from Server:**
- `RLO OK\n` - User was logged in and successfully logged out
- `RLO UNR\n` - User is not registered
- `RLO NOK\n` - User is not logged in
- `RLO WRP\n` - Password is incorrect
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- `successful logout` - When response is `RLO OK`
- `unknown user` - When response is `RLO UNR`
- `user not logged in` - When response is `RLO NOK`

**What the Command Does in the Server:**
- Checks if user with UID is registered
- If not registered: responds with `RLO UNR`
- Checks if user is logged in
- If not logged in: responds with `RLO NOK`
- Checks if password matches
- If password doesn't match: responds with `RLO WRP`
- If all checks pass: logs out the user, responds with `RLO OK`

---

### 2.3 unregister

**User Input:** `unregister`

**Message to Server:** `UNR UID password\n` (UDP)  
*Note: UID and password are retrieved from the current session*

**Possible Responses from Server:**
- `UNR OK\n` - User was successfully unregistered
- `UNR UNR\n` - User is not registered
- `UNR NOK\n` - User is not logged in
- `UNR WRP\n` - Password is incorrect
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- `successful unregister` - When response is `UNR OK`
- `unknown user` - When response is `UNR UNR`
- `incorrect unregister attempt` - When response is `UNR NOK` or `UNR WRP`

**What the Command Does in the Server:**
- Checks if user with UID is registered
- If not registered: responds with `UNR UNR`
- Checks if user is logged in
- If not logged in: responds with `UNR NOK`
- Checks if password matches
- If password doesn't match: responds with `UNR WRP`
- If all checks pass: logs out the user, removes user from database, responds with `UNR OK`

---

### 2.4 changePass

**User Input:** `changePass oldPassword newPassword`

**Message to Server:** `CPS UID oldPassword newPassword\n` (TCP)  
*Note: UID is retrieved from the current session*

**Possible Responses from Server:**
- `RCP OK\n` - Password was successfully changed
- `RCP NLG\n` - User is not logged in
- `RCP NOK\n` - Old password is incorrect
- `RCP NID\n` - User does not exist
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- `successful password change` - When response is `RCP OK`
- `user not logged In` - When response is `RCP NLG`
- `incorrect password` - When response is `RCP NOK`
- `unknown user` - When response is `RCP NID`

**What the Command Does in the Server:**
- Checks if user is logged in
- If not logged in: responds with `RCP NLG`
- Checks if user exists
- If user doesn't exist: responds with `RCP NID`
- Checks if old password matches the saved password
- If old password doesn't match: responds with `RCP NOK`
- If all checks pass: updates password to newPassword, responds with `RCP OK`

---

### 2.5 myevents (or mye)

**User Input:** `myevents` or `mye`

**Message to Server:** `LME UID password\n` (UDP)  
*Note: UID and password are retrieved from the current session*

**Possible Responses from Server:**
- `RME OK EID1 state1 EID2 state2 ...\n` - List of events created by user
- `RME NOK\n` - User has not created any events
- `RME NLG\n` - User is not logged in
- `RME WRP\n` - Password is incorrect
- `ERR\n` - Invalid command syntax or parameters

**State Values:**
- `1` - Event is still accepting reservations
- `0` - Event was in the past
- `2` - Event is still in the future but sold out
- `3` - Event was closed by the user

**User Display Messages:**
- Displays list of events with their states - When response is `RME OK ...`
- `user has not created any events` - When response is `RME NOK`

**What the Command Does in the Server:**
- Checks if user is logged in
- If not logged in: responds with `RME NLG`
- Checks if password matches
- If password doesn't match: responds with `RME WRP`
- Checks if user has created any events
- If no events: responds with `RME NOK`
- If events exist: retrieves all events created by user, formats them with EID and state, responds with `RME OK EID1 state1 EID2 state2 ...`

---

### 2.6 myreservations (or myr)

**User Input:** `myreservations` or `myr`

**Message to Server:** `LMR UID password\n` (UDP)  
*Note: UID and password are retrieved from the current session*

**Possible Responses from Server:**
- `RMR OK EID1 date1 time1 value1 EID2 date2 time2 value2 ...\n` - List of reservations made by user
- `RMR NOK\n` - User has not made any reservations
- `RMR NLG\n` - User is not logged in
- `RMR WRP\n` - Password is incorrect
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- Displays list of reservations with EID, date/time, and number of seats - When response is `RMR OK ...`
- `user has not made any reservation` - When response is `RMR NOK`

**What the Command Does in the Server:**
- Checks if user is logged in
- If not logged in: responds with `RMR NLG`
- Checks if password matches
- If password doesn't match: responds with `RMR WRP`
- Checks if user has made any reservations
- If no reservations: responds with `RMR NOK`
- If reservations exist: retrieves all reservations made by user, formats them with EID, date/time (dd-mm-yyyy hh:mm:ss), and number of seats, responds with `RMR OK EID1 date1 time1 value1 ...`

---

### 2.7 create

**User Input:** `create name event_fname event_date num_attendees`

**Parameters:**
- `name`: Short event name (up to 10 alphanumeric characters)
- `event_fname`: Path to file describing the event (must exist locally)
- `event_date`: Date and time in format `dd-mm-yyyy hh:mm` (separated by space in user input)
- `num_attendees`: Number of available seats (10-999)

**Message to Server:** `CRE UID password name event_date attendance_size Fname Fsize Fdata\n` (TCP)  
*Note: UID and password are retrieved from the current session. The file is read and its contents (Fdata) and size (Fsize) are included. Fname is the filename without path. event_date is in format `dd-mm-yyyy hh:mm` (combined).*

**Possible Responses from Server:**
- `RCE OK EID\n` - Event was successfully created with assigned EID (3-digit number)
- `RCE NOK\n` - Event could not be created (invalid parameters or no space)
- `RCE NLG\n` - User is not logged in
- `RCE WRP\n` - Password is incorrect
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- `Event created successfully` - When response is `RCE OK EID` (EID should be displayed)

**What the Command Does in the Server:**
- Checks if user is logged in
- If not logged in: responds with `RCE NLG`
- Checks if password matches
- If password doesn't match: responds with `RCE WRP`
- Validates event parameters (name, date, attendance_size, filename, file size)
- Checks if there's space for a new event (max 999 events)
- If validation fails or no space: responds with `RCE NOK`
- If all checks pass: creates new event, assigns unique EID (3-digit), stores file locally, responds with `RCE OK EID`

---

### 2.8 close

**User Input:** `close EID`

**Parameters:**
- `EID`: Event identifier (3-digit number)

**Message to Server:** `CLS UID password EID\n` (TCP)  
*Note: UID and password are retrieved from the current session*

**Possible Responses from Server:**
- `RCL OK\n` - Event was successfully closed
- `RCL NOK\n` - User does not exist or password is incorrect
- `RCL NLG\n` - User is not logged in
- `RCL NOE\n` - Event EID does not exist
- `RCL EOW\n` - Event is not owned by user UID
- `RCL SLD\n` - Event is already sold out
- `RCL PST\n` - Event date has already passed
- `RCL CLO\n` - Event was previously closed
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- `Event closed successfully` - When response is `RCL OK`
- `event is sold-out` - When response is `RCL SLD`
- `event has already expired` - When response is `RCL PST`

**What the Command Does in the Server:**
- Checks if user is logged in
- If not logged in: responds with `RCL NLG`
- Checks if user exists and password matches
- If user doesn't exist or password incorrect: responds with `RCL NOK`
- Checks if event exists
- If event doesn't exist: responds with `RCL NOE`
- Checks if event belongs to user
- If event doesn't belong to user: responds with `RCL EOW`
- Checks if event was already closed
- If already closed: responds with `RCL CLO`
- Checks if event date has passed
- If date passed: responds with `RCL PST`
- Checks if event is sold out
- If sold out: responds with `RCL SLD`
- If all checks pass: closes the event (sets state to 3), responds with `RCL OK`

---

### 2.9 list

**User Input:** `list`

**Message to Server:** `LST\n` (TCP)

**Possible Responses from Server:**
- `RLS OK EID1 name1 state1 event_date1 EID2 name2 state2 event_date2 ...\n\n` - List of all events (ends with `\n\n`)
- `RLS NOK\n` - No events have been created
- `ERR\n` - Invalid command syntax

**State Values:**
- `1` - Event is in the future and not sold out
- `0` - Event is in the past
- `2` - Event is in the future but sold out
- `3` - Event was closed by the owner

**Event Date Format:** `dd-mm-yyyy hh:mm`

**User Display Messages:**
- Displays list of all events with EID, name, state, and date - When response is `RLS OK ...`
- `no events has yet been created` - When response is `RLS NOK`

**What the Command Does in the Server:**
- Checks if any events exist
- If no events: responds with `RLS NOK`
- If events exist: retrieves all events, formats each with EID, name, state, and event_date, sends list separated by spaces, ends with `\n\n`, responds with `RLS OK EID1 name1 state1 event_date1 ...`

---

### 2.10 show

**User Input:** `show EID`

**Parameters:**
- `EID`: Event identifier (3-digit number)

**Message to Server:** `SED EID\n` (TCP)

**Possible Responses from Server:**
- `RSE OK UID name event_date attendance_size Seats_reserved Fname Fsize Fdata\n` - Event details with file
- `RSE NOK\n` - Event does not exist or other error
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- Displays event details: owner UID, name, date, total seats, reserved seats, file name and size - When response is `RSE OK ...`
- File is stored locally and its name/directory is displayed
- Indicates if event is closed or sold-out

**What the Command Does in the Server:**
- Checks if event exists
- If event doesn't exist: responds with `RSE NOK`
- If event exists: retrieves event details (owner UID, name, date, total seats, reserved seats), reads file (Fname, Fsize, Fdata), formats response with all information, responds with `RSE OK UID name event_date attendance_size Seats_reserved Fname Fsize Fdata`

---

### 2.11 reserve

**User Input:** `reserve EID value`

**Parameters:**
- `EID`: Event identifier (3-digit number)
- `value`: Number of seats to reserve (1-999)

**Message to Server:** `RID UID password EID people\n` (TCP)  
*Note: UID and password are retrieved from the current session*

**Possible Responses from Server:**
- `RRI ACC\n` - Reservation was accepted
- `RRI REJ n_seats\n` - Reservation was rejected, n_seats remaining seats available
- `RRI NOK\n` - Event EID is not active
- `RRI NLG\n` - User is not logged in
- `RRI CLS\n` - Event is closed
- `RRI SLD\n` - Event is sold out
- `RRI PST\n` - Event's date has passed
- `RRI WRP\n` - Password is incorrect
- `ERR\n` - Invalid command syntax or parameters

**User Display Messages:**
- `accepted` - When response is `RRI ACC`
- `refused` - When response is `RRI REJ n_seats` (displays number of remaining seats)
- `event is no longer active` - When response is `RRI NOK`, `RRI CLS`, `RRI SLD`, or `RRI PST`

**What the Command Does in the Server:**
- Checks if user is logged in
- If not logged in: responds with `RRI NLG`
- Checks if password matches
- If password doesn't match: responds with `RRI WRP`
- Checks if event exists and is active
- If event doesn't exist or not active: responds with `RRI NOK`
- Checks if event is closed
- If closed: responds with `RRI CLS`
- Checks if event date has passed
- If date passed: responds with `RRI PST`
- Checks if event is sold out
- If sold out: responds with `RRI SLD`
- Checks if there are enough seats available
- If not enough seats: responds with `RRI REJ n_seats` (where n_seats is remaining available seats)
- If all checks pass: creates reservation, updates event reserved seats, responds with `RRI ACC`

---

### 2.12 exit

**User Input:** `exit`

**Message to Server:** None (local command)

**Possible Responses from Server:** N/A

**User Display Messages:**
- `User is logged in, cannot exit, please logout first` - If user is still logged in
- `Exiting program` - If user is logged out

**What the Command Does in the Server:**
- N/A (local command, no server interaction)

**What the Command Does in the Client:**
- Checks if user is logged in
- If logged in: displays error message and does not exit
- If logged out: exits the application

---

## Protocol Notes

- All messages end with `\n` (newline character)
- UID is always sent using 6 digits
- EID is always sent using 3 digits (001-999)
- Password is 8 alphanumeric characters
- Separation between message fields consists of a single space
- Status field `ERR` is used when the syntax of the request message was incorrect or when parameter values are invalid
- If an unexpected protocol message is received, the reply is `ERR`
- File size (Fsize) is limited to 10 MB (10,000,000 bytes), transmitted using a maximum of 8 digits
- Filenames (Fname) are limited to 24 alphanumerical characters (plus '-', '_' and '.'), including the separating dot and extension
