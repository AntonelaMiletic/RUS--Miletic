# Control Flow Graph (CFG) – Lab1

```mermaid
flowchart TD
    A[Start] --> B[setup()]
    B --> C[Inicijalizacija LED pinova]
    C --> D[Inicijalizacija tipkala]
    D --> E[Inicijalizacija HC-SR04]
    E --> F[Postavljanje vanjskih prekida]
    F --> G[Postavljanje timer prekida]
    G --> H[loop()]

    H --> I[Čitanje i brisanje zastavica u kritičnoj sekciji]
    I --> J{flagTimer?}
    J -- Da --> K[handleTimerEvent()]
    J -- Ne --> L{flagHigh?}
    L -- Da --> M[handleHighEvent()]
    L -- Ne --> N{flagMed?}
    N -- Da --> O[handleMedEvent()]
    N -- Ne --> P{flagLow?}
    P -- Da --> Q[handleLowEvent()]
    P -- Ne --> R[Nema prekidnog događaja]

    K --> S[readDistanceCm()]
    M --> S
    O --> S
    Q --> S
    R --> S

    S --> T{distance < ALERT_DISTANCE?}
    T -- Da --> U[handleAlertEvent(distance)]
    T -- Ne --> V[digitalWrite(LED_ALERT, LOW)]
    U --> W[delay(100)]
    V --> W
    W --> H

    X[ISR High] --> X1[Postavi flagHigh]
    Y[ISR Med] --> Y1[Postavi flagMed]
    Z[ISR Low] --> Z1[Postavi flagLow]
    TT[ISR Timer] --> TT1[Postavi flagTimer]

    X1 -. povratak u loop .-> H
    Y1 -. povratak u loop .-> H
    Z1 -. povratak u loop .-> H
    TT1 -. povratak u loop .-> H