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

---

## 3. Servidor ES - Organização de dados

O servidor ES instalado na máquina 'tejo' do laboratório LT5 usa uma estrutura de directorias para armazenar toda a informação referente ao protocolo de forma persistente entre sessões.

Os alunos podem replicar total ou parcialmente a estrutura de directorias aqui descrita, a qual foi concebida tendo em vista a simplificação do processamento para armazenamento flexível e indexação de eventos e reservas, aproveitando ainda as estruturas de dados previstas no filesystem do Linux para obter facilmente a ordenação de ficheiros nele contidos.

O presente guia exemplifica também, em linguagem C, a obtenção de conteúdos de directorias ordenados e conversões de tempos e datas úteis para a realização do projecto.

### 3.1 A directoria de trabalho do servidor ES

Na directoria de trabalho do servidor ES encontram-se duas directorias designadas `USERS` e `EVENTS`. Assume-se que no arranque do servidor ES estas duas directorias já estão criadas.

Ilustra-se a seguir a estrutura da base de dados do servidor, em que `ESDIR` é a directoria onde se encontra o executável ES.

```
ESDIR
├── USERS
│   ├── (uid1)
│   │   ├── (uid1)_pass.txt
│   │   ├── (uid1)_login.txt
│   │   ├── CREATED
│   │   │   ├── (eid1).txt
│   │   │   └── (eid2).txt
│   │   └── RESERVED
│   │       ├── R-(uid1)-(date1).txt
│   │       ├── R-(uid1)-(date2).txt
│   │       └── R-(uid1)-(date3).txt
│   ├── (uid2)
│   └── (uid3)
└── EVENTS
    ├── (eid1)
    │   ├── START_(eid1).txt
    │   ├── RES_(eid1).txt
    │   ├── DESCRIPTION
    │   │   └── (desc fname1)
    │   ├── END (eid1).txt
    │   └── RESERVATIONS
    │       ├── R-(uid1)-(date1).txt
    │       ├── R-(uid2)-(date2).txt
    │       └── R-(uid3)-(date3).txt
    ├── (eid2)
    └── (eid3)
```

O servidor ES criará dentro da directoria `USERS` uma directoria por cada utilizador que se regista. A designação da directoria de utilizador coincide com o UID do utilizador em causa. Do mesmo modo, o servidor ES criará dentro da directoria `EVENTS` uma directoria por cada evento criado. A designação da directoria de evento coincide com o EID do evento em causa.

**Na directoria de cada utilizador (sob USERS) são criados:**

- Um ficheiro `(uid) pass.txt` que contém a password do utilizador. Este ficheiro existirá enquanto o utilizador permanecer registado.
- Um ficheiro `(uid) login.txt` indicando que o utilizador está em sessão. Este ficheiro existe apenas enquanto o utilizador estiver em sessão.
- Uma directoria designada `CREATED` contendo informação sobre todos os eventos criados pelo utilizador. A cada evento iniciado pelo utilizador corresponde um ficheiro dentro da directoria `CREATED`.
- Uma directoria designada `RESERVED` contendo informação sobre todos os eventos nos quais o utilizador reservou lugares. A cada evento no qual o utilizador efectuou uma reserva, corresponde um ficheiro dentro da directoria `RESERVED`. Um duplicado deste ficheiro existe também sob a directoria `RESERVATIONS` de cada evento.

Quando um utilizador remove o seu registo (comando `unregister`), os ficheiros `(uid) pass.txt` e `(uid) login.txt` e apenas estes são removidos, preservando-se as respectivas directorias e o seu conteúdo.

Se o utilizador voltar a registar-se, será criado um novo ficheiro `(uid) pass.txt`, assumindo-se que no novo registo, o utilizador herdará toda a informação guardada nas directorias `CREATED` e `RESERVED` em momentos anteriores. O efeito prático resultante de um utilizador cancelar o seu registo e voltar a registar-se é equivalente à alteração da password.

Os ficheiros guardados nas directorias `CREATED` e `RESERVED` destinam-se a permitir ao servidor recuperar de forma rápida a lista de todos os eventos relevantes para cada utilizador evitando buscas exaustivas sobre a directoria `EVENTS`, contendo esta última informação mais completa embora organizada de outra forma.

**Na directoria de cada evento (sob EVENTS) é criada uma directoria por cada evento criado. Cada directoria de evento conterá:**

- Um ficheiro `START (eid).txt` que contém toda a informação relevante sobre o evento. Este ficheiro é criado no momento de criação do evento quando o utilizador emite o comando `create`.
- Um ficheiro `RES (eid).txt` que contém o número total de reservas acumuladas para o evento. Este ficheiro é criado com valor 0 quando o utilizador emite o comando `create`.
- Uma directoria `DESCRIPTION` que contém o ficheiro `desc fname` o qual contém uma imagem ou uma descrição do evento criado. Este ficheiro é carregado por TCP para o servidor quando o utilizador emite o comando `create`.
- Um ficheiro `END (eid).txt` que só é criado quando o evento é dado como encerrado.
- Uma directoria `RESERVATIONS` que contém todas as reservas efectuadas para o evento em causa. A directoria `RESERVATIONS` conterá um ficheiro por cada operação de reserva de lugares. O nome de cada ficheiro de reserva contém o UID do utilizador que fez a reserva e a data de recepção da reserva no servidor para permitir a recuperação de todas as reservas ordenadas por data. Os ficheiros contidos na directoria `RESERVATIONS` são cópias dos ficheiros contidos nas directorias `RESERVED` dos users que efectuaram reservas no evento em causa.

### 3.2 Formatos dos ficheiros que contêm informação relevante

Descreve-se aqui o conteúdo dos ficheiros da base de dados. Os ficheiros `login.txt` e aqueles que estão na directoria `CREATED`, valem por apenas estarem presentes, independentemente da informação que possam conter.

Descreve-se a seguir a organização da informação relevante contida nos restantes ficheiros:

**O ficheiro `pass.txt`** contém a password do utilizador. Se o utilizador emitir o comando `unregister`, o ficheiro `pass.txt` é apagado.

**O ficheiro `START`** contém uma única linha com os seguintes campos:

```
UID event name desc fname event attend start date start time
```

em que:
- `UID` é a identificação do utilizador com formato fixo de seis caracteres numéricos.
- `name` é o nome dado pelo utilizador ao evento em causa.
- `desc fname` é o nome do ficheiro de descrição do evento.
- `event attend` é o número total de lugares do evento.
- `start date` é a data de início do evento no formato `dd-mm-yyyy HH:MM`

**Se existir um ficheiro `END`**, ele indica que o evento se encontra encerrado. Nesse caso, o ficheiro `END` contém uma linha única com os seguintes campos:

```
end datetime
```

em que:
- `end datetime` é a data de encerramento do evento no formato `dd-mm-yyyy HH:MM:SS`

**Os ficheiros contidos na directoria `RESERVED`** têm a mesma designação e conteúdo dos ficheiros que se encontram sob a directoria `RESERVATIONS`. O nome destes ficheiros é:

```
R-(uid)-(date) (time).txt
```

Em que `(uid)` é o UID do utilizador que efectuou a reserva, `(date)` é a data em que foi efectuada a reserva no formato `YYYY-MM-DD` e `(time)` é a hora da reserva no formato `HHMMSS`. Se por absurdo um utilizador humano conseguir emitir duas reservas com um intervalo inferior a um segundo, o segundo ficheiro gerado irá sobrepor-se ao primeiro se ambos tiverem a mesma designação. No entanto isso não constitui problema pois os ficheiros sob as directorias `RESERVED` e `RESERVATIONS` têm valor meramente informativo. O ficheiro `RES (eid).txt` contém o real totalizador das reservas para um evento.

**O conteúdo de cada ficheiro de reserva** é:

```
UID res num res datetime
```

em que:
- `UID` é a identificação do utilizador que efectuou a reserva.
- `res num` é o número de lugares reservados.
- `res datetime` é a data da reserva em causa no formato `DD-MM-YYYY HH:MM:SS`.

O estado de um evento não se encontra gravado em ficheiro algum. Sempre que necessário, o servidor ES calcula o estado do evento a partir dos factores que o determinam.

O ficheiro `END .txt` que assinala a terminação de um evento é criado sempre que o utilizador que criou o evento decide fechá-lo ou sempre que o servidor, em acesso à base de dados percebe que a data do evento se encontra no passado. No primeiro caso de terminação, a data de fecho do evento é a data real na qual o evento foi encerrado. No segundo caso, a data de fecho do evento é a sua data de ocorrência.

### 3.3 Exemplo de execução

Para clarificar a organização de dados apresenta-se a seguir um exemplo de interacção entre aplicações clientes `user` e o servidor ES.

- O utilizador `111111` começou por registar-se emitindo o comando `login` pela primeira vez. Deu então início a um evento com a designação 'Conference' carregando o ficheiro 'Conference 01.jpg' e estipulando para este evento uma data de ocorrência `05-12-2025 15:00`. O servidor ES notificou a aplicação `user` do sucesso da operação e atribuiu ao evento acabado de criar a identificação `001`.

- Posteriormente, numa outra aplicação `user`, o utilizador `222222` registou-se emitindo o comando `login` pela primeira vez, e reservou 100 lugares no evento com `EID==001`.

A base de dados do servidor ES ficou com o seguinte conteúdo:

```
ESDIR
├── USERS
│   ├── 111111
│   │   ├── 111111 pass.txt
│   │   ├── 111111 login.txt
│   │   ├── CREATED
│   │   │   └── 001.txt
│   │   └── RESERVED
│   └── 222222
│       ├── 222222 pass.txt
│       ├── 222222 login.txt
│       ├── CREATED
│       └── RESERVED
│           └── R-222222-20251120 121505.txt
└── EVENTS
    └── 001
        ├── START 001.txt
        ├── RES 001.txt
        ├── DESCRIPTION
        │   └── Conference 01.jpg
        └── RESERVATIONS
            └── R-222222-20251120 121505.txt
```

Após esta interacção, o ficheiro `RES 001.txt` contém o valor `100`.

A dada altura, o utilizador `222222` abandonou a sessão e o utilizador `333333` registou-se, tendo logo de seguida reservado 50 lugares no evento `001`. Após a emissão dessa reserva, o utilizador `111111` decidiu encerrar o evento antecipadamente. Então o conteúdo da base de dados passou a ser:

```
ESDIR
├── USERS
│   ├── 111111
│   │   ├── 111111 pass.txt
│   │   ├── 111111 login.txt
│   │   ├── CREATED
│   │   │   └── 001.txt
│   │   └── RESERVED
│   ├── 222222
│   │   ├── 222222 pass.txt
│   │   ├── CREATED
│   │   └── RESERVED
│   │       └── R-222222-20251120 121505.txt
│   └── 333333
│       ├── 333333 pass.txt
│       ├── 333333 login.txt
│       ├── CREATED
│       └── RESERVED
│           └── R-333333-20251120 121510.txt
└── EVENTS
    └── 001
        ├── START 001.txt
        ├── RES 001.txt
        ├── DESCRIPTION
        │   └── Conference 01.jpg
        ├── END 001.txt
        └── RESERVATIONS
            ├── R-222222-20251120 121505.txt
            └── R-333333-20251120 121510.txt
```

No final destas interacções, o ficheiro `RES 001.txt` contém o valor `150`.

---

## 4. Complementos de programação

Nesta secção sugerem-se procedimentos e exemplifica-se a utilização de algumas funções em C com a finalidade de facilitar a implementação do projecto.

### 4.1 Sobre o encerramento dos eventos

O servidor ES em operação no 'tejo' não toma a decisão de encerrar os eventos a determinados momentos. Não usa portanto temporizadores ou varrimentos periódicos à base de dados para encerrar os eventos.

Em vez disso, ele aproveita as interacções geradas pelas aplicações clientes que lhe acedem de forma concorrente, para verificar se um dado evento deve ser encerrado. Quando o servidor solicitado para uma dada operação que acede a um evento, verifica que já passou a data de encerramento para esse evento, cria na respectiva directoria o ficheiro `END`. Se o utilizador que criou o evento, decide encerrá-lo, então o servidor ES, no cumprimento dessa ordem de encerramento antecipado, encerra o evento criando o ficheiro `END`.

Se por exemplo, um utilizador pede o detalhe sobre um dado evento, o servidor aproveita essa solicitação (mensagem `SED` por TCP) para verificar se o evento ultrapassou o prazo de validade e em caso afirmativo, encerra o evento criando o ficheiro `END` mas enviando sempre o detalhe sobre o evento ao utilizador que o solicitou. Neste exemplo, o utilizador que solicitou o detalhe não se apercebe que o servidor ES aproveitou o seu pedido referente ao evento em causa para verificar se o prazo do mesmo tinha expirado.

Se um utilizador emitiu uma operação de `RID` para um dado evento, o servidor ES verifica em primeiro lugar se o evento já foi marcado como encerrado. Caso contrário, verifica de seguida se o evento já ultrapassou o seu prazo e responde em conformidade. Caso isso aconteça, marca o evento como encerrado. Caso o evento esteja dentro do prazo, cria os ficheiros respectivos nas directorias `RESERVATIONS` e `RESERVED` e responde afirmativamente ao utilizador.

Os alunos podem usar o procedimento acima descrito para controlar o estado dos eventos, ou podem usar outro que entendam conveniente como seja por exemplo a programação de temporizadores no ES orientada para esse fim.

### 4.2 Criação de directorias e subdirectorias

Exemplo de criação de directoria e subdirectorias para novo evento:

```c
int CreateEVENTDir(int EID)
{
    char EID_dirname[15];
    char RESERV_dirname[25];
    char DESC_dirname[25];
    int ret;

    if (EID<1 || EID>999)
        return(0);

    sprintf(EID_dirname, "EVENTS/%03d", EID);
    ret=mkdir(EID_dirname, 0700);
    if (ret==-1)
        return(0);

    sprintf(RESERV_dirname, "EVENTS/%03d/RESERVATIONS", EID);
    ret=mkdir(RESERV_dirname, 0700);
    if (ret==-1)
    {
        rmdir(EID_dirname);
        return(0);
    }

    sprintf(DESC_dirname, "EVENTS/%03d/DESCRIPTION", EID);
    ret=mkdir(DESC_dirname, 0700);
    if (ret==-1)
    {
        rmdir(EID_dirname);
        rmdir(RESERV_dirname);
        return(0);
    }

    return(1);
}
```

### 4.3 Criação e eliminação de ficheiro

Exemplos de funções de criação e eliminação do ficheiro de login:

```c
int CreateLogin(char *UID)
{
    char login_name[35];
    FILE *fp;

    if (strlen(UID)!=6)
        return(0);

    sprintf(login_name, "USERS/%s/%s login.txt", UID, UID);
    fp=fopen(login_name, "w");
    if (fp==NULL)
        return(0);

    fprintf(fp, "Logged in\n");
    fclose(fp);
    return(1);
}

int EraseLogin(char *UID)
{
    char login_name[35];

    if (strlen(UID)!=6)
        return(0);

    sprintf(login_name, "USERS/%s/%s login.txt", UID, UID);
    unlink(login_name);
    return(1);
}
```

### 4.4 Obtenção de informações sobre ficheiro

Exemplo com função para determinação de existência e tamanho do ficheiro de descrição de evento:

```c
#include <stat.h>

int CheckDescFile(char *fname)
{
    struct stat file_stat;
    int ret_stat;

    ret_stat=stat(fname, &file_stat);
    if (ret_stat==-1 || file_stat.st_size==0)
        return(0);

    return(file_stat.st_size);
}
```

### 4.5 Listagem ordenada de conteúdos de directorias

Há situações que requerem a ordenação de informação contida em directorias. Para tal, o servidor ES no 'tejo' aproveita as funcionalidades do filesystem do Linux no que respeita à obtenção de listagens de directorias com os conteúdos ordenados. Exemplifica-se a seguir uma função hipotética `GetEventList()`. As entradas relevantes da directoria são carregadas uma a uma para a variável apontada por `list`.

```c
#include <dirent.h>

int GetEventList(char *EID, EVENTLIST *list)
{
    struct dirent **file_list;
    int n_entries, no_events, i_ent=0;
    char dirname[55];

    sprintf(dirname, "USERS/%s/CREATED/", EID);
    n_entries = scandir(dirname, &file_list, 0, alphasort);
    no_events =0;

    if (n_entries <= 0)
        return(0);
    else
    {
        while (i_ent < n_entries)
        {
            if (file_list[i_ent]->d_name[0] != '.')
            {
                memcpy(list->event_no[no_events], file_list[i_ent]->d_name, 3);
                list->event_no[no_events][3] = 0;
                ++no_events;
            }
            free(file_list[i_ent]);
            ++i_ent;
        }
        list->no_events=no_events;
        free(file_list);
    }

    return(no_events);
}
```

Variantes da função `GetEventList()` podem ser usadas para seleccionar apenas a maior reserva, ou o evento.

### 4.6 Processamento de tempos e datas

A execução da função `time(&fulltime)`, retorna na variável designada `fulltime` que é do tipo `time_t`, o número de segundos decorridos desde a data `1970-01-01 00:00:00` até ao momento presente - também conhecido por Unix time stamp. Recomenda-se cuidado na utilização do tipo `time_t`, nomeadamente na sua interacção com inteiros. Nalgumas situações o tipo `time_t` pode interagir com inteiros sem precauções especiais. Os alunos devem verificar caso a caso e nas máquinas que vão usar para o desenvolvimento as precauções a tomar para o efeito. O número de segundos decorridos desde `1970-01-01 00:00:00` até ao momento em que o presente projecto vai ser avaliado não excede a capacidade de um inteiro de 32 bits (4 bytes) com sinal.

A conversão do número de segundos decorridos desde `1970-01-01 00:00:00` até um dado momento para o formato `DD-MM-YYYY HH:MM:SS` (data de calendário) consegue-se usando a função `gmtime()` como se exemplifica a seguir:

```c
#include <time.h>

time_t fulltime;
struct tm *current_time;
char timestr[20];

time(&fulltime); // Get current time in seconds starting at 1970-...
current_time = gmtime(&fulltime); // Convert time_t to DD-MM-YYYY HH:MM:SS. current_time points to a struct of type tm
sprintf(timestr, "%2d-%02d-%04d %02d:%02d:%02d",
    current_time->tm_mday, current_time->tm_mon+1, current_time->tm_year+1900,
    current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
```

Para comparação de datas pode ser útil converter uma determinada data de calendário na Unix time stamp. Para tal preenche-se uma estrutura do tipo `struct tm` com os valores relevantes e usa-se a função `mktime()`. Como se exemplifica a seguir para a conversão da data de calendário de um evento para Unix time stamp:

```c
#include <time.h>

time_t creation_unix_time;
struct tm creation_datetime;

creation_datetime.tm_mday=event_day;
creation_datetime.tm_mon=event_month-1;
creation_datetime.tm_year=event_year-1900;
creation_datetime.tm_hour=event_hour;
creation_datetime.tm_min=event_min;
creation_datetime.tm_sec=0;

creation_unix_time=mktime(&creation_datetime);
```

### 4.7 Extensões ao protocolo

A secção do enunciado do projecto - 7 Open Issues - sugere a possibilidade de estender o protocolo especificado. A directoria `RESERVATIONS` sob cada evento, acima descrita, não tem interesse prático na actual especificação do protocolo. Contudo, a informação nela contida permite recuperar rapidamente o detalhe das reservas efectuadas para cada evento.

Os alunos podem estender o protocolo na vertente da comunicação TCP para recuperar essa informação, adicionando comandos que permitam obtê-la com critérios de filtragem especificados nesses novos comandos. Como por exemplo: Todas as reservas efectuadas para um dado evento entre duas datas. Ou todas as reservas efectuadas para um dado evento por um utilizador.
