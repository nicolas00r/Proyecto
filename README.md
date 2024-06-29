## Descripción

Esta aplicación permite emular partidas de Monopoly con un alta fidelidad, respetando todas sus reglas importantes. Los jugadores pueden realizar acciones como recorrer el mapa, interactuar con las propiedades (compra de propiedad, compra de casas, hipoteca), e incluso pueden llegar a interactuar con sus rivales en la partida si así lo desean (sistema de negociación entre jugadores).

## Cómo compilar y ejecutar

Dado que el sistema está diseñado para ser accesible y fácil de probar, recomendamos usar [Repl.it](http://repl.it/) para una rápida configuración y ejecución. Sigue estos pasos para comenzar:

1. Visita [Repl.it](https://repl.it/).
2. Crea una nueva cuenta o inicia sesión si ya tienes una.
3. Una vez en tu dashboard, selecciona "New Repl" y elige "Import from GitHub".
4. Pega la URL del repositorio: `https://github.com/nicolas00r/Proyecto`.
5. [Repl.it](http://repl.it/) clonará el repositorio y preparará un entorno de ejecución.
6. Presiona el botón "Run" para compilar y ejecutar la aplicación.

## Funcionalidades

### Funcionando correctamente:

- Inicio de una nueva partida, acompañado de una asignación inicial de datos del jugador (nombre, dinero inicial, turnos).
- Listado de las reglas más importantes del juego.
- Visualización del tablero
- Impresión por pantalla del estado del jugador en cada turno (nombre, dinero actual, posición actual, etc.).
- Recorrido del tablero (funcionando bajo la lógica de lanzamiento de 2 dados al azar), aplicando efectos variados dependiendo en la casilla donde se caiga.
- Compra y subasta de propiedades, compra y venta de casas para las mismas, además de la posibilidad de hipotecar propiedades para obtener dinero de regreso.
- Intercambio entre jugadores (Dinero y propiedades)
- Finalización del turno por parte del jugador (indicando que ya realizó todas las acciones deseadas para este turno).
- Declaración de bancarrota por parte del jugador (terminando su participación en la partida de manera prematura).

### Problemas conocidos:
- Se deben respetar las indicaciones del juego, tales como al momento de responder una pregunta con s/n, debe responderse con estos caracteres, por el contrario el programa podría tener comportamientos indebidos.
- Los nombres de los jugadores deben tener un largo máximo de 200 caracteres.

### A mejorar:

- Apartado visual del juego (representación visual del tablero y los datos del jugador de una forma más agradable a la vista).
- Implementación de la regla que obliga a repartir las casas al comprarlas (un ejemplo de esta regla sería que al comprar 4 casas para un barrio, no podría colocarlas todas en una sola propiedad, teniendo que repartirlas).
- Implementación de las funciones de guardar partida y cargar partida, las cuales estuvieron consideradas hasta etapas avanzadas del proyecto.

## Ejemplo de acciones a realizar en una partida
Comenzaremos mostrando las reglas del juego
````
╔══════════════════════════════════════════════════╗
║        ＭＯＮＯＰＯＬY    ＶＡＬＰＡＲＡÍＳＯ         ║
╚══════════════════════════════════════════════════╝
=== MENÚ INICIAL ===
1. Nueva partida
2. Cargar partida
3. Reglas
4. Salir del juego
Seleccione una opción: 3
````

````
Mostrando reglas del juego...


=== Reglas de Monopoly ===
1. En esta versión de Monopoly, la computadora actua como banquero.
2. El objetivo del juego es ser el último jugador en pie, es decir, sin caer en bancarrota.
3. Un jugador cae en bancarrota al tener una deuda más grande de lo que puede pagar, es decir, que ni hipotecando y vendiendo todos sus bienes podría llegar a saldar dicha deuda.
4. Cuando un jugador se encuentre con una deuda que no pueda pagar, no se le permitirá intercambiar con otros jugadores. La única forma de salvarse de la bancarrota será hipotecando todas sus propiedades.
5. Las partidas deben componerse de 2 a 4 jugadores.
6. Los jugadores pueden moverse libremente por el tablero, sufiendo diferentes efectos en su travesía.
7. Si un jugador cae en una propiedad sin dueño, puede comprarla. Si por el contrario esta ya tiene dueño, el jugador deberá pagar renta a este.
8. Si un jugador posee una propiedad, este puede hipotecarla para obtener la mitad del costo de la propiedad sumado a la mitad del precio del total de casas compradas en la propiedad. Para recuperar una propiedad hipotecada, el jugador deberá pagar un monto que depende de factores como el valor de la propiedad y la cantidad de casas que esta posea.
9. Las propiedades pueden hipotecarse con casas construidas, no es necesario venderlas para poder hipotecar.
10. El jugador puede caer sobre casillas de impuestos o cartas, las cuales son de fortuna o arca comunal y ejercen distintos efectos sobre dicho jugador.
11. Si un jugador acaba en la carcel, deberá esperar 3 turnos o pagar una fianza para salir.
12. Para comprar casas en sus propiedades, antes el jugador deberá poseer todas las demás propiedades de dicha zona.
13. El jugador puede repartir las casas compradas para sus propiedades como guste, por ejemplo, si compra 4 casas para un barrio, puede ubicar todas ellas en una sola propiedad, como también puede poner 2 casas en 2 propiedades distintas.
14. Si el jugador saca dobles en sus dados (ambos son el mismo número), debe lanzarlos nuevamente. En caso de obtener dobles 3 veces consecutivas, el jugador debe irse directo a la cárcel.
15. Al pasar por la casilla de salida, el jugador cobra $2000.
16. Si un jugador tiene la opción de comprar una propiedad y no lo hace, esta se pondrá en subasta y se pujará hasta encontrar un dueño. En caso de que ningún jugador puje, se le entregará la propiedad al último jugador consultado en la subasta.
17. La subasta iniciara con puja base correspondiente a la mitad del precio de la propiedad.
18. Si un jugador cae en una propiedad hipotecada, el dueño de esta no percibirá beneficios de renta de la misma.
19. Los jugadores pueden llevar a cabo negociaciones entre si, ofreciendo bienes de uno y otro para buscar cerrar un trato.
20. Al caer en la cárcel, el jugador puede esperar 3 turnos para salir de esta, o también puede pagar $500 de fianza para salir de forma anticipada.
===========================
````

Ahora comenzaremos una nueva partida

````
╔══════════════════════════════════════════════════╗
║        ＭＯＮＯＰＯＬY    ＶＡＬＰＡＲＡÍＳＯ         ║
╚══════════════════════════════════════════════════╝
=== MENÚ INICIAL ===
1. Nueva partida
2. Cargar partida
3. Reglas
4. Salir del juego
Seleccione una opción: 1
````
Diremos que queremos una partida de 2 jugadores, sus nombres serán Perro y Sombrero.
````
Iniciando nueva partida...
Ingrese el número de jugadores (2 a 4): 2
Ingrese el nombre del jugador 1: Perro
Ingrese el nombre del jugador 2: Sombrero
````
Veremos que el jugador que inicia la partida es Perro, por lo que es su turno y debe lanzar los dados.
````
Perro ES TU TURNO!

╔══════════════════════════════════════════════════╗
║                 MAPA DEL JUEGO                   ║
╚══════════════════════════════════════════════════╝
    00  01  02  03  04  05  06  07  08  09  10 
    39                                      11 
    38                                      12 
    37                                      13 
    36                                      14 
    35                                      15 
    34                                      16 
    33                                      17 
    32                                      18 
    31                                      19 
    30  29  28  27  26  25  24  23  22  21  20 

Posición en el tablero: 00

╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Perro
Dinero actual: 15000
Turnos de penalización en cárcel: 0
Te encuentras en: SALIDA
Tipo: Salida
Propietario: -
════════════════════════════════════════════════════

Presiona enter para lanzar los dados!
````
Al lanzar los dados, vemos que obtenemos dobles, por lo que tenemos un turno adicional, además caimos en una casilla de carta comunal, por lo que se nos aplica el efecto de una carta de ese tipo.
````
¡Haz lanzado los dados: 1 y 1 (Total: 2)!

Haz obtenido dobles!

Ganas un turno extra!

¡Te moviste a la casilla 2!

¡CARTA DE LA ARCA COMÚNAL!

Carta sacada: HOSPITALIZACIÓN. PAGUE $1000.

Presiona enter para continuar...
````
Al haber lanzado los dados nuevamente, hemos caido en una propiedad, por lo que vamos a proceder a comprarla.
````
¡Haz lanzado los dados: 6 y 4 (Total: 10)!

¡Te moviste a la casilla 12!

Dinero Actual: 14000

╔══════════════════════════════════════════════════╗
║            DETALLES DE LA PROPIEDAD              ║
╚══════════════════════════════════════════════════╝
Nombre: CAMPUS CURAUMA PUCV
Sector: CURAUMA
Precio: $1500
Renta: $235
Hipotecada: No
Precio por casa: $1000
Numero de casas: 0
Posición: 12

¿Quieres comprar CAMPUS CURAUMA PUCV por 1500? (s/n): s

Felicidades Perro, has comprado la propiedad CAMPUS CURAUMA PUCV por 1500!

Presiona enter para continuar...
````

Ya teniendo una propiedad, vamos a hipotecarla para obtener una fracción del dinero por el que la compramos.
````
╔══════════════════════════════════════════════════╗
║                 MAPA DEL JUEGO                   ║
╚══════════════════════════════════════════════════╝
    00  01  02  03  04  05  06  07  08  09  10 
    39                                      11 
    38                                      12 
    37                                      13 
    36                                      14 
    35                                      15 
    34                                      16 
    33                                      17 
    32                                      18 
    31                                      19 
    30  29  28  27  26  25  24  23  22  21  20 

Posición en el tablero: 12

╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Perro
Dinero actual: 12500
Turnos de penalización en cárcel: 0
Te encuentras en: CAMPUS CURAUMA PUCV
Tipo: Propiedad
Propietario: Perro
════════════════════════════════════════════════════

¿Qué acción deseas realizar?

1. Modificar campos de tus propiedades
2. Intercambiar con otros jugadores
3. Finalizar turno
4. Declararte en bancarrota
````
Seleccionamos la opción 1 (Modificar campos de tus propiedades) e hipotecamos CAMPUS CURAUMA PUCV por $750
````
╔══════════════════════════════════════════════════╗
║            DETALLES DE LA PROPIEDAD              ║
╚══════════════════════════════════════════════════╝
Nombre: CAMPUS CURAUMA PUCV
Sector: CURAUMA
Precio: $1500
Renta: $235
Hipotecada: No
Precio por casa: $1000
Numero de casas: 0
Posición: 12

¿Qué deseas hacer?
1. Comprar casas
2. Vender casas
3. Hipotecar propiedades
4. Deshipotecar propiedades
5. Salir de este menú
3
¿
Quieres hipotecar la propiedad CAMPUS CURAUMA PUCV por 750? (s/n): s

Felicidades Perro, has hipotecado la propiedad CAMPUS CURAUMA PUCV y has recibido 750!

Recuerda que no podrás cobrar renta por CAMPUS CURAUMA PUCV mientras esté hipotecada.

¿Deseas continuar en este menú? (s/n): n
````
````
...
````

Ya en el turno de Sombrero, tiraremos los dados.
````
Sombrero ES TU TURNO!

╔══════════════════════════════════════════════════╗
║                 MAPA DEL JUEGO                   ║
╚══════════════════════════════════════════════════╝
    00  01  02  03  04  05  06  07  08  09  10 
    39                                      11 
    38                                      12 
    37                                      13 
    36                                      14 
    35                                      15 
    34                                      16 
    33                                      17 
    32                                      18 
    31                                      19 
    30  29  28  27  26  25  24  23  22  21  20 

Posición en el tablero: 00

╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Sombrero
Dinero actual: 15000
Turnos de penalización en cárcel: 0
Te encuentras en: SALIDA
Tipo: Salida
Propietario: -
════════════════════════════════════════════════════

Presiona enter para lanzar los dados!
````
Hemos caido en una casilla de fortuna, por lo que sufriremos los efectos de una carta de este tipo.
````
¡Haz lanzado los dados: 6 y 1 (Total: 7)!

¡Te moviste a la casilla 7!

¡CARTA DE LA FORTUNA!

Carta sacada: AVANCE HASTA LA CASILLA DE SALIDA, COBRE $2000

Pasaste por la salida, cobra $2000

Presiona enter para continuar...
````
Ya que sombrero no ha obtenido propiedades, hará negocios con su rival Perro para obtener una a cambio de dinero.
````
╔══════════════════════════════════════════════════╗
║            DETALLES DE LA PROPIEDAD              ║
╚══════════════════════════════════════════════════╝
Nombre: CAMPUS CURAUMA PUCV
Sector: CURAUMA
Precio: $1500
Renta: $235
Hipotecada: Sí
Precio por casa: $1000
Numero de casas: 0
Posición: 12

¿Deseas elegir esta propiedad? (s/n): s
¿Deseas continuar eligiendo propiedades? (s/n): n
````
````
Ahora debes elegir lo que le ofreceras para el intercambio

¿Cuanto dinero deseas ofrecer: 
2000

¿Que propiedades ofreceras?

Presiona enter para continuar a elegir propiedades...

Este jugador no posee propiedades
====================================================

Perro se te ha propuesto el siguiente intercambio: 

====================================================

Dinero pedido: 0

Se han solicitado las siguientes propiedades: 

╔══════════════════════════════════════════════════╗
║            DETALLES DE LA PROPIEDAD              ║
╚══════════════════════════════════════════════════╝
Nombre: CAMPUS CURAUMA PUCV
Sector: CURAUMA
Precio: $1500
Renta: $235
Hipotecada: Sí
Precio por casa: $1000
Numero de casas: 0
Posición: 12

====================================================

Dinero ofrecido: 2000

Se han ofrecido las siguientes propiedades: 
-

====================================================

Perro ¿Deseas aceptar el intercambio? (s/n): s

¡EL INTERCAMBIO FUE ACEPTADO!

Presione enter para salir de este menú...
````
````
...
````
Ya en el turno de Perro, tiraremos los dados.
````
Perro ES TU TURNO!

╔══════════════════════════════════════════════════╗
║                 MAPA DEL JUEGO                   ║
╚══════════════════════════════════════════════════╝
    00  01  02  03  04  05  06  07  08  09  10 
    39                                      11 
    38                                      12 
    37                                      13 
    36                                      14 
    35                                      15 
    34                                      16 
    33                                      17 
    32                                      18 
    31                                      19 
    30  29  28  27  26  25  24  23  22  21  20 

Posición en el tablero: 12

╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Perro
Dinero actual: 15250
Turnos de penalización en cárcel: 0
Te encuentras en: CAMPUS CURAUMA PUCV
Tipo: Propiedad
Propietario: Sombrero
════════════════════════════════════════════════════

Presiona enter para lanzar los dados!
````
Hemos caido en una propiedad, por lo que la compraremos.
````

¡Haz lanzado los dados: 6 y 1 (Total: 7)!

¡Te moviste a la casilla 19!

Dinero Actual: 15250

╔══════════════════════════════════════════════════╗
║            DETALLES DE LA PROPIEDAD              ║
╚══════════════════════════════════════════════════╝
Nombre: ESCUELA ECONOMÍA PUCV
Sector: COSTA VALPO
Precio: $2000
Renta: $320
Hipotecada: No
Precio por casa: $1000
Numero de casas: 0
Posición: 19

¿Quieres comprar ESCUELA ECONOMÍA PUCV por 2000? (s/n): s

Felicidades Perro, has comprado la propiedad ESCUELA ECONOMÍA PUCV por 2000!

Presiona enter para continuar...
````
````
...
````
Finalmente, el jugador Perro se declara voluntariamente en bancarrota, dando como ganador al jugador Sombrero.
````
╔══════════════════════════════════════════════════╗
║                 MAPA DEL JUEGO                   ║
╚══════════════════════════════════════════════════╝
    00  01  02  03  04  05  06  07  08  09  10 
    39                                      11 
    38                                      12 
    37                                      13 
    36                                      14 
    35                                      15 
    34                                      16 
    33                                      17 
    32                                      18 
    31                                      19 
    30  29  28  27  26  25  24  23  22  21  20 

Posición en el tablero: 19

╔══════════════════════════════════════════════════╗
║               ESTADO DEL JUGADOR                 ║
╚══════════════════════════════════════════════════╝
════════════════════════════════════════════════════
Jugador: Perro
Dinero actual: 13250
Turnos de penalización en cárcel: 0
Te encuentras en: ESCUELA ECONOMÍA PUCV
Tipo: Propiedad
Propietario: Perro
════════════════════════════════════════════════════

¿Qué acción deseas realizar?

1. Modificar campos de tus propiedades
2. Intercambiar con otros jugadores
3. Finalizar turno
4. Declararte en bancarrota
4

Perro haz sido eliminado, estas en bancarrota.

Presione enter para continuar

¡El jugador Sombrero es el ganador del juego!

Presiona enter para salir
````

## Contribuciones individuales

### Nicolas Reed:
- Ideación del proyecto
- Desarrollo del funcionamiento del juego
- Codificación de menús de subastas e intercambio
- Adaptación de los TDA'S para el desarrollo de la app
- Revisión de errores
- Autoevaluación 3/3 

### Joaquín Garrido:
- Ideación del proyecto
- Desarrollo de cartas y aplicaciones de sus efectos
- Redacción del informe inicial e informe final
- Redacción del README
- Revisión de errores
- Autoevaluación 3/3 

### Inti Liberona:
- Ideación del proyecto
- Planilla de propiedades
- Lógica de compra y venta de casas
- Implementación visual del tablero
- Autoevaluación 3/3 

### Benjamín Gonzalez:
- Ideación del proyecto
- Redacción del informe final
- Lógica asignación de jugadores
- Manejo del hipotecado y deshipotecado
- Autoevaluación 3/3 