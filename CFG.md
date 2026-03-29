# Control Flow Graph (CFG) – Lab1

```mermaid
flowchart TD
    A[Start] --> B[setup]
    B --> C[Init LED pinova]
    C --> D[Init tipkala]
    D --> E[Init HC-SR04]
    E --> F[Postavljanje vanjskih prekida]
    F --> G[Postavljanje timer prekida]
    G --> H[loop]

    H --> I[Ucitaj i obrisi zastavice]
    I --> J{flagTimer}
    J -- Da --> K[Obrada timer dogadjaja]
    J -- Ne --> L{flagHigh}
    L -- Da --> M[Obrada high dogadjaja]
    L -- Ne --> N{flagMed}
    N -- Da --> O[Obrada med dogadjaja]
    N -- Ne --> P{flagLow}
    P -- Da --> Q[Obrada low dogadjaja]
    P -- Ne --> R[Nema prekidnog dogadjaja]

    K --> S[Ocitaj udaljenost]
    M --> S
    O --> S
    Q --> S
    R --> S

    S --> T{distance manji od ALERT_DISTANCE}
    T -- Da --> U[Obrada alarmnog dogadjaja]
    T -- Ne --> V[Ugasi LED_ALERT]
    U --> W[Povratak u loop]
    V --> W
    W --> H

    X[ISR High] --> X1[Postavi flagHigh]
    Y[ISR Med] --> Y1[Postavi flagMed]
    Z[ISR Low] --> Z1[Postavi flagLow]
    TT[ISR Timer] --> TT1[Postavi flagTimer]

    X1 -. povratak .-> H
    Y1 -. povratak .-> H
    Z1 -. povratak .-> H
    TT1 -. povratak .-> H